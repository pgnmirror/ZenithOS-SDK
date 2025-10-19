#!/bin/bash
# ============================================
#  ZenithOS SDK Build Script (zMake)
#  Copyright (C) 2025 ne5link
# ============================================

set -e

START_TIME=$(date +%s)

echo -e "\e[95m==== ZenithOS SDK Build Session ====\e[0m"
echo -e "\e[96mStarting zMake...\e[0m"
echo ""

echo -e "[1/5] Checking build tools..."
sleep 0.5
missing=()

command -v gcc >/dev/null 2>&1 || missing+=("gcc")
command -v python3 >/dev/null 2>&1 || missing+=("python3")

if [ ${#missing[@]} -ne 0 ]; then
  echo -e "\e[91mMissing tools: ${missing[*]}\e[0m"
  echo -e "\e[91mBuild failed!!\e[0m"
  exit 1
fi
echo -e "\e[92mOK\e[0m"
echo ""

echo -e "[2/5] Creating Python .VENV..."
sleep 0.5
python3 -m venv .venv
source .venv/bin/activate
echo -e "\e[92mOK\e[0m"
echo ""

echo -e "[3/5] Download requirements: PySide6"
pip install --quiet PySide6 || { echo -e "\e[91mPySide6 install failed!\e[0m"; exit 1; }
echo -e "\e[92mOK\e[0m"
echo ""

echo -e "[4/5] Download requirements: PyInstaller"
pip install --quiet pyinstaller || { echo -e "\e[91mPyInstaller install failed!\e[0m"; exit 1; }
echo -e "\e[92mOK\e[0m"
echo ""

echo -e "[5/5] Finishing tools creating."
sleep 0.5
echo -e "\e[92mEnvironment ready!\e[0m"
echo ""
sleep 1

# =============================
#         BUILD STAGE
# =============================

echo -e "\e[95m==== Starting Build Stage ====\e[0m"
sleep 1

# ---------- BUILD 1 ----------
echo -e "[1/5] Build: frontend/studio.py"
pyinstaller --onefile frontend/studio.py --distpath frontend/dist --workpath frontend/build >/dev/null 2>&1 || { echo -e "\e[91mStudio build failed!\e[0m"; exit 1; }
echo -e "\e[92mOK\e[0m"
echo ""

# ---------- BUILD 2 ----------
echo -e "[2/5] Build: frontend/assembly.c"
gcc frontend/assembly.c -o frontend/assembly || { echo -e "\e[91mC build failed!\e[0m"; exit 1; }
echo -e "\e[92mOK\e[0m"
echo ""

# ---------- BUILD 3 ----------
echo -e "[3/5] Mkdir: build-out"
mkdir -p buildout
echo -e "\e[92mOK\e[0m"
echo ""

# ---------- BUILD 4 ----------
echo -e "[4/5] Install: studio → buildout/"
mv frontend/dist/studio buildout/ 2>/dev/null || { echo -e "\e[91mMove failed!\e[0m"; exit 1; }
echo -e "\e[92mOK\e[0m"
echo ""

# ---------- BUILD 5 ----------
echo -e "[5/5] Install: assembly → buildout/"
mv frontend/assembly buildout/ 2>/dev/null || { echo -e "\e[91mMove failed!\e[0m"; exit 1; }
echo -e "\e[92mOK\e[0m"
echo ""

END_TIME=$(date +%s)
BUILD_TIME=$((END_TIME - START_TIME))

echo -e "\e[92mBuild complete!!\e[0m [${BUILD_TIME}s]"
echo -e "\e[94mOutput directory: ./buildout\e[0m"
deactivate
