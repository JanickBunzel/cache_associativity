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
- [ ] Mind. ein Fallbeispiel für das Speicherzugriffsmuster unseres ausgewählten Algorithmus
- [ ] Ergebnisse in Readme dokumentieren

### C Rahmenprogramm (3.2, s.4):
- [x] Parameter aus Konsole lesen
- [x] Fehlerhafte Optionen korrekt abfangen und entsp. `stderr` ausgeben
- [x] Requests aus .csv eingabedatei in Request array lesen
- [x] Simulation starten (run_simulation in c++)
- [x] Hex oder Dezimal als Adresse oder Wert in CSV richtig verarbeiten
- [x] Default Werte für Optionen setzen


### SystemC Programm (3.3, s.5):
- [x] run_simulation connecten
- [ ] **Makefile** erstellen, welches eine klare ausführbare main datei erstellt
- [ ] **Tracefile** während der Simulation erstellen
- [ ] **Cache** implementieren: direct & fourway

### Abzugebende Dateien (4, s.7):
- `Readme.md` : Dokumentation/Bericht + persönliche Abschnitte
- `Makefile`
- `src/` : Quellcode mit Rahmenprogramm und Simulation
- `examples/` : Verzeichnis für csv-Fallbeispiele
- `slides/` :
	- Präsentationsfolien und weiteres Material
	- `slides.pdf` Einzige Datei für die Präsentation

### Finish Up:
- [ ] SystemC Path aus _evironment variable_ lesen (statt zurzeit workspace folder) -> Zulip
- [ ] Auf **Rechnerhalle** compilieren!
- [ ] Consistency: Code Design/Format, Sprache (Janez Rotman: Bevorzugt Englischer Code), Comments
- [ ] Alle TODO's im Code aufräumen

### Offene Fragen
- Was muss im Tracefile angezeigt werden?
- Wie muss ein write funktioneren, es werden ja keine Daten gespeichert, da wir ja keine Hintergrundspeicher haben, aus dem bei einem Cachmiss Daten geladen werden können.
- Wie funktioniert ein Cache Hit beim schreiben
- Write Request: Wann cache hit und wann miss?

### Code Style
```
int someVariable;

void this_is_a_method()
{
    // Code
}
```
