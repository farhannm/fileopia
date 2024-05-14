#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dirent.h"
#include <stdbool.h>
#include "fileopia.h"

int main() {
    Node* root = createNode(ROOT_PATH, 1); // Membuat root directory
    Node* currentDir = root; // Current directory awal di root

    char command[100], arg1[100], arg2[100];

    printf("+--------------------------------------------------------------+\n");
    printf("|                 f i l e o p i a [Version 1.0]                |\n");
    printf("|      (c) Kelompok 3. On Development. All right reserved      |\n");
    printf("+--------------------------------------------------------------+\n");

    printf("\n[INFO] \x1b[32m show-command \x1b[0m for common commands\n");

    while (true) {
        printf("\n%s>", currentDir->name);
        scanf("%s", command);

        if (strcmp(command, "dir") == 0) {
            dir(currentDir);
        }
        else if (strcmp(command, "cd") == 0) {
            scanf("%s", arg1);
            currentDir = cd(currentDir, arg1);
        }
        else if (strcmp(command, "show-command") == 0) {
            showCommandsInfo();
        }
        else if (strcmp(command, "cls") == 0) {
            clearScreen();
        }
        else if (strcmp(command, "exit") == 0) {
            break;
        }
        else {
            printf("[ERROR] Invalid command.\n");
        }
    }

    freeDirectory(root);

    return 0;
}