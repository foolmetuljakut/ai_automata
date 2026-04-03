# 🧪 Epic 4 - Manueller Test Plan

## 📖 Setup

**Szenario:**
- 1 Zelle mit NN (5 Input → 8 Hidden → 2 Output)
- ~20 Futter-Punkte auf Spielfläche
- Zelle hat Random-Initialisierten Gewichte

---

## ✅ **TEST CASE 1: Zelle steuert Futter an**

**Ziel:** Verifiziere dass sich die Zelle basierend auf NN-Output bewegt und zum Futter ausrichtet.

### Schritte:

1. **Starte App** `./build/ai_automata`
   - Spielfläche sollte sichtbar sein
   - 1 grüner Kreis (Zelle) in der Mitte
   - ~20 gelbe Punkte (Futter) verteilt

2. **Beobachte Standard-Verhalten** (ohne Start)
   - Zelle sollte NICHT bewegen
   - Zelle sollte NICHT rotieren

3. **Klicke Start/Pause Button**
   - Zelle sollte sich SOFORT zu bewegen anfangen
   - Zelle sollte sich zum nächsten Futter drehen

4. **Beobachte mind. 10 Sekunden**
   - ✅ Zelle bewegt sich kontinuierlich
   - ✅ Zelle rotiert/dreht sich zum Futter
   - ✅ Weiße Linie (Richtungsmarkierung) zeigt Bewegungsrichtung
   - ✅ Sensorik-Kanäle sollten Orange-Linien zeigen (wenn Zeige Sensorik ON)

5. **Prüfe Sensorik (optional)**
   - Aktiviere "Zeige Sensorik" Checkbox
   - Orange Linien sollten von der Zelle ausgehen
   - Linien sollten mit Zelle-Rotation mitdrehen

6. **Prüfe Motorik (optional)**
   - Aktiviere "Zeige Motorik" Checkbox
   - Gelber Pfeil sollte die Bewegungsrichtung zeigen
   - Pfeil sollte mit Zelle-Rotation mitdrehen

### Erwartete Ergebnisse:

- ✅ Zelle bewegt sich ständig (nicht stillstehend)
- ✅ Zelle dreht sich zum Futter
- ✅ White Line (Richtung) ist sichtbar und rotiert
- ✅ Orange Linien (Sensorik) folgen mit Zelle
- ✅ Gelber Pfeil (Motorik) zeigt Bewegungsrichtung
- ✅ Keine Crashes oder Fehler

---

## 📊 Detaillierte Beobachtungs-Punkte

### Bewegung:
- [ ] Zelle bewegt sich in verschiedene Richtungen
- [ ] Bewegung ist nicht linear (zeigt NN-Output Variabilität)

### Rotation:
- [ ] Weiße Linie dreht sich mit der Zelle
- [ ] Rotation ändert sich kontinuierlich (nicht frozen)

### Sensorik:
- [ ] Orange Linien sind sichtbar (wenn toggle ON)
- [ ] Linien rotieren mit der Zelle
- [ ] Linien sind kurz (Sensor Range)

### Motorik:
- [ ] Gelber Pfeil ist sichtbar (wenn toggle ON)
- [ ] Pfeil zeigt in Bewegungsrichtung
- [ ] Pfeil dreht sich mit der Zelle

### Learning (optional advanced):
- [ ] Nach ~5 Sekunden: Futter sollte näher kommen
- [ ] Nach ~10 Sekunden: Zelle sollte "besser zielen"
- [ ] Flexibilität sollte über Zeit sinken (sichtbar in Bewegungs-Variabilität)

---

## 🚨 Fehler-Checkliste (Falls was nicht passt)

- ❌ Zelle bewegt sich nicht → **Check: ist Simulation RUNNING?**
- ❌ Zelle rotiert nicht → **Check: ist Rotation-Update implementiert?**
- ❌ Sensorik-Linien sichtbar aber nicht rotierend → **Check: Rotation wird bei DrawCellSensors genutzt?**
- ❌ Zelle nur in gleicher Richtung → **Check: NN-Weights random initialisiert?**

---

## 📝 Dein Feedback Format

Nach 10+ Sekunden Beobachtung:

- **/** = Alles funktioniert! Zelle bewegt/dreht sich, steuert Futter an
- **x** = Fehler! [Beschreib was nicht passt]
- **?** = Unsicher, kann folgende Frage stellen: [Frage]

---

## 💡 Zusätzliche Tests (optional)

### Mehrere Zellen:
1. Erhöhe "Zellanzahl" Slider auf 3
2. Starte Simulation
3. Beobachte: Alle 3 sollten unabhängig sich bewegen

### Parameter-Änderung zur Laufzeit:
1. Starte Simulation (1 Zelle)
2. **Während läuft**: Ändere "Sensorik Visualisierung"
3. Orange Linien sollten an/ausschalten ohne Crash

### Reset:
1. Click Stop
2. Zelle sollte stoppen
3. Click Start
4. Zelle sollte wieder ganz neuen Bewegungsmustern folgen

---

## ✅ Test-Abschluss

Wenn alles passt, antworte einfach: **(/)**

Dann machen wir die finale Pipeline und Epic 4 ist DONE! 🎉

