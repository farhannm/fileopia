#ifndef FILEOPIA_H
#define FILEOPIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Path untuk root directory
#define ROOT_PATH "C:\\"

// Struktur Node
typedef struct Node {
    char* name;             // Nama dari direktori atau file
    bool is_directory;       // Menandakan apakah node adalah direktori (1) atau file (0)
    struct Node* parent;    // Pointer ke parent node
    struct Node* child;     // Pointer ke child node (sub-direktori atau file)
    struct Node* next;      // Pointer ke next sibling node
} Node;

// Fungsi untuk membuat node baru
Node* createNode(const char* name, int is_directory);

// Fungsi untuk menambahkan child node ke parent node
void addChild(Node* parent, Node* child);

// Fungsi untuk menambahkan sibling node
void addSibling(Node* node, Node* sibling);

// Fungsi untuk mencari node dengan nama tertentu pada level yang diberikan
Node* findNode(Node* root, const char* name, int is_directory);

// Fungsi untuk mencetak struktur direktori secara rekursif
void printDirectoryStructure(Node* root, int depth);

// Fungsi untuk membebaskan memori dari seluruh struktur direktori
void freeDirectory(Node* root);

// Fungsi untuk menampilkan isi direktori
void dir(Node* currentDir);

// Fungsi untuk mengubah current directory
Node* cd(Node* currentDir, const char* name);

// Fungsi untuk membuat direktori baru
void md(Node* currentDir, const char* name);

// Fungsi untuk menghapus direktori
void rd(Node* currentDir, const char* name);

// Fungsi untuk menyalin file
void copy(const char* source, const char* destination);

// Fungsi untuk menghapus file
void del(const char* name);

// Fungsi untuk mengubah nama file
void ren(const char* oldName, const char* newName);

/*
*
* UI
*
*/

void clearScreen();
void showCommandsInfo();

#endif /* FILEOPIA_H */
