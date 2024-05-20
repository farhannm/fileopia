#ifndef STATVFS_H
#define STATVFS_H

#include <sys/types.h>

// If fsblkcnt_t is not defined, define it
#ifndef fsblkcnt_t
typedef unsigned long fsblkcnt_t;
#endif

// If fsfilcnt_t is not defined, define it
#ifndef fsfilcnt_t
typedef unsigned long fsfilcnt_t;
#endif

// Define struct statvfs if not defined
#ifndef _STATVFS_DEFINED
#define _STATVFS_DEFINED

struct statvfs {
    unsigned long  f_bsize;    /* file system block size */
    unsigned long  f_frsize;   /* fragment size */
    fsblkcnt_t     f_blocks;   /* size of fs in f_frsize units */
    fsblkcnt_t     f_bfree;    /* # free blocks */
    fsblkcnt_t     f_bavail;   /* # free blocks for non-root */
    fsfilcnt_t     f_files;    /* # inodes */
    fsfilcnt_t     f_ffree;    /* # free inodes */
    fsfilcnt_t     f_favail;   /* # free inodes for non-root */
    unsigned long  f_fsid;     /* file system ID */
    unsigned long  f_flag;     /* mount flags */
    unsigned long  f_namemax;  /* maximum filename length */
};

#endif // _STATVFS_DEFINED

// Function prototype for statvfs
int statvfs(const char* path, struct statvfs* buf);
int fstatvfs(int, struct statvfs*);

#endif // STATVFS_H
