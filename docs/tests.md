# Tests

## Unittests

Mit den [Unittests](../tests/unit/test-myfs.cpp) wurden die Hilfsfunktionen getestet.

### TEST_CASE( "intToChar / charToInt" )
Mit diesem Test wurden die Hilfsfunktionen [charToInt](./funktionen.md#int-chartointchar-chars-int-numberofchars) und [intToChar](./funktionen.md#void-inttocharint-number-char-buffer-int-numberofchars). 
Überprüft wurde ob jeweils die richtigen Werte zurückgegeben werden von den zwei Funktionen, bei einem spezifischen Input.



## Funktionale Tests

Die funktionalen Tests sind in dem separaten Repository [myfs-test](https://github.com/robin-hartmann/myfs-test) abgelegt.

### Verwendete Technologien

| | | |
|-|-|-|
| Sprache | [Typescript](https://www.typescriptlang.org/) | Typisiertes Superset von JavaScript, das zu JavaScript kompiliert
| Laufzeitumgebung | [Node.js](https://nodejs.org/) | Eine JavaScript Laufzeitumgebung, die auf Chromes V8 JavaScript Engine aufbaut
| Framework | [AVA](https://github.com/avajs/ava) | Ein futuristischer Testausführer für Node.js

### Integration mit MyFS

[myfs-test](https://github.com/robin-hartmann/myfs-test) ist als [npm Package veröffentlicht](https://www.npmjs.com/package/myfs-test), das in der `package.json` Datei von MyFS als Abhängigkeit definiert ist. Zum Starten der Tests muss nach der Installation der Abhängigkeiten mit `npm install` nur der Befehl `npm run test-functional` ausgeführt werden. Dieser übergibt beim Start die Pfade zu `mkfs.myfs` und `mount.myfs` in Form von Umgebungsvariablen.

Skript, das von `npm run test-functional` aufgerufen wird:
```bash
#!/bin/bash
scriptDir=$(dirname "$0")

binDirRel="$scriptDir/../../cmake-build-debug/bin"
binDirAbs=$(cd "$binDirRel"; pwd)

mkfs="$binDirAbs/mkfs.myfs"
mount="$binDirAbs/mount.myfs"

pushd "$scriptDir/../.." > /dev/null
MYFS_BIN_MKFS="$mkfs" MYFS_BIN_MOUNT="$mount" npx myfs-test
popd > /dev/null
```

### Umfang

- Container
    - Erstellen
    - Einhängen
        - Ohne Inhalt
        - Mit Inhalt
    - Aushängen
- Root-Verzeichnis
    - Einträge abfragen
    - Metadaten überprüfen
- Dateien
    - Erstellen
        - Ohne Inhalt
        - Mit Inhalt
    - Löschen
    - Lesen
    - Schreiben
        - Initial
            - Einzelner Block
            - Mehrere Blöcke
                - Ohne Lücken
                - Mit Lücken
        - Überschreiben
        - Anhängen
    - Metadaten überprüfen

### Umsetzung

Die Tests wickeln alles vollautomatisiert ab - vom Erstellen und Initialisieren des Containers über das Einhängen und Verwenden bis hin zum Aushängen und Aufräumen der erzeugten Dateien. Nur im Fehlerfall werden die erzeugten Dateien beibehalten, um die Ursache nachvollziehen zu können. Sämtliche Dateien (und auch das Verzeichnis zum Einhängen) erhalten eindeutige Namen (vordefiniertes Prefix + zufallsgeneriertes Suffix) und werden im Verzeichnis für temporären Dateien abgelegt.

Generieren der eindeutigen Namen mithilfe des npm Packages [tmp](https://www.npmjs.com/package/tmp):
```typescript
// Modul test.ts
function init(t: TypedExecutionContext, initFiles: FileInfo[] = []) {
    t.context.initFiles = initFiles;
    t.context.cleanupCbs = [];

    // required for mkfs
    t.context.containerPath = getName(t, 'container', '.bin');
    t.context.mkfsOutLogFile = createFile(t, 'mkfs-out-log', '.log');
    t.context.mkfsErrLogFile = createFile(t, 'mkfs-err-log', '.log');

    if (t.context.initFiles.length) {
        t.context.initFilesDir = createDir(t, 'init-files');
    }

    // required for mount
    t.context.mountLogFile = createFile(t, 'mount-log', '.log');
    t.context.mountDir = createDir(t, 'mount');
}

// Modul tmp.ts
export const TMP_BASE_PREFIX = 'myfs';

export function createFile(t: TypedExecutionContext, purpose: string, extension?: string) {
    // fileSync wird von tmp bereitgestellt
    const file = fileSync({ prefix: getFullPrefix(purpose), postfix: extension, keep: true });
    addToCleanup(t, file);
    return resolvePath(file.name);
}

function getFullPrefix(purpose: string) {
    return `${TMP_BASE_PREFIX}-${purpose}-`;
}
```

Ausgabe der Pfade im Fehlerfall:
```
  ✖ write › only half of block four and five after remount
    write › cleanup for only half of block four and five
    ℹ due to failure, created files won't be cleaned up automatically {
        containerFile: '/private/var/folders/mc/3l3d_vk17yl41ww8np5hvllwjhfcdn/T/myfs-container-564656P0BbTnb4eFN.bin',
        mkfsErrLogFile: '/private/var/folders/mc/3l3d_vk17yl41ww8np5hvllwjhfcdn/T/myfs-mkfs-err-log-56465JAOFnoA1TM13.log',
        mkfsOutLogFile: '/private/var/folders/mc/3l3d_vk17yl41ww8np5hvllwjhfcdn/T/myfs-mkfs-out-log-56465G0uQ9UeuONIh.log',
        mountDir: '/private/var/folders/mc/3l3d_vk17yl41ww8np5hvllwjhfcdn/T/myfs-mount-56465p1ula6Fsh8nD',
        mountLogFile: '/private/var/folders/mc/3l3d_vk17yl41ww8np5hvllwjhfcdn/T/myfs-mount-log-56465ORVVY8jWbYAA.log',
      }
    ℹ to delete all files created by myfs-test, just run "rm -rf /var/folders/mc/3l3d_vk17yl41ww8np5hvllwjhfcdn/T/myfs*"
```

Zudem wird von AVA eine genaue Beschreibung der Fehlerursache ausgegeben:
```
  write › only half of block four and five

  /Users/robin.hartmann/dev/git/myfs-test/src/util/fs.ts:61

   60:   t.is(a.length, b.length, message);        
   61:   t.is(a.toString(), b.toString(), message);
   62: }                                           

  after remount

  Difference:

  - `MYFS
  + `Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam cursus odio sed tortor euismod, eu viverra lacus congue. Cras in metus quis felis porttitor pharetra. Nulla eget tellus ac arcu mollis pulvinar a sit amet velit. Praesent mollis quam eget ornare imperdiet. Pellentesque ornare dolor vel dapibus euismod. In suscipit purus et libero feugiat dapibus. Aenean aliquam rhoncus volutpat. Donec cursus erat egestas metus finibus, quis varius risus rutrum. Duis rhoncus rutrum mi quis mattis. Pellentesque iaculis urna dolor, eu elementum ipsum euismod ac. Nam consequat tristique metus vitae facilisis. Aliquam volutpat enim mi, eu suscipit tortor efficitur eu. In vel turpis augue. Fusce varius magna iaculis eros tincidunt euismod. Cras vel orci felis. Aliquam magna diam, suscipit eu accumsan in, aliquet id lacus. Nulla elit turpis, pharetra et odio vitae, rhoncus consequat nulla. Proin scelerisque dolor vel consectetur luctus. Nulla malesuada dolor vel ex cursus placerat. Quis varius nibh.␊

  isEqual (src/util/fs.ts:61:5)
  src/util/fs.ts:99:5
  step (transpiled/src/util/fs.js:32:23)
  Object.next (transpiled/src/util/fs.js:13:53)
  fulfilled (transpiled/src/util/fs.js:4:58)
```

Automatisches Aufräumen der erzeugten Dateien bei erfolgreicher Durchführung:
```typescript
// Modul tmp.ts
function addToCleanup(t: TypedExecutionContext, tmpRef: TmpRef) {
    let cleanupCb: CleanupCb;

    if (typeof tmpRef === 'string') {
        cleanupCb = () => unlinkSync(tmpRef);
    } else {
        cleanupCb = tmpRef.removeCallback;
    }

    t.context.cleanupCbs.push(cleanupCb);
}

// Modul test.ts
function cleanup(t: TypedExecutionContext) {
    let error;

    if (t.context.success) {
        try {
            t.context.cleanupCbs.forEach(cleanupCb => cleanupCb());
            return;
        } catch (e) {
            error = e;
        }
    }
    // ...
}
```

Ausgabe bei erfolgreicher Durchführung:
```
Running functional tests
Testing...

  ✔ empty › root directory has proper attributes
  ✔ timestamps › atime, ctime and mtime are initialized when a file is created
  ✔ timestamps › (only) atime is updated when file is read
  ✔ timestamps › atime, ctime and mtime are updated when file content is modified
  ✔ basic › can create file without content
  ✔ basic › can create file with content
  ✔ empty › contains nothing
  ✔ basic › can overwrite files
  ✔ basic › can append to file
  ✔ basic › all files have proper attributes
  ✔ basic › can delete file
  ✔ write › entire first two blocks + 1
  ✔ write › entire first five blocks
  ✔ write › entire first five blocks - 1
  ✔ write › entire first five blocks + 1
  ✔ write › first block and half of second
  ✔ write › first block and half of fifth
  ✔ write › only second block
  ✔ write › only third and fifth block
  ✔ write › entire first block + 1
  ✔ write › entire first two blocks
  ✔ write › entire first two blocks - 1
  ✔ basic › changes are persisted (575ms)
  ✔ write › only sixth and seventh block and blocks ten to twelve
  ✔ write › entire first block
  ✔ write › entire first block - 1
  ✔ write › only half of block four and five

  27 tests passed

OK
```

Jeder Testfall wird in einem separaten Container durchgeführt, sofern nicht anders definiert, um gegenseitige Beeinflussungen der Testfälle zu vermeiden.

Test Suite mit unabhängigen Testfällen zum Testen eines leeren Containers:
```typescript
// initializeTest ist nicht parametrisiert
// dadurch ist sharedContext = false
// und die Testfälle erhalten jeweils eigene Container
const test = initializeTest();

// Testfälle werden asynchron in beliebiger Reihenfolge ausgeführt
test('root directory has proper attributes', validateRootAttrs);

test('contains nothing', (t) => {
    const entries = readdirSync(resolve(t));

    t.is(entries.length, 0);
});
```

Test Suite mit abhängigen Testfällen zum Testen von Lesen und Schreiben:
```typescript
// initializeTest mit sharedContext = true
// wodurch sich alle Testfälle denselben Container teilen
const test = initializeTest(true);
const fileName1 = 'neuedatei1.txt';
const fileName2 = 'neuedatei2.txt';
const content1 = 'Hello World';
const content2 = 'Hello World 2';

// Testfälle werden synchron in Serie ausgeführt
test.serial('can create file without content', (t) => {
    const path = resolve(t, fileName1);

    closeSync(openSync(path, 'w'));
    t.is(readFileSync(path).toString(), '');
});

test.serial('can create file with content', (t) => {
    const path = resolve(t, fileName2);

    writeFileSync(path, content1);
    t.is(readFileSync(path).toString(), content1);
});

test.serial('can overwrite files', (t) => {
    const path1 = resolve(t, fileName1);
    const path2 = resolve(t, fileName2);

    writeFileSync(path1, content1);
    writeFileSync(path2, content2);
    t.is(readFileSync(path1).toString(), content1);
    t.is(readFileSync(path2).toString(), content2);
});

test.serial('all files have proper attributes', t => validateFilesAttrs(t, 2));

test.serial('changes are persisted', async (t) => {
    await remount(t);

    const path2 = resolve(t, fileName2);

    validateFilesAttrs(t, 1);
    t.is(readFileSync(path2).toString(), content2);
});
```

Validieren der Metadaten von Dateien:
```typescript
export function validateFilesAttrs(t: TypedExecutionContext, expectedFileCount?: number) {
    const userInfo = getUserInfo();
    const files = getFiles(t, expectedFileCount);

    files.forEach((stats) => {
        t.is(stats.mode & fsConstants.S_IFMT, fsConstants.S_IFREG);
        t.is(stats.mode & ~fsConstants.S_IFMT, 0o644);
        t.is(stats.uid, userInfo.uid);
        t.is(stats.gid, userInfo.gid);
        t.is(stats.nlink, 1);
    });
}

export function getFiles(t: TypedExecutionContext, expectedFileCount?: number) {
    const files = getEntriesStats(t)
        .filter(stats => stats.isFile());

    if (expectedFileCount !== undefined) {
        t.is(files.length, expectedFileCount);
    }

    return files;
}

export function getEntriesStats(t: TypedExecutionContext) {
    return readdirSync(t.context.mountDir)
        .map(entryName => statSync(resolve(t, entryName)));
}
```

Test Suite mit unabhängigen Testfällen zum Testen von komplexen Schreibvorgängen:
```typescript
const test = initializeTest();
const blockSize = 512;
const fileName = 'lorem-ipsum.txt';
const shouldRemount = true;

const simpleWrite = (byteCount: number) =>
    genericSimpleWrite(fileName, byteCount, shouldRemount);
const fragmentedWrite = (fragmentedByteCounts: FragmentedByteCount[]) =>
    genericFragmentedWrite(fileName, fragmentedByteCounts, shouldRemount);

test('entire first block', simpleWrite(blockSize));
test('entire first block - 1', simpleWrite(blockSize - 1));
test('entire first block + 1', simpleWrite(blockSize + 1));

test('first block to half of second', simpleWrite(blockSize * 1.5));
test('first block to half of fifth', simpleWrite(blockSize * 4.5));

test('only second block', fragmentedWrite([{
    gapLength: blockSize,
    byteCount: blockSize,
}]));
test('only sixth and seventh block and blocks ten to twelve', fragmentedWrite([
    {
        gapLength: blockSize * 5,
        byteCount: blockSize * 2,
    },
    {
        gapLength: blockSize * 2,
        byteCount: blockSize * 3,
    },
]));
```

Bei den komplexen Schreibvorgängen wird eine Textdatei mit einem 1000 Zeichen langen Lorem Ipsum als Datenquelle verwendet:
```typescript
// Modul fs.ts
export function simpleWrite(fileName: string, byteCount: number, shouldRemount: boolean = false) {
    return fragmentedWrite(fileName, [{ byteCount, gapLength: 0 }], shouldRemount);
}

export function fragmentedWrite(
    fileName: string,
    fragmentedByteCounts: FragmentedByteCount[],
    shouldRemount: boolean = false,
) {
    return async function (t: TypedExecutionContext) {
        const path = resolve(t, fileName);
        // open file for reading and writing
        // the file is created (if it does not exist) or it fails (if it exists)
        const fd = openSync(path, 'wx+');
        let entireData = Buffer.from('');
        let lastFragmentEnd = 0;

        for (const fragment of fragmentedByteCounts) {
            const data = generateData(fragment.byteCount);
            const zeroes = Buffer.alloc(fragment.gapLength);
            const position = lastFragmentEnd + fragment.gapLength;

            lastFragmentEnd = position + data.length;
            entireData = Buffer.concat([entireData, zeroes, data]);
            writeSync(fd, data, 0, data.length, position);
        }

        isEqual(t, readFileSync(fd), entireData, 'before remount');
        closeSync(fd);

        if (!shouldRemount) {
            return;
        }

        await remount(t);
        isEqual(t, readFileSync(path), entireData, 'after remount');
    };
}

export function isEqual(t: TypedExecutionContext, a: Buffer, b: Buffer, message?: string) {
    t.is(a.length, b.length, message);
    t.is(a.toString(), b.toString(), message);
}

// Modul data.ts
const LOREM_IPSUM_NAME = 'lorem-ipsum-1000.txt';
const PKG_ROOT = getPkgRoot(__dirname) as string;
const PKG_RES = resolve(PKG_ROOT, 'res');
const LOREM_IPSUM = readFileSync(resolve(PKG_RES, LOREM_IPSUM_NAME));

export const generateData = (byteCount: number) => {
    const data = Buffer.allocUnsafe(byteCount);
    let remainingBytes = byteCount;

    while (remainingBytes > 0) {
        remainingBytes -= LOREM_IPSUM.copy(
            data,
            byteCount - remainingBytes,
            0,
            Math.min(LOREM_IPSUM.byteLength, remainingBytes),
        );
    }

    assert(!remainingBytes, 'remainingBytes must be 0');

    return data;
};
```
