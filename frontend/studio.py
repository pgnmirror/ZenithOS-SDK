from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QPushButton, QVBoxLayout,
    QLabel, QTextEdit, QMessageBox, QHBoxLayout, QStackedWidget,
    QDialog, QComboBox, QLineEdit, QCheckBox, QToolTip,
    QFileDialog, QListWidget, QListWidgetItem, QSplashScreen,
    QTreeWidget, QTreeWidgetItem, QSplitter, QInputDialog, QPlainTextEdit
)
from PySide6.QtGui import (
    QFont, QKeySequence, QAction, QColor, QTextCharFormat,
    QSyntaxHighlighter, QTextCursor, QPixmap
)
from PySide6.QtCore import Qt, QPropertyAnimation, QEasingCurve, QEvent, QPoint, QTimer, QProcess
import sys, os, subprocess, re, time, shutil, zipfile

def log(message):
    timestamp = f"[{time.time():.2f}]"
    print(f"{timestamp} {message}")

class CSyntaxHighlighter(QSyntaxHighlighter):
    def __init__(self, document):
        super().__init__(document)
        self.highlighting_rules = []
        keyword_format = QTextCharFormat()
        keyword_format.setForeground(QColor("#FF79C6"))
        keyword_format.setFontWeight(QFont.Bold)
        keywords = [
            "int", "char", "float", "if", "else", "for", "while", "return", "void",
            "struct", "typedef", "enum", "const", "static", "extern", "switch", "case",
            "break", "continue", "goto", "sizeof", "long", "short", "unsigned", "signed"
        ]
        self.function_format = QTextCharFormat()
        self.function_format.setForeground(QColor("#FFD700"))
        self.function_format.setFontWeight(QFont.Bold)
        self.functions = [
            "printf", "scanf", "malloc", "free", "strlen", "strcmp", "strcpy",
            "fopen", "fclose", "fread", "fwrite", "exit", "perror", "system"
        ]
        for word in keywords:
            pattern = r'\b' + word + r'\b'
            self.highlighting_rules.append((re.compile(pattern), keyword_format))
        string_format = QTextCharFormat()
        string_format.setForeground(QColor("#F1FA8C"))
        self.highlighting_rules.append((re.compile(r'"[^"\\]*(\\.[^"\\]*)*"'), string_format))
        number_format = QTextCharFormat()
        number_format.setForeground(QColor("#BD93F9"))
        self.highlighting_rules.append((re.compile(r'\b\d+\b'), number_format))
        function_pattern = r'\b(' + '|'.join(self.functions) + r')\b\s*\('
        self.highlighting_rules.append((re.compile(function_pattern), self.function_format))
        directive_format = QTextCharFormat()
        directive_format.setForeground(QColor("#8BE9FD"))
        self.highlighting_rules.append((re.compile(r'#\s*(include|define)\b'), directive_format))

    def highlightBlock(self, text):
        for pattern, fmt in self.highlighting_rules:
            for match in pattern.finditer(text):
                start, end = match.span()
                self.setFormat(start, end - start, fmt)

class ZenithOSApp(QMainWindow):
    def __init__(self):
        super().__init__()
        QToolTip.setFont(QFont("Arial", 10))
        app = QApplication.instance()
        if app is not None:
            app.setStyleSheet("QToolTip { color: white; background-color: #2b2b2b; border: 1px solid #dddddd; padding: 5px; }")

        self.setWindowTitle("ZenithOS Studio")
        self.showMaximized()
        self.setStyleSheet("background-color: #1F001F;")
        self.current_theme = "purple"
        self.stacked_widget = QStackedWidget()
        self.setCentralWidget(self.stacked_widget)

        self.plugins_window = None
        self.plugins_list_widget = None

        # processes
        self.build_process = None
        self.run_process = None

        self.main_menu = self.create_main_menu()
        self.editor_page = self.create_editor_page()
        self.stacked_widget.addWidget(self.main_menu)
        self.stacked_widget.addWidget(self.editor_page)
        self.set_shortcuts()
        log("Starting ZenithOS Studio..")

    def set_shortcuts(self):
        save_shortcut = QKeySequence(Qt.CTRL | Qt.Key_S)
        save_action = QAction(self)
        save_action.setShortcut(save_shortcut)
        save_action.triggered.connect(self.save_project)
        self.addAction(save_action)

        find_shortcut = QKeySequence(Qt.CTRL | Qt.Key_F)
        find_action = QAction(self)
        find_action.setShortcut(find_shortcut)
        find_action.triggered.connect(self.toggle_project_tree)
        self.addAction(find_action)

        show_search_shortcut = QKeySequence(Qt.CTRL | Qt.SHIFT | Qt.Key_F)
        show_search_action = QAction(self)
        show_search_action.setShortcut(show_search_shortcut)
        show_search_action.triggered.connect(self.show_search_bar)
        self.addAction(show_search_action)

    def create_main_menu(self):
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setAlignment(Qt.AlignCenter)
        title = QLabel("ZenithOS Studio")
        title.setFont(QFont("Arial", 32))
        title.setStyleSheet("color: white;")
        layout.addWidget(title)
        button_style = """
            QPushButton {
                background-color: #6200EE;
                color: white;
                font-size: 16px;
                border: none;
                padding: 10px 20px;
                border-radius: 20px;
            }
            QPushButton:hover {
                background-color: #3700B3;
            }
        """
        new_project_button = QPushButton("New Project")
        new_project_button.setStyleSheet(button_style)
        new_project_button.clicked.connect(self.create_new_project)
        layout.addWidget(new_project_button)

        about_button = QPushButton("About")
        about_button.setStyleSheet(button_style)
        about_button.clicked.connect(self.show_about)
        layout.addWidget(about_button)

        settings_button = QPushButton("Settings")
        settings_button.setStyleSheet(button_style)
        settings_button.clicked.connect(self.open_settings)
        layout.addWidget(settings_button)

        plugins_button = QPushButton("Plugins")
        plugins_button.setStyleSheet(button_style)
        plugins_button.clicked.connect(self.open_plugins_window)
        layout.addWidget(plugins_button)

        exit_button = QPushButton("Exit")
        exit_button.setStyleSheet(button_style)
        exit_button.clicked.connect(self.close)
        layout.addWidget(exit_button)
        return widget

    def create_editor_page(self):
        widget = QWidget()
        layout = QVBoxLayout(widget)

        # TAB BAR (Editor / SDK Settings)
        tab_layout = QHBoxLayout()
        tab_layout.setSpacing(0)
        tab_layout.setContentsMargins(0, 0, 0, 0)
        tab_button_style = """
            QPushButton {
                background-color: #6200EE;
                color: white;
                font-size: 16px;
                border: none;
                padding: 10px 20px;
                border-top-left-radius: 20px;
                border-top-right-radius: 20px;
            }
            QPushButton:hover {
                background-color: #3700B3;
            }
            QPushButton:checked {
                background-color: #3700B3;
            }
        """
        self.editor_tab_button = QPushButton("Editor")
        self.api_tab_button = QPushButton("SDK Settings")
        self.editor_tab_button.setCheckable(True)
        self.api_tab_button.setCheckable(True)
        self.editor_tab_button.setStyleSheet(tab_button_style)
        self.api_tab_button.setStyleSheet(tab_button_style)
        self.editor_tab_button.clicked.connect(lambda: self.switch_editor_tab(0))
        self.api_tab_button.clicked.connect(lambda: self.switch_editor_tab(1))
        tab_layout.addWidget(self.editor_tab_button)
        tab_layout.addWidget(self.api_tab_button)
        layout.addLayout(tab_layout)

        # STACK for tabs
        self.editor_stack = QStackedWidget()

        # ----- setup splitter: left tree + editor on right -----
        self.splitter = QSplitter(Qt.Horizontal)

        # project tree (left) - hidden by default width 0
        self.project_tree = QTreeWidget()
        self.project_tree.setHeaderHidden(True)
        self.project_tree.setStyleSheet("background-color: #2C0032; color: #E0E0E0;")
        self.project_tree.setMaximumWidth(0)
        self.project_tree.itemDoubleClicked.connect(self.open_file_from_tree)

        # EDITOR PANEL (right)
        self.editor_content_panel = QWidget()
        editor_content_layout = QVBoxLayout(self.editor_content_panel)

        # TOP TOOLS (RUN / COMPILE / COMPILE TO ZAPP / SAVE / TREE / SEARCH)
        tool_layout = QHBoxLayout()
        tool_layout.setContentsMargins(0, 0, 0, 0)
        tool_button_style = """
            QPushButton {
                background-color: #6200EE;
                color: white;
                font-size: 14px;
                border: none;
                padding: 6px 12px;
                border-radius: 12px;
            }
            QPushButton:hover { background-color: #3700B3; }
        """
        self.run_button = QPushButton("Run Project")
        self.run_button.setStyleSheet(tool_button_style)
        self.run_button.clicked.connect(self.run_project_in_terminal)
        tool_layout.addWidget(self.run_button)

        self.compile_button = QPushButton("Compile (gcc)")
        self.compile_button.setStyleSheet(tool_button_style)
        self.compile_button.clicked.connect(self.compile_project)
        tool_layout.addWidget(self.compile_button)

        self.zapp_button = QPushButton("Compile to .ZAPP")
        self.zapp_button.setStyleSheet(tool_button_style)
        self.zapp_button.clicked.connect(self.compile_to_zapp)
        tool_layout.addWidget(self.zapp_button)

        save_top_btn = QPushButton("Save")
        save_top_btn.setStyleSheet(tool_button_style)
        save_top_btn.clicked.connect(self.save_project)
        tool_layout.addWidget(save_top_btn)

        toggle_tree_btn = QPushButton("Toggle Tree")
        toggle_tree_btn.setStyleSheet(tool_button_style)
        toggle_tree_btn.clicked.connect(self.toggle_project_tree)
        tool_layout.addWidget(toggle_tree_btn)

        show_search_btn = QPushButton("Search")
        show_search_btn.setStyleSheet(tool_button_style)
        show_search_btn.clicked.connect(self.show_search_bar)
        tool_layout.addWidget(show_search_btn)

        tools_widget = QWidget()
        tools_widget.setLayout(tool_layout)
        editor_content_layout.addWidget(tools_widget)

        # SEARCH BAR (hidden by default)
        self.search_bar = QWidget()
        search_layout = QHBoxLayout(self.search_bar)
        self.search_input = QLineEdit()
        self.search_input.setStyleSheet("""
            QLineEdit {
                background-color: #6A0DAD; 
                color: white;              
                border-radius: 64px;      
                padding: 8px 12px;           
                font-size: 14px;
            }
            QLineEdit:focus {
                border: 2px solid #BB86FC; 
            }
        """)
        self.search_input.textChanged.connect(self.highlight_search)
        self.case_checkbox = QCheckBox("Case sensitive")
        self.case_checkbox.setStyleSheet("color: white;")
        search_layout.addWidget(self.search_input)
        search_layout.addWidget(self.case_checkbox)
        self.search_bar.setFixedHeight(0)
        editor_content_layout.addWidget(self.search_bar)

        # Text editor
        self.text_edit = QTextEdit()
        self.highlighter = CSyntaxHighlighter(self.text_edit.document())
        self.text_edit.setStyleSheet("font-family: 'Courier New'; font-size: 14px; background-color: #2C0032; color: #E0E0E0; padding: 5px;")
        editor_content_layout.addWidget(self.text_edit)

        # TERMINAL (integrated)
        self.terminal_output = QPlainTextEdit()
        self.terminal_output.setReadOnly(True)
        self.terminal_output.setFixedHeight(180)
        self.terminal_output.setStyleSheet("background-color: black; color: #00FF00; font-family: 'Courier New';")
        editor_content_layout.addWidget(self.terminal_output)

        # BOTTOM: only Assembly and Back
        bottom_button_layout = QHBoxLayout()
        bottom_button_layout.setContentsMargins(0, 0, 0, 0)
        assembly_button = QPushButton("Assembly")
        assembly_button.setStyleSheet(tool_button_style)
        assembly_button.clicked.connect(self.compile_to_zapp)
        bottom_button_layout.addWidget(assembly_button)

        back_button = QPushButton("Back")
        back_button.setStyleSheet(tool_button_style)
        back_button.clicked.connect(self.back_to_main)
        bottom_button_layout.addWidget(back_button)

        editor_content_layout.addLayout(bottom_button_layout)

        # add widgets to splitter
        self.splitter.addWidget(self.project_tree)
        self.splitter.addWidget(self.editor_content_panel)
        self.splitter.setStretchFactor(0, 0)
        self.splitter.setStretchFactor(1, 1)

        # add splitter to editor stack
        self.editor_stack.addWidget(self.splitter)

        # API SETTINGS PANEL
        self.api_settings_panel = QWidget()
        api_layout = QVBoxLayout(self.api_settings_panel)
        api_layout.setAlignment(Qt.AlignTop)
        title_label = QLabel("API")
        title_label.setFont(QFont("Arial", 24))
        title_label.setStyleSheet("color: white;")
        api_layout.addWidget(title_label)

        # VERSION LIST 
        versions = [
            "3.1 (Beta SDK!)", "3.0 (Blue Cosmos)", "2.3", "2.2", "2.1",
            "2.0 (Starry Soup)", "1.1.2", "1.1.1", "1.1", "1.0.1", "1.0", "0.7 (Celestial Peak)"
        ]

        target_layout = QHBoxLayout()
        target_label = QLabel("Target version of ZenithOS API:")
        target_label.setFont(QFont("Arial", 16))
        target_label.setStyleSheet("color: white;")
        target_layout.addWidget(target_label)
        self.target_api_combo = QComboBox()
        self.target_api_combo.addItems(versions)
        self.target_api_combo.setStyleSheet(
            "QComboBox { background-color: #2C0032; color: #E0E0E0; padding: 5px; font-size: 16px; border: none; border-radius: 10px; }"
            "QComboBox QAbstractItemView { background-color: #2C0032; color: #E0E0E0; }"
        )
        self.target_api_combo.setMaxVisibleItems(15)
        target_layout.addWidget(self.target_api_combo)
        api_layout.addLayout(target_layout)

        min_layout = QHBoxLayout()
        min_label = QLabel("Minimal version of ZenithOS API:")
        min_label.setFont(QFont("Arial", 16))
        min_label.setStyleSheet("color: white;")
        min_layout.addWidget(min_label)
        self.min_api_combo = QComboBox()
        self.min_api_combo.addItems(versions)
        self.min_api_combo.setStyleSheet(
            "QComboBox { background-color: #2C0032; color: #E0E0E0; padding: 5px; font-size: 16px; border: none; border-radius: 10px; }"
            "QComboBox QAbstractItemView { background-color: #2C0032; color: #E0E0E0; }"
        )
        self.min_api_combo.setMaxVisibleItems(15)
        min_layout.addWidget(self.min_api_combo)
        api_layout.addLayout(min_layout)

        self.version_hints = {
            "0.7 (Celestial Peak)": "A very old version. Press F1 For more",
            "1.0 (Celestial Peak)": "First official release. Press F1 For more",
            "1.0.1": "Adding support to some network protocols. Press F1 For more",
            "1.1": "Added new network protocols.. Press F1 For more",
            "1.1.1": "Patch update. Press F1 For more",
            "1.1.2": "Last minor update of 1.1. Press F1 For more",
            "2.0 (Starry Soup)": "Big update, new SDK engine. Press F1 For more",
            "2.1": "Stability improvements. Press F1 For more",
            "2.2": "More bug fixes. Press F1 For more",
            "2.3": "SDK optimized. Press F1 For more",
            "3.0 (Blue Cosmos)": "Massive UI overhaul. Press F1 For more",
            "3.1 (Beta SDK!)": "Press F1 For more"
        }

        # install hints & event filter
        self.setup_version_hints(self.target_api_combo)
        self.setup_version_hints(self.min_api_combo)

        save_button = QPushButton("Save")
        save_button.setStyleSheet(tool_button_style)
        save_button.clicked.connect(self.save_api_settings)
        api_layout.addWidget(save_button)

        self.editor_stack.addWidget(self.api_settings_panel)

        self.editor_tab_button.setChecked(True)
        self.editor_stack.setCurrentIndex(0)
        layout.addWidget(self.editor_stack)
        return widget

    def setup_version_hints(self, combo: QComboBox):
        for i in range(combo.count()):
            text = combo.itemText(i)
            hint = self.version_hints.get(text, "")
            if hint:
                combo.setItemData(i, hint, Qt.ToolTipRole)
        combo.setMouseTracking(True)
        combo.installEventFilter(self)

    def eventFilter(self, obj, event):
        # handle events for version combos
        if obj in (getattr(self, "target_api_combo", None), getattr(self, "min_api_combo", None)):
            if event.type() == QEvent.KeyPress:
                if event.key() == Qt.Key_F1:
                    idx = obj.currentIndex()
                    text = obj.itemText(idx)
                    if text.startswith("0.7"):
                        QMessageBox.information(self, "Version Info",
                            "Version has been created in 29.06.2025. Officially, LICGX no longer supports this version. Don't recommend creating apps on it.")
                    elif text.startswith("1."):
                        QMessageBox.information(self, "Version Info",
                            "Versions in the 1.x branch are no longer supported. The last supported version was 1.1.2.")
                    elif text.startswith("2."):
                        QMessageBox.information(self, "Version Info",
                            "Versions in the 2.x branch are fully supported. You can safely develop apps on this branch.")
                    elif text.startswith("3.") or text.startswith("11.") or text.startswith("12."):
                        QMessageBox.information(self, "Version Info",
                            "This branch is modern. Recommended for new apps.")
                    else:
                        QMessageBox.information(self, "Version Info", f"You pressed F1 for {text}.")
                    return True
            elif event.type() == QEvent.Enter:
                idx = obj.currentIndex()
                text = obj.itemText(idx)
                hint = self.version_hints.get(text, "")
                if hint:
                    global_pos = obj.mapToGlobal(obj.rect().topLeft())
                    pt = QPoint(global_pos.x(), global_pos.y() - 30)
                    QToolTip.showText(pt, hint, obj)
                return False
        return super().eventFilter(obj, event)

    def open_plugins_window(self):
        include_dir = "./include"
        if not os.path.exists(include_dir):
            try:
                os.makedirs(include_dir)
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Failed to create include directory:\n{e}")
                return

        if self.plugins_window is None:
            dlg = QDialog(self)
            dlg.setWindowTitle("Plugins")
            dlg.setStyleSheet("background-color: #2C0032; color: white; padding: 12px;")
            dlg_layout = QVBoxLayout()

            title = QLabel("Plugins")
            title.setFont(QFont("Arial", 18))
            dlg_layout.addWidget(title)

            subtitle = QLabel("All plugins:")
            subtitle.setFont(QFont("Arial", 14))
            subtitle.setStyleSheet("color: white;")
            dlg_layout.addWidget(subtitle)

            self.plugins_list_widget = QListWidget()
            self.plugins_list_widget.setStyleSheet("background-color: #1F001F; color: #E0E0E0; padding: 6px;")
            dlg_layout.addWidget(self.plugins_list_widget, 1)

            add_btn = QPushButton("Add my plugins")
            add_btn.setStyleSheet("""
                QPushButton {
                    background-color: #AAAAAA;
                    color: black;
                    font-size: 14px;
                    border-radius: 20px;
                    padding: 8px 16px;
                }
                QPushButton:hover { background-color: #888888; }
            """)
            add_btn.clicked.connect(self.add_plugin_file)
            dlg_layout.addWidget(add_btn)

            note_label = QLabel("Before adding your .h file plugins, make sure they're written properly.")
            note_label.setFont(QFont("Arial", 10))
            note_label.setStyleSheet("color: gray;")
            dlg_layout.addWidget(note_label)

            close_btn = QPushButton("Close")
            close_btn.setStyleSheet("""
                QPushButton {
                    background-color: #6200EE;
                    color: white;
                    font-size: 14px;
                    border-radius: 12px;
                    padding: 6px 12px;
                }
                QPushButton:hover { background-color: #3700B3; }
            """)
            close_btn.clicked.connect(dlg.accept)
            dlg_layout.addWidget(close_btn)

            dlg.setLayout(dlg_layout)
            dlg.setFixedSize(420, 420)
            self.plugins_window = dlg

        self.update_plugins_list()
        self.plugins_window.exec()

    def update_plugins_list(self):
        include_dir = "./include"
        if not os.path.exists(include_dir):
            files = []
        else:
            files = [f for f in os.listdir(include_dir) if f.endswith(".h")]
        if self.plugins_list_widget is None:
            return
        self.plugins_list_widget.clear()
        if not files:
            item = QListWidgetItem("No plugins found.")
            item.setFlags(Qt.ItemIsEnabled)
            self.plugins_list_widget.addItem(item)
        else:
            for fn in sorted(files):
                item = QListWidgetItem(fn)
                self.plugins_list_widget.addItem(item)

    def add_plugin_file(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Select Plugin Header File", "", "Header Files (*.h)")
        if not file_path:
            return
        file_name = os.path.basename(file_path)
        target_path = os.path.join("./include", file_name)
        try:
            if not os.path.exists("./include"):
                os.makedirs("./include")
            if os.path.exists(target_path):
                resp = QMessageBox.question(self, "Overwrite?",
                    f"File {file_name} already exists in ./include. Overwrite?",
                    QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
                if resp != QMessageBox.Yes:
                    return
            shutil.copy2(file_path, target_path)
            QMessageBox.information(self, "Added", f"{file_name} was added to ./include")
            self.update_plugins_list()
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to add plugin:\n{str(e)}")

    def toggle_project_tree(self):
        self.update_project_tree()
        current_w = self.project_tree.width()
        target = 250 if current_w == 0 else 0
        anim = QPropertyAnimation(self.project_tree, b"maximumWidth")
        anim.setDuration(220)
        anim.setStartValue(current_w)
        anim.setEndValue(target)
        anim.setEasingCurve(QEasingCurve.InOutCubic)
        anim.start()
        self.anim_tree = anim

    def update_project_tree(self):
        self.project_tree.clear()
        root = QTreeWidgetItem(self.project_tree, ["My project >>"])
        root.setExpanded(True)

        for dirpath, dirnames, filenames in os.walk("."):
            parts = dirpath.split(os.sep)
            if any(p.startswith(".") and p != "." for p in parts):
                continue
            rel_dir = os.path.relpath(dirpath, ".")
            parent_item = root
            if rel_dir != ".":
                segments = rel_dir.split(os.sep)
                cur = root
                for seg in segments:
                    found = None
                    for i in range(cur.childCount()):
                        if cur.child(i).text(0) == seg:
                            found = cur.child(i)
                            break
                    if found is None:
                        found = QTreeWidgetItem(cur, [seg])
                        found.setExpanded(True)
                    cur = found
                parent_item = cur

            for f in sorted(filenames):
                if f.endswith(".c") or f.endswith(".h"):
                    rel_path = os.path.relpath(os.path.join(dirpath, f), ".")
                    item = QTreeWidgetItem(parent_item, [f])
                    item.setToolTip(0, os.path.abspath(rel_path))

    def open_file_from_tree(self, item, column):
        path = item.toolTip(0)
        if not path:
            return
        if os.path.isdir(path):
            return
        try:
            with open(path, "r", encoding="utf-8", errors="ignore") as fh:
                content = fh.read()
            self.text_edit.setPlainText(content)
            self.stacked_widget.setCurrentWidget(self.editor_page)
            self.editor_tab_button.setChecked(True)
            self.setWindowTitle(f"ZenithOS SDK - {os.path.basename(path)}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to open file:\n{e}")

    def switch_editor_tab(self, index):
        self.editor_stack.setCurrentIndex(index)
        self.editor_tab_button.setChecked(index == 0)
        self.api_tab_button.setChecked(index == 1)

    def create_new_project(self):
        if not os.path.exists("main.c"):
            with open("main.c", "w", encoding="utf-8") as f:
                f.write("/* new ZenithOS project */\n#include <stdio.h>\nint main(){ printf(\"hi\\n\"); return 0; }\n")
        self.stacked_widget.setCurrentWidget(self.editor_page)
        self.open_file_from_path("main.c")

    def open_file_from_path(self, relpath):
        abs_path = os.path.abspath(relpath)
        try:
            with open(abs_path, "r", encoding="utf-8", errors="ignore") as fh:
                self.text_edit.setPlainText(fh.read())
            self.setWindowTitle(f"ZenithOS SDK - {os.path.basename(relpath)}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to open {relpath}:\n{e}")

    # --- Build / Run flows using QProcess to keep GUI responsive ---
    
    def compile_project(self):
        missing = []
        for tool in ["gcc"]:
            if shutil.which(tool) is None:
                missing.append(tool)
        if missing:
            QMessageBox.warning(
                self,
                "Missing Tools",
                "The following tools are missing: " + ", ".join(missing) + "\nInstall them to compile projects."
            )
            return

        self.save_project(silent=True)
        self.clear_terminal()
        self.append_terminal("Starting compilation (gcc)...")

        # Ask flags
        flags, ok = QInputDialog.getText(self, "GCC Flags", "Enter GCC flags (leave empty for none):")
        if not ok:
            self.append_terminal("Compilation cancelled by user.")
            return

        cmd = ["gcc", "main.c", "-o", "app", "-I./include"]
        # auto add openssl flags if sapi.h detected
        if os.path.exists("./include/sapi.h"):
            self.append_terminal("Detected sapi.h -> adding OpenSSL flags automatically (-lcrypto -lssl)")
            cmd.extend(["-lcrypto", "-lssl"])

        if flags.strip():
            cmd.extend(flags.split())

        if self.build_process is not None:
            try:
                self.build_process.kill()
            except:
                pass
            self.build_process = None

        self.build_process = QProcess(self)
        self.build_process.setProgram(cmd[0])
        self.build_process.setArguments(cmd[1:])
        self.build_process.setProcessChannelMode(QProcess.MergedChannels)
        self.build_process.readyReadStandardOutput.connect(self.on_build_output)
        self.build_process.finished.connect(self.on_build_finished)
        self.build_process.start()

    def on_build_output(self):
        if not self.build_process:
            return
        data = self.build_process.readAllStandardOutput().data().decode(errors="ignore")
        if data:
            self.append_terminal(data.rstrip("\n"))

    def on_build_finished(self, exit_code, exit_status):
        if exit_code == 0:
            self.append_terminal("Build finished successfully -> ./app")
            QMessageBox.information(self, "Build", "Compilation successful! Binary: ./app")
        else:
            self.append_terminal(f"Build failed with exit code {exit_code}")
            QMessageBox.critical(self, "Build failed", "Compilation failed. Check terminal output.")
        self.build_process = None

    def run_project_in_terminal(self):
        if not os.path.exists("app"):
            QMessageBox.warning(self, "Run", "Binary ./app not found. Compiling first...")
            self.compile_project()
            return

        args_text, ok = QInputDialog.getText(self, "Run App", "Enter arguments (separated by spaces):")
        if not ok:
            return
        args = []
        if args_text.strip():
            args = args_text.split()

        self.clear_terminal()
        self.append_terminal("Starting app...")

        if self.run_process is not None:
            try:
                self.run_process.kill()
            except:
                pass
            self.run_process = None

        self.run_process = QProcess(self)
        program = "./app"
        self.run_process.setProgram(program)
        self.run_process.setArguments(args)
        self.run_process.setProcessChannelMode(QProcess.MergedChannels)
        self.run_process.readyReadStandardOutput.connect(self.on_run_output)
        self.run_process.finished.connect(self.on_run_finished)
        try:
            self.run_process.start()
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to start app:\n{e}")
            self.run_process = None

    def on_run_output(self):
        if not self.run_process:
            return
        data = self.run_process.readAllStandardOutput().data().decode(errors="ignore")
        if data:
            self.append_terminal(data.rstrip("\n"))

    def on_run_finished(self, exit_code, exit_status):
        self.append_terminal(f"App finished with exit code {exit_code}")
        self.run_process = None

    def compile_to_zapp(self):
        self.save_project(silent=True)
        self.clear_terminal()
        self.append_terminal("Compiling and packaging to .ZAPP...")
        # Ask for manifest info
        name, ok1 = QInputDialog.getText(self, "Manifest", "App name:")
        if not ok1: return
        version, ok2 = QInputDialog.getText(self, "Manifest", "Version:")
        if not ok2: return
        author, ok3 = QInputDialog.getText(self, "Manifest", "Author:")
        if not ok3: return
        description, ok4 = QInputDialog.getMultiLineText(self, "Manifest", "Description:")
        if not ok4: return
        with open("manifest.json", "w", encoding="utf-8") as f:
            f.write("{\n"
                    f"    \"name\": \"{name}\",\n"
                    f"    \"version\": \"{version}\",\n"
                    f"    \"author\": \"{author}\",\n"
                    f"    \"description\": \"{description}\",\n"
                    f"    \"binary\": \"app\"\n"
                    "}\n")
        self.append_terminal("Manifest created: manifest.json")

        gcc_path = shutil.which("gcc")
        if gcc_path:
            flags, ok = QInputDialog.getText(self, "GCC Flags", "Enter GCC flags (or leave empty for none):")
        if not ok:
            self.append_terminal("Compilation cancelled by user.")
            return
        cmd = ["gcc", "main.c", "-o", "app", "-I./include"]
        if flags.strip():
            cmd.extend(flags.split())
            proc = QProcess(self)
            proc.setProgram(cmd[0])
            proc.setArguments(cmd[1:])
            proc.setProcessChannelMode(QProcess.MergedChannels)
            proc.readyReadStandardOutput.connect(lambda: self.append_terminal(proc.readAllStandardOutput().data().decode(errors="ignore").rstrip("\n")))
            proc.start()
            proc.waitForFinished(-1)
            if proc.exitCode() != 0:
                self.append_terminal("Compilation failed, aborting .ZAPP packaging.")
                QMessageBox.critical(self, "Error", "Compilation failed. See terminal.")
                return
            else:
                self.append_terminal("Compilation OK.")
        else:
            self.append_terminal("gcc not found, skipping native compilation.")

        try:
            zapp_name = "project.zapp"
            tmp_zip = "project_tmp.zip"
            with zipfile.ZipFile(tmp_zip, "w", zipfile.ZIP_DEFLATED) as zf:
                if os.path.exists("main.c"):
                    zf.write("main.c", arcname="main.c")
                if os.path.exists("include"):
                    for root, _, files in os.walk("include"):
                        for f in files:
                            full = os.path.join(root, f)
                            arc = os.path.relpath(full, ".")
                            zf.write(full, arcname=arc)
                if os.path.exists("manifest.json"):
                    zf.write("manifest.json", arcname="manifest.json")
            if os.path.exists(zapp_name):
                os.remove(zapp_name)
            os.rename(tmp_zip, zapp_name)
            self.append_terminal(f"Packaged -> {zapp_name}")
            QMessageBox.information(self, "ZAPP", f"Created {zapp_name}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to create .ZAPP:\n{e}")

    def clear_terminal(self):
        self.terminal_output.clear()

    def append_terminal(self, text):
        self.terminal_output.appendPlainText(text)

    def clean_project(self):
        removed = []
        for f in ["app", "project.zapp", "manifest.json"]:
            if os.path.exists(f):
                try:
                    os.remove(f)
                    removed.append(f)
                except:
                    pass
        if removed:
            QMessageBox.information(self, "Clean", "Removed: " + ", ".join(removed))
        else:
            QMessageBox.information(self, "Clean", "Nothing to remove.")

    def back_to_main(self):
        if self.build_process is not None or self.run_process is not None:
            resp = QMessageBox.question(self, "Processes running",
                "There are running processes. Stop them and go back?", QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
            if resp != QMessageBox.Yes:
                return
            try:
                if self.build_process is not None:
                    self.build_process.kill()
                if self.run_process is not None:
                    self.run_process.kill()
            except:
                pass
        self.stacked_widget.setCurrentWidget(self.main_menu)

    def save_project(self, silent=False):
        try:
            with open("main.c", "w", encoding="utf-8") as f:
                f.write(self.text_edit.toPlainText())
            if not silent:
                msg = QMessageBox()
                msg.setIcon(QMessageBox.Information)
                msg.setText("Project saved successfully!")
                msg.setWindowTitle("Save")
                msg.setStyleSheet("""
                    QMessageBox { background-color: #4B0082; color: white; }
                    QPushButton { background-color: #6200EE; color: white; font-size: 16px; border: none; padding: 10px 20px; border-radius: 20px; }
                    QPushButton:hover { background-color: #3700B3; }
                """)
                msg.exec()
            else:
                self.append_terminal("Saved main.c")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to save project:\n{e}")

    def save_api_settings(self):
        target_version = self.target_api_combo.currentText()
        min_version = self.min_api_combo.currentText()
        QMessageBox.information(self, "Saved!", f"Target version: {target_version}\nMinimal version: {min_version}")

    def show_about(self):
        about_window = QDialog(self)
        about_window.setWindowTitle("About ZenithOS SDK")
        about_window.setStyleSheet("background-color: #4B0082; color: white; padding: 20px;")
        layout = QVBoxLayout()
        title = QLabel("ZenithOS SDK 13.0")
        title.setFont(QFont("Arial", 24))
        layout.addWidget(title)
        info = QLabel("Developed by ne5link\nSDK Version: 13.0G")
        info.setFont(QFont("Arial", 16))
        layout.addWidget(info)
        ok_button = QPushButton("OK")
        ok_button.setStyleSheet("QPushButton { background-color: #6200EE; color: white; font-size: 16px; border: none; padding: 10px 20px; border-radius: 20px; } QPushButton:hover { background-color: #3700B3; }")
        ok_button.clicked.connect(about_window.accept)
        layout.addWidget(ok_button)
        about_window.setLayout(layout)
        about_window.setFixedSize(400, 300)
        about_window.exec()

    def open_settings(self):
        settings_dialog = QDialog(self)
        settings_dialog.setWindowTitle("Settings")
        settings_dialog.setStyleSheet("background-color: #2C0032; color: white; padding: 20px;")
        layout = QVBoxLayout()
        title = QLabel("UI Theme")
        title.setFont(QFont("Poppins", 24))
        layout.addWidget(title)
        theme_label = QLabel("Color theme:")
        theme_label.setFont(QFont("Poppins", 16))
        layout.addWidget(theme_label)
        theme_combo = QComboBox()
        theme_combo.addItems(["Purple (default)", "Dark"])
        theme_combo.setStyleSheet("QComboBox { background-color: #2C0032; color: #E0E0E0; padding: 5px; font-size: 16px; border: none; border-radius: 10px; } QComboBox QAbstractItemView { background-color: #2C0032; color: #E0E0E0; }")
        theme_combo.currentIndexChanged.connect(self.change_theme)
        layout.addWidget(theme_combo)
        ok_button = QPushButton("OK")
        ok_button.setStyleSheet("QPushButton { background-color: #6200EE; color: white; font-size: 16px; border: none; padding: 10px 20px; border-radius: 20px; } QPushButton:hover { background-color: #3700B3; }")
        ok_button.clicked.connect(settings_dialog.accept)
        layout.addWidget(ok_button)
        settings_dialog.setLayout(layout)
        settings_dialog.exec()

    def change_theme(self, index):
        if index == 1:
            self.setStyleSheet("background-color: #121212; color: white;")
            self.current_theme = "dark"
            self.text_edit.setStyleSheet("font-family: 'Courier New'; font-size: 14px; background-color: #1E1E1E; color: #C0C0C0; padding: 5px;")
        else:
            self.setStyleSheet("background-color: #1F001F; color: white;")
            self.current_theme = "purple"
            self.text_edit.setStyleSheet("font-family: 'Courier New'; font-size: 14px; background-color: #2C0032; color: #E0E0E0; padding: 5px;")

    def show_search_bar(self):
        anim = QPropertyAnimation(self.search_bar, b"maximumHeight")
        anim.setDuration(200)
        anim.setStartValue(self.search_bar.maximumHeight())
        anim.setEndValue(40)
        anim.setEasingCurve(QEasingCurve.InOutCubic)
        anim.start()
        self.anim = anim
        self.search_input.setFocus()

    def highlight_search(self):
        extra_selections = []
        text = self.search_input.text()
        if text:
            pattern = re.escape(text)
            if self.case_checkbox.isChecked():
                regex = re.compile(pattern)
            else:
                regex = re.compile(pattern, re.IGNORECASE)
            for match in regex.finditer(self.text_edit.toPlainText()):
                selection = QTextEdit.ExtraSelection()
                selection.format.setBackground(QColor("yellow"))
                cursor = self.text_edit.textCursor()
                cursor.setPosition(match.start())
                cursor.setPosition(match.end(), QTextCursor.KeepAnchor)
                selection.cursor = cursor
                extra_selections.append(selection)
        self.text_edit.setExtraSelections(extra_selections)

if __name__ == "__main__":
    app = QApplication(sys.argv)

    splash_path = "StudioAssets/studio.png"
    if os.path.exists(splash_path):
        splash = QSplashScreen(QPixmap(splash_path))
        splash.setWindowFlags(Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint)
        splash.show()
        QTimer.singleShot(800, splash.close)
        QTimer.singleShot(800, lambda: ZenithOSApp().show())
    else:
        ZenithOSApp().show()

    sys.exit(app.exec())

