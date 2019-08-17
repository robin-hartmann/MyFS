//
//  myfs.h
//  myfs
//
//  Created by Oliver Waldhorst on 02.08.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

#ifndef myfs_h
#define myfs_h

#include <fuse.h>
#include <cmath>
#include <errno.h>
#include <string.h>
#include "blockdevice.h"
#include "myfs-structs.h"
#include <unistd.h>
#include <sys/types.h>



class MyFS {
private:
    static MyFS *_instance;
    FILE *logFile;
    
public:
    static MyFS *Instance();
    
    // TODO: Add attributes of your file system here

    BlockDevice *blockDevice;

    bool DMAP[NUM_DMAP_BLOCKS * BLOCK_SIZE * 8] = {};
    char FILENAME[NUM_ROOT_BLOCKS][NAME_LENGTH + 1];
    int FAT[NUM_FAT_BLOCKS * NUM_ADRESS_PER_BLOCK] = {};
    fileCacheInfo fileCacheInfos[NUM_ROOT_BLOCKS];

    int numberOfFiles = 0;
    int numberOfUsedDATABLOCKS = 0;
    u_int64_t numberOfwrittenBytes = 0;
    
    MyFS();
    ~MyFS();
    
    // --- Methods called by FUSE ---
    // For Documentation see https://libfuse.github.io/doxygen/structfuse__operations.html
    int fuseGetattr(const char *path, struct stat *statbuf);
    int fuseReadlink(const char *path, char *link, size_t size);
    int fuseMknod(const char *path, mode_t mode, dev_t dev);
    int fuseMkdir(const char *path, mode_t mode);
    int fuseUnlink(const char *path);
    int fuseRmdir(const char *path);
    int fuseSymlink(const char *path, const char *link);
    int fuseRename(const char *path, const char *newpath);
    int fuseLink(const char *path, const char *newpath);
    int fuseChmod(const char *path, mode_t mode);
    int fuseChown(const char *path, uid_t uid, gid_t gid);
    int fuseTruncate(const char *path, off_t newSize);
    int fuseUtime(const char *path, struct utimbuf *ubuf);
    int fuseOpen(const char *path, struct fuse_file_info *fileInfo);
    int fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
    int fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
    int fuseStatfs(const char *path, struct statvfs *statInfo);
    int fuseFlush(const char *path, struct fuse_file_info *fileInfo);
    int fuseRelease(const char *path, struct fuse_file_info *fileInfo);
    int fuseFsync(const char *path, int datasync, struct fuse_file_info *fi);
#ifdef __APPLE__
    int fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x);
    int fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x);
#else
    int fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags);
    int fuseGetxattr(const char *path, const char *name, char *value, size_t size);
#endif
    void* fuseInit(struct fuse_conn_info *conn);
    int fuseListxattr(const char *path, char *list, size_t size);
    int fuseRemovexattr(const char *path, const char *name);
    int fuseOpendir(const char *path, struct fuse_file_info *fileInfo);
    int fuseReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo);
    int fuseReleasedir(const char *path, struct fuse_file_info *fileInfo);
    int fuseFsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo);
    int fuseTruncate(const char *path, off_t offset, struct fuse_file_info *fileInfo);
    int fuseCreate(const char *, mode_t, struct fuse_file_info *);
    void fuseDestroy();
    // TODO: Add methods of your file system here
    int getFilePosition(const char *path);
    u_int32_t getSizeOfCharArray(const char *buf);
    int readBlock(u_int32_t blockNo,char *buf, size_t size, off_t offset);
    void transferBytes(char *firstBuf, size_t size, off_t firstOff, char* secondBuf, off_t secondOff);
    void transferBytes(const char *firstBuf, size_t size, off_t firstOff, char* secondBuf, off_t secondOff);
    int getFileSize(int position);
    bool isFileExisting(const char *path);
    const char* remDirPath(const char *path);
    bool isFilenameCorrect(const char* path);
    bool isDirPathCorrect(const char *path);
    bool isDirPath(const char* path);
    int charToInt(char* chars, int numberOfChars);
    void intToChar(int number, char* buffer, int numberOfChars);
    void readDMap();
    void writeDMap();
    void setBitinChar(int position, bool value, char* buffer);
    void setDataBlocksUnused(u_int32_t position);
    void searchfreeBlocks(size_t size, u_int32_t* blockAdressBuffer);
    int readSectionByList(int filePosition, u_int32_t* list, char* buf, size_t size, off_t offset);
    int readSection(u_int32_t startblock, char* buffer, size_t size, off_t offset);
    void writeSection(u_int32_t startblock,const char* buffer, size_t size, off_t offset);
    void writeSectionByList(u_int32_t* list, const char* buf, size_t size, off_t offset);
    void writeFAT();
    void readFAT();
    int writeSBLOCK();
    int readSBlock();
    void readRoot();
    int sizeToBlocks(size_t size);
    u_int32_t getFirstPointer(int filePosition);
    void clearCharArray(char* buffer, size_t size);
    u_int32_t createFATEntrie(u_int32_t startposition, size_t oldFATSize, size_t newFATSize);
    void getFATList(u_int32_t* list, u_int32_t startposition, int numberOfBlocks, int startnumber);
    int writeRoot(u_int32_t position, char* buf);
    int createNewFile(const char *path, mode_t mode);
};

#endif /* myfs_h */
