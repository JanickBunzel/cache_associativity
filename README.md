# cache_associativity

## Notes
- Aufgabe richtig lesen und Praktikumsordnung? -> wissen ist die Macht
- Complaints -> schleimen + Argumentation + Höfflichkeit

### Code
- keine Branches (sowieso egal?)
- immer bei der Rechnerhalle schauen (compiled das Projekt? -> sonst 0%)
- Codestil ist wichtig -> gute Kommentare etc.
- meistens code nicht gut in der Präsentation -> einfach die Gatter/Schalter einblenden
- nicht alles über ein Modul machen! -> mehrere Module das unterschiedliche Aufgaben löst

- Schaltkreisanalyse: Tracefile -> Performanz -> Reads and writes

### Präsentation
- 16 x 9 Format -> Folien
- Farben mit guten Kontrast
- Foliennummer -> für Fragen
- 15 Minuten -> 5 Min pro Person
- Uhr mitnehmen


### Prüfung & Abgabe
- Jeder sollte zu allem fragen beantworten können (man muss über alles bescheid wissen) -> wichtig für die Prfüung
- Projektbericht: 400 Wörter -> einfach für das Verständnis -> bringt nur 1 oder 2 Punkte -> individueller Part sollte vorhanden sein
- Projektbericht im Markdown Format


# TODO

### Recherche (3.1, s.3):
- [x] übliche Größen
- [ ] direkt vs 4-fach
- [ ] Speicherzugriffsverhalten eines speicherintensiven Algorithmus -> .csv Dateien mit Beispielen
- [ ] Mit eigener Simulation Verhalten in Bezug auf Zugriffszeiten beobachten
- [ ] Ergebnisse in Readme dokumentieren


### C Rahmenprogramm (3.2, s.4):
- [x] Parameter aus Konsole lesen
- [x] Fehlerhafte Optionen korrekt abfangen und entsp. `stderr` ausgeben
- [x] Requests aus .csv eingabedatei in Request array lesen
- [ ] Default Werte für Optionen setzen
- [ ] Simulation starten (run_simulation in c++)


### SystemC Programm (3.3, s.5):
- [ ] Makefile erstellen, welches eine klare ausführbare main datei erstellt
- [ ] Tracefile während der Simulation erstellen
- [ ] run_simulation implementieren und cache mit requests bearbeiten

### Abzugebende Dateien (4, s.7):
- `Readme.md` : Dokumentation/Bericht + persönliche Abschnitte
- `Makefile`
- `src/` : Quellcode mit Rahmenprogramm und Simulation
- `slides/` :
	- Präsentationsfolien und weiteres Material
	- `slides.pdf` Einzige Datei für die Präsentation

### Finish Up:
- [ ] Checkup: Fehlerhafte Optione abfangen

### Offene Fragen Julian
- Wie groß ist eine Cachzeile?
- Was muss im Tracefile angeziegt werden?
- Wie muss ein write funktioneren, es werden ja keine Daten gespeichert, da wir ja keine Hintergrundspeicher haben, aus dem bei einem Cachmiss Daten geladen werden können.
