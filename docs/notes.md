# Notizen zur Aufgabenstellung

## Teil 1: Read-Only Dateisystem

### [Spezifikation](spec.md#Teil-1-Read-Only-Dateisystem)

### Teil 1a

Definition der Struktur des Dateisystems und der Softwarearchitektur.

### Teil 1b

Erstellen und Initialisieren des Block Devices (Container) über `mkfs.myfs <container-datei> [datei1, datei2, ...]`.

- um die Attribute der Originaldateien zu lesen wird `stat()` verwendet
- es müssen nicht unbedingt FUSE Operationen verwendet werden
    - `BlockDevice` can direkt verwendet werden
    - aber FUSE Operationen könnten implementiert und in Teil 2 wiederverwendet werden

### Teil 1c

Einhängen des Block Devices über `mount.myfs <container-datei> <log-datei> <ziel-verzeichnis>`.

- kann ausgehängt werden über...
    - Linux: `fusermount --unmount`
    - macOS: `umount`

### Teil 1d

Ausgiebiges Testen

- mit dem Aufruf `mount.myfs -s` kann man FUSE zwingen nur einen Thread zu verwenden, um das Debugging zu vereinfachen

## Teil 2: Read-Write Dateisystem

### [Spezifikation](spec.md#Teil-2-Read-Write-Dateisystem)

## Teil 3: Dokumentation

## Zusätzliche Informationen

### [Dokumentation für libfuse](http://libfuse.github.io/doxygen/)

### [Beispiele für libfuse](https://github.com/libfuse/libfuse/tree/master/example)

### Struktur des MyFS Containers (minimal)
- Superblock
    - Informationen über das Dateisystem (Größe, Position von Einträngen, etc.)
- DMAP
    - Positionen leerer Datenblöcke
- File Allocation Table (FAT)
    - Positionen zusammengehöriger Datenblöcke
- Root-Verzeichnis der Dateien
    - Dateiattribute
        - Name
        - Größe
        - Besitzer (`uid` und `gid`)
        - Zugriffsrechte (`mode`)
        - Zeitstempel
            - letzter Zugriff (`atime`)
            - letzte Änderung der Metadaten (`ctime`)
            - letzte Modifikation des Dateiinhalts (`mtime`)
        - Pointer auf den ersten Datenblock
- Datenblöcke

### Operationen
- Verzeichnis
    - readdir(path)
        - enthaltene Einträge auflisten
    - mkdir(path, mode)
        - anlegen
    - rmdir(path)
        - löschen
- Datei
    - mknod(path, mode, dev)
        - anlegen
    - unlink(path)
        - löschen
    - rename(old, new)
        - umbenennen bzw. verschieben
    - open(path, flags)
        - öffnen
    - read(path, buf, length, offset, fh)
        - lesen
    - write(path, buf, size, offset, fh)
        - schreiben
    - truncate(path, len, fh)
        - auf Länge zuschneiden
    - flush(path, fh)
        - auf Datenträger persistieren
    - release(path, fh)
        - endgültig schließen
- Metadaten
    - getattr(path)
        - lesen
    - chmod(path, mode)
        - Zugriffsrechte ändern
    - chown(path, uid, gid)
        - Besitzer ändern
    - fsinit(self)
        - Dateisystem initialisieren

### Passende Fehlercodes
- von FUSE aufgerufene Funktionen geben zurück bei...
    - ...Erfolg: `value` >= 0
    - ...Fehler: `-errorCode`
- Fehlercodes definiert in `errno.h`
    - ENOSYS
        - Funktion nicht implementiert
    - EROFS
        - Read-Only Dateisystem
    - EPERM
        - Operation nicht erlaubt
    - EACCES
        - Zugriff verweigert
    - ENOENT
        - Eintrag existiert nicht
    - EIO
        - E/A-Fehler
    - EEXIST
        - Eintrag existiert bereits
    - ENOTDIR
        - Eintrag ist kein Verzeichnis
    - EISDIR
        - Eintrag ist ein Verzeichnis
    - ENOTEMPTY
        - Verzeichnis ist nicht leer
