#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>
#include "dirent.h"
#include <stdbool.h>
#include "fileopia.h"

// Fungsi untuk membuat node baru
Node* createNode(const char* name, int is_directory) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->name = strdup(name);
    newNode->is_directory = is_directory;
    newNode->parent = NULL;
    newNode->child = NULL;
    newNode->next = NULL;

    return newNode;
}

// Fungsi untuk menambahkan child node ke parent node
void addChild(Node* parent, Node* child) {
    child->parent = parent;
    if (parent->child == NULL)
        parent->child = child;
    else {
        Node* temp = parent->child;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = child;
    }
}

// Fungsi untuk menambahkan sibling node
void addSibling(Node* node, Node* sibling) {
    while (node->next != NULL)
        node = node->next;
    node->next = sibling;
}

// Fungsi untuk mencari node dengan nama tertentu pada level yang diberikan
Node* findNode(Node* root, const char* name, int is_directory) {
    Node* temp = root;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0 && temp->is_directory == is_directory)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

// Fungsi untuk mencetak struktur direktori secara rekursif
void printHierarchyStructure(Node* root, int depth) {
    if (root == NULL)
        return;

    // Print current node with proper indentation
    for (int i = 0; i < depth; i++)
        printf("    ");
    printf("%s%s\n", root->name, (root->is_directory) ? "/" : "");

    // Recursively print child nodes
    printHierarchyStructure(root->child, depth + 1);

    // Recursively print sibling nodes
    printHierarchyStructure(root->next, depth);
}

void printParentHierarchy(Node* currentDir, int depth) {
    if (currentDir == NULL)
        return;

    // Print current directory
    printf("%s", currentDir->name);
    if (currentDir->parent == NULL)
        printf(" (root)");

    // Print child directories
    if (currentDir->child != NULL) {
        printf("\n");
        Node* child = currentDir->child;
        while (child != NULL) {
            for (int i = 0; i < depth; i++)
                printf("    ");
            printf("L %s\n", child->name);
            printParentHierarchy(child, depth + 1);
            child = child->next;
        }
    }

    // Recursively print parent hierarchy
    if (currentDir->parent != NULL)
        printParentHierarchy(currentDir->parent, depth);
}




// Fungsi untuk membebaskan memori dari seluruh struktur direktori
void freeDirectory(Node* root) {
    if (root == NULL)
        return;

    freeDirectory(root->child);
    freeDirectory(root->next);
    free(root->name);
    free(root);
}

// Fungsi untuk menampilkan waktu modifikasi file
void getFileModifiedTime(const char* filename, char* mod_time) {
    struct stat attr;
    stat(filename, &attr);
    strftime(mod_time, 20, "%d/%m/%Y  %H:%M", localtime(&attr.st_mtime));
}

// Fungsi untuk menampilkan ukuran file
long getFileSize(const char* filename) {
    struct stat attr;
    stat(filename, &attr);
    return attr.st_size;
}

// Fungsi untuk menampilkan isi direktori
void dir(Node* currentDir) {
    struct dirent* de;
    DIR* dr = opendir(currentDir->name);

    if (dr == NULL) {
        printf("[ERROR] Could not open current directory\n");
        return;
    }

    printf("\n");

    while ((de = readdir(dr)) != NULL) {
        // Allocate enough space for path
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/%s", currentDir->name, de->d_name);

        char mod_time[20];
        getFileModifiedTime(path, mod_time);

        if (de->d_type == DT_DIR) {
            // Print directory entry
            const char* type = "<DIR>";
            printf("%-19s%-12s%-20s\n", mod_time, type, de->d_name);

            // Add child node
            Node* child = createNode(de->d_name, 1); // 1 for directory type
            addChild(currentDir, child);
        }
    }


    printHierarchyStructure(currentDir, 1);

    closedir(dr);
}




// Fungsi untuk mengubah current directory
Node* cd(Node* currentDir, const char* name) {
    Node* foundDir = findNode(currentDir, name, 1);
    if (foundDir != NULL)
        return foundDir;
    else {
        
        char newPath[PATH_MAX];
        snprintf(newPath, PATH_MAX, "%s\\%s", currentDir->name, name);
        if (opendir(newPath) != NULL) {
            
            return createNode(newPath, 1);
        }
        else {
            printf("[ERROR] The system cannot find the path specified.\n");
            return currentDir;
        }
    }
}


// FUngsi untuk clear screen
void clearScreen() {
    system("cls");
}

// Fungsi untuk menampilkan informasi tentang commands
void showCommandsInfo() {
    printf("List of commands:\n\n");
    printf("1. \x1b[36m`dir`\x1b[0m: Lists files and directories in the current directory.\n");
    printf("2. \x1b[36m`cd`\x1b[0m: Changes the current directory.\n");
    printf("3. \x1b[36m`md`\x1b[0m: Creates a new directory.\n");
    printf("4. \x1b[36m`rd`\x1b[0m: Removes (deletes) a directory.\n");
    printf("5. \x1b[36m`copy`\x1b[0m: Copies files.\n");
    printf("6. \x1b[36m`del`\x1b[0m: Deletes files.\n");
    printf("7. \x1b[36m`ren`\x1b[0m: Renames files.\n");
    printf("8. \x1b[36m`exit`\x1b[0m: Close the program.\n");
}


//void showCommandsInfo() {
//    printf("+-----+----------------------------------------------------------------------------------------------------------+\n");
//    printf("| %-4s | %-30s | %-70s |\n", "No", "Command", "Description");
//    printf("+-----+----------------------------------------------------------------------------------------------------------+\n");
//    printf("| %-4d | %-30s | %-70s |\n", 1, "dir", "Lists files and directories in the current directory.");
//    printf("| %-4d | %-30s | %-70s |\n", 2, "cd", "Changes the current directory.");
//    printf("| %-4d | %-30s | %-70s |\n", 3, "md", "Creates a new directory.");
//    printf("| %-4d | %-30s | %-70s |\n", 4, "rd", "Removes (deletes) a directory.");
//    printf("| %-4d | %-30s | %-70s |\n", 5, "copy", "Copies files.");
//    printf("| %-4d | %-30s | %-70s |\n", 6, "del", "Deletes files.");
//    printf("| %-4d | %-30s | %-70s |\n", 7, "ren", "Renames files.");
//    printf("| %-4d | %-30s | %-70s |\n", 8, "type", "Displays the content of a text file.");
//    printf("| %-4d | %-30s | %-70s |\n", 9, "edit", "Opens a simple text editor.");
//    printf("| %-4d | %-30s | %-70s |\n", 10, "format", "Formats a disk or a diskette.");
//    printf("| %-4d | %-30s | %-70s |\n", 11, "chkdsk", "Checks a disk for errors.");
//    printf("| %-4d | %-30s | %-70s |\n", 12, "tree", "Displays a graphical representation of the directory structure.");
//    printf("| %-4d | %-30s | %-70s |\n", 13, "attrib", "Displays or changes file attributes.");
//    printf("| %-4d | %-30s | %-70s |\n", 14, "ping", "Sends ICMP Echo Request packets to test network connectivity.");
//    printf("| %-4d | %-30s | %-70s |\n", 15, "ipconfig", "Displays IP configuration information.");
//    printf("| %-4d | %-30s | %-70s |\n", 16, "net", "Manages network resources.");
//    printf("| %-4d | %-30s | %-70s |\n", 17, "mode", "Configures system devices.");
//    printf("| %-4d | %-30s | %-70s |\n", 18, "date", "Displays or sets the system date.");
//    printf("| %-4d | %-30s | %-70s |\n", 19, "time", "Displays or sets the system time.");
//    printf("| %-4d | %-30s | %-70s |\n", 20, "set", "Displays, sets, or removes environment variables.");
//    printf("| %-4d | %-30s | %-70s |\n", 21, "tasklist", "Displays a list of currently running tasks.");
//    printf("| %-4d | %-30s | %-70s |\n", 22, "taskkill", "Terminates one or more running tasks.");
//    printf("| %-4d | %-30s | %-70s |\n", 23, "fc", "Compares two files or sets of files.");
//    printf("| %-4d | %-30s | %-70s |\n", 24, "help", "Displays help information for commands.");
//    printf("| %-4d | %-30s | %-70s |\n", 25, "exit", "Exits the Command Prompt.");
//    printf("+-----+-------------------------------------------+--------------------------------------------------------------+\n");
//}