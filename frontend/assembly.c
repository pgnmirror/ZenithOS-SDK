#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void print_header() {
    printf("\033[1;35m");
    printf("=== ZenithOS Assembly ===\n");
    printf("\033[0m");
}

void compile_project() {
    char flags[256];
    printf("Any other flags for gcc? Type 'no' if none: ");
    fgets(flags, sizeof(flags), stdin);
    flags[strcspn(flags, "\n")] = 0;

    printf("\033[1;36mCompiling source into executable...\033[0m\n");

    char cmd[512];
    if (strcmp(flags, "no") == 0 || strlen(flags) == 0) {
        snprintf(cmd, sizeof(cmd), "gcc main.c -o app -I./include");
    } else {
        snprintf(cmd, sizeof(cmd), "gcc main.c -o app -I./include %s", flags);
    }

    int ret = system(cmd);
    if (ret == 0) {
        printf("\033[1;32mCompilation finished successfully! -> app\033[0m\n");
    } else {
        printf("\033[1;31mCompilation failed!\033[0m\n");
    }
}

void create_manifest() {
    char name[100], version[50], author[100], description[256];

    printf("\n\033[1;33m=== Zapp Manifest Wizard ===\033[0m\n");
    printf("App name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    printf("Version: ");
    fgets(version, sizeof(version), stdin);
    version[strcspn(version, "\n")] = 0;

    printf("Author: ");
    fgets(author, sizeof(author), stdin);
    author[strcspn(author, "\n")] = 0;

    printf("Description: ");
    fgets(description, sizeof(description), stdin);
    description[strcspn(description, "\n")] = 0;

    FILE *f = fopen("manifest.json", "w");
    if (!f) {
        perror("manifest.json");
        return;
    }

    fprintf(f,
        "{\n"
        "    \"name\": \"%s\",\n"
        "    \"version\": \"%s\",\n"
        "    \"author\": \"%s\",\n"
        "    \"description\": \"%s\",\n"
        "    \"binary\": \"app\"\n"
        "}\n",
        name, version, author, description
    );
    fclose(f);

    printf("\033[1;32mManifest created: manifest.json\033[0m\n");
}

void create_zapp() {
    printf("\033[1;36mPackaging into project.zapp...\033[0m\n");
    system("zip -q project.zapp app manifest.json");
    printf("\033[1;32mproject.zapp created successfully!\033[0m\n");
}

void showversion() {
    printf("ZenithOS Assembly Project v2025.10\n");
    printf("Defixx Version: 2.7\n");
}

void getct () {
  printf("Preparing.. If there any issues with downloading, make sure wget is installed.\n");
  usleep(2000);
  system("wget https://pgnmirror.github.io/25a06iz/cmdtools.zip");
}

void check_tools() {
    struct { const char* cmd; const char* name; } tools[] = {
        {"gcc", "GCC"},
        {"zip", "ZIP"},
        {"wget", "Wget"}
    };

    int missing = 0;
    for (int i = 0; i < 3; i++) {
        char check[128];
        snprintf(check, sizeof(check), "command -v %s > /dev/null 2>&1", tools[i].cmd);
        if (system(check) != 0) {
            printf("\033[1;31mError: %s not found! Please install it.\033[0m\n", tools[i].name);
            missing = 1;
        }
    }

    if (missing) {
        printf("\033[1;33mOne or more tools are missing. SDK might not work properly.\033[0m\n");
        sleep(2);
    } else {
        printf("\033[1;32mAll required tools found! Ready to go.\033[0m\n");
    }
}

int main() {
    print_header(); 
    check_tools();

    while (1) {
        char cmd[50];
        printf("\nzenith-asm> ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0;

        if (strcmp(cmd, "compile") == 0) {
            compile_project();
        } else if (strcmp(cmd, "czapp") == 0) {
            compile_project();
            create_manifest();
            create_zapp();
        } else if (strcmp(cmd, "exit") == 0) {
            printf("Bye!\n");
            break;
        } else if (strcmp(cmd, "sdkinfo") == 0) {
            showversion();
        } else if (strcmp(cmd, "getct") == 0) {
            getct();
        } else if (strcmp(cmd, "run") == 0) {
        char args[256];
        printf("Args for app (or enter for none): ");
        fgets(args, sizeof(args), stdin);
        args[strcspn(args, "\n")] = 0;
        char runcmd[512];
        if (strlen(args) == 0) snprintf(runcmd, sizeof(runcmd), "./app");
        else snprintf(runcmd, sizeof(runcmd), "./app %s", args);
        system(runcmd);

        } else if (strcmp(cmd, "help") == 0) {
            printf("Commands:\n");
            printf("  compile - build app (from main.c) with optional flags\n");
            printf("  sdkinfo - shows everything about sdk\n");
            printf("  czapp   - compile + manifest + package into project.zapp with optional flags\n");
            printf("  exit    - quit\n");
            printf("  help    - show this help\n");
            printf("  getct   - Get CMDTools\n");
            printf("  clean   - cleanup previous build files\n");
            printf("  run     - run the app");
            
         } else if (strcmp(cmd, "clean") == 0) {
    system("rm -f app manifest.json project.zapp");
    printf("\033[1;32mCleaned up previous build files.\033[0m\n");

        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }

    return 0;
}
