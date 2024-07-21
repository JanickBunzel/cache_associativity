# Cache Simulation und Analyse (A11) - Projektbericht


## Aufgabenstellung
Im Rahmen des Moduls Grundlagenpraktikum "Rechnerarchitektur" haben wir uns mit der Simulation und Analyse von Caches beschäftigt.

Ziel des Projekts war es, übliche Größen und Speicherzugriffszeiten von Caches zu recherchieren und ein Programm zu entwickeln, das Zugriffe auf einen Direct-Mapped-Cache und einen 4-fach assoziativen Cache simuliert. Die Ergebnisse wurden ausgewertet. Die Implementierung des Rahmenprogrammes erfolgte in C, die Simulation mit SystemC in C++.


## Zusammenfassung der Erkenntnisse der Literaturrecherche
Es gibt verschiedene Levels von Caches, die sich in Größe und Latenz unterscheiden (typisch: L1, L2, L3). Die Größen variieren von kleinen, schnellen Caches (L1: 8KB - 64KB) bis zu größeren wie dem L3-Cache (2MB - 8MB).

Sie reduzieren die Speicherzugriffszeiten signifikant, da sie Daten zwischenspeichern. Die Latenzen variieren dabei von 1-4 Taktzyklen für L1-Caches bis zu 30-40 Takten für L3-Caches. Die Latenz für den Hauptspeicher (RAM) beträgt ca. 100-200 Taktzyklen.

Es gibt verschiedene Assoziativitätsgrade von Caches. Wir haben den direkt-mapped Cache (1-fach assoziativ) dem 4-fach assoziativen Cache gegenübergestellt.

Die Caches haben wir nach dem _Write-Allocate_ Verhalten implementiert.
Bei einem Cache-Miss wird die entsprechende Cacheline in den Cache geladen (bei einem übergreifenden Offset entspricht dies zusätzlich einer zweiten Cacheline).

## Messumgebung und Ergebnisse der Analyse des Projekts
Für die Analyse haben wir Speicherzugriffsmuster mit Skripten generiert und so einerseits Validierungsdurchläufe zum Testen und realitätsnahe Simulationen durchgeführt. (Fallbeispiele im Ordner `examples/`)

Diese umfassen ua. die Iteration und Sortierung von Arrays, Matrixmultiplikationen und Extrembeispiele welche die Stärken der beiden Cache-Arten zeigen.

Die Analyse ergab folgende Ergebnisse:

Die 4-fach assoziativen Caches erzielen bei großen Iterationsbereichen und häufigen wiederholten Zugriffen eine deutlich höhere Hit-Rate, da sie durch die Assoziativität weniger Verdrängungen durchführen müssen. Dies erfordert jedoch eine komplexere Architektur und kann zu längeren Zugriffszeiten führen.

Dahingegen sind Direct-Mapped Caches einfach zu implementieren und bieten schnelle Zugriffszeiten. Sie sind besonders bei räumlich verteilten Zugriffen effizient. Aufgrund ihres einfachen Designs benötigen sie weniger Hardware-Ressourcen.


## Persönliche Beiträge

### Michael
Hauptsächlich habe ich theoretische Informationen über Caches anhand von realen Beispielen recherchiert und dazu Grafiken erstellt. Weiterhin habe ich die Berechnung der Gatteranzahl implementiert, Verifikationsarbeiten am Cache durchgeführt und die LRU-Strategie umgesetzt.


### Julian
Ich habe mich mit der SystemC-Bibliothek auseinandergesetzt, die Architektur von Cache, CPU und Speicher konzipiert und Prototypen erstellt. Ich habe die Speicherverwaltung sowie die Cache-Implementierung realisiert. Zudem habe ich zur Entwicklung von Tests und Zugriffsmustern beigetragen.

### Janick
Umgesetzt habe ich als erstes das Rahmenprogramm, da dies der Einstiegspunkt des Programms ist.
Im Verlauf des Projektes habe ich die Simulationsmodule implementiert und die Analyse (Algorithmen & Verifikation) und Struktur des Projektes vorangetrieben (Git, Ordnerstruktur, Format, etc.)
