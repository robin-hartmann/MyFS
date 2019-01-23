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
    const char* remPath = remDirPath(path);
    bool filenameIsCorrect = isFilenameCorrect(path);

    if (!filenameIsCorrect && *remPath != '/' && getSizeOfCharArray(remPath) <= NAME_LENGTH) {
        statbuf->st_mode = S_IFDIR | 0555;
        statbuf->st_nlink = 2;
        return 0;
    } else if (filenameIsCorrect) {
        char buffer[NUM_ACCESS_RIGHT_BYTE];
        readBlock((u_int32_t ) getFilePosition(path) + START_ROOT_BLOCKS, buffer, NUM_ACCESS_RIGHT_BYTE, START_ACCESS_RIGHT_BYTE);
        statbuf->st_mode = charToInt(buffer, NUM_ACCESS_RIGHT_BYTE);
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
    if (!isDirPathCorrect(path) || !isFilenameCorrect(path)) {
        return ENOENT;
    } else if (isFileExisting(path)) {
        return EEXIST;
    } else if (numberOfFiles >= 64) {
        return ENOSPC;
    }
    u_int32_t freePosition = 0;
    const char* filename = remDirPath(path);
    size_t lentghOFFilename = getSizeOfCharArray(filename);

    for(;FILENAME[freePosition][0] != '\0'; freePosition++);

    transferBytes(filename, lentghOFFilename + 1, 0, FILENAME[freePosition], 0);
    numberOfFiles++;
    writeROOT(freePosition, filename, 0, "\0", "\0", "\0", "\0", "\0", "\0", 0);
    RETURN(0);;
}

int MyFS::fuseMkdir(const char *path, mode_t mode) {
    LOGM();
    return 0;
}

int MyFS::fuseUnlink(const char *path) {
    LOGM();
    if (!isDirPathCorrect(path) || !isFilenameCorrect(path)) {
        return ENOENT;
    } else if (isFileExisting(path)) {
        return EEXIST;
    }

    int filePosition = getFilePosition(path);
    if(getFileSize(filePosition) > 0) {
        setDataBlocksUnused(getFirstPointer(filePosition));
    }

    numberOfwrittenBytes -= getFileSize(filePosition);
    numberOfUsedDATABLOCKS -= sizeToBlocks(getFileSize(filePosition));

    writeROOT(filePosition, "\0", 0, "\0", "\0", "\0", "\0", "\0", "\0", 0);

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

//Fertig
int MyFS::fuseOpen(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    
    if(isFileExisting(path)) {
        openFiles[getFilePosition(path)] = true;
        return 0;
    }
    
    return EEXIST;
}

//Nochmal ändern aufgrund neuer Methoden
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
                                                              : (actuallFATPosition = FAT[actuallFATPosition]);
                actuallFATPosition = FAT[actuallFATPosition];
            }
            for (u_int64_t j = 0; j < size && !endOfFile; j += BLOCK_SIZE) {
                readBlock((u_int32_t ) actuallFATPosition + START_DATA_BLOCKS, buffer, BLOCK_SIZE - i, i);
                transferBytes(buffer, j + BLOCK_SIZE < size ? BLOCK_SIZE - i : size - j, i, buf, j);
                actuallFATPosition == FAT[actuallFATPosition] ? endOfFile = true : (actuallFATPosition = FAT[actuallFATPosition]);

                i = 0;
            }
        }
    }
    RETURN(0);
}

int MyFS::fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    
    if (!isFileExisting(path)) {
        return EEXIST;
    }
    int filePosition = getFilePosition(path);
    if (!openFiles[filePosition]) {
        return EBADF;
    } else if (sizeToBlocks(size) - sizeToBlocks((size_t) getFileSize(filePosition)) > (NUM_DATA_BLOCKS - numberOfUsedDATABLOCKS)) { //abändern
        return ENOSPC;
    }

    const char* filename = remDirPath(path);
    if (getFileSize(filePosition) > 0) {
        setDataBlocksUnused( getFirstPointer(filePosition));
    }
    numberOfwrittenBytes -= getFileSize(filePosition);
    numberOfUsedDATABLOCKS -= sizeToBlocks(getFileSize(filePosition));

    u_int32_t blockAdress[sizeToBlocks(size)];
    searchfreeBlocks(size, blockAdress);

    writeSectionByList(blockAdress, buf, size, offset);

    numberOfUsedDATABLOCKS += sizeToBlocks(size);
    numberOfwrittenBytes += size;

    int i = 0;
    for (; i < sizeToBlocks(size); i++) {
        FAT[blockAdress[i]] = blockAdress[i + 1];
    }
    FAT[blockAdress[i]] =  blockAdress[i];

    writeROOT(filePosition, filename, size, "\0", "\0", "\0", "\0", "\0", "\0", blockAdress[0]);

    return size;
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

//Fertig
int MyFS::fuseOpendir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
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

//Fertig
int MyFS::fuseReleasedir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();

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

//Fertig 2.0
int MyFS::fuseCreate(const char *path, mode_t mode, struct fuse_file_info *fileInfo) {
    LOGM();
    if (!isDirPathCorrect(path) || !isFilenameCorrect(path)) {
        return ENOENT;
    } else if (isFileExisting(path)) {
        return EEXIST;
    } else if (numberOfFiles >= 64) {
        return ENOSPC;
    }
    u_int32_t freePosition = 0;
    const char* filename = remDirPath(path);
    size_t lentghOFFilename = getSizeOfCharArray(filename);

    for(;FILENAME[freePosition][0] != '\0'; freePosition++);

    transferBytes(filename, lentghOFFilename + 1, 0, FILENAME[freePosition], 0);
    numberOfFiles++;
    writeROOT(freePosition, filename, 0, "\0", "\0", "\0", "\0", "\0", "\0", 0);
    RETURN(0);
}

void MyFS::fuseDestroy() {
    writeSBLOCK();
    writeDMap();
    writeFAT();
    blockDevice->close();
    delete blockDevice;
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
        BlockDevice* newblockDevice = new BlockDevice(BLOCK_SIZE);
        newblockDevice->open(((MyFsInfo *) fuse_get_context()->private_data)->contFile);
        this->blockDevice = newblockDevice;

        readSBlock();
        readDMap();
        readFAT();

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

    if (size + offset <= BD_BLOCK_SIZE){
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
u_int32_t MyFS::getSizeOfCharArray(char *buf) {
    u_int32_t size = 0;
    while(buf[size] != '\0') {
        size++;
    }
    return size;
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

/**
 * Entfernt den Dateipfad vom Dateinamen.
 * @param path Dateipfad / Dateiname
 * @return Dateiname
 */
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

/**
 * Überprüft, ob der Filename correkt ist.
 * Achtung: Auf den Dateipfad vorher nicht removeDirPath() ausführen!
 * @param path Dateipfad / Dateiname
 * @return true, wenn Dateiname korrekt ist
 */
bool MyFS::isFilenameCorrect(const char *path) {
    const char *startOfFilename = remDirPath(path);
    if (getSizeOfCharArray(startOfFilename) > NAME_LENGTH || (*startOfFilename == '/' || *startOfFilename == '\0' ||
    (*startOfFilename == '.' && (*(startOfFilename + 1) == '\0' || (*(startOfFilename + 1) == '.' &&
    *(startOfFilename + 2) == '\0' ))))) {
        return false;
    }
    for (int i= 0; startOfFilename[i] != '\0'; i++) {
        if (startOfFilename[i] == '/') {
            return false;
        }
    }
    return true;
}

/**
 * Überprüft ob der übergebene Dateipfad Correct ist.
 * Achtung: Der Dateipfad darf davor nicht durch remDirPath() gelaufen sein! Das verfälscht das Ergebnis.
 * @param path Dateipfad
 * @return true, wenn Dateipfad korrekt ist
 */
bool MyFS::isDirPathCorrect(const char *path) {
    const char *dir = remDirPath(path);
    if (*(dir) == '\0' || ((*dir == '/' || *dir == '.') && *(dir + 1) == '\0') || (*(dir) == '.' && *(dir + 1) == '.' && *(dir + 2) == '\0')) {
        return false;
    }
    return true;
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
    int sum = 0;
    if (numberOfChars <= 4) {
        for (int i = 0; i < numberOfChars; i++) {
            sum += ((int) chars[i]) << (i * 8);
        }
    }
    return sum;

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
            char buffer[BLOCK_SIZE];
            writeSection(START_DMAP_BLOCKS, buffer, NUM_DMAP_BLOCKS*BLOCK_SIZE,0);
    for (int i = 0; i<NUM_DATA_BLOCKS*NUM_DMAP_BLOCKS; i++) {
        int whichChar = (i - (i % 8))/8 ;
        int whichBitinChar = i % 8;
        DMAP[i] = (buffer[whichChar] >> whichBitinChar) & 1;
    }

}




void::MyFS::setBitinChar(int position, bool value, char* buffer){
    int whichChar = (position - (position % 8))/8 ;
    int whichBitinChar = position % 8;

    if (value){
        buffer[whichChar] |= 1 << whichBitinChar;
    }else {
        buffer[whichChar] &= ~(1 << whichBitinChar);
    }

}


void MyFS::writeDMap(){
    char buffer[NUM_DATA_BLOCKS];
    for(int i=0; i<NUM_DATA_BLOCKS ; i++) {
        setBitinChar(i, DMAP[i], buffer);
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
        numberOfWriteBytes = size > BLOCK_SIZE ? BLOCK_SIZE : size;
        transferBytes(buf + numberOfWrittenBytes + offset, numberOfWriteBytes, 0, buffer, 0);
        numberOfWrittenBytes += numberOfWriteBytes;
        size -= numberOfWriteBytes;
        blockDevice->write(list[i], buffer);
    }
}

void MyFS::setDataBlocksUnused(u_int32_t position){ // auf basis der position wird das FAT durchgesucht nach des restlichen Blöcken und diese werden auf unused gestzt. -> Rekursiv lösen
    if(position == FAT[position]){
    }else{
        if(DMAP[position] == true) {
            DMAP[position] = false;
            numberOfUsedDATABLOCKS--;
            MyFS::setDataBlocksUnused(FAT[position]);
        }else{
            error("");
        }
    }

}



void MyFS::searchfreeBlocks(size_t size, u_int32_t* blockAdressBuffer){
    int counter = 0;
    int iterator = 0;
    size = sizeToBlocks(size);


    while(counter != size) {
        if (iterator == NUM_DATA_BLOCKS) {
            blockAdressBuffer = nullptr;
            counter = size;
        } else {
            if (DMAP[iterator] == false) {
                blockAdressBuffer[counter] = START_DATA_BLOCKS + iterator;
                counter++;
            }
            iterator++;

        }
    }
}


int MyFS::readSectionByList(u_int32_t* list, char* buf, size_t size, off_t offset) {
   size_t writtenBytes = 0;
   size_t readBytes = 0;
    for(int i = 0; size > 0; i++) {
        readBytes = size + offset > BLOCK_SIZE ? BLOCK_SIZE - offset : size;
        if(readBytes > 0) {
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
    char SBLOCK[BLOCK_SIZE];
    char buffer[4];
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
    int lengthOfFilename = getSizeOfCharArray(filename);

    transferBytes(filename, lengthOfFilename < NUM_FILENAME_BYTE? lengthOfFilename + 1 : NUM_FILENAME_BYTE, 0, ROOTBlock, START_FILENAME_BYTE);

    intToChar((int) size, buffer, NUM_FILE_SIZE_BYTE);
    transferBytes(buffer, NUM_FILE_SIZE_BYTE, 0, ROOTBlock, START_FILE_SIZE_BYTE);

    intToChar(firstDataBlock, buffer, NUM_POINTER_BYTE);
    transferBytes(buffer, NUM_POINTER_BYTE, 0, ROOTBlock, START_POINTER_BYTE);

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