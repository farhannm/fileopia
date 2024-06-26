﻿#pragma warning(disable : 4996)
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

    headerUI();

    while (true) {
        // Cetak prompt dengan path lengkap dari current directory
        printf("\n%s>", currentDir->name);
        scanf("%s", command);

        if (strcmp(command, "dir") == 0) {
            dir(currentDir);
        }
        else if (strcmp(command, "cd") == 0) {
            scanf("%s", arg1);
            // Update current directory menggunakan fungsi cd yang telah diperbarui
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
            scanf("%s", arg1);
            copy(currentDir, arg1);
        }
        else if (strcmp(command, "del") == 0) {
            scanf("%s", arg1);
            del(arg1);
        }
        else if (strcmp(command, "ren") == 0) {
            scanf("%s %s", arg1, arg2);
            ren(currentDir, arg1, arg2);
        }
        else if (strcmp(command, "show-command") == 0) {
            showCommandsInfo();
        }
        else if (strcmp(command, "cls") == 0) {
            clearScreen();
            headerUI();
        }
        else if (strcmp(command, "exit") == 0) {
            break;
        }
        else {
            printf("[!] Invalid command. Please input a valid command.\n");
        }
    }

    freeDirectory(root);

    return 0;
}
