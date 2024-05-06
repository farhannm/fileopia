#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dirent.h"
#include <stdbool.h>
#include "fileopia.h"

int main() {
    Node* root = createNode("C:\\Users\\hp", 1); // Membuat root directory
    Node* currentDir = root; // Current directory awal di root

    char command[100], arg1[100], arg2[100];

    printf("+--------------------------------------------------------------+\n");
    printf("|                 f i l e o p i a [Version 1.0]                |\n");
    printf("|      (c) Kelompok 3. On Development. All right reserved      |\n");
    printf("+--------------------------------------------------------------+\n");

    printf("\n[INFO] 'show-command' for common commands\n");

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
        else if (strcmp(command, "md") == 0) {
            scanf("%s", arg1);
            md(currentDir, arg1);
        }
        else if (strcmp(command, "rd") == 0) {
            scanf("%s", arg1);
            rd(currentDir, arg1);
        }
        else if (strcmp(command, "copy") == 0) {
            scanf("%s %s", arg1, arg2);
            copy(arg1, arg2);
        }
        else if (strcmp(command, "del") == 0) {
            scanf("%s", arg1);
            del(arg1);
        }
        else if (strcmp(command, "ren") == 0) {
            scanf("%s %s", arg1, arg2);
            ren(arg1, arg2);
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
            printf("Invalid command.\n");
        }
    }

    // Menghapus struktur direktori sebelum program berakhir
    freeDirectory(root);
    return 0;
}