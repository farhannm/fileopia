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
void printDirectoryStructure(Node* root, int depth) {
    if (root == NULL)
        return;

    for (int i = 0; i < depth; i++)
        printf("    ");

    printf("%s%s\n", root->name, (root->is_directory) ? "/" : "");

    printDirectoryStructure(root->child, depth + 1);
    printDirectoryStructure(root->next, depth);
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
    printf("Directory of %s:\n\n", currentDir->name);

    struct dirent* de;
    DIR* dr = opendir(currentDir->name);

    if (dr == NULL) {
        printf("Could not open current directory\n");
        return;
    }

    while ((de = readdir(dr)) != NULL) {
        // Allocate enough space for path
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/%s", currentDir->name, de->d_name);

        char mod_time[20];
        getFileModifiedTime(path, mod_time);

        const char* type = (de->d_type == DT_DIR) ? "<DIR>" : "";
        printf("%-19s%-12s%-20s\n", mod_time, type, de->d_name);
    }

    closedir(dr);
}


// Fungsi untuk mengubah current directory
Node* cd(Node* currentDir, const char* name) {
    Node* foundDir = findNode(currentDir, name, 1);
    if (foundDir != NULL)
        return foundDir;
    else {
        // Check if the directory exists in the current directory
        char newPath[PATH_MAX];
        snprintf(newPath, PATH_MAX, "%s\\%s", currentDir->name, name);
        if (opendir(newPath) != NULL) {
            // Directory exists, change to it
            return createNode(newPath, 1);
        }
        else {
            printf("The system cannot find the path specified.\n");
            return currentDir;
        }
    }
}


// Fungsi untuk membuat direktori baru
void md(Node* currentDir, const char* name) {
    Node* newDir = createNode(name, 1);
    addChild(currentDir, newDir);
}

// Fungsi untuk menghapus direktori
void rd(Node* currentDir, const char* name) {
    Node* dirToRemove = findNode(currentDir, name, 1);
    if (dirToRemove != NULL) {
        if (dirToRemove->child != NULL)
            printf("Cannot remove directory '%s': Directory is not empty.\n", name);
        else {
            // Menghapus node dari parent
            Node* parent = dirToRemove->parent;
            if (parent != NULL) {
                if (parent->child == dirToRemove)
                    parent->child = dirToRemove->next;
                else {
                    Node* temp = parent->child;
                    while (temp->next != dirToRemove)
                        temp = temp->next;
                    temp->next = dirToRemove->next;
                }
            }
            free(dirToRemove->name);
            free(dirToRemove);
        }
    }
    else {
        printf("Directory '%s' not found.\n", name);
    }
}

// Fungsi untuk menyalin file
void copy(const char* source, const char* destination) {
    FILE* sourceFile, * destFile;
    char ch;
    sourceFile = fopen(source, "r");
    if (sourceFile == NULL) {
        printf("Unable to open source file\n");
        return;
    }
    destFile = fopen(destination, "w");
    if (destFile == NULL) {
        printf("Unable to create destination file\n");
        fclose(sourceFile);
        return;
    }
    while ((ch = fgetc(sourceFile)) != EOF) {
        fputc(ch, destFile);
    }
    fclose(sourceFile);
    fclose(destFile);
}

// Fungsi untuk menghapus file
void del(const char* name) {
    if (remove(name) == 0)
        printf("File '%s' deleted successfully.\n", name);
    else
        printf("Unable to delete file '%s'.\n", name);
}

// Fungsi untuk mengubah nama file
void ren(const char* oldName, const char* newName) {
    if (rename(oldName, newName) == 0)
        printf("File '%s' renamed to '%s' successfully.\n", oldName, newName);
    else
        printf("Unable to rename file '%s'.\n", oldName);
}

// FUngsi untuk clear screen
void clearScreen() {
    system("cls");
}

// Fungsi untuk menampilkan informasi tentang commands
void showCommandsInfo() {
    printf("+-----+-------------------------------------------+--------------------------------------------------------------+\n");
    printf("| %-4s | %-30s | %-70s |\n", "No", "Command", "Description");
    printf("+-----+-------------------------------------------+--------------------------------------------------------------+\n");
    printf("| %-4d | %-30s | %-70s |\n", 1, "dir", "Lists files and directories in the current directory.");
    printf("| %-4d | %-30s | %-70s |\n", 2, "cd", "Changes the current directory.");
    printf("| %-4d | %-30s | %-70s |\n", 3, "md", "Creates a new directory.");
    printf("| %-4d | %-30s | %-70s |\n", 4, "rd", "Removes (deletes) a directory.");
    printf("| %-4d | %-30s | %-70s |\n", 5, "copy", "Copies files.");
    printf("| %-4d | %-30s | %-70s |\n", 6, "del", "Deletes files.");
    printf("| %-4d | %-30s | %-70s |\n", 7, "ren", "Renames files.");
    printf("| %-4d | %-30s | %-70s |\n", 8, "type", "Displays the content of a text file.");
    printf("| %-4d | %-30s | %-70s |\n", 9, "edit", "Opens a simple text editor.");
    printf("| %-4d | %-30s | %-70s |\n", 10, "format", "Formats a disk or a diskette.");
    printf("| %-4d | %-30s | %-70s |\n", 11, "chkdsk", "Checks a disk for errors.");
    printf("| %-4d | %-30s | %-70s |\n", 12, "tree", "Displays a graphical representation of the directory structure.");
    printf("| %-4d | %-30s | %-70s |\n", 13, "attrib", "Displays or changes file attributes.");
    printf("| %-4d | %-30s | %-70s |\n", 14, "ping", "Sends ICMP Echo Request packets to test network connectivity.");
    printf("| %-4d | %-30s | %-70s |\n", 15, "ipconfig", "Displays IP configuration information.");
    printf("| %-4d | %-30s | %-70s |\n", 16, "net", "Manages network resources.");
    printf("| %-4d | %-30s | %-70s |\n", 17, "mode", "Configures system devices.");
    printf("| %-4d | %-30s | %-70s |\n", 18, "date", "Displays or sets the system date.");
    printf("| %-4d | %-30s | %-70s |\n", 19, "time", "Displays or sets the system time.");
    printf("| %-4d | %-30s | %-70s |\n", 20, "set", "Displays, sets, or removes environment variables.");
    printf("| %-4d | %-30s | %-70s |\n", 21, "tasklist", "Displays a list of currently running tasks.");
    printf("| %-4d | %-30s | %-70s |\n", 22, "taskkill", "Terminates one or more running tasks.");
    printf("| %-4d | %-30s | %-70s |\n", 23, "fc", "Compares two files or sets of files.");
    printf("| %-4d | %-30s | %-70s |\n", 24, "help", "Displays help information for commands.");
    printf("| %-4d | %-30s | %-70s |\n", 25, "exit", "Exits the Command Prompt.");
    printf("+-----+-------------------------------------------+--------------------------------------------------------------+\n");
}