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

#include "myfs.h"
#include "myfs-info.h"






MyFS* MyFS::_instance = NULL;

MyFS* MyFS::Instance() {
    if(_instance == NULL) {
        _instance = new MyFS();
    }
    return _instance;
}

MyFS::MyFS() {
    this->logFile= stderr;
}

MyFS::~MyFS() {
    
}

int MyFS::fuseGetattr(const char *path, struct stat *statbuf) {
    LOGM();
    const char* remPath = remDirPath(path);
    bool filenameIsCorrect = isFilenameCorrect(path);

    if (!filenameIsCorrect && *remPath != '/' && !getSizeOfCharArray(remPath) > NAME_LENGTH) {
        statbuf->st_mode = S_IFDIR | 0555;
        statbuf->st_nlink = 2;
        return 0;
    } else if (filenameIsCorrect) {
        char buffer[NUM_ACCESS_RIGHT_BYTE];
        readBlock(getFilePosition(path) + START_ROOT_BLOCKS, buffer, NUM_ACCESS_RIGHT_BYTE, START_ACCESS_RIGHT_BYTE);
        statbuf->st_mode = charToInt(buffer);
        statbuf->st_nlink = 1;
        return 0;
    }
    return ENOENT;
}

int MyFS::fuseReadlink(const char *path, char *link, size_t size) {
    LOGM();
    return 0;
}

int MyFS::fuseMknod(const char *path, mode_t mode, dev_t dev) {
    LOGM();
    
    // TODO: Implement this!
    
    RETURN(0);
}

int MyFS::fuseMkdir(const char *path, mode_t mode) {
    LOGM();
    return 0;
}

int MyFS::fuseUnlink(const char *path) {
    LOGM();
    
    // TODO: Implement this!
    
    RETURN(0);
}

int MyFS::fuseRmdir(const char *path) {
    LOGM();
    return 0;
}

int MyFS::fuseSymlink(const char *path, const char *link) {
    LOGM();
    return 0;
}

int MyFS::fuseRename(const char *path, const char *newpath) {
    LOGM();
    return 0;
}

int MyFS::fuseLink(const char *path, const char *newpath) {
    LOGM();
    return 0;
}

int MyFS::fuseChmod(const char *path, mode_t mode) {
    LOGM();
    return 0;
}

int MyFS::fuseChown(const char *path, uid_t uid, gid_t gid) {
    LOGM();
    return 0;
}

int MyFS::fuseTruncate(const char *path, off_t newSize) {
    LOGM();
    return 0;
}

int MyFS::fuseUtime(const char *path, struct utimbuf *ubuf) {
    LOGM();
    return 0;
}

int MyFS::fuseOpen(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    
    if(isFileExisting(path)) {
        openFiles[getFilePosition(path)] = true;
        return 0;
    }
    
    return EEXIST;
}

int MyFS::fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();

    if (size > 0 && offset >= 0 && isFileExisting(path) && openFiles[getFilePosition(path)]) {
        int actuallFATPosition = this->getFilePosition(path);
        bool endOfFile = false;
        char buffer[BLOCK_SIZE];
        u_int64_t i = offset;

        if (getFileSize(actuallFATPosition) > 0) {
            for (; i >= BLOCK_SIZE && !endOfFile; i = BLOCK_SIZE) {
                actuallFATPosition == FAT[actuallFATPosition] ? endOfFile = true
                                                              : actuallFATPosition = FAT[actuallFATPosition];
                actuallFATPosition = FAT[actuallFATPosition];
            }
            for (u_int64_t j = 0; j < size && !endOfFile; j += BLOCK_SIZE) {
                readBlock(actuallFATPosition + START_DATA_BLOCKS, buffer, BLOCK_SIZE - i, i);
                transferBytes(buffer, j + BLOCK_SIZE < size ? BLOCK_SIZE - i : size - j, i, buf, j);
                actuallFATPosition == FAT[actuallFATPosition] ? endOfFile = true : actuallFATPosition = FAT[actuallFATPosition];

                i = 0;
            }
        }
    }
    RETURN(0);
}

int MyFS::fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this!
    
    RETURN(0);
}

int MyFS::fuseStatfs(const char *path, struct statvfs *statInfo) {
    LOGM();
    return 0;
}

int MyFS::fuseFlush(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    return 0;
}

int MyFS::fuseRelease(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();

    if(isFileExisting(path)) {
        openFiles[getFilePosition(path)] = false;
    }
    
    RETURN(0);
}

int MyFS::fuseFsync(const char *path, int datasync, struct fuse_file_info *fi) {
    LOGM();
    return 0;
}

int MyFS::fuseListxattr(const char *path, char *list, size_t size) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseRemovexattr(const char *path, const char *name) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseOpendir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    // TODO: Implement this!
    if(!isDirPathCorrect(path)) {
        RETURN(ENOTDIR);
    }
    isDirOpen = true;
    
    RETURN(0);
}

int MyFS::fuseReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    // TODO: Implement this!
    if(!isDirOpen) {
        RETURN(EPERM);
    }
    if(!isDirPathCorrect(path)) {
        RETURN(ENOTDIR);
    }

    for (int i = 0; i < NUM_ROOT_BLOCKS; i++) {
        if (FILENAME[i][0] != '\0') {
            filler(buf, FILENAME[i], NULL, 0);
        }

    }
    RETURN(0);
    
    // <<< My new code
}

int MyFS::fuseReleasedir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    // TODO: Implement this!
    if(!isDirPathCorrect(path)) {
        RETURN(ENOTDIR);
    }
    isDirOpen = false;


    RETURN(0);
}

int MyFS::fuseFsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseTruncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseCreate(const char *path, mode_t mode, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this!
    
    RETURN(0);
}

void MyFS::fuseDestroy() {
    LOGM();
}

void* MyFS::fuseInit(struct fuse_conn_info *conn) {
    // Open logfile
    this->logFile= fopen(((MyFsInfo *) fuse_get_context()->private_data)->logFile, "w+");
    if(this->logFile == NULL) {
        fprintf(stderr, "ERROR: Cannot open logfile %s\n", ((MyFsInfo *) fuse_get_context()->private_data)->logFile);
    } else {
        //    this->logFile= ((MyFsInfo *) fuse_get_context()->private_data)->logFile;
        
        // turn of logfile buffering
        setvbuf(this->logFile, NULL, _IOLBF, 0);
        
        LOG("Starting logging...\n");
        LOGM();
        
        // you can get the containfer file name here:
        LOGF("Container file name: %s", ((MyFsInfo *) fuse_get_context()->private_data)->contFile);
        
        // TODO: Implement your initialization methods here
        readStructures();
        // MYFsInfo ist ein Struct -> darauf wird ein pointer erzeugt um an den namen des containers zu kommen
    }
    
    RETURN(0);
}

#ifdef __APPLE__
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x) {
#else
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
#endif
    LOGM();
    RETURN(0);
}
    
#ifdef __APPLE__
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x) {
#else
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size) {
#endif
    LOGM();
    RETURN(0);
}

int MyFS::getFilePosition(const char *path) {
    if (isFilenameCorrect(path)) {
        return -1;
    }
    const char *StartOfFilename = remDirPath(path);
    for(int i = 0; i < NUM_ROOT_BLOCKS; i++) {
        for(int j = 0;  *(StartOfFilename + j) == (FILENAME[i][j]); j++) {
            if(*(StartOfFilename + j) == '\0') {
                return i;
            }
        }
    }
    return -1;
}

int MyFS::getFileSize(int position) {
    char buffer[NUM_FILE_SIZE_BYTE];
    this->readBlock(START_ROOT_BLOCKS + position, buffer, NUM_FILE_SIZE_BYTE, START_FILE_SIZE_BYTE);

    return (((int) buffer[0])<<8) + ((int) buffer[1]);
}

//TODO implement return value
/*
 * Saves the wanted block in the given Char Array.
 * @var blockNo Block you want to read from the device
 * @var *buf buffe, where you want to save the Data. Have to be >= size
 * @var size number of chars you want to get. Have to be given: size + offset <= BD_Block_Size
 * @var offset number of char you want to pass before reading. size + offset <= BD_Block-Size
 * @return
 */
int MyFS::readBlock(u_int32_t blockNo, char *buf, size_t size, off_t offset){
    u_int32_t i = 0;
    int64_t j = offset;
    char buffer[size];

    blockDevice->read(blockNo, buffer);

    if (size + offset <= BD_BLOCK_SIZE) {
        while (i < size) {
            buf[i] = buffer[j];
            i++;
            j++;
        }
    }
    return 0;
}

void MyFS::transferBytes(char *firstBuf, size_t size, off_t firstOff, char* secondBuf, off_t secondOff) {
    for(u_int64_t i = 0; i < size; i++) {
        *(secondBuf + secondOff + i) = *(firstBuf + firstOff + i);
    }
}


//TODO Was wenn Array größer 32 Bit Int?
/*
 * Return the Size of the Char-Array.
 * '\0' not included.
 * @var *buf pointer auf das char Array
 * @return size of the Array
 */
u_int32_t MyFS::getSizeOfCharArray(char *buf) {
    u_int32_t size = 0;
    while(buf[size] != '\0') {
        size++;
    }
    return size;
}

u_int32_t MyFS::getSizeOfCharArray(const char *buf) {
    u_int32_t size = 0;
    while(buf[size] != '\0') {
        size++;
    }
    return size;
}

const char* MyFS::remDirPath(const char *path) {
    bool removedPath = false;
    int NumOfPathChars = 0;
    if(*path == '/') {
       NumOfPathChars++;
    }
    while(!removedPath) {
        removedPath = true;
        if (*(path + NumOfPathChars) == '.' && *(path+ NumOfPathChars + 1) == '/') {
            NumOfPathChars += 2;
            removedPath = false;
        }
        if (*(path + NumOfPathChars) == '.' && *(path + NumOfPathChars + 1) == '.' &&
        *(path + NumOfPathChars + 2) == '/') {
            NumOfPathChars += 3;
            removedPath = false;
        }
    }
    return (path + NumOfPathChars);
}


bool MyFS::isFilenameCorrect(const char *path) {
    const char *StartOfFilename = remDirPath(path);
    if (getSizeOfCharArray(StartOfFilename) > NAME_LENGTH || (*StartOfFilename == '/' || *StartOfFilename == '\0' ||
    (*StartOfFilename == '.' && (*(StartOfFilename + 1) == '\0' || (*(StartOfFilename + 1) == '.' &&
    *(StartOfFilename + 2) == '\0' ))))) {
        return false;
    }
    return true;
}

bool MyFS::isDirPathCorrect(const char *path) {
    const char *dir = remDirPath(path);
    if (*(dir) == '\0' || ((*dir == '/' || *dir == '.') && *(dir + 1) == '\0') || (*(dir) == '.' && *(dir + 1) == '.' && *(dir + 2) == '\0')) {
        return true;
    }
    return false;
}

bool MyFS::isFileExisting(const char *path) {
    return getFilePosition(path) >= 0;
}

int MyFS::charToInt(char* chars) {
    int sum = 0;
    if (getSizeOfCharArray(chars) <= 4) {
        for (int i = 0; chars[i] != '\0'; i++) {
            sum += ((int) chars[i]) << (i * 8);
        }
    }
    return sum;

}

char* MyFS::intToChar(int number) {
    int sizeOfCharArray = 4;
    for (; sizeOfCharArray > 1 && number == ((number << ((5 - sizeOfCharArray) * 8) >> ((5 - sizeOfCharArray) * 8))); sizeOfCharArray--);
    char* chars= new char[sizeOfCharArray + 1];
    for (int i = 0; i < sizeOfCharArray; i++){
        chars[i] = (char) ((number >> (8 * i)) & 0b11111111);
    }
    chars[sizeOfCharArray] = '\0';
    return chars;
}


void MyFS::readStructures(){

}



