# Design

## Vorgaben

### "Offiziell"

Konstante Werte, von denen wir ausgehen:

- NAME_LENGTH
	- 255 Zeichen
- BLOCK_SIZE
	- 512 Bytes
- NUM_DIR_ENTRIES
	- 64 Einträge
- NUM_OPEN_FILES
	- 64 Einträge

### Selbst definiert

Konstante Werte, von denen wir ausgehen:

- NUM_DATA_BLOCKS
	- 2^16 Blöcke

## Superblock
Informationen über das Dateisystem.

- Namen des Dateisystems = "MYFS"
	- 4 Byte = 4 * sizeof(char)
- Anzahl gespeicherter Dateien bzw. belegter Inodes
	- 7 Bit = log2(1 (für 0) + 64 (max. Anzahl Dateien) + 1 (max. Anzahl Verzeichnisse))
- Anzahl belegter Blöcke
	- 17 Bit = log2(1 (für 0) + 2^16 (für Anzahl Blöcke))
- Anzahl belegter Bytes
	- 25 Byte + 1 Bit = log2(1 (für 0) + 2^16 * 512)

### Größe

1 Block

## DMAP
Array von Bits zur Angabe, ob Block frei ist. So viele Einträge, wie es Datenblöcke gibt. 1 = belegt, 0 = frei. Nicht verwendete Bits werden auf 1 gesetzt.

```
// Überprüfen ob Block belegt ist
isBlockUsed = (bool) dMap[relativeAddress];
```

### Größe

```
// Berechnen der von der DMAP belegten Blöcke
dMapSize = NUM_DATA_BLOCKS * 1/8;
dMapBlockCount = roundUp(dMapSize / BLOCK_SIZE);
```
 
16 Blöcke

## FAT
Array der Folgeblöcke. So viele Einträge, wie es Datenblöcke gibt. Wenn ein Block keinen Folgeblock hat, referenziert er sich selbst.

```
// Bestimmen der relativen Folgeadresse eines Datenblocks
nextRelativeAddress = fat[currentRelativeAddress];

// Überprüfen ob es keine weitere Folgeadresse gibt
isEndBlock = currentRelativeAddress == fat[currentRelativeAddress];
// der EndBlock referenziert sich selbst

// Bestimmen der absoluten Adressen eines Datenblocks
absoluteAddress = dataStartAddress + relativeAddress;
```

### Größe
```
// Berechnen der von dem FAT belegten Blöcke
addressByteCount = sizeof(NUM_DATA_BLOCKS);
fatSize = NUM_DATA_BLOCKS * addressByteCount;
fatBlockCount = roundUp(fatSize / BLOCK_SIZE));
```

256 Blöcke

## Root-Verzeichnis
65 Inodes, jeweils in einem Block. 64 für Dateien, 1 für Root-Verzeichnis. Root-Verzeichnis liegt in erstem Block und wird über `.` und `..` referenziert. Wenn Größe 0 ist, wird der Zeiger auf den ersten Datenblock ignoriert.

Je Inode:
- Dateiname
	- 255 Byte
- Dateigröße (Bei max. Dateigröße von 32 MiB)
	- 25 Byte + 1 Bit = log2(1 (für 0) + 2^16 (Anzahl der Blöcke) * 512 (Bytes pro Block))
- Benutzer ID
	- 1 Byte (überprüfen)
- Gruppen ID
	- 1 Byte (überprüfen)
- Zugriffsrechte
	- 10 Bit
- 3x Zugriffs-Timestamps
	- 3x 4 Byte
- Zeiger auf ersten Datenblock (relativ zur Startadresse der Daten-Blöcke)
	- 2 Byte

### Größe
65 Blöcke (NUM_DIR_ENTRIES + 1)

## Daten
Rohdaten.

### Größe
2^16 Blöcke (NUM_DATA_BLOCKS)
