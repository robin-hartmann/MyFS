# Funktionen

Beim Initialisieren des Blockdevices werden zuerst S-Block, D-Map, FAT und die Namen aus dem Rootverzeichnis ausgelesen und somit im Arbeitspeicher abgelegt. 
S-Block, D-Map und das FAT werden immer beim Aufruf der Funktion fuseRelease auf das Blockdevice zurück geschrieben mit den aktualisierten Daten. 
Der Rootverzeichnis wird immer direkt auf dem Blockdevice aktualsiert, wenn an diesem eine Änderung vorgenommen wird. 
Durch die unten aufgeführten Hilfsfunktionen konnten wir Code mehrfach verwenden. 
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
__Parameter__
* const char *path: _Pfad der Datei, von welcher die Position angefragt wird._


 Durch diese Funktion kann die Position der angefragten Datei im Root-Verzeichnis lokalisiert werden.
Dies wird dadurch erreicht, dass alle Einträge des Root-Verzeichnisses an der 
Position des Datei Namens vergliechen wird, mit dem angefragten Pfad/Datei.


### int readBlock(u_int32_t blockNo,char *buf, size_t size, off_t offset);
__Parameter__
* u_int32_t blockNo: _Nummer des angeforderten Blocks._
* char *buf:   _Char Array, in welches der angeforderte Block geschrieben wird._ 
* size_t size: _Die Menge an Bytes, die aus dem Block gelesen werden sollen._
* off_t offset: _Der Offset, ab dem die Daten ausgelesen werden sollen im angeforderten Block._

Diese Funktion liest den angeforderten Block aus dem Blockdevice aus und speichert diesen in dem übergebenen char Array ab.
### void transferBytes(char *firstBuf, size_t size, off_t firstOff, char* secondBuf, off_t secondOff);
__Parameter__
* char *firstBuf: _Char Array, welches in einen zweites Array übertragen werden soll._
* size_t size: _Die Größe des firstBuf._
* off_t firstOff: _Der Offset, ab wo die Daten übernommen werden sollen._
* char* secondBuf: _Das zweite char Array, in welches das erste char Array übertragen wird._ 
* off_t secondOff: _Der Offset, ab wo die neuen Daten geschrieben werden sollen im zweiten Array._


Schreibt den erstem Buffer (firstBuf) in den zweiten Buffer (secondBuf). Es besteht jeweils die Möglichkeit ein Offset anzugeben.


### int getFileSize(int position);
__Parameter__
* int position: _Position im Root-Verzeichnis_

Liest aus dem Root-Block die Größe der angefragten Datei aus.

### bool isFileExisting(const char *path);
__Parameter__
* const char *path: _Pfad einer Datei_

Überprüft, Ob eine Datei beireits existiert. Dies wird durch Aufrufen der getFilePosition() Funktion erreicht, da diese, wenn sie eine Datei nicht findet -1 zurück gibt.
 
### const char* remDirPath(const char *path);
__Parameter__
* const char *path: _Pfad einer Datei_


Entfernt den Dateipfad vom Dateinamen.


### bool isFilenameCorrect(const char* path);
__Parameter__
* const char* path: _Pfad einer Datei_


Überprüft, ob der Dateiname korrekt ist.
### bool isDirPathCorrect(const char *path);
__Parameter__ 
* const char *path: _Pfad einer Datei_

Überprüft ob der übergebene Dateipfad korrekt ist.

### bool isDirPath(const char* path);
__Parameter__
* const char *path: _Pfad einer Datei_

Überprüft ob der Pfad ein korrekter Verzeichnispfad ist und keine Datei.

### int charToInt(char* chars, int numberOfChars);
__Parameter__
* char* chars: _Char Array, in welchem eine Zahl codiert ist._
* int numberOfChars: _Größe des char Arrays._

Konvertiert eine in ein Char abgespeicherte Zahl zurück in einen Integer.
### void intToChar(int number, char* buffer, int numberOfChars);
__Parameter__
* int number: _Die Zahl die konvertiert werden soll_
* char* buffer: _Ein char Array, in welches die konvertierte Zahl gespeichert werden soll._
* int numberOfChars: _Anzahl der chars bzw. die Größe in Byte, des char Buffers._

Konvertiert einen Integer in einen Char. Die Zahl wird dabei in das Dualsystem konvertiert.
 
### void readDMap();
Liest die DMap aus dem Blockdevice aus. 
### void writeDMap();
Schreibt die im Speicher(Memory) liegende DMap auf das Blockdevice.
### void setDataBlocksUnused(u_int32_t position);
__Parameter__
* u_int32_t position: _Start Block einer Datei._

Die Blöcke einer Dateien werden in der DMap und im FAT freigegeben.
### void searchfreeBlocks(size_t size, u_int32_t* blockAdressBuffer);
__Parameter__
* size_t size: _Anzhal der Blöcke, die angefordert werden._
* u_int32_t blockAdressBuffer: _Array in welches die Nummern der freien Blöcken gespeichert werden._


Sucht nach freien Blöcken und gibt die Nummern dieser zurück.
### int readSectionByList(int filePosition, u_int32_t* list, char* buf, size_t size, off_t offset);
__Parameter__
* int filePosition: _Position des Root-Blocks einer Datei im Root-Verzeichnis._
* u_int32_t* list: _Liste der angefragten Blöcken_
* char* buf: _char Array, in welches die ausgelesen Daten gespeichert werden_
* size_t size: _Anzahl der Bytes die in das char Array geschrieben werden sollen.
* off_t offset: Position ab der von dem Blockdevice gelesen werden soll.

Liest die Blöcke, die in der übergebenen Liste angegeben sind aus dem Blockdevice aus. 
Dabei wird davor überprüft, ob einer dieser Blöcke schon gecacht wurde. Ausserdem wird am Ende des Lesevorgangs der zuletzt ausgelesene Block gecached.
### int readSection(u_int32_t startblock, char* buffer, size_t size, off_t offset);
__Parameter__
* u_int32_t startblock: _Erster Block der gelesen werden soll._
* char* buffer: _char Array, in welches die ausgelesen Daten gespeichert werden_
* size_t size: _Anzahl der Bytes die in das char Array geschrieben werden sollen._
* off_t offset: _Position ab der von dem Blockdevice gelesen werden soll._

Berechnet sich die zu lesende Blöcke aus und fordert diese über die Funktion readSectionByList an. 
### void writeSection(u_int32_t startblock,const char* buffer, size_t size, off_t offset);
__Parameter__
* u_int32_t startblock: _Erster Block der geschrieben werden soll._
* char* buffer: _char Array, welches auf das Blockdevice geschrieben werden soll_
* size_t size: _Anzahl der Bytes die aus dem char Array auf das Blockdevice geschrieben werden sollen._
* off_t offset: _Position aber der Daten von dem char Array geschrieben werden sollen._


Die Funktion berechnet, wie viele Blöcke geschrieben werden müssen und übergibt diese an writeSectionByList.
### void writeSectionByList(u_int32_t* list, const char* buf, size_t size, off_t offset);
__Parameter__
* u_int32_t* list: _Blöcke, die beschrieben werden sollen._
* char* buffer: _char Array, welches auf das Blockdevice geschrieben werden soll_
* size_t size: _Anzahl der Bytes die aus dem char Array auf das Blockdevice geschrieben werden sollen._
* off_t offset: _Position aber der Daten von dem char Array geschrieben werden sollen._

Schreibt den übergebenen Buffer auf das Blockdevice.
### void writeFAT();
Schreibt das FAT aus dem Speicher(Memory) auf das Blockdevice.
### void readFAT();
Liest das FAT aus dem Blockdevice aus. 
### int writeSBLOCK();
Schreibt den S-Block auf das Blockdevice.
### int readSBlock();
Liest den S-Block aus dem Blockdevice aus. 
### void readRoot();
Liest die Filenamen aus dem Root-Verzeichnis, das auf dem Blockdevice liegt aus.
### int sizeToBlocks(size_t size);
__Parameter__
* size_t size: _Größe in Bytes._

Berechnet die benötigte Anzahl an Böcken, um die übergebene Größe an Daten zu speichern.
### u_int32_t getFirstPointer(int filePosition);
__Parameter__
* int filePosition: _Position im des Root-Blocks._

Liest den Startblock des Files aus dem Root-Block aus. 
### void clearCharArray(char* buffer, size_t size);
__Parameter__
* char* buffer: _char Array Pointer._

Befüllt den übergebenen Buffer mit '\0'. 
### u_int32_t createFATEntrie(u_int32_t startposition, size_t oldFATSize, size_t newFATSize);
__Parameter__
* u_int32_t startposition: _Nummer des Startblocks._
* size_t oldFATSize: _Alte Anzhal der Blöcke._
* size_t newFATSize: _Neue Anzahl der Blöcke._

Erzeugt ein FAT Eintrag. 
### void getFATList(u_int32_t* list, u_int32_t startposition, int numberOfBlocks, int startnumber);
__Parameter__
* u_int32_t* list: _unisgned int32 Array, in welches die Nummbern der Blöcke geschrieben werden._
* u_int32_t startposition: _Nummer des Start-Blocks._
* int numberOfBlocks: _Anzhal der Blöcke._
* int startnumber: _

Gibt die Nummern der Blöcke in denen die Datei geschrieben ist aus dem FAT zurück. 
### int writeRoot(u_int32_t position, char* buf);
__Paramter__
* u_int32_t position: _Position des Root-Blocks._
* char* buf: _Buffer, in welchem die zu schreibende Daten sind._


Schreibt in den Root-Block. 
### int createNewFile(const char *path, mode_t mode);
__Parameter__
* const char *path: _Pfad der Datei, die angelegt wird._
* mode_t mode: _Berechtigungen der Datei._

Legt eine neue Datei an und erstellt den Root-Block für diese Datei. 