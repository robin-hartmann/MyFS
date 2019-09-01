# Funktionen

## Fusefunktionen

| Methode        | Zweck           |
| ------------- |:-------------:|
| fuseInit()      | initialisiert das Blockdevice und öffnet die Containerdatei | 
| fuseRelease()      | schließt eine zuvor geöffnete Datei      |
| fuseReaddir() | gibt Informationen über den Verzeichnisinhalt zurück      |
| fuseGetattr()      | liefert Informationen über eine Datei zurück | 
| fuseOpen()      | öffnet eine Datei um sie zu lesen     |
| fuseRead() | liest von einer Datei     |
| fuseMknod()      | erzeugt eine neue Datei | 
| fuseWrite()      | schreibt in eine Datei   |
| fuseUnlink() | löscht eine Datei   |

## Hilfsfunktionen: 


### int getFilePosition(const char *path);
Durch diese Funktion kann die Position der angefragten Datei im Root-Verzeichnis lokalisiert werden.
Dies wird dadurch erreicht, dass alle Einträge des Root-Verzeichnisses an der 
Position des Datei Namens vergliechen wird, mit dem angefragten Pfad/Datei.
### int readBlock(u_int32_t blockNo,char *buf, size_t size, off_t offset);
Gibt den angeforderten Block in dem übergebenen char Buffer zurück. 
### void transferBytes(char *firstBuf, size_t size, off_t firstOff, char* secondBuf, off_t secondOff);
Schreibt den erstem Buffer (firstBuf) in den zweiten Buffer (secondBuf). Es besteht jeweils die Möglichkeit ein Offset anzugeben.
### int getFileSize(int position);
Liest aus dem Root Verzeichnis die Größe der angefragten Datei aus.
### bool isFileExisting(const char *path);
Überprüft, Ob eine Datei beireits existiert. Dies wird durch Aufrufen der getFilePosition() Funktion erreicht, da diese, wenn sie eine Datei nicht findet -1 zurück gibt. 
### const char* remDirPath(const char *path);
Entfernt den Dateipfad vom Dateinamen.
### bool isFilenameCorrect(const char* path);
Überprüft, ob der Filename korrekt ist.
Achtung: Auf den Dateipfad vorher nicht removeDirPath() ausführen!
### bool isDirPathCorrect(const char *path);
Überprüft ob der übergebene Dateipfad korrekt ist.
### bool isDirPath(const char* path);
Überprüft ob der Pfad ein korrekter Verzeichnispfad ist und keine Datei.
### int charToInt(char* chars, int numberOfChars);
Konvertiert eine in ein Char abgespeicherte Zahl zurück in einen Integer.
### void intToChar(int number, char* buffer, int numberOfChars);
Konvertiert einen Integer in einen Char. Die Zahl wird dabei in das Dualsystem konvertiert. 
### void readDMap();
Liest die DMap aus dem Blockdevice aus. 
### void writeDMap();
Schreibt die DMap auf das Blockdevice.
### void setDataBlocksUnused(u_int32_t position);
Blöcke im FAT und DMAP werden auf nicht verwendet gesetzt. 
### void searchfreeBlocks(size_t size, u_int32_t* blockAdressBuffer);
Sucht nach freien Blöcken und gibt diese zurück. Übergeben wird die angeforderte anzahl von Blöcken und ein Buffer.
### int readSectionByList(int filePosition, u_int32_t* list, char* buf, size_t size, off_t offset);
Liest die Blöcke, die in der übergenen Liste angegeben sind aus dem Blockdevice aus. 
Dabei wird davor überprüft, ob einer dieser Blöcke schon gecacht wurde. Ausserdem wird am ende des Lesevorgangs der zuletzt ausgelesene Block gecached.
### int readSection(u_int32_t startblock, char* buffer, size_t size, off_t offset);
Berechnet sich die zu lesende Blöcke aus und fordert diese über die Funktion readSectionByList an. 
### void writeSection(u_int32_t startblock,const char* buffer, size_t size, off_t offset);
Die Funktion berechnet, wie viele Blöcke geschrieben werden müssen und übergibt diese an writeSectionByList.
### void writeSectionByList(u_int32_t* list, const char* buf, size_t size, off_t offset);
Schreibt den übergebenen Buffer auf das Blockdevice.
### void writeFAT();
Schreibt das FAT aus dem RAM auf das Blockdevice.
### void readFAT();
Liest das FAT aus dem Blockdevice aus. 
### int writeSBLOCK();
Schreibt den S-Block auf das Blockdevice.
### int readSBlock();
Liest den S-Block aus dem Blockdevice aus. 
### void readRoot();
Liest die Filenamen aus dem Root-Block, das auf dem Blockdevice liegt aus
### int sizeToBlocks(size_t size);
Berechnet die benötigte Anzahl an Böcken, um die übergebene Größe an Daten zu speichern.
### u_int32_t getFirstPointer(int filePosition);
Liest den Startblock des Files aus dem Rootblock aus. 
### void clearCharArray(char* buffer, size_t size);
Befüllt den übergebenen Buffer mit '\0'. 
### u_int32_t createFATEntrie(u_int32_t startposition, size_t oldFATSize, size_t newFATSize);
Erzeugt ein FAT Eintrag. 
### void getFATList(u_int32_t* list, u_int32_t startposition, int numberOfBlocks, int startnumber);
Gibt die Nummbern der Blöcke in denen die Datei geschrieben ist aus dem FAT zurück. 
### int writeRoot(u_int32_t position, char* buf);
Schreibt in den Rootblock. 
### int createNewFile(const char *path, mode_t mode);
Legt eine neue Datei an und erstellt somit den Rootblock für diese Datei. 