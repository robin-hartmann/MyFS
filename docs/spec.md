# Spezifikation

## Teil 1: Read-Only Dateisystem

### `mkfs.myfs` erstellen

Erstellt ein Block Device (Container) und initialisiert es.

- wird wie folgt aufgerufen:<br>
  `mkfs.myfs <container-datei> [datei1, datei2, ...]`
    - ausgewählte Dateien werden alle im Root-Verzeichnis des Containers platziert unabhängig von dem Ordner aus dem sie stammen#
        - die Anzahl der Dateien ist beschränkt auf `NUM_DIR_ENTRIES`
        - wirft `EEXIST` falls eine bereits existierende Datei hinzugefügt wird
    - die meisten Attribute (Name, Größe, `atime`, `mtime`, `ctime`) werden von der Originaldatei kopiert
        - die Länge des Dateinamens ist beschränkt auf `NAME_LENGTH`
    - Besitzer (`uid` und `gid`) wird abhängig vom aktuallen Nutzer gesetzt
    - Zugriffsrechte (`mode`) der Dateien sind gesetzt auf `S_IFREG | 0644`

### `mount.myfs` erstellen

Hängt das Block Device ein.

- wird wie folgt aufgerufen:<br>
  `mount.myfs <container-datei> <log-datei> <ziel-verzeichnis>`
- das Root-Verzeichnis gibt die folgenden Werte zurück für...
    - ...seine eigenen Zugriffsrechte (`mode`): `S_IFDIR | 0755`
    - ...seinen eigenen Verknüpfungen (`nlink`): `2`
    - ...Verknüpfungen einer Datei (`nlink`): `1`
- die Anzahl der gleichzeit geöffneten Dateien ist beschränkt auf `NUM_OPEN_FILES`
- wirft `EACCES` falls versucht wird Dateien oder Verzeichnisse hinzuzufügen, zu ändern oder zu löschen
- die folgenden Operationen müssen implementiert werden (minimal):
    - Container
        - `MyFS::fuseInit()`
        - `MyFS::fuseRelease()`
    - Verzeichnis
        - `MyFS::fuseOpendir()`
            - kann zusammen mit dem Container geöffnet werden (fuseInit())
        - `MyFS::fuseReaddir()`
            - wirft `ENOTDIR` falls es für ein anderes Verzeichnis außer Root aufgerufen wird
        - `MyFS::fuseReleasedir()`
            - kann zusammen mit dem Container freigegeben werden (fuseRelease())
        - `MyFS::fuseGettatr()`
            - wirft `ENOENT` falls es für einen Eintrag aufgerufen wird, der nicht existiert
    - Datei
        - `MyFS::fuseOpen()`
        - `MyFS::fuseRead()`
            - der zuletzt gelesene Block soll gepuffert werden
        - `MyFS::fuseRelease()`
        - relevant Fehler
            - wirft `ENFILE` falls zu viele Dateien offen sind
            - wirft `ENOENT` falls eine Datei nicht gefunden wurde
            - wirft `EBADF` falls `fileInfo->fh` auf keine offene Datei zeigt
            - wirft `ENXIO` falls versucht wird auf einen Index außerhalb der Grenzen der Datei zuzugreifen
- für jede geöffnete Datei sollte der zuletzt gelesene Block gepuffert werden

## Teil 2: Read-Write Dateisystem

### `mkfs.myfs` erweitern

- legt ein leeres Block Device mit einer fixen Größe an (mindestens 30 MB)
    - optional kann es weiterhin mit Dateien initialisiert werden

### `mount.myfs` erweitern

- Dateien auf dem Block Device können gelesen, geändert (überschrieben, angehängt) sowie gelöscht werden und neue Dateien können hinzugefügt werden
- wirft `EACCES` falls versucht wird ein neues Verzeichnis anzulegen
- die folgenden Operationen müssen nun zusätzlich implementiert werden (minimal):
    - neue Datei anlegen
        - `MyFS::fuseMknod()`
            - wird aufgerufen falls `MyFS::fuseGetattr()` zuvor `ENOENT` zurückgegeben hat
            - wirft `EEXIST` falls die Datei bereits existiert
            - wirft `ENOSPC` falls nicht genügend Speicherplatz vorhanden ist
        - `MyFS::fuseCreate()`
            - nur wenn notwendig
    - Dateiinhalt (über)schreiben
        - `MyFS::write()`
            - die Datie muss zuvor geöffnet sein, es sollte also `fileInfo` verwendet werden
            - muss gepuffert werden
            - wirft `EBADF` falls `fileInfo->fh` auf keine offene Datei zeigt
            - wirft `ENOSPC`falls nicht genügend Speicherplatz vorhanden ist
    - Datei löschen
        - `MyFS::unlink()`
            - wirft `ENOENT` falls die Datei nicht existiert
