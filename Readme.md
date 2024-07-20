# Cache Simulation und Analyse (A11) - Projektbericht


## Aufgabenstellung
Im Rahmen des Moduls Grundlagenpraktikum "Rechnerarchitektur" haben wir uns mit der Simulation und Analyse von Caches beschäftigt.

Ziel des Projekts war es, übliche Größen und Speicherzugriffszeiten zu recherchieren und ein Programm zu entwickeln, dass die Zugriffe auf einen Direct-Mapped-Cache und einen 4-fach assoziativen Cache simuliert, welche wir auswerten. Die Implementierung des Rahmenprogrammes sollte in C erfolgen, die Simulation mit SystemC in C++.


## Zusammenfassung der Erkenntnisse der Literaturrecherche
Es gibt verschiedene Levels von Caches, die sich in Größe und Latenz abwägen. (L1, L2, L3) Die Größen variieren dabei von einem kleinen, schnellen Cache (L1: 8KB - 64KB) bis größeren wie dem L3-Cache (2MB - 8MB).

Sie reduzieren die Speicherzugriffszeiten signifikant, da sie Daten zwischenspeichern. Die Latenzen variieren dabei von 1-4 Takten für L1-Caches bis hin zu 30-40 Takten für L3-Caches.

Es gibt verschiedene Assoziativitätsgrade von Caches, währen wir den direkt abgebildeten Cache (1-fach assoziativ) dem 4-fach assoziativen Cache gegenüberstellen.


## Methodik und Messumgebung der verwendeten Analysemethoden
Für die Analyse haben wir Speicherzugriffsmuster mit Skripten generiert und so realitätsnahe Simulationen durchgeführt. Dabei zeigen ua. die Iteration oder Sortierung von Arrays, Matrixmultiplikationen und Extrembeispiele die Stärken der Cache-arten.
Außerdem haben wir Validierungsläufe durchgeführt, um die Korrektheit der Simulation zu überprüfen.
Alle Fallbeispiele sind im Ordner `examples/` zu finden.

Das Abstraktionslevel in SystemC haben wir gewählt, um die Kommunikation zwischen den Modulen dessen Latenzen zu simulieren. Die Caches haben wir nach dem _Write-Allocate_ Verhalten implementiert.
Bei dem bei einem Schreibzugriff wird die gesamte Cacheline, teilweise auch die zweite Cacheline bei einem übergreifenden Offset, in den Cache geladen.


## Ergebnisse des Gesamtprojekts
Die Ergebnisse haben zwei Richtungen gezeigt: Die Performanz der Caches hinsichtlich der Hitrate und die Speicherzugriffszeiten.

Die 4-fach assoziativen Caches erzielen bei großen Iterationsbereichen und häufigen wiederholten Zugriffen eine deutlich höhere Hitrate, da sie durch die Assoziativität weniger Verdrängungen durchführen müssen. Die Assotiativität führt jedoch zu einer höheren Komplexität und damit zu längeren Zugriffszeiten.

Dahingegen sind Direct-Mapped Caches einfach zu implementieren und bieten schnelle Zugriffszeiten. Sie sind besonders bei räumlich verteilten Zugriffen effizient. Aufgrund ihres einfachen Designs benötigen sie außerdem weniger Hardware-Ressourcen.


## Persönliche Beiträge

### Michi
moin

### Julian
servus

### Janick
hallo
