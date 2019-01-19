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

    if (!filenameIsCorrect && *remPath != '/' && getSizeOfCharArray(remPath) <= NAME_LENGTH) {
        statbuf->st_mode = S_IFDIR | 0555;
        statbuf->st_nlink = 2;
        return 0;
    } else if (filenameIsCorrect) {
        char buffer[NUM_ACCESS_RIGHT_BYTE];
        readBlock((u_int32_t ) getFilePosition(path) + START_ROOT_BLOCKS, buffer, NUM_ACCESS_RIGHT_BYTE, START_ACCESS_RIGHT_BYTE);
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
        //readStructures();
        // MYFsInfo ist ein Struct -> darauf wird ein pointer erzeugt um an den namen des containers zu kommen
    }
    
    RETURN(0);
}
/*
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
*/
/**
 * Gibt die Positon einer Datei im Root-Verzeichnis zurück.
 * Achtung: Voher nicht remDirPath() auf den Pfad anwenden!
 * @param path Dateipfad + Dateiname
 * @return -1 wenn Datei nicht existiert. Ansonsten die DateiPosition
 */
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

/**
 * Gibt die Dateigröße einer Datei zurück
 * @param position Die position der Datei im Root-Verzeichnis (getFilePosition())
 * @return größe der Datei
 */
int MyFS::getFileSize(int position) {
    char buffer[NUM_FILE_SIZE_BYTE + 1];
    buffer[NUM_FILE_SIZE_BYTE] = '\0';
    this->readBlock(START_ROOT_BLOCKS + position, buffer, NUM_FILE_SIZE_BYTE, START_FILE_SIZE_BYTE);

    return charToInt(buffer);
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
    char buffer[size];

    blockDevice->read(blockNo, buffer); //wird unötig gelesen wenn size = 0

    if (size + offset <= BD_BLOCK_SIZE) {
        while (i < size) {
            buf[i] = buffer[j];
            i++;
            j++;
        }
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
 * @return true, wenn Dateiname correkt ist
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
        return true;
    }
    return false;
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
 * @return intege based on the input
 */
int MyFS::charToInt(char* chars) {
    int sum = 0;
    if (getSizeOfCharArray(chars) <= 4) {
        for (int i = 0; chars[i] != '\0'; i++) {
            sum += ((int) chars[i]) << (i * 8);
        }
    }
    return sum;

}

/**
 * Wandelt einen Integer in bis zu 4 chars um. Achtung: Es wird kein \0 hinter das letzte chars geschrieben
 * @param number Integer, der umgewandelt werden soll
 * @param buffer char-Array in den die chars geschrieben werden sollen
 */
void MyFS::intToChar(int number, char* buffer) {
    int sizeOfCharArray = 4;
    for (; sizeOfCharArray > 1 && number == ((number << ((5 - sizeOfCharArray) * 8) >> ((5 - sizeOfCharArray) * 8))); sizeOfCharArray--);
    for (int i = 0; i < sizeOfCharArray; i++){
        buffer[i] = (char) ((number >> (8 * i)) & 0b11111111);
    }
}

void MyFS::readDMap(){ // Dmap is completely rewritten each time to blockdevice - idear: write only block, which are changed
            char buffer[BLOCK_SIZE];
            blockDevice->read(1, buffer);
            setCharBitstoBool(buffer);

}


void::MyFS::setCharBitstoBool(char* buffer) {
    for (int i = 0; i<NUM_DATA_BLOCKS; i++) {
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
void MyFS::writeSection(u_int32_t startblock, char* buffer, size_t size, off_t offset){

    int numberOfBlocks = ((size - (size % BLOCK_SIZE)) / BLOCK_SIZE) + 1; //Die Warnung kann ignoriert werden
    u_int32_t list[numberOfBlocks];
    for (int i = 0; i < numberOfBlocks; i++) {
        list[i] = startblock + i;
    }
    writeSectionByList(list, buffer, size, offset);
}

void MyFS::writeSectionByList(u_int32_t* list, char* buf, size_t size, off_t offset) {
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

void MyFS::setDataBlocksUnused(int &position){ // auf basis der position wird das FAT durchgesucht nach des restlichen Blöcken und diese werden auf unused gestzt. -> Rekursiv lösen
    if(position == FAT[position]){
    }else{
        DMAP[position] = false;
        MyFS::setDataBlocksUnused(FAT[position]);
    }
}



void MyFS::searchfreeBlocks(size_t size, int* blockAdressBuffer){ // Falls keine Blöcke mehr frei bzw. nicht mehr genug vorhanden sind, wird ein NULLPOINTER zurück gegeben.
    int counter = 0;
    int iterator = 0;
    while(counter != size) {
        if (iterator == NUM_DATA_BLOCKS) {
            blockAdressBuffer = nullptr;
            counter = size;
        } else {
            if (DMAP[iterator] == false) {
                blockAdressBuffer[counter] = iterator;
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
    int numberOfBlocks = (((size + offset) - ((size + offset) % BLOCK_SIZE)) / BLOCK_SIZE) + 1;   //Warnung kann ignoriert werden
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
        intToChar(FAT[i], numberbuffer);
        transferBytes(numberbuffer, ADRESS_LENGTH_BYTE, 0, buffer, ADRESS_LENGTH_BYTE * i);

    }
    writeSection(START_FAT_BLOCKS,buffer,NUM_FAT_BLOCKS * BLOCK_SIZE, 0);

}

void MyFS::readFAT() {
    char buffer[NUM_FAT_BLOCKS * BLOCK_SIZE];
    char numberbuffer[ADRESS_LENGTH_BYTE];
    readBlock(START_FAT_BLOCKS, buffer, NUM_FAT_BLOCKS*BLOCK_SIZE,0);
    for(int i = 0; i< NUM_FAT_BLOCKS * BLOCK_SIZE; i++){
        transferBytes(buffer, ADRESS_LENGTH_BYTE, i*ADRESS_LENGTH_BYTE, numberbuffer, 0);
        FAT[i]= charToInt(numberbuffer);
    }
}







