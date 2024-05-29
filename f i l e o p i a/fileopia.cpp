#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>
#include <direct.h>
#include <sys/stat.h>
#include "statvfs.h"
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

// Fungsi untuk menghapus child dari parent node
void removeChild(Node* parent, Node* child) {
    if (parent->child == child) {
        parent->child = child->next;
    }
    else {
        Node* sibling = parent->child;
        while (sibling->next != NULL && sibling->next != child) {
            sibling = sibling->next;
        }
        if (sibling->next == child) {
            sibling->next = child->next;
        }
    }
    free(child);
}

void updateNodeName(Node* currentDir, const char* oldName, const char* newName) {
    Node* child = currentDir->child;
    while (child != NULL) {
        if (strcmp(child->name, oldName) == 0) {
            free(child->name);
            child->name = strdup(newName);
            return;
        }
        child = child->next;
    }
}

// Fungsi untuk menambahkan sibling node
void addSibling(Node* node, Node* sibling) {
    while (node->next != NULL)
        node = node->next;
    node->next = sibling;
}

// Fungsi untuk mencari node dengan nama tertentu pada level yang diberikan
Node* findNode(Node* currentDir, const char* name, int is_directory) {
    Node* child = currentDir->child;
    while (child != NULL) {
        if (child->is_directory == is_directory && strcmp(child->name, name) == 0) {
            return child;
        }
        child = child->next;
    }
    return NULL;
}

// Fungsi untuk mencari node dengan path lengkap
Node* findNodeByPath(Node* root, const char* path) {
    if (root == NULL || path == NULL || *path == '\0') {
        return NULL;
    }

    // If the path is root itself
    if (strcmp(root->name, path) == 0) {
        return root;
    }

    char* pathCopy = strdup(path);
    if (pathCopy == NULL) {
        return NULL;
    }

    // Normalize path by converting backslashes to slashes
    for (char* p = pathCopy; *p; ++p) {
        if (*p == '\\') {
            *p = '/';
        }
    }

    Node* currentNode = root;
    char* token = strtok(pathCopy, "/");
    while (token != NULL) {
        Node* child = currentNode->child;
        while (child != NULL) {
            if (strcmp(child->name, token) == 0) {
                currentNode = child;
                break;
            }
            child = child->next;
        }
        if (child == NULL) { // Directory not found
            free(pathCopy);
            return NULL;
        }
        token = strtok(NULL, "/");
    }

    free(pathCopy);
    return currentNode;
}

// Fungsi untuk mencetak struktur direktori secara rekursif
void printHierarchyStructure(Node* root, int depth) {
    if (root == NULL)
        return;

    // Print current node with proper indentation
    for (int i = 0; i < depth; i++)
        printf("    ");

    if (root->parent == NULL)
        printf("\x1b[1;37m %s\x1b[0m \x1b[1;33m --root\x1b[0m", root->name);
    else
        printf("\n      \x1b[1;35mL\x1b[0m %s", root->name);

    // Recursively print child nodes
    printHierarchyStructure(root->child, depth + 1);

    // Recursively print sibling nodes
    printHierarchyStructure(root->next, depth);
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


void dir(Node* currentDir) {
    struct dirent* de;
    DIR* dr = opendir(currentDir->name);

    if (dr == NULL) {
        printf("[ERROR] Could not open current directory\n");
        return;
    }

    int file_count = 0;
    int dir_count = 0;
    long long total_file_size = 0;

    printf("\nDirectory of %s\n\n", currentDir->name);

    while ((de = readdir(dr)) != NULL) {
        // Allocate enough space for path
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/%s", currentDir->name, de->d_name);

        char mod_time[20];
        getFileModifiedTime(path, mod_time);

        if (de->d_type == DT_DIR) {
            if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
                // Print directory entry
                printf("%-19s<DIR>       %-20s\n", mod_time, de->d_name);
                dir_count++;

                // Add child node
                Node* child = createNode(de->d_name, 1); // 1 for directory type
                addChild(currentDir, child);
            }
        }
        else {
            // Print file entry3
            struct stat st;
            stat(path, &st);
            printf("%-19s%-12lld%-20s\n", mod_time, (long long)st.st_size, de->d_name);
            total_file_size += st.st_size;
            file_count++;
        }
    }

    closedir(dr);

    // Print summary
    printf("\n            %d File(s)  %lld bytes\n", file_count, total_file_size);
    printf("\n            %d Dir(s)   [free space not calculated]\n", dir_count);

    // Print current dir hierarchy
    printf("\n\x1b[32m [!] Current directory hierarchy\x1b[0m\n");
    printHierarchyStructure(currentDir, 0);
    printf("\n");
}

// Fungsi untuk mengubah current directory
Node* cd(Node* currentDir, const char* name) {
    if (strcmp(name, "..") == 0) {
        if (currentDir->parent != NULL) {
            return currentDir->parent;
        }
        else {
            printf("[ERROR] You are already at the root directory.\n");
            return currentDir;
        }
    }

    Node* foundDir = findNode(currentDir, name, 1);
    if (foundDir != NULL) {
        return foundDir;
    }

    else {
        char newPath[PATH_MAX];
        snprintf(newPath, PATH_MAX, "%s\\%s", currentDir->name, name);
        if (opendir(newPath) != NULL) {
            Node* newNode = createNode(newPath, 1);
            newNode->parent = currentDir;
            return newNode;
        }
        else {
            printf("[ERROR] The system cannot find the path specified.\n");
            return currentDir;
        }
    }
}

// Fungsi untuk membuat direktori baru
void md(Node* currentDir, const char* name) {
    if (!isValidName(name)) {
        printf("[ERROR] Invalid directory name: %s\n", name);
        return;
    }
    if (directoryExists(currentDir, name)) {
        printf("[ERROR] Directory with name '%s' already exists\n", name);
        return;
    }

    Node* newDir = createNode(name, 1);
    addChild(currentDir, newDir);

    // Create the actual directory in the file system
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s\\%s", currentDir->name, name);
    if (mkdir(path) == -1) {
        perror("[ERROR] Could not create directory");
        // Remove the node from the structure if directory creation fails
        removeChild(currentDir, newDir);
    }
    else {
        printf("[SUCCESS] Directory '%s' created successfully at '%s'.\n", name, path);
    }
}

// Fungsi untuk menghapus direktori
void rd(Node* currentDir, const char* name) {
    Node* dirToRemove = findNode(currentDir, name, 1); // Find directory node
    if (dirToRemove != NULL) {
        if (dirToRemove->child != NULL) {
            printf("Cannot remove directory '%s': Directory is not empty.\n", name);
        }
        else {
            // Remove node from parent
            Node* parent = dirToRemove->parent;
            if (parent != NULL) {
                if (parent->child == dirToRemove) {
                    parent->child = dirToRemove->next;
                }
                else {
                    Node* temp = parent->child;
                    while (temp->next != dirToRemove)
                        temp = temp->next;
                    temp->next = dirToRemove->next;
                }
            }

            // Remove the actual directory in the file system
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", currentDir->name, name);
            if (rmdir(path) == -1) {
                perror("[ERROR] Could not remove directory");
            }
            else {
                free(dirToRemove->name);
                free(dirToRemove);
                printf("[SUCCESS] Directory '%s' removed successfully.\n", name);
            }
        }
    }
    else {
        printf("[ERROR] Directory '%s' not found.\n", name);
    }
}

void copyNode(Node* source, Node* destinationParent) {
    if (source == NULL || destinationParent == NULL || !destinationParent->is_directory) {
        return;
    }

    // Create a copy of the source node under the destination parent
    Node* newNode = createNode(source->name, source->is_directory);
    if (newNode == NULL) {
        printf("[ERROR] Failed to create node '%s'\n", source->name);
        return;
    }
    newNode->parent = destinationParent;

    // Link the new node to the parent's child list
    if (destinationParent->child == NULL) {
        destinationParent->child = newNode;
    }
    else {
        Node* sibling = destinationParent->child;
        while (sibling->next != NULL) {
            sibling = sibling->next;
        }
        sibling->next = newNode;
    }

    // Recursively copy all children of the source node
    Node* child = source->child;
    while (child != NULL) {
        copyNode(child, newNode);
        child = child->next;
    }
}


// Fungsi untuk menyalin file
void copy(Node* root, const char* sourcePath) {
    Node* sourceNode = findNodeByPath(root, sourcePath);
    if (sourceNode == NULL || !sourceNode->is_directory) {
        printf("[ERROR] Source directory '%s' not found or is not a directory\n", sourcePath);
        return;
    }

    char destPath[PATH_MAX];
    snprintf(destPath, PATH_MAX, "%s--copy", sourcePath);

    const char* parentPathEnd = strrchr(sourcePath, '/');
    if (!parentPathEnd) {
        parentPathEnd = strrchr(sourcePath, '\\');
    }

    Node* parentDir = NULL;
    if (parentPathEnd) {
        char parentPath[PATH_MAX];
        strncpy(parentPath, sourcePath, parentPathEnd - sourcePath);
        parentPath[parentPathEnd - sourcePath] = '\0';
        parentDir = findNodeByPath(root, parentPath);
    }
    else {
        parentDir = root;
    }

    if (parentDir == NULL || !parentDir->is_directory) {
        printf("[ERROR] Parent directory of '%s' not found or is not a directory\n", sourcePath);
        return;
    }

    Node* copiedNode = createNode(destPath + ((parentDir == root) ? 0 : (parentPathEnd - sourcePath) + 1), 1);
    copiedNode->parent = parentDir;

    if (parentDir->child == NULL) {
        parentDir->child = copiedNode;
    }
    else {
        Node* sibling = parentDir->child;
        while (sibling->next != NULL) {
            sibling = sibling->next;
        }
        sibling->next = copiedNode;
    }

    Node* child = sourceNode->child;
    while (child != NULL) {
        copyNode(child, copiedNode);
        child = child->next;
    }

    printf("[SUCCESS] Directory '%s' copied to '%s'\n", sourcePath, destPath);
}

// Fungsi untuk menghapus file
void del(const char* name) {
    if (remove(name) == 0)
        printf("File '%s' deleted successfully.\n", name);
    else
        printf("Unable to delete file '%s'.\n", name);
}

// Fungsi untuk mengubah nama file
void ren(Node* currentDir, const char* oldName, const char* newName) {
    if (!isValidName(newName)) {
        printf("[ERROR] Invalid new name: %s\n", newName);
        return;
    }

    // Construct full paths for old and new names
    char oldPath[PATH_MAX];
    snprintf(oldPath, PATH_MAX, "%s/%s", currentDir->name, oldName);

    char newPath[PATH_MAX];
    snprintf(newPath, PATH_MAX, "%s/%s", currentDir->name, newName);

    if (!fileExists(oldPath)) {
        printf("[ERROR] Directory '%s' does not exist.\n", oldName);
        return;
    }

    if (rename(oldPath, newPath) == 0) {
        printf("[SUCCESS] Directory '%s' renamed to '%s' successfully.\n", oldName, newName);
        updateNodeName(currentDir, oldName, newName);  // Update in the Node structure
    }
    else {
        printf("[ERROR] Unable to rename directory '%s'. Error: %s\n", oldName, strerror(errno));
    }
}


// FUngsi untuk clear screen
void clearScreen() {
    system("cls");
}

void headerUI() {
    printf("+--------------------------------------------------------------+\n");
    printf("|                 f i l e o p i a [Version 1.0]                |\n");
    printf("|      (c) Kelompok 3. On Development. All right reserved      |\n");
    printf("+--------------------------------------------------------------+\n");

    printf("\n[INFO]\x1b[1;32m show-command\x1b[0m for common commands\n");
}

// Fungsi untuk menampilkan informasi tentang commands
void showCommandsInfo() {
    printf("\n  [Available commands]\n");
    printf("    1. \x1b[36m`dir`\x1b[0m: Lists files and directories in the current directory.\n");
    printf("    2. \x1b[36m`cd`\x1b[0m: Changes the current directory.\n");
    printf("    2. \x1b[36m`cd ..`\x1b[0m: Exit the current directory.\n");
    printf("    3. \x1b[36m`md`\x1b[0m: Creates a new directory.\n");
    printf("    4. \x1b[36m`rd`\x1b[0m: Removes (deletes) a directory.\n");
    printf("    5. \x1b[36m`copy`\x1b[0m: Copies directory.\n");
    printf("    6. \x1b[36m`del`\x1b[0m: Deletes directory.\n");
    printf("    7. \x1b[36m`ren`\x1b[0m: Renames directory.\n");
    printf("    8. \x1b[36m`exit`\x1b[0m: Close the program.\n");
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


/*
 *
 * Validation
 *
 */

int isValidName(const char* name) {
    // Check if the name is empty
    if (name == NULL || strlen(name) == 0) {
        return 0;
    }
    // Check for invalid characters (for simplicity, let's assume we don't allow '/' and '\')
    const char* invalidChars = "<>:\"/\\|?*";
    if (strpbrk(name, invalidChars) != NULL) {
        return 0;
    }
    return 1;
}

int directoryExists(Node* currentDir, const char* name) {
    Node* child = currentDir->child;
    while (child != NULL) {
        if (strcmp(child->name, name) == 0 && child->is_directory) {
            return 1; // Directory with the same name exists
        }
        child = child->next;
    }
    return 0;
}

int fileExists(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}