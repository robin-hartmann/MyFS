# MyFS

## Aufgabenstellung

Im Rahmen des Betriebsysteme Labors im 3. Semester an der Hochschule Karlsruhe - Technik und Wirtschaft war es die Aufgabe ein Dateisystem zu implementieren.
Dieses ist in der Lage Dateien zu öffnen, zu schreiben, zu lesen, zu verändern und zu löschen. 
Um diese Aufgabe zu bewerkstelligen, wurde auf [FUSE-Filesystem in Userpsace](https://de.wikipedia.org/wiki/Filesystem_in_Userspace) zurückgegriffen, da ansonsten Kernel-Programmierung notwendig ist. 
Die Programmierung des Dateisystems ist in C++. 

Der erste Schritt war es sich das [Design](docs/design.md) des Dateisystems zu überlegen. Danach wurde das Design in Code umgesetzt. Ausserdem wurden zum Testen des Dateisystems ausführliche [Tests](docs/tests.md) geschrieben.

## Voraussetzungen
Um dieses Projekt bauen und ausführen zu können, muss die folgende Software installiert sein:
* fuse
* pkg-config

Zum Ausführen der funktionalen Tests muss zudem Folgendes installiert sein:
* [Node.js](https://nodejs.org) - JavaScript Laufzeitumgebung

### Installation auf macOS
1. [osxfuse](https://osxfuse.github.io/) herunterladen von [hier](https://github.com/osxfuse/osxfuse/releases) und installieren
1. [Homebrew](https://brew.sh/) installieren wie [hier](https://docs.brew.sh/Installation) beschrieben
1. Homebrew verwenden um das Package [`pkg-config`](https://brewformulas.org/Pkg-config) zu installieren:<br>
  `brew install pkg-config`

## Dokumentation

### [Design](docs/design.md)

### [Spezifikation](docs/spec.md)

### [Funktionen](docs/funktionen.md)

### [Tests](docs/tests.md)


### [Notizen zur Aufgabenstellung](docs/notes.md)

## Ausführen der Tests

⚠️ Die MyFS Binärdateien werden NICHT automatisch gebaut wenn man die Tests ausführt. Falls also der Quellcode von MyFS geändert wurde oder überhaupt noch nicht gebaut wurde, dann müssen die Binärdateien vor dem Ausführen der Tests gebaut werden. Ansonsten werden stattdessen die alten Binärdateien getestet oder die Tests schlagen komplett fehl. ⚠️

### Über das Terminal

Einen (oder mehrere) der folgenden Befehle in dem Verzeichnis ausführen, das die `package.json` Datei enthält:

```bash
# Unittests
npm run test-unit

# funktionale Tests
npm install # nur notwendig bei der initialen Installation oder für Updates
npm run test-functional

# manuelle funktionale Tests
npm run test-functional-manual

# alle Tests außer die manuellen
npm test
```
