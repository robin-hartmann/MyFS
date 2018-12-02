## S-Block
Informationen über das Dateisystem.

- Namen des Dateisystems
- vorgegebene Parameter
    - NAME_LENGTH = 255 Zeichen
    - BLOCK_SIZE = 512 Bytes
    - NUM_DIR_ENTRIES = 64
    - NUM_OPEN_FILES = 64
- weitere Parameter
    - DATA_SIZE = mind. 30 MB
    - Datenvolumen = DATA_SIZE + Metadaten
    - Startadressen
        - D-Map
        - FAT
        - Root-Verzeichnis
        - Daten
- @todo

## D-Map
Array von Bits zur Angabe, ob Block frei ist. So viele Einträge, wie es Datenblöcke gibt.

```
// determine if block is used
isBlockUsed = (bool) dMap[relativeAddress];

// calculate amount of blocks occupied by D-Map
dataBlockCount = roundUp(DATA_SIZE / BLOCK_SIZE);
dMapSize = dataBlockCount * 1/8;
dMapBlockCount = roundUp(dMapSize / BLOCK_SIZE);
```

## FAT
Array der Folgeblöcke. So viele Einträge, wie es Datenblöcke gibt.

```
// get a data block's following relative address
nextRelativeAddress = fat[currentRelativeAddress];

// determine a data block's absolute address
absoluteAddress = dataStartAddress + relativeAddress;

// calculate amount of blocks occupied by FAT
dataBlockCount = roundUp(DATA_SIZE / BLOCK_SIZE);
addressBitCount = size(dataBlockCount);

if (pow(2, addressBitCount - 1) == dataBlockCount) {
    addressBitCount++; // pow(2, addressBitCount - 1) is interpreted as null
}

fatSize = dataBlockCount * addressBitCount * 1/8;
fatBlockCount = roundUp(fatSize / BLOCK_SIZE));
```

## Root-Verzeichnis
Inode (NUM_DIR_ENTRIES viele)

- Dateinamen (NAME_LENGTH viele Zeichen)
- @todo

## Daten
Rohdaten.

```
// calculate amount of block occupied by data
dataBlockCount = roundUp(DATA_SIZE / BLOCK_SIZE);
```
-Max 4 GB großes Dateisystem


-Dblock: nicht verwendete Bits werden auf 1 gesetzt
-Dblock: 1 belegt, 0 frei

-Fat: nicht verwendete Areale mit NULL beschriften
-Fat: NULL besteht aus 1…..1
-Fat: immer Zahlen

-Daten sind immer UTF8

-Root: 9 Bit Dateinamen länge
	Dateinamen (max 255 byte)
	Dateigröße: 28 Bit (Bei max Dateigröße von 30 MB)
	Benutzer/Gruppen ID 16 Bit
	Dateirechte 10 Bit
	Zugriff: (	1 Byte Tag
			1 Byte Monat
			2 Byte Jahr
			1 Byte Stunde
			1 Byte Sekunde
		) x3 			(Überprüfe!!!!!)
	8 Bit Zeigerlänge
	Zeiger

-Sblock:	4 Byte Name des Dateisystems/100
		4 Byte Größe des Dateisystems
		4 Byte Gesamt Datasize
		3 Byte Anzahl der freien Blöcke
		Datum letzte Änderung:
			(	1 Byte Tag
				1 Byte Monat
				2 Byte Jahr
				1 Byte Stunde
				1 Byte Sekunde
			)
		Startadressen: 8 Byte Dmap
				8 Byte FAT
				8 Byte Root
				8 Byte Daten
				
				