//
//  myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 02.08.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

// For documentation of FUSE methods see https://libfuse.github.io/doxygen/structfuse__operations.html

#undef DEBUG

// TODO: Comment this to reduce debug messages
#define DEBUG
#define DEBUG_METHODS
#define DEBUG_RETURN_VALUES


#include "macros.h"

#include <cstdlib>
#include "myfs.h"
#include "myfs-info.h"


MyFS *MyFS::_instance = NULL;

MyFS *MyFS::Instance() {
    if (_instance == NULL) {
        _instance = new MyFS();
    }
    return _instance;
}

MyFS::MyFS() {
    this->logFile = stderr;
}

MyFS::~MyFS() {

}

int MyFS::fuseGetattr(const char *path, struct stat *statbuf) {
    LOGM();
    LOGF("Args: path: %s", path);

    // @todo doesn't always seem to work for root directory
    if (isDirPath(path)) {
        statbuf->st_uid = geteuid();
        LOGF("uid: %d", statbuf->st_uid);
        statbuf->st_gid = getegid();
        LOGF("gid: %d", statbuf->st_gid);

        statbuf->st_mode = S_IFDIR | 0755;
        LOGF("mode: %d", statbuf->st_mode);
        statbuf->st_nlink = 2;
        LOGF("nlink: %d", statbuf->st_nlink);
        statbuf->st_blksize = BLOCK_SIZE;
        LOGF("blksize: %d", statbuf->st_blksize);

        RETURN(0);
    } else if (isFilenameCorrect(path) && isFileExisting(path)) {
        char rootBLOCK[BLOCK_SIZE];
        readBlock((u_int32_t) getFilePosition(path) + START_ROOT_BLOCKS, rootBLOCK, BLOCK_SIZE, 0);

        statbuf->st_size = charToInt(rootBLOCK + START_FILE_SIZE_BYTE, NUM_FILE_SIZE_BYTE);
        LOGF("size: %d", statbuf->st_size);
        statbuf->st_uid = charToInt(rootBLOCK + START_USERID_BYTE, NUM_USERID_BYTE);
        LOGF("uid: %d", statbuf->st_uid);
        statbuf->st_gid = charToInt(rootBLOCK + START_GROUPID_BYTE, NUM_GROUPID_BYTE);
        LOGF("gid: %d", statbuf->st_gid);

#ifdef __APPLE__
        statbuf->st_atimespec.tv_sec = charToInt(rootBLOCK + START_ATIME_BYTE, NUM_TIMESTAMP_BYTE);
        LOGF("atimespec: %d", statbuf->st_atimespec.tv_sec);
        statbuf->st_mtimespec.tv_sec = charToInt(rootBLOCK + START_MTIME_BYTE, NUM_TIMESTAMP_BYTE);
        LOGF("mtimespec: %d", statbuf->st_mtimespec.tv_sec);
        statbuf->st_ctimespec.tv_sec = charToInt(rootBLOCK + START_CTIME_BYTE, NUM_TIMESTAMP_BYTE);
        LOGF("ctimespec: %d", statbuf->st_ctimespec.tv_sec);
#else
        statbuf->st_atim.tv_sec = charToInt(rootBLOCK + START_ATIME_BYTE, NUM_TIMESTAMP_BYTE);
        LOGF("atim: %d", statbuf->st_atim.tv_sec);
        statbuf->st_mtim.tv_sec = charToInt(rootBLOCK + START_MTIME_BYTE, NUM_TIMESTAMP_BYTE);
        LOGF("mtim: %d", statbuf->st_mtim.tv_sec);
        statbuf->st_ctim.tv_sec = charToInt(rootBLOCK + START_CTIME_BYTE, NUM_TIMESTAMP_BYTE);
        LOGF("ctim: %d", statbuf->st_ctim.tv_sec);
#endif

        statbuf->st_mode = charToInt(rootBLOCK + START_ACCESS_RIGHT_BYTE, NUM_ACCESS_RIGHT_BYTE);
        LOGF("mode: %d", statbuf->st_mode);
        statbuf->st_nlink = 1;
        LOGF("nlink: %d", statbuf->st_nlink);
        statbuf->st_blksize = BLOCK_SIZE;
        LOGF("blksize: %d", statbuf->st_blksize);

        RETURN (0);
    }

    RETURN(-ENOENT);
}

int MyFS::fuseReadlink(const char *path, char *link, size_t size) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseMknod(const char *path, mode_t mode, dev_t dev) {
    LOGM();
    LOGF("Args: path: %s, mode: %d", path, mode);
    int returnValue = createNewFile(path, mode);
    RETURN(returnValue);
}

int MyFS::fuseMkdir(const char *path, mode_t mode) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseUnlink(const char *path) {
    LOGM();
    LOGF("Args: path: %s", path);
    LOGF("Total Number of DATA Blocks: %d", NUM_DATA_BLOCKS);
    LOGF("Used Data Blocks: %d", numberOfUsedDATABLOCKS);
    LOGF("Free DATA Blocks: %d", NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS);
    LOGF("Number of written Bytes: %d", numberOfwrittenBytes);

    if (!isDirPathCorrect(path) || !isFilenameCorrect(path)) {
        RETURN(-ENOENT);
    } else if (!isFileExisting(path)) {
        RETURN(-EEXIST);
    }

    int filePosition = getFilePosition(path);
    if (getFileSize(filePosition) > 0) {
        setDataBlocksUnused(getFirstPointer(filePosition));
    }

    numberOfwrittenBytes -= getFileSize(filePosition);
    number_of_files--;
    transferBytes("\0", 1, 0, FILENAME[filePosition], 0);

    char rootBlock[BLOCK_SIZE];
    clearCharArray(rootBlock, BLOCK_SIZE);
    writeRoot(filePosition, rootBlock);

    fileCacheInfos[filePosition] = {
            .cachedBlockNo = 0,
            .cachedBlock = nullptr
    };

    LOGF("Used Data Blocks: %d", numberOfUsedDATABLOCKS);
    LOGF("Free DATA Blocks: %d", NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS);
    LOGF("Number of written Bytes: %d", numberOfwrittenBytes);

    RETURN(0);
}

int MyFS::fuseRmdir(const char *path) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseSymlink(const char *path, const char *link) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseRename(const char *path, const char *newpath) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseLink(const char *path, const char *newpath) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseChmod(const char *path, mode_t mode) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseChown(const char *path, uid_t uid, gid_t gid) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseTruncate(const char *path, off_t newSize) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseUtime(const char *path, struct utimbuf *ubuf) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseOpen(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Args: path: %s, size: %d, offset: %d", path, (int) size, (int) offset);

    if (isFileExisting(path) && size > 0) {
        size = size > getFileSize(getFilePosition(path)) ? getFileSize(getFilePosition(path)) : size;

        int numberOfBlocks = sizeToBlocks(size + offset);
        int filePosition = getFilePosition(path);
        char rootBlock[BLOCK_SIZE];

        clearCharArray(rootBlock, BLOCK_SIZE);
        readBlock((u_int32_t) filePosition + START_ROOT_BLOCKS, rootBlock, BLOCK_SIZE, 0);
        intToChar((int) time(nullptr), rootBlock + START_ATIME_BYTE, NUM_TIMESTAMP_BYTE);
        writeRoot(filePosition, rootBlock);

        u_int32_t list[numberOfBlocks];
        u_int32_t aktuallFATPosition = charToInt(rootBlock + START_POINTER_BYTE, NUM_POINTER_BYTE);
        getFATList(list, aktuallFATPosition, numberOfBlocks, START_DATA_BLOCKS);
        readSectionByList(filePosition, list, buf, size, offset);

        RETURN(size);
    }

    RETURN(-ENOENT);
}

int MyFS::fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Args: path: %s, size: %d, offset: %d", path, (int) size, (int) offset);

    if (!isFileExisting(path)) {
        RETURN(-EEXIST);
    }

    int filePosition = getFilePosition(path);
    char rootBlock[BLOCK_SIZE];
    clearCharArray(rootBlock, BLOCK_SIZE);
    readBlock((u_int32_t) START_ROOT_BLOCKS + filePosition, rootBlock, BLOCK_SIZE, 0);

    size_t oldFileSize = (size_t) charToInt((rootBlock + START_FILE_SIZE_BYTE), NUM_FILE_SIZE_BYTE);
    LOGF("OLDFILESIZE: %d", oldFileSize);

    if (sizeToBlocks(size + offset) - sizeToBlocks(oldFileSize) > (NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS)) {
        RETURN(-ENOSPC);
    }

    int firstPointer = 0;
    if (oldFileSize > 0) {
        firstPointer = getFirstPointer(filePosition);
    }
    if (size + offset > oldFileSize) {
        firstPointer = createFATEntrie(firstPointer, oldFileSize, size + offset);
    }
    u_int32_t list[sizeToBlocks(size + offset)];
    getFATList(list, firstPointer, -1, START_DATA_BLOCKS);
    writeSectionByList(list, buf, size, offset);
    getFATList(list, firstPointer, 1, 0);

    intToChar(list[0], rootBlock + START_POINTER_BYTE, NUM_POINTER_BYTE);

    if (size + offset > oldFileSize) {
        intToChar(size + offset, rootBlock + START_FILE_SIZE_BYTE, NUM_FILE_SIZE_BYTE);
    }

    intToChar((int) time(nullptr), rootBlock + START_MTIME_BYTE, NUM_TIMESTAMP_BYTE);
    transferBytes(rootBlock, NUM_TIMESTAMP_BYTE, START_MTIME_BYTE, rootBlock, START_CTIME_BYTE);

    writeRoot(filePosition, rootBlock);

    fileCacheInfos[filePosition] = {
            .cachedBlock = nullptr,
            .cachedBlockNo = 0
    };

    RETURN(size);
}

int MyFS::fuseStatfs(const char *path, struct statvfs *statInfo) {
    LOGM();
    LOGF("Args: path: %s", path);

    if (isDirPath(path)) {
        statInfo->f_blocks = NUM_DATA_BLOCKS;
        LOGF("block: %d", statInfo->f_blocks);
        statInfo->f_bfree = NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS;
        LOGF("bfree: %d", statInfo->f_bfree);
        statInfo->f_bavail = NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS;
        LOGF("bavail: %d", statInfo->f_bavail);
        statInfo->f_bsize = BLOCK_SIZE;
        LOGF("bsize: %lu", statInfo->f_bsize);
        statInfo->f_ffree = NUM_DIR_ENTRIES - number_of_files;
        LOGF("ffree: %d", statInfo->f_ffree);
        statInfo->f_files = number_of_files;
        LOGF("files: %d", statInfo->f_files);
        RETURN(0);
    }

    RETURN(-ENOTDIR);
}

int MyFS::fuseFlush(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseRelease(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Args: path: %s", path);

    if (isFileExisting(path)) {
        writeSBLOCK();
        writeDMap();
        writeFAT();
    }

    RETURN(0);
}

int MyFS::fuseFsync(const char *path, int datasync, struct fuse_file_info *fi) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseListxattr(const char *path, char *list, size_t size) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseRemovexattr(const char *path, const char *name) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseOpendir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int
MyFS::fuseReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Args: path: %s", path);

    if (!isDirPathCorrect(path)) {
        RETURN(-ENOTDIR);
    }

    if (isFileExisting(path)) {
        RETURN (0);
    }

    for (int i = 0; i < NUM_ROOT_BLOCKS; i++) {
        if (FILENAME[i][0] != '\0') {
            filler(buf, FILENAME[i], NULL, 0);
        }

    }

    RETURN(0);
}

int MyFS::fuseReleasedir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseFsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseTruncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

int MyFS::fuseCreate(const char *path, mode_t mode, struct fuse_file_info *fileInfo) {
    // Doesn't need to be implemented, because it is not used by the wrapper
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

void MyFS::fuseDestroy() {
    LOGM();
    blockDevice->close();
    delete blockDevice;

    for (int i = 0; i < NUM_ROOT_BLOCKS; i++) {
        delete fileCacheInfos[i].cachedBlock;
    }
}

void *MyFS::fuseInit(struct fuse_conn_info *conn) {
    // Open logfile
    this->logFile = fopen(((MyFsInfo *) fuse_get_context()->private_data)->logFile, "w+");

    if (this->logFile == NULL) {
        fprintf(stderr, "ERROR: Cannot open logfile %s\n", ((MyFsInfo *) fuse_get_context()->private_data)->logFile);
    } else {
        // turn off logfile buffering
        setvbuf(this->logFile, NULL, _IOLBF, 0);

        LOG("Starting logging...\n");
        LOGM();

        LOGF("Container file name: %s", ((MyFsInfo *) fuse_get_context()->private_data)->contFile);

        // TODO: Implement your initialization methods here
        BlockDevice *newblockDevice = new BlockDevice(BLOCK_SIZE);
        newblockDevice->open(((MyFsInfo *) fuse_get_context()->private_data)->contFile);
        this->blockDevice = newblockDevice;

        readSBlock();
        readDMap();
        readFAT();
        readRoot();
        LOG("END OF INIT()");
    }

    RETURN(0);
}

#ifdef __APPLE__

int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x) {
#else
    int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
#endif
    LOGM();
    LOG("not implemented");
    RETURN(0);
}


#ifdef __APPLE__

int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x) {
#else
    int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size) {
#endif
    LOGM();
    LOG("not implemented");
    RETURN(0);
}

/**
 * Returns the position of a file in the root directory.
 * Attention: Do not apply remDirPath() to the path before!
 * @param path: File path + file name
 * @return -1 if file does not exist. Otherwise the filePosition
 */
int MyFS::getFilePosition(const char *path) {
    if (!isFilenameCorrect(path)) {
        return -1;
    }
    const char *StartOfFilename = remDirPath(path);
    for (int i = 0; i < NUM_ROOT_BLOCKS; i++) {
        for (int j = 0; *(StartOfFilename + j) == (FILENAME[i][j]); j++) {
            if (*(StartOfFilename + j) == '\0') {
                return i;
            }
        }
    }
    return -1;
}

/**
 * Returns the size of a file in byte.
 * @param position: The position of the file in the root directory (getFilePosition())
 * @return size of the file.
 */
int MyFS::getFileSize(int position) {
    char buffer[NUM_FILE_SIZE_BYTE];
    this->readBlock(START_ROOT_BLOCKS + position, buffer, NUM_FILE_SIZE_BYTE, START_FILE_SIZE_BYTE);
    return charToInt(buffer, NUM_FILE_SIZE_BYTE);
}


/**
 * Stores the requested block in the given char array.
 * @param blockNo: Block you want to read from the device
 * @param buf: buffer, where you want to save the Data. Have to be >= size
 * @param size: number of chars you want to get. Have to be given: size + offset <= BD_Block_Size
 * @param offset: number of char you want to pass before reading. size + offset <= BD_Block-Size
 * @return
 */
int MyFS::readBlock(u_int32_t blockNo, char *buf, size_t size, off_t offset) {
    char buffer[BLOCK_SIZE];
    clearCharArray(buffer, BLOCK_SIZE);
    blockDevice->read(blockNo, buffer); //wird unötig gelesen wenn size = 0
    if (size + offset <= BLOCK_SIZE) {
        transferBytes(buffer, size, offset, buf, 0);
    }
    return 0;
}

/**
 * Writes bytes from the first buffer to the second buffer.
 * @param firstBuf: Buffer to be transferred.
 * @param size: Number of bytes to be transferred
 * @param firstOff: Offset, where to start reading from
 * @param secondBuf: Buffer into which the first buffer is written
 * @param secondOff: Offset, where to start writing from
 */
void MyFS::transferBytes(char *firstBuf, size_t size, off_t firstOff, char *secondBuf, off_t secondOff) {
    for (u_int64_t i = 0; i < size; i++) {
        *(secondBuf + secondOff + i) = *(firstBuf + firstOff + i);
    }
}

/**
 * Writes bytes from the first buffer to the second buffer.
 * @param firstBuf: Buffer to be transferred.
 * @param size: Number of bytes to be transferred
 * @param firstOff: Offset, where to start reading from
 * @param secondBuf: Buffer into which the first buffer is written
 * @param secondOff: Offset, where to start writing from
 */
void MyFS::transferBytes(const char *firstBuf, size_t size, off_t firstOff, char *secondBuf, off_t secondOff) {
    for (u_int64_t i = 0; i < size; i++) {
        *(secondBuf + secondOff + i) = *(firstBuf + firstOff + i);
    }
}

/**
 * Removes the file path from the file name.
 * @param path: File path / File name
 * @return filename
 */
const char *MyFS::remDirPath(const char *path) {
    int index = strlen(path) + 1;
    for (; index > 0 && *(path + index - 1) != '/'; index--);
    if (*(path + index) == '.' && *(path + index + 1) == '\0') {
        index += 1;
    } else if (*(path + index) == '.' && *(path + index + 1) == '.' &&
               *(path + index + 2) == '\0') {
        index += 2;
    }
    return (path + index);
}

/**
 * Checks if the filename is correct.
 * Attention: Do not run removeDirPath() on the file path before!
 * @param path: File path / File name
 * @return true, if file name + file path is correct
 */
bool MyFS::isFilenameCorrect(const char *path) {
    return (isDirPathCorrect(path) && !isDirPath(path) && strlen(remDirPath(path)) <= NAME_LENGTH);
}

/**
 * Checks if the file path is correct.
 * @param path: File path
 * @return true, if file path is correct
 */
bool MyFS::isDirPathCorrect(const char *path) {
    bool removedPath = false;
    int NumOfPathChars = 0;
    if (*path == '/') {
        NumOfPathChars++;
    }
    while (!removedPath) {
        removedPath = true;
        if (*(path + NumOfPathChars) == '.' && *(path + NumOfPathChars + 1) == '/') {
            NumOfPathChars += 2;
            removedPath = false;
        }
        if (*(path + NumOfPathChars) == '.' && *(path + NumOfPathChars + 1) == '.' &&
            *(path + NumOfPathChars + 2) == '/') {
            NumOfPathChars += 3;
            removedPath = false;
        }
    }
    if (*(path + NumOfPathChars) == '.' && *(path + NumOfPathChars + 1) == '\0') {
        NumOfPathChars += 1;
    } else if (*(path + NumOfPathChars) == '.' && *(path + NumOfPathChars + 1) == '.' &&
               *(path + NumOfPathChars + 2) == '\0') {
        NumOfPathChars += 2;
    }
    return (path + NumOfPathChars) == remDirPath(path);
}

/**
 * Checks if the path is a correct directory path and not a file.
 * @param path: File path
 * @return true, if it is a correct directory path
 */
bool MyFS::isDirPath(const char *path) {
    return (isDirPathCorrect(path) && *remDirPath(path) == '\0');
}

/**
 * Checks if the file already exists.
 * @param path: File name / File path
 * @return true, if the file exists
 */
bool MyFS::isFileExisting(const char *path) {
    return getFilePosition(path) >= 0;
}

/**
 * Converts a char array into an integer.
 * @param chars: Array with maximum 4 chars
 * @param numberOfChars: Number of chars the array has
 * @return intege based on the input; 0, if 0 >= numberOfChars > 4
 */
int MyFS::charToInt(char *chars, int numberOfChars) {
    u_int32_t sum = 0;
    if (numberOfChars <= 4) {
        for (int i = 0; i < numberOfChars; i++) {
            sum = sum | ((u_int8_t) chars[i]) << (i * 8);
        }
    }
    return (int) sum;

}

/**
 * Converts an integer to up to 4 (+ \0) chars.
 * @param number: Integer that is to be converted.
 * @param buffer: char array in which the chars should be written
 * @param numberOfChars: Number of chars to be written to the integer array.
 */
void MyFS::intToChar(int number, char *buffer, int numberOfChars) {
    //for (; sizeOfCharArray > 1 && number == ((number << ((5 - sizeOfCharArray) * 8) >> ((5 - sizeOfCharArray) * 8))); sizeOfCharArray--);
    if (numberOfChars > 0 && numberOfChars <= 4) {
        for (int i = 0; i < numberOfChars; i++) {
            buffer[i] = (char) ((number >> (8 * i)) & 0b11111111);
        }
    }
}

/**
 *  Reads dMap from blockdevice
 */
void MyFS::readDMap() {
    char buffer[NUM_DMAP_BLOCKS * BLOCK_SIZE];
    clearCharArray(buffer, NUM_DMAP_BLOCKS * BLOCK_SIZE);
    readSection(START_DMAP_BLOCKS, buffer, NUM_DMAP_BLOCKS * BLOCK_SIZE, 0);
    for (int i = 0; i < NUM_DATA_BLOCKS; i++) {
        int whichChar = (i - (i % 8)) / 8;
        int whichBitinChar = i % 8;
        DMAP[i] = (buffer[whichChar] >> whichBitinChar) & 0b1;
    }
}

/**
 *  Write dMap to blockdevice
 */

void MyFS::writeDMap() {
    LOGM();
    char buffer[NUM_DMAP_BLOCKS * BLOCK_SIZE];
    clearCharArray(buffer, NUM_DMAP_BLOCKS * BLOCK_SIZE);
    int whichChar = 0;
    int whichBitinChar = 0;

    for (int position = 0; position < NUM_DATA_BLOCKS; position++) {
        whichChar = (position - (position % 8)) / 8;
        whichBitinChar = position % 8;
        if (DMAP[position]) {
            buffer[whichChar] |= 1 << whichBitinChar;
        } else {
            buffer[whichChar] &= ~(1 << whichBitinChar);
        }

    }
    writeSection(START_DMAP_BLOCKS, buffer, NUM_DMAP_BLOCKS * BLOCK_SIZE, 0);
}

/**
 * Writes the given buffer to the block device.
 * @param startblock: First block written.
 * @param buffer: char array written to the block device.
 * @param size: Number of bytes to be written from the char array to the block device.
 * @param offset: Offset on the block device
 */
void MyFS::writeSection(u_int32_t startblock, const char *buffer, size_t size, off_t offset) {

    int numberOfBlocks = sizeToBlocks(size);
    u_int32_t list[numberOfBlocks];
    for (int i = 0; i < numberOfBlocks; i++) {
        list[i] = startblock + i;
    }
    writeSectionByList(list, buffer, size, offset);
}

/**
 * Writes the given buffer to the block device by List.
 * @param list: Blocks which should be written.
 * @param buf: char array written to the block device.
 * @param size: Number of bytes to be written from the char array to the block device.
 * @param offset: Offset on the block device
 */
void MyFS::writeSectionByList(u_int32_t *list, const char *buf, size_t size, off_t offset) {
    char buffer[BLOCK_SIZE];
    clearCharArray(buffer, BLOCK_SIZE);
    size_t numberOfWriteBytes = 0;
    size_t numberOfWrittenBytes = 0;
    for (int i = 0; size > 0; i++) {
        if (offset < BLOCK_SIZE) {
            clearCharArray(buffer, BLOCK_SIZE);
            numberOfWriteBytes = size > BLOCK_SIZE ? BLOCK_SIZE : size;
            if (numberOfWriteBytes < BLOCK_SIZE) {
                readBlock(list[i], buffer, offset, 0);
            }
            transferBytes(buf, numberOfWriteBytes, numberOfWrittenBytes, buffer, offset);
            numberOfWrittenBytes += numberOfWriteBytes;
            size -= numberOfWriteBytes;
            offset = 0;
            blockDevice->write(list[i], buffer);
        } else {
            offset -= BLOCK_SIZE;
        }
    }
}

/**
 * The blocks of a file are released in DMap and FAT.
 * @param position: Start block of a file.
 */
void MyFS::setDataBlocksUnused(u_int32_t position) {
    u_int32_t aktuallPosition = position;
    int i = 0;

    for (; aktuallPosition != FAT[aktuallPosition]; i++) {
        if (DMAP[aktuallPosition]) {
            DMAP[aktuallPosition] = false;
            aktuallPosition = FAT[aktuallPosition];
            numberOfUsedDATABLOCKS--;
        }
    }

    if (DMAP[aktuallPosition]) {
        DMAP[aktuallPosition] = false;
        numberOfUsedDATABLOCKS--;
    }
}

/**
 * Searches for free blocks and returns their numbers.
 * @param size: Number of blocks requested.
 * @param blockAdressBuffer: Array in which the numbers of the free blocks are stored.
 */
void MyFS::searchfreeBlocks(size_t size, u_int32_t *blockAdressBuffer) {
    int nrOfBlocks = sizeToBlocks(size);
    bool exit = false;
    int foundBlocks = 0;

    for (int i = 0; foundBlocks < nrOfBlocks && !exit; i++) {
        if (!DMAP[i]) {
            blockAdressBuffer[foundBlocks] = i;
            foundBlocks++;
        }
        if (i == NUM_DATA_BLOCKS - 1) {
            exit = true;
            blockAdressBuffer = nullptr;
        }
    }
}

/**
 * Reads the blocks specified in the passed list from the block device.
 * @param filePosition: Position of the root block of a file in the root directory.
 * @param list: List of requested blocks.
 * @param buf: Char Array in which the read data is stored.
 * @param size: Number of bytes written to the char array.
 * @param offset: Position from which the block device reads.
 */
int MyFS::readSectionByList(int filePosition, u_int32_t *list, char *buf, size_t size, off_t offset) {
    size_t writtenBytes = 0;
    size_t bytesRead = 0;
    fileCacheInfo *fileCacheInfo = filePosition > -1
            ? &fileCacheInfos[filePosition]
            : nullptr;

    for (int i = 0; size > 0; i++) {
        bytesRead = size + offset > BLOCK_SIZE ? (offset > BLOCK_SIZE ? 0 : BLOCK_SIZE - offset) : size;

        if (bytesRead > 0) {
            u_int32_t blockNo = list[i];

            if (fileCacheInfo != nullptr
                && fileCacheInfo->cachedBlock != nullptr
                && fileCacheInfo->cachedBlockNo == blockNo) {
                transferBytes(fileCacheInfo->cachedBlock, bytesRead, offset, buf + writtenBytes, 0);
                LOGF("Reading content of block %d of file %d from cache", blockNo, filePosition);
            } else {
                readBlock(blockNo, buf + writtenBytes, bytesRead, offset);

                if (fileCacheInfo != nullptr) {
                    if (fileCacheInfo->cachedBlock == nullptr) {
                        fileCacheInfo->cachedBlock = new char[BLOCK_SIZE];
                    }

                    transferBytes(buf + writtenBytes, bytesRead, offset, fileCacheInfo->cachedBlock, 0);
                    fileCacheInfo->cachedBlockNo = blockNo;
                    LOGF("Caching content of block %d of file %d", blockNo, filePosition);
                }
            }

            writtenBytes += bytesRead;
            size -= bytesRead;
        }

        offset > BLOCK_SIZE ? offset -= BLOCK_SIZE : offset = 0;
    }
}

/**
 * Fills the buffer with data from the StartBlock and the following blocks.
 * @param startblock: First block that is to be read.
 * @param buffer: Buffer into which the data is to be written.
 * @param size: Number of bytes to be written into the buffer.
 * @param offset: Position from which the block device is to be read.
 * @return
 */
int MyFS::readSection(u_int32_t startblock, char *buffer, size_t size, off_t offset) {
    int numberOfBlocks = sizeToBlocks(size + offset);
    u_int32_t list[numberOfBlocks];
    for (u_int32_t i = 0; i < numberOfBlocks; i++) {
        list[i] = startblock + i;
    }
    return readSectionByList(-1, list, buffer, size, offset);
}

/**
 * Writes the FAT from the memory to the block device.
 */
void MyFS::writeFAT() {
    LOGM();
    char buffer[NUM_FAT_BLOCKS * BLOCK_SIZE];
    clearCharArray(buffer, NUM_FAT_BLOCKS * BLOCK_SIZE);
    char numberbuffer[ADRESS_LENGTH_BYTE];
    clearCharArray(numberbuffer, ADRESS_LENGTH_BYTE);
    for (int i = 0; i < NUM_FAT_BLOCKS * NUM_ADRESS_PER_BLOCK; i++) {
        intToChar(FAT[i], numberbuffer, ADRESS_LENGTH_BYTE);
        transferBytes(numberbuffer, ADRESS_LENGTH_BYTE, 0, buffer, ADRESS_LENGTH_BYTE * i);
    }
    writeSection(START_FAT_BLOCKS, buffer, NUM_FAT_BLOCKS * BLOCK_SIZE, 0);

}

/**
 * Reads the FAT from the block device.
 */
void MyFS::readFAT() {
    char buffer[NUM_FAT_BLOCKS * BLOCK_SIZE];
    clearCharArray(buffer, NUM_FAT_BLOCKS * BLOCK_SIZE);
    char numberbuffer[ADRESS_LENGTH_BYTE];
    clearCharArray(numberbuffer, ADRESS_LENGTH_BYTE);
    readSection(START_FAT_BLOCKS, buffer, NUM_FAT_BLOCKS * BLOCK_SIZE, 0);
    for (int i = 0; i < NUM_FAT_BLOCKS * NUM_ADRESS_PER_BLOCK; i++) {
        transferBytes(buffer, ADRESS_LENGTH_BYTE, i * ADRESS_LENGTH_BYTE, numberbuffer, 0);
        FAT[i] = charToInt(numberbuffer, ADRESS_LENGTH_BYTE);
    }
}

/**
 * Writes the S-Block to the Blockdevice.
 */
int MyFS::writeSBLOCK() {
    LOGM();
    char SBLOCK[BLOCK_SIZE];
    char buffer[4];
    clearCharArray(SBLOCK, BLOCK_SIZE);
    clearCharArray(buffer, 4);
    clearCharArray(SBLOCK, BLOCK_SIZE);
    transferBytes(NAME_FILESYSTEM, NUM_NAME_FILESYSTEM_BYTE, 0, SBLOCK, START_FILENAME_BYTE);
    intToChar(number_of_files, buffer, NUM_RESERVED_ENTRIES_BYTE);
    transferBytes(buffer, NUM_RESERVED_ENTRIES_BYTE, 0, SBLOCK, START_RESERVED_ENTRIES_BYTE);
    intToChar(numberOfUsedDATABLOCKS, buffer, NUM_RESERVED_BLOCKS_BYTE);
    transferBytes(buffer, NUM_RESERVED_BLOCKS_BYTE, 0, SBLOCK, START_RESERVED_BLOCKS_BYTE);
    intToChar((int) numberOfwrittenBytes, buffer, NUM_RESERVED_DATA_BYTES_BYTE);
    transferBytes(buffer, NUM_RESERVED_DATA_BYTES_BYTE, 0, SBLOCK, START_RESERVED_DATA_BYTES_BYTE);

    blockDevice->write(START_SUPER_BLOCKS, SBLOCK);
    return 0;
}

/**
 * Reads the S-Block from the Blockdevice.
 */
int MyFS::readSBlock() {
    char SBLOCK[BLOCK_SIZE];
    clearCharArray(SBLOCK, BLOCK_SIZE);
    char fileNAME[NUM_FILENAME_BYTE];
    blockDevice->read(START_SUPER_BLOCKS, SBLOCK);
    transferBytes(SBLOCK, NUM_FILE_SIZE_BYTE, 0, fileNAME, 0);
    if (strcmp(fileNAME, NAME_FILESYSTEM) != 0) {
        return -EIO;
    }
    char numOfFiles[NUM_RESERVED_ENTRIES_BYTE];
    transferBytes(SBLOCK, NUM_RESERVED_ENTRIES_BYTE, NUM_FILE_SIZE_BYTE, numOfFiles, 0);
    number_of_files = charToInt(numOfFiles, NUM_RESERVED_ENTRIES_BYTE);

    char numOfDataBlocks[NUM_RESERVED_BLOCKS_BYTE];
    transferBytes(SBLOCK, NUM_RESERVED_BLOCKS_BYTE, NUM_FILE_SIZE_BYTE + NUM_RESERVED_ENTRIES_BYTE, numOfDataBlocks, 0);
    numberOfUsedDATABLOCKS = charToInt(numOfDataBlocks, NUM_RESERVED_BLOCKS_BYTE);

    char numOfwrittenBytes[NUM_RESERVED_DATA_BYTES_BYTE];
    transferBytes(SBLOCK, NUM_RESERVED_DATA_BYTES_BYTE,
                  NUM_FILE_SIZE_BYTE + NUM_RESERVED_ENTRIES_BYTE + NUM_RESERVED_BLOCKS_BYTE, numOfwrittenBytes, 0);
    numberOfwrittenBytes = (u_int64_t) charToInt(numOfwrittenBytes, NUM_RESERVED_DATA_BYTES_BYTE);


    return 0;
}

/**
 * Reads the filenames from the root directory on the block device.
 */
void MyFS::readRoot() {
    for (int i = 0; i < NUM_ROOT_BLOCKS; i++) {
        readBlock((u_int32_t) START_ROOT_BLOCKS + i, FILENAME[i], NUM_FILENAME_BYTE, 0);
    }
}

/**
 * Calculates the number of blocks required to store the passed size of data.
 * @param size: Number of bytes.
 */
int MyFS::sizeToBlocks(size_t size) {
    return (int) (((size - 1) - ((size - 1) % BLOCK_SIZE)) / BLOCK_SIZE) + 1;
}

/**
 * Reads the start block of the file from the root block.
 * @param filePosition: Position of the root block in the root directory.
 */
u_int32_t MyFS::getFirstPointer(int filePosition) {
    char pointer[NUM_POINTER_BYTE];
    readBlock(START_ROOT_BLOCKS + filePosition, pointer, NUM_POINTER_BYTE, START_POINTER_BYTE);
    return (u_int32_t) charToInt(pointer, NUM_POINTER_BYTE);
}

/**
 * Fills the transferred buffer with '\0'.
 * @param buffer: char Array Pointer.
 * @param size: Size of the char array.
 */
void MyFS::clearCharArray(char *buffer, size_t size) {
    for (int i = 0; i < size; i++) {
        buffer[i] = '\0';
    }
}

/**
 * Creates a FAT entry.
 * @param startposition: Number of the Start Block.
 * @param oldFileSize: Old Size of the file in bytes.
 * @param newFileSize: New Size of the file in bytes.
 */
u_int32_t MyFS::createFATEntrie(u_int32_t startposition, size_t oldFileSize, size_t newFileSize) {
    LOGM();
    LOGF("Startposition: %d; oldFileSize: %d; newFileSize: %d", startposition, oldFileSize, newFileSize);
    int oldNumberOfBlocks = sizeToBlocks(oldFileSize);
    int newNumberOfBlocks = sizeToBlocks(newFileSize);
    u_int32_t list[oldNumberOfBlocks];
    if (oldFileSize > 0) {
        getFATList(list, startposition, -1, 0);
    }

    if (oldNumberOfBlocks > newNumberOfBlocks) {
        if (newNumberOfBlocks > 0) {
            FAT[list[newNumberOfBlocks - 1]] = list[newNumberOfBlocks - 1];
        }
        setDataBlocksUnused(list[newNumberOfBlocks]);
        numberOfUsedDATABLOCKS += newNumberOfBlocks - oldNumberOfBlocks;
        numberOfwrittenBytes += newFileSize - oldFileSize;
        return list[0];
    } else if (newNumberOfBlocks > oldNumberOfBlocks) {
        u_int32_t freeBlocks[newNumberOfBlocks - oldNumberOfBlocks];
        searchfreeBlocks(newFileSize - oldFileSize, freeBlocks);
        if (freeBlocks != nullptr) {
            if (oldNumberOfBlocks > 0) {
                //LOGF("FAT[%d] = %d", list[oldNumberOfBlocks - 1], freeBlocks[0]);
                FAT[list[oldNumberOfBlocks - 1]] = freeBlocks[0];
            }
            int i = 0;
            for (; i < newNumberOfBlocks - oldNumberOfBlocks - 1; i++) {
                //LOGF("FAT[%d] = %d", freeBlocks[i], freeBlocks[i + 1]);
                FAT[freeBlocks[i]] = freeBlocks[i + 1];
                DMAP[freeBlocks[i]] = true;
            }
            //LOGF("FAT[%d] = %d", freeBlocks[i], freeBlocks[i]);
            FAT[freeBlocks[i]] = freeBlocks[i];
            DMAP[freeBlocks[i]] = true;
            numberOfUsedDATABLOCKS += newNumberOfBlocks - oldNumberOfBlocks;
            numberOfwrittenBytes += newFileSize - oldFileSize;
            return oldNumberOfBlocks > 0 ? list[0] : freeBlocks[0];
        }
    }
    numberOfwrittenBytes += newFileSize - oldFileSize;
    return startposition;
}

/**
 * Writes the numbers of the blocks in which the file is stored to the u_int32 array.
 * @param list: unisgned int32 array in which the numbers of the blocks are written.
 * @param startposition: Number of the start block.
 * @param numberOfBlocks: Number of blocks. With -1 all blocks of the file are read out.
 * @param startnumber: Offset = start of the data blocks.
 */
void MyFS::getFATList(u_int32_t *list, u_int32_t startposition, int numberOfBlocks, int startnumber) {
    LOG("getFatList() START");
    LOGF("startposition: %d, Startnumber: %d", startposition, startnumber);
    u_int32_t currentposition = startposition;
    int i = 0;

    for (; currentposition != FAT[currentposition] && (numberOfBlocks <= 0 || i < numberOfBlocks ); i++) {
        list[i] = currentposition + startnumber;
        currentposition = FAT[currentposition];
    }
    list[i] = currentposition + startnumber;
    LOG("END");
}

/**
 * Write in the root block.
 * @param position: Position of the root block.
 * @param buf: Buffer in which the updated root block is stored.
 */
int MyFS::writeRoot(u_int32_t position, char *buf) {
    return blockDevice->write(position + START_ROOT_BLOCKS, buf);
}
/**
 * Creates a new file and creates the root block for this file.
 * @param path: Path to the file being created.
 * @param mode: File permissions.
 */
int MyFS::createNewFile(const char *path, mode_t mode) {
    if (!isDirPathCorrect(path) || !isFilenameCorrect(path)) {
        return -ENOTDIR;
    } else if (isFileExisting(path)) {
        return -EEXIST;
    } else if (number_of_files >= 64) {
        return -ENOSPC;
    }

    char rootBlock[BLOCK_SIZE];
    clearCharArray(rootBlock, BLOCK_SIZE);
    u_int32_t freePosition = 0;
    const char *filename = remDirPath(path);
    size_t lentghOFFilename = strlen(filename);

    for (; FILENAME[freePosition][0] != '\0'; freePosition++);

    intToChar(S_IFREG | 0644, rootBlock + START_ACCESS_RIGHT_BYTE, NUM_ACCESS_RIGHT_BYTE);

    transferBytes(filename, lentghOFFilename + 1, 0, FILENAME[freePosition], 0);
    transferBytes(filename, lentghOFFilename + 1, 0, rootBlock, START_FILENAME_BYTE);
    number_of_files++;

    intToChar((int) time(nullptr), rootBlock + START_ATIME_BYTE, NUM_TIMESTAMP_BYTE);
    transferBytes(rootBlock, NUM_TIMESTAMP_BYTE, START_ATIME_BYTE, rootBlock, START_MTIME_BYTE);
    transferBytes(rootBlock, NUM_TIMESTAMP_BYTE, START_ATIME_BYTE, rootBlock, START_CTIME_BYTE);

    intToChar(geteuid(), rootBlock + START_USERID_BYTE, NUM_USERID_BYTE);
    intToChar(getegid(), rootBlock + START_GROUPID_BYTE, NUM_GROUPID_BYTE);

    int returnValue = writeRoot(freePosition, rootBlock);
    RETURN(returnValue);
}
