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
    LOGF("Path: %s", path);

    if(isDirPath(path)){
        statbuf->st_mode = S_IFDIR | 0755;
        statbuf->st_nlink = 2;
        statbuf->st_size = numberOfwrittenBytes;
        statbuf->st_blksize = BLOCK_SIZE;
        statbuf->st_blocks = numberOfUsedDATABLOCKS;

        // @todo uid und gid im container abspeichern
        statbuf->st_uid = geteuid();
        statbuf->st_gid = getegid();
        RETURN(0);
    } else if (isFilenameCorrect(path) && isFileExisting(path)) {
        char rootBLOCK[BLOCK_SIZE];
        readBlock((u_int32_t) getFilePosition(path) + START_ROOT_BLOCKS, rootBLOCK, BLOCK_SIZE, 0);

        statbuf->st_size = charToInt(rootBLOCK + START_FILE_SIZE_BYTE, NUM_FILE_SIZE_BYTE);
        statbuf->st_uid = charToInt(rootBLOCK + START_USERID_BYTE, NUM_USERID_BYTE);
        statbuf->st_gid = charToInt(rootBLOCK + START_GROUPID_BYTE, NUM_GROUPID_BYTE);

        #ifdef __APPLE__
        statbuf->st_atimespec.tv_sec = charToInt(rootBLOCK + START_FIRST_TIMESTAMP_BYTE, NUM_TIMESTAMP_BYTE);
        statbuf->st_mtimespec.tv_sec = charToInt(rootBLOCK + START_SECOND_TIMESTAMP_BYTE, NUM_TIMESTAMP_BYTE);
        statbuf->st_ctimespec.tv_sec = charToInt(rootBLOCK + START_THIRD_TIMESTAMP_BYTE, NUM_TIMESTAMP_BYTE);
        #else
        statbuf->st_atim.tv_sec = charToInt(rootBLOCK + START_FIRST_TIMESTAMP_BYTE, NUM_TIMESTAMP_BYTE);
        statbuf->st_mtim.tv_sec = charToInt(rootBLOCK + START_SECOND_TIMESTAMP_BYTE, NUM_TIMESTAMP_BYTE);
        statbuf->st_ctim.tv_sec = charToInt(rootBLOCK + START_THIRD_TIMESTAMP_BYTE, NUM_TIMESTAMP_BYTE);
        #endif

        statbuf->st_mode = S_IFREG | 0644;//charToInt(rootBlock + START_ACCESS_RIGHT_BYTE, NUM_ACCESS_RIGHT_BYTE);
        statbuf->st_nlink = 1;


        RETURN (0);
    }

    RETURN(-ENOENT);
}

int MyFS::fuseReadlink(const char *path, char *link, size_t size) {
    LOGM();
    return 0;
}

int MyFS::fuseMknod(const char *path, mode_t mode, dev_t dev) {
    LOGM();
    LOGF("Path: %s", path);
    if (!isDirPathCorrect(path) || !isFilenameCorrect(path)) {
        RETURN(-ENOTDIR);
    } else if (isFileExisting(path)) {
        RETURN(EEXIST);
    } else if (numberOfFiles >= 64) {
        RETURN(-ENOSPC);
    }
    u_int32_t freePosition = 0;
    const char* filename = remDirPath(path);
    size_t lentghOFFilename = getSizeOfCharArray(filename);
    char timestamp[NUM_TIMESTAMP_BYTE];
    clearCharArray(timestamp, NUM_TIMESTAMP_BYTE);

    intToChar((int) time(NULL), timestamp, NUM_TIMESTAMP_BYTE);

    for(;FILENAME[freePosition][0] != '\0'; freePosition++);

    transferBytes(filename, lentghOFFilename + 1, 0, FILENAME[freePosition], 0);
    numberOfFiles++;

    char userID[NUM_USERID_BYTE];
    intToChar(geteuid(),userID,NUM_USERID_BYTE);
    char groupID[NUM_GROUPID_BYTE];
    intToChar(getegid(),groupID,NUM_GROUPID_BYTE);

    LOGF("euid: %i", geteuid());
    LOGF("egid: %i", getegid());

    writeROOT(freePosition, filename, 0, userID, groupID, "\0", timestamp, timestamp, timestamp, 0);
    RETURN(0);
}

int MyFS::fuseMkdir(const char *path, mode_t mode) {
    LOGM();
    return 0;
}

int MyFS::fuseUnlink(const char *path) {
    LOGM();
    LOGF("Path: %s", path);
    LOGF("Total Number of DATA Blocks: %d", NUM_DATA_BLOCKS);
    LOGF("Used Data Blocks: %d", numberOfUsedDATABLOCKS);
    LOGF("Free DATA Blocks: %d",NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS);
    LOGF("Number of written Bytes: %d", numberOfwrittenBytes);
    if (!isDirPathCorrect(path) || !isFilenameCorrect(path)) {
        LOG("Fehler 1");
        RETURN(-ENOENT);
    } else if (!isFileExisting(path)) {
        LOG("Fehler 2");
        RETURN(-EEXIST);
    }

    int filePosition = getFilePosition(path);
    if(getFileSize(filePosition) > 0) {
        setDataBlocksUnused(getFirstPointer(filePosition));
    }

    numberOfwrittenBytes -= getFileSize(filePosition);
    //numberOfUsedDATABLOCKS -= sizeToBlocks(getFileSize(filePosition));
    numberOfFiles--;
    transferBytes("\0", 1, 0, FILENAME[filePosition], 0);
    writeROOT(filePosition, "\0", 0, "\0", "\0", "\0", "\0", "\0", "\0", 0);

    LOGF("Used Data Blocks: %d", numberOfUsedDATABLOCKS);
    LOGF("Free DATA Blocks: %d",NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS);
    LOGF("Number of written Bytes: %d", numberOfwrittenBytes);

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
    LOGF("PATH: %s; NEWPATH: %s", path, newpath);
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

//Fertig
int MyFS::fuseOpen(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Path: %s", path);
    if(isFileExisting(path)) {
        openFiles[getFilePosition(path)] = true;
        RETURN(0);
    }
    
    RETURN(-EEXIST);
}

int MyFS::fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Path: %s; Size: %d; Offset: %d", path, (int) size, (int) offset);

    if(isFileExisting(path) && openFiles[getFilePosition(path)] && size > 0) {
        size = size > getFileSize(getFilePosition(path)) ? getFileSize(getFilePosition(path)) : size;
        int numberOfBlocks = sizeToBlocks(size + offset);
        char buffer[NUM_FILE_SIZE_BYTE];
        readBlock(getFilePosition(path) + START_ROOT_BLOCKS, buffer, NUM_POINTER_BYTE, START_POINTER_BYTE);
        u_int32_t list[numberOfBlocks];
        u_int32_t aktuallFATPosition = charToInt(buffer, NUM_FILE_SIZE_BYTE);
        getFATList(list, aktuallFATPosition, numberOfBlocks, START_DATA_BLOCKS);
        readSectionByList(list, buf, size, offset);
        RETURN(size);
    }
    RETURN(-ENOENT);
}

int MyFS::fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Path: %s; Size: %d; Offset: %d", path, (int) size, (int) offset);
    LOGF("Total Number of DATA Blocks: %d", NUM_DATA_BLOCKS);
    LOGF("Used Data Blocks: %d", numberOfUsedDATABLOCKS);
    LOGF("Free DATA Blocks: %d",NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS);
    LOGF("Number of needed Blocks: %d", sizeToBlocks(size));
    LOGF("Number of written Bytes: %d", numberOfwrittenBytes);
    if (!isFileExisting(path)) {
        LOG("Fehler 1");
        RETURN(-EEXIST);
    }
    int filePosition = getFilePosition(path);
    char rootBlock[BLOCK_SIZE];
    readBlock((u_int32_t) START_ROOT_BLOCKS + filePosition, rootBlock, BLOCK_SIZE, 0);

    size_t oldFileSize = (size_t) charToInt((rootBlock + START_FILE_SIZE_BYTE), NUM_FILE_SIZE_BYTE);
    if (!openFiles[filePosition]) {
        LOG("Fehler 2");
        RETURN(-EBADF);
    } else if (sizeToBlocks(size + offset) - sizeToBlocks(oldFileSize) > (NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS)) { //abändern
        LOG("Fehler 3");
        RETURN(-ENOSPC);
    }

    const char* filename = remDirPath(path);

    char atime[NUM_TIMESTAMP_BYTE];
    transferBytes(rootBlock, NUM_TIMESTAMP_BYTE, START_FIRST_TIMESTAMP_BYTE, atime, 0);
    char mtime[NUM_TIMESTAMP_BYTE];
    intToChar(time(NULL), mtime, NUM_TIMESTAMP_BYTE);
    char ctime[NUM_TIMESTAMP_BYTE];
    intToChar(time(NULL), ctime, NUM_TIMESTAMP_BYTE);

    char userID[NUM_USERID_BYTE];
    intToChar(geteuid(),userID,NUM_USERID_BYTE);
    char groupID[NUM_GROUPID_BYTE];
    intToChar(getegid(),groupID,NUM_GROUPID_BYTE);

    //LOGF("USERNAME: %d", gete)

    int firstPointer = 0;
    if(oldFileSize > 0) {
        firstPointer = getFirstPointer(filePosition);
    }
    LOGF("WRITE readedFIRSTPointer = %d", firstPointer);
    firstPointer = createFATEntrie(firstPointer,oldFileSize, size + offset);
    u_int32_t list[sizeToBlocks(size + offset)];
    getFATList(list, firstPointer, -1, START_DATA_BLOCKS);
    writeSectionByList(list, buf, size, offset);
    getFATList(list, firstPointer, -1, 0);
    writeROOT(filePosition, filename, size + offset, userID, groupID, "\0", atime, mtime, ctime, list[0]);
    LOGF("Used Data Blocks: %d", numberOfUsedDATABLOCKS);
    LOGF("Free DATA Blocks: %d",NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS);
    LOGF("Number of written Bytes: %d", numberOfwrittenBytes);
    RETURN(size);
}

int MyFS::fuseStatfs(const char *path, struct statvfs *statInfo) {
    LOGM();
    LOGF("PATH: %s", path);
    if(isDirPath(path)){
        statInfo->f_blocks = NUM_DATA_BLOCKS;
        statInfo->f_bfree = NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS;
        statInfo->f_bavail = NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS;
        statInfo->f_bsize = BLOCK_SIZE;
        statInfo->f_ffree = NUM_DIR_ENTRIES - numberOfFiles;
        statInfo->f_files = numberOfFiles;
        RETURN(0);
    }
    RETURN(-ENOTDIR);
}

int MyFS::fuseFlush(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    return 0;
}

int MyFS::fuseRelease(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Path: %s", path);
    if(isFileExisting(path)) {
        openFiles[getFilePosition(path)] = false;
    }
    RETURN(0);
}

int MyFS::fuseFsync(const char *path, int datasync, struct fuse_file_info *fi) {
    LOGM();
    LOGF("PATH: %s", path);
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

//Fertig
int MyFS::fuseOpendir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Path: %s", path);
    if(!isDirPathCorrect(path)) {
        RETURN(ENOTDIR);
    }
    isDirOpen = true;
    
    RETURN(0);
}

int MyFS::fuseReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Path: %s", path);

    if(isFileExisting(path)) {
       // RETURN (0);
    }

    if(!isDirOpen) {
        RETURN(-EPERM);
    }
    if(!isDirPathCorrect(path)) {
        RETURN(-ENOTDIR);
    }

    if(isFileExisting(path)) {
        RETURN (0);
    }

    for (int i = 0; i < NUM_ROOT_BLOCKS; i++) {
        if (FILENAME[i][0] != '\0') {
            filler(buf, FILENAME[i], NULL, 0);
        }

    }
    RETURN(0);
    
    // <<< My new code
}

//Fertig
int MyFS::fuseReleasedir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    LOGF("Path: %s", path);
    if(!isDirPath(path)) {
        RETURN(-ENOTDIR);
    }
    isDirOpen = false;
    //writeSBLOCK();
    //writeDMap();
    //writeFAT();
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
    // Doesn't need to be implemented, because it is not used by the wrapper
    LOGM();
    RETURN(0);
}

void MyFS::fuseDestroy() {
    LOGM();
    //writeSBLOCK();
    //writeDMap();
    //writeFAT();
    blockDevice->close();
    delete blockDevice;
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
        BlockDevice* newblockDevice = new BlockDevice(BLOCK_SIZE);
        newblockDevice->open(((MyFsInfo *) fuse_get_context()->private_data)->contFile);
        this->blockDevice = newblockDevice;

        //readSBlock();
        //readDMap();
        //readFAT();
        //readRoot();
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

/**
 * Gibt die Positon einer Datei im Root-Verzeichnis zurück.
 * Achtung: Voher nicht remDirPath() auf den Pfad anwenden!
 * @param path Dateipfad + Dateiname
 * @return -1 wenn Datei nicht existiert. Ansonsten die DateiPosition
 */
int MyFS::getFilePosition(const char *path) {
    if (!isFilenameCorrect(path)) {
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

/**
 * Gibt die Dateigröße einer Datei zurück
 * @param position Die position der Datei im Root-Verzeichnis (getFilePosition())
 * @return größe der Datei
 */
int MyFS::getFileSize(int position) {
    char buffer[NUM_FILE_SIZE_BYTE];
    this->readBlock(START_ROOT_BLOCKS + position, buffer, NUM_FILE_SIZE_BYTE, START_FILE_SIZE_BYTE);
    return charToInt(buffer, NUM_FILE_SIZE_BYTE);
}

//TODO implement return value
/**
 * Saves the wanted block in the given Char Array.
 * @param blockNo Block you want to read from the device
 * @param buf buffer, where you want to save the Data. Have to be >= size
 * @param size number of chars you want to get. Have to be given: size + offset <= BD_Block_Size
 * @param offset number of char you want to pass before reading. size + offset <= BD_Block-Size
 * @return
 */
int MyFS::readBlock(u_int32_t blockNo, char *buf, size_t size, off_t offset){
    u_int32_t i = 0;
    int64_t j = offset;
    char buffer[BLOCK_SIZE];
    blockDevice->read(blockNo, buffer); //wird unötig gelesen wenn size = 0
    if (size + offset <= BLOCK_SIZE){
        transferBytes(buffer, size, offset, buf, 0);
    }
    return 0;
}

/**
 * Schreibt Bytes aus dem ersten Buffer, in den zweiten Buffer.
 * @param firstBuf Erster Buffer
 * @param size  Anzahl der Bytes, die übertragen werden soll
 * @param firstOff Offset, ab welcher Stelle angefangen werden soll zu lesen
 * @param secondBuf Zweiter Buffer
 * @param secondOff Offset, ab welcher Stelle angefangen werden soll zu schreiben
 */
void MyFS::transferBytes(char *firstBuf, size_t size, off_t firstOff, char* secondBuf, off_t secondOff) {
    for(u_int64_t i = 0; i < size; i++) {
        *(secondBuf + secondOff + i) = *(firstBuf + firstOff + i);
    }
}

/**
 * Schreibt Bytes aus dem ersten Buffer, in den zweiten Buffer.
 * @param firstBuf Erster Buffer
 * @param size  Anzahl der Bytes, die übertragen werden soll
 * @param firstOff Offset, ab welcher Stelle angefangen werden soll zu lesen
 * @param secondBuf Zweiter Buffer
 * @param secondOff Offset, ab welcher Stelle angefangen werden soll zu schreiben
 */
void MyFS::transferBytes(const char *firstBuf, size_t size, off_t firstOff, char* secondBuf, off_t secondOff) {
    for(u_int64_t i = 0; i < size; i++) {
        *(secondBuf + secondOff + i) = *(firstBuf + firstOff + i);
    }
}

/**
 * Return the Size of the Char-Array.
 * '\0' not included.
 * @param buf pointer auf das char Array
 * @return size of the Array
 */
u_int32_t MyFS::getSizeOfCharArray(const char *buf) {
    u_int32_t size = 0;
    while(buf[size] != '\0') {
        size++;
    }
    return size;
}

//Fetig 2.0
/**
 * Entfernt den Dateipfad vom Dateinamen.
 * @param path Dateipfad / Dateiname
 * @return Dateiname
 */
const char* MyFS::remDirPath(const char *path) {
    int laeufer = getSizeOfCharArray(path) + 1;
    for(; laeufer > 0 && *(path + laeufer - 1) != '/'; laeufer--);
    if (*(path +  laeufer) == '.' && *(path+  laeufer + 1) == '\0') {
         laeufer += 1;
    } else if (*(path +  laeufer) == '.' && *(path +  laeufer + 1) == '.' &&
               *(path +  laeufer + 2) == '\0') {
         laeufer += 2;
    }
    return (path + laeufer);
}

//Fertig 2.0
/**
 * Überprüft, ob der Filename korrekt ist.
 * Achtung: Auf den Dateipfad vorher nicht removeDirPath() ausführen!
 * @param path Dateipfad / Dateiname
 * @return true, wenn Dateiname + DateiPfad korrekt ist
 */
bool MyFS::isFilenameCorrect(const char *path) {
    return (isDirPathCorrect(path) && !isDirPath(path) && getSizeOfCharArray(remDirPath(path)) <= NAME_LENGTH);
}

//Fetig 2.0
/**
 * Überprüft ob der übergebene Dateipfad Correct ist.
 * Achtung: Der Dateipfad darf davor nicht durch remDirPath() gelaufen sein! Das verfälscht das Ergebnis.
 * @param path Dateipfad
 * @return true, wenn Dateipfad korrekt ist
 */
bool MyFS::isDirPathCorrect(const char *path) {
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
    if (*(path + NumOfPathChars) == '.' && *(path+ NumOfPathChars + 1) == '\0') {
        NumOfPathChars += 1;
    } else if (*(path + NumOfPathChars) == '.' && *(path + NumOfPathChars + 1) == '.' &&
        *(path + NumOfPathChars + 2) == '\0') {
        NumOfPathChars += 2;
    }
    return (path + NumOfPathChars) == remDirPath(path);
}

//Fetig 2.0
/**
 * Überprüft ob der Pfad ein korrekter DirPfad ist und keine Datei
 * @param path Dateipfad
 * @return true, wenn es ein korrekter Dirpfad ist
 */
bool MyFS::isDirPath(const char* path) {
    return (isDirPathCorrect(path) && *remDirPath(path) == '\0');
}

/**
 * Überprüft, Ob eine Datei beireits existiert.
 * @param path Dateiame / Dateipfad
 * @return true, wenn die Datei existiert
 */
bool MyFS::isFileExisting(const char *path) {
    return getFilePosition(path) >= 0;
}

/**
 * Wandelt ein Char-Array in einen Integer um.
 * @param chars char Array mit maximal 4 chars
 * @param numberOfChars Anzahl der Chars, die das array hat
 * @return intege based on the input; 0, wenn 0 >= numberOfChars > 4
 */
int MyFS::charToInt(char* chars, int numberOfChars) {
    u_int32_t sum = 0;
    if (numberOfChars <= 4) {
        for (int i = 0; i < numberOfChars; i++) {
            sum = sum| ((u_int8_t) chars[i]) << (i * 8);
        }
    }
    return (int) sum;

}

/**
 * Wandelt einen Integer in bis zu 4 (+ \0) chars um.
 * @param number Integer, der umgewandelt werden soll
 * @param buffer char-Array in den die chars geschrieben werden sollen
 * @param numberOfChars Anzahl der chars , die in das IntegerArray geschrieben werden sollen.
 */
void MyFS::intToChar(int number, char* buffer, int numberOfChars) {
    //for (; sizeOfCharArray > 1 && number == ((number << ((5 - sizeOfCharArray) * 8) >> ((5 - sizeOfCharArray) * 8))); sizeOfCharArray--);
    if (numberOfChars > 0 && numberOfChars <= 4) {
        for (int i = 0; i < numberOfChars; i++) {
            buffer[i] = (char) ((number >> (8 * i)) & 0b11111111);
        }
    }
}

void MyFS::readDMap(){
            char buffer[NUM_DMAP_BLOCKS*BLOCK_SIZE];
           readSection(START_DMAP_BLOCKS, buffer, NUM_DMAP_BLOCKS*BLOCK_SIZE,0);
    for (int i = 0; i<NUM_DATA_BLOCKS; i++) {
        int whichChar = (i - (i % 8))/8 ;
        int whichBitinChar = i % 8;
        DMAP[i] = (buffer[whichChar] >> whichBitinChar) & 1;
    }

}

void MyFS::writeDMap(){
    LOGM();
    char buffer[NUM_DMAP_BLOCKS*BLOCK_SIZE];
    int whichChar = 0;
    int whichBitinChar = 0;


    for(int position=0; position<NUM_DATA_BLOCKS ; position++) {
         whichChar = (position - (position % 8))/8 ;
         whichBitinChar = position % 8;
        if (DMAP[position]){
            buffer[whichChar] |= 1 << whichBitinChar;
        }else {
            buffer[whichChar] &= ~(1 << whichBitinChar);
        }

    }
    writeSection(START_DMAP_BLOCKS, buffer, NUM_DMAP_BLOCKS * BLOCK_SIZE, 0);
}

/**
 * Buffer kann auf blockdevice geschrieben werden.
 * ---> noch nicht fertig : ToDo
 * @param startblock
 * @param buffer
 * @param size
 * @param offset
 */
void MyFS::writeSection(u_int32_t startblock, const char* buffer, size_t size, off_t offset){

    int numberOfBlocks = sizeToBlocks(size);
    u_int32_t list[numberOfBlocks];
    for (int i = 0; i < numberOfBlocks; i++) {
        list[i] = startblock + i;
    }
    writeSectionByList(list, buffer, size, offset);
}

void MyFS::writeSectionByList(u_int32_t* list, const char* buf, size_t size, off_t offset) {
    char buffer[BLOCK_SIZE];
    size_t numberOfWriteBytes = 0;
    size_t numberOfWrittenBytes = 0;
    for(int i = 0; size > 0; i++) {
        if(offset < BLOCK_SIZE) {
            clearCharArray(buffer, BLOCK_SIZE);
            numberOfWriteBytes = size > BLOCK_SIZE ? BLOCK_SIZE : size;
            readBlock(list[i], buffer, offset, 0);
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

//fehler. letzter Block wird nicht gelöscht
void MyFS::setDataBlocksUnused(u_int32_t position){ // auf basis der position wird das FAT durchgesucht nach des restlichen Blöcken und diese werden auf unused gestzt. -> Rekursiv lösen
    u_int32_t aktuallPosition = position;
    int i = 0;

    for (; aktuallPosition != FAT[aktuallPosition]; i++) {
        if (DMAP[aktuallPosition]) {
            DMAP[aktuallPosition] = false;
            aktuallPosition = FAT[aktuallPosition];
            numberOfUsedDATABLOCKS--;
        } else {
            LOG("FEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEHler");
        }
    }
    if (DMAP[aktuallPosition]) {
        DMAP[aktuallPosition] = false;
        numberOfUsedDATABLOCKS--;
    } else {
        LOG("FEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEHler");
    }
}



void MyFS::searchfreeBlocks(size_t size, u_int32_t* blockAdressBuffer){
    int nrOfBlocks = sizeToBlocks(size);
    bool exit = false;
    int foundBlocks = 0;

    for (int i = 0; foundBlocks < nrOfBlocks && !exit; i++) {
        if (!DMAP[i]) {
            blockAdressBuffer[foundBlocks] = i;
            //LOGF("blockAdressBuffer[%d] = %d", foundBlocks, i);
            foundBlocks++;
        }
        if(i == NUM_DATA_BLOCKS - 1) {
            LOG("KEINE FREIen Blöcke");
            exit = true;
            blockAdressBuffer = nullptr;
        }
    }
}


int MyFS::readSectionByList(u_int32_t* list, char* buf, size_t size, off_t offset) {
   size_t writtenBytes = 0;
   size_t readBytes = 0;
    for(int i = 0; size > 0; i++) {
        readBytes = size + offset > BLOCK_SIZE ? (offset > BLOCK_SIZE ? 0 : BLOCK_SIZE - offset ): size;
        if(readBytes > 0) {
            //LOGF("Section: READ BLOCK %d", list[i]);
            readBlock(list[i], buf + writtenBytes, readBytes, offset);
            writtenBytes += readBytes;
            size -= readBytes;
        }
        offset > BLOCK_SIZE ? offset -= BLOCK_SIZE : offset = 0;
    }
}

/**
 * Füllt den Buffer mit Daten aus dem StartBlock und den nachfolgenden Blöcken
 * @param startblock Erster Block der gelesen werden soll
 * @param buffer Buffer, in den die Daten geschrieben werden sollen
 * @param size Anzahl der Bytes die in den Buffer geschrieben werden sollen
 * @param offset Stelle ab der vom Blockdevice gelesen werden soll
 * @return
 */
int MyFS::readSection(u_int32_t startblock, char* buffer, size_t size, off_t offset){
    int numberOfBlocks = sizeToBlocks(size + offset);
    u_int32_t list[numberOfBlocks];
    for (u_int32_t i = 0; i < numberOfBlocks; i++) {
        list[i] = startblock + i;
    }
    return readSectionByList(list, buffer, size, offset);
}

void MyFS::writeFAT() {
    LOGM();
    char buffer[NUM_FAT_BLOCKS * BLOCK_SIZE];
    char numberbuffer[ADRESS_LENGTH_BYTE];
    for (int i = 0; i < NUM_FAT_BLOCKS * NUM_ADRESS_PER_BLOCK; i++) {
        intToChar(FAT[i], numberbuffer, ADRESS_LENGTH_BYTE);
        transferBytes(numberbuffer, ADRESS_LENGTH_BYTE, 0, buffer, ADRESS_LENGTH_BYTE * i);

    }
    writeSection(START_FAT_BLOCKS,buffer,NUM_FAT_BLOCKS * BLOCK_SIZE, 0);

}

void MyFS::readFAT() {
    char buffer[NUM_FAT_BLOCKS * BLOCK_SIZE];
    char numberbuffer[ADRESS_LENGTH_BYTE];
    readSection(START_FAT_BLOCKS, buffer, NUM_FAT_BLOCKS*BLOCK_SIZE,0);
    for(int i = 0; i< NUM_FAT_BLOCKS * NUM_ADRESS_PER_BLOCK; i++){
        transferBytes(buffer, ADRESS_LENGTH_BYTE, i*ADRESS_LENGTH_BYTE, numberbuffer, 0);
        FAT[i]= charToInt(numberbuffer, ADRESS_LENGTH_BYTE);
    }
}

int MyFS::writeSBLOCK() {
    LOGM();
    char SBLOCK[BLOCK_SIZE];
    char buffer[4];
    clearCharArray(SBLOCK, BLOCK_SIZE);
    transferBytes(NAME_FILESYSTEM, NUM_NAME_FILESYSTEM_BYTE, 0, SBLOCK, START_FILENAME_BYTE);
    intToChar(numberOfFiles, buffer, NUM_RESERVED_ENTRIES_BYTE);
    transferBytes(buffer, NUM_RESERVED_ENTRIES_BYTE, 0 , SBLOCK, START_RESERVED_ENTRIES_BYTE);
    intToChar(numberOfUsedDATABLOCKS, buffer, NUM_RESERVED_BLOCKS_BYTE);
    transferBytes(buffer, NUM_RESERVED_BLOCKS_BYTE, 0, SBLOCK, START_RESERVED_BLOCKS_BYTE);
    intToChar((int) numberOfwrittenBytes,buffer, NUM_RESERVED_DATA_BYTES_BYTE);
    transferBytes(buffer, NUM_RESERVED_DATA_BYTES_BYTE, 0, SBLOCK, START_RESERVED_DATA_BYTES_BYTE);

    blockDevice->write(START_SUPER_BLOCKS,SBLOCK);
    return 0;
}

int MyFS::readSBlock(){
    char SBLOCK[BLOCK_SIZE];
    char fileNAME[NUM_FILENAME_BYTE];
    blockDevice->read(START_SUPER_BLOCKS, SBLOCK);
    transferBytes(SBLOCK,NUM_FILE_SIZE_BYTE,0,fileNAME,0);
    if( strcmp(fileNAME, NAME_FILESYSTEM) != 0){
        return EIO;
    }
    char numOfFiles[NUM_RESERVED_ENTRIES_BYTE];
    transferBytes(SBLOCK, NUM_RESERVED_ENTRIES_BYTE, NUM_FILE_SIZE_BYTE, numOfFiles, 0);
    numberOfFiles = charToInt(numOfFiles, NUM_RESERVED_ENTRIES_BYTE);

    char numOfDataBlocks[NUM_RESERVED_BLOCKS_BYTE];
    transferBytes(SBLOCK,NUM_RESERVED_BLOCKS_BYTE, NUM_FILE_SIZE_BYTE+NUM_RESERVED_ENTRIES_BYTE, numOfDataBlocks, 0 );
    numberOfUsedDATABLOCKS = charToInt(numOfDataBlocks, NUM_RESERVED_BLOCKS_BYTE);

    char numOfwrittenBytes[NUM_RESERVED_DATA_BYTES_BYTE];
    transferBytes(SBLOCK,NUM_RESERVED_DATA_BYTES_BYTE, NUM_FILE_SIZE_BYTE+NUM_RESERVED_ENTRIES_BYTE+NUM_RESERVED_BLOCKS_BYTE, numOfwrittenBytes, 0 );
    numberOfwrittenBytes = (u_int64_t) charToInt(numOfwrittenBytes, NUM_RESERVED_DATA_BYTES_BYTE);


    return 0;
}

void MyFS::readRoot() {
    for (int i = 0; i < NUM_ROOT_BLOCKS ;i++) {
        readBlock((u_int32_t) START_ROOT_BLOCKS + i, FILENAME[i], NUM_FILENAME_BYTE, 0);
    }
}

//Fertig 2.0
/**
 * Schreibt einen Eintrag ins ROOT, mit den angegebenen Parametern.
 * @param position gibt an, in welchen Root-Block geschrieben werden soll
 * @param filename  Name der Datei (Ohne Pfad)
 * @param size Größe der Datei
 * @param userID
 * @param groupID
 * @param accesRight
 * @param firstTimestamp
 * @param secondTimestamp
 * @param thirdTimestamp
 * @param firstDataBlock Pointer auf den ersten DataBlock
 * @return
 */
int MyFS::writeROOT(u_int32_t position, const char* filename, size_t size, char* userID, char* groupID, char* accesRight, char* firstTimestamp, char* secondTimestamp, char* thirdTimestamp, int firstDataBlock) {
    char ROOTBlock[BLOCK_SIZE];
    char buffer[4];
    clearCharArray(ROOTBlock, BLOCK_SIZE);
    int lengthOfFilename = getSizeOfCharArray(filename);
    transferBytes(filename, lengthOfFilename < NUM_FILENAME_BYTE ? lengthOfFilename + 1 : NUM_FILENAME_BYTE, 0, ROOTBlock, START_FILENAME_BYTE);

    intToChar((int) size, buffer, NUM_FILE_SIZE_BYTE);
    transferBytes(buffer, NUM_FILE_SIZE_BYTE, 0, ROOTBlock, START_FILE_SIZE_BYTE);

    intToChar(firstDataBlock, buffer, NUM_POINTER_BYTE);
    transferBytes(buffer, NUM_POINTER_BYTE, 0, ROOTBlock, START_POINTER_BYTE);

    transferBytes(firstTimestamp, NUM_TIMESTAMP_BYTE, 0, ROOTBlock, START_FIRST_TIMESTAMP_BYTE);
    transferBytes(secondTimestamp, NUM_TIMESTAMP_BYTE, 0, ROOTBlock, START_SECOND_TIMESTAMP_BYTE);
    transferBytes(thirdTimestamp, NUM_TIMESTAMP_BYTE, 0, ROOTBlock, START_THIRD_TIMESTAMP_BYTE);

    transferBytes(userID, NUM_USERID_BYTE, 0, ROOTBlock, START_USERID_BYTE);
    transferBytes(groupID, NUM_GROUPID_BYTE, 0, ROOTBlock, START_GROUPID_BYTE);

    LOGF("WRITE ROOTBLOCK: %d;Size: %d; Pointer: %d", position + START_ROOT_BLOCKS, size, firstDataBlock);
    LOGF("atime: %d, mtime: %d, ctime: %d",charToInt(firstTimestamp, NUM_TIMESTAMP_BYTE), charToInt(secondTimestamp, NUM_TIMESTAMP_BYTE), charToInt(thirdTimestamp, NUM_TIMESTAMP_BYTE));
    LOGF("UID: %d; GID: %d", charToInt(userID, NUM_USERID_BYTE), charToInt(groupID, NUM_GROUPID_BYTE));
    return blockDevice->write(position + START_ROOT_BLOCKS, ROOTBlock);
}

int MyFS::sizeToBlocks(size_t size) {
    return (int) (((size - 1) - ((size - 1) % BLOCK_SIZE)) / BLOCK_SIZE) + 1;
}

u_int32_t MyFS::getFirstPointer(int filePosition) {
    char pointer[NUM_POINTER_BYTE];
    readBlock(START_ROOT_BLOCKS + filePosition,pointer, NUM_POINTER_BYTE, START_POINTER_BYTE);
    return (u_int32_t) charToInt(pointer, NUM_POINTER_BYTE);
}

void MyFS::clearCharArray(char* buffer, size_t size) {
    for (int i = 0; i < size; i++) {
    buffer[i] = '\0';
    }
}

u_int32_t MyFS::createFATEntrie(u_int32_t startposition, size_t oldFileSize, size_t newFileSize) {
    LOGM();
    LOGF("Startposition: %d; oldFileSize: %d; newFileSize: %d", startposition, oldFileSize, newFileSize);
    int oldNumberOfBlocks = sizeToBlocks(oldFileSize);
    int newNumberOfBlocks = sizeToBlocks(newFileSize);
    u_int32_t list[oldNumberOfBlocks];
    if (oldFileSize > 0) {
        getFATList(list, startposition, -1, 0);
    }
    LOG("Checkpoint 2.5");

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
                LOG("Checkpoint 11");
                //LOGF("FAT[%d] = %d", list[oldNumberOfBlocks - 1], freeBlocks[0]);
                FAT[list[oldNumberOfBlocks - 1]] = freeBlocks[0];
            }
            int i = 0;
            for (; i < newNumberOfBlocks - oldNumberOfBlocks - 1; i++) {
                //LOGF("FAT[%d] = %d", freeBlocks[i], freeBlocks[i + 1]);
                FAT[freeBlocks[i]] = freeBlocks[i + 1];
                DMAP[freeBlocks[i]] = true;
            }
            LOG("Checkpoint 13");
            //LOGF("FAT[%d] = %d", freeBlocks[i], freeBlocks[i]);
            FAT[freeBlocks[i]] = freeBlocks[i];
            DMAP[freeBlocks[i]] = true;
            numberOfUsedDATABLOCKS += newNumberOfBlocks - oldNumberOfBlocks;
            numberOfwrittenBytes += newFileSize - oldFileSize;
            return oldNumberOfBlocks > 0 ? list[0] : freeBlocks[0];
        }
    }
    LOG("Checkpoint END");
    numberOfwrittenBytes += newFileSize - oldFileSize;
    return startposition;
}

void MyFS::getFATList(u_int32_t* list, u_int32_t startposition, int numberOfBlocks, int startnumber) {
    LOG("getFatList() START");
    LOGF("startposition: %d, Startnumber: %d", startposition, startnumber);
    u_int32_t aktuallPosition = startposition;
    int i = 0;

    for (; aktuallPosition != FAT[aktuallPosition] && (numberOfBlocks > 0 ? i < numberOfBlocks : true); i++) {
        list[i] = aktuallPosition + startnumber;
        aktuallPosition = FAT[aktuallPosition];
    }
    LOGF("list[%d] = %d", i, aktuallPosition + startnumber);
    list[i] = aktuallPosition + startnumber;
    LOG("END");
}
