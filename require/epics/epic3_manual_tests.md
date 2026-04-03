# 🧪 Epic 3 - Manueller Test-Plan

## 📖 Epic 3 Anforderungen Zusammenfassung

### **GUI-Änderungen:**
1. ✅ Start/Stop Buttons → Einzelner **Start/Pause Toggle Button**
   - Funktionalität bleibt gleich, einfacher zu bedienen
   
2. ✅ "Parameter:" → "Zell-Parameter:" (Umbenennung)

3. ✅ **Neue Simulation-Parameter Section** (OBEN im Dashboard):
   - Anzahl der Zellen
   - Anzahl der Futterstückchen
   - Dauer einer Generation (Zeit)

4. ✅ **Layout-Reorganisation:**
   ```
   Dashboard:
   [Simulations-Parameter]  ← NEU, oben
   [Start/Pause Button]      ← Oben
   [Save Button]             ← Oben
   ─────────────────────
   [Zell-Parameter]          ← Umbenannt (war "Parameter:")
   [Zeige Sensorik/Motorik]
   [Status Text]
   ```

5. ✅ **WICHTIG: Parameter wirken sich SOFORT aus!**
   - Zellanzahl ändern → neue Zellen sichtbar auf Spielfläche
   - Futter ändern → mehr/weniger Futter sichtbar
   - Generation-Dauer ändern → Simulation wird schneller/langsamer

---

## 🧪 Manueller Test-Plan für Epic 3

### **Vor den Tests:**
```bash
./pipeline.sh cleanup && ./pipeline.sh configure_cmake && ./pipeline.sh execute_build && ./pipeline.sh run_unit_tests
./build/ai_automata
```

---

## ✅ **TEST CASE 1: GUI Layout - Neue Struktur**

**Ziel:** Verifiziere, dass die neue GUI-Struktur korrekt ist.

### Erwartete Ergebnisse:
- ✅ **Oben im Dashboard:** "Simulations-Parameter" Section
  - Slider für "Zellanzahl" (1-10)
  - Slider für "Futter-Stückchen" (10-100)
  - Slider für "Generations-Dauer" (0.5-10 Sekunden)
  
- ✅ **Darunter:** Start/Pause Button (einzeln, nicht zwei separate)
  
- ✅ **Darunter:** Save Button
  
- ✅ **Darunter:** "Zell-Parameter:" (statt "Parameter:")
  - Slider für Sensor Range, Sensor Angle, Hidden Neurons
  - Checkboxes für Sensorik/Motorik

### Feedback: **/** = OK | **x** = Fehler

---

## ✅ **TEST CASE 2: Start/Pause Toggle Button funktioniert**

**Ziel:** Der neue Toggle-Button funktioniert korrekt.

### Schritte:
1. Klicke Start/Pause → Status = RUNNING, Zelle bewegt sich
2. Klicke erneut → Status = STOPPED, Zelle stoppt
3. Klicke wieder → Status = RUNNING, Zelle setzt sich fort (nicht resetten)

### Feedback: **/** = OK | **x** = Fehler

---

## ✅ **TEST CASE 3: Zellanzahl-Parameter wirkt sich aus**

**Ziel:** Slider "Zellanzahl" erzeugt sofort neue Zellen auf der Spielfläche.

### Schritte:
1. Standardmäßig: 1 grüner Kreis sichtbar
2. Verschiebe "Zellanzahl" auf 3-5
3. **Sofort** sollten mehrere grüne Kreise sichtbar sein (nicht erst nach Click)
4. Jede Zelle hat eigenen gelben Pfeil
5. Optional: Zurück auf 1 → nur noch 1 Zelle

### Feedback: **/** = OK | **x** = Parameter funktioniert nicht

---

## ✅ **TEST CASE 4: Futter-Stückchen-Parameter wirkt sich aus**

**Ziel:** Slider "Futter-Stückchen" ändert sofort Anzahl der Futter-Punkte.

### Schritte:
1. Spielfläche hat gelbe/rote Punkte (Futter)
2. Verschiebe "Futter-Stückchen" runter → weniger Punkte sichtbar
3. Verschiebe runter → mehr Punkte sichtbar
4. Änderung sollte **sofort** passieren

### Feedback: **/** = OK | **x** = Parameter funktioniert nicht

---

## ✅ **TEST CASE 5: Generations-Dauer-Parameter wirkt sich aus**

**Ziel:** Slider "Generations-Dauer" ändert die Simulations-Geschwindigkeit.

### Schritte:
1. Starte Simulation (Start/Pause)
2. Verschiebe "Generations-Dauer" runter (0.5s) → schneller
3. Verschiebe runter (10s) → langsamer
4. Position-Werte sollten schneller/langsamer steigen

### Feedback: **/** = OK | **x** = Parameter funktioniert nicht

---

## ✅ **TEST CASE 6: Zell-Parameter funktionieren noch**

**Ziel:** Alte Zell-Parameter funktionieren wie vorher.

### Schritte:
1. Text "Zell-Parameter:" vorhanden (nicht "Parameter:")
2. Toggle "Zeige Sensorik" an/aus → orange Linien an/aus
3. Toggle "Zeige Motorik" an/aus → gelbe Pfeile an/aus
4. Slider "Sensor Range" verschiebbar

### Feedback: **/** = OK | **x** = Fehler

---

## ✅ **TEST CASE 7: Mehrere Zellen mit Sensorik und Motorik**

**Ziel:** 3+ Zellen funktionieren zusammen.

### Schritte:
1. Stelle "Zellanzahl" auf 3 → 3 grüne Kreise
2. Toggle "Zeige Sensorik" → 3 × 5 orange Linien = 15 Linien
3. Toggle "Zeige Motorik" → 3 gelbe Pfeile
4. Starte Simulation → alle 3 bewegen sich zusammen

### Feedback: **/** = OK | **x** = Fehler

---

## ✅ **TEST CASE 8: Parameter zur Laufzeit ändern**

**Ziel:** Parameter während laufender Simulation änderbar.

### Schritte:
1. Starte Simulation (Start/Pause)
2. Während laufen: Ändere "Zellanzahl" → neue Zelle erscheint sofort
3. Während laufen: Ändere "Futter-Stückchen" → neue Punkte sofort sichtbar
4. Während laufen: Ändere "Generations-Dauer" → Geschwindigkeit ändert sich sofort

### Feedback: **/** = OK | **x** = Parameter-Änderungen nicht zur Laufzeit möglich

---

## ✅ **TEST CASE 9: Save Button (kein Crash)**

**Ziel:** Save Button crasht nicht.

### Schritte:
1. Klicke auf Save Button
2. App sollte nicht crashen
3. Nichts passiert (noch nicht implementiert) = OK

### Feedback: **/** = OK | **x** = Crash oder Fehler

---

## 📊 Test-Matrix

| # | Test | Status |
|---|------|--------|
| 1 | GUI Layout | ⬜ |
| 2 | Start/Pause Toggle | ⬜ |
| 3 | Zellanzahl-Parameter | ⬜ |
| 4 | Futter-Parameter | ⬜ |
| 5 | Generations-Dauer | ⬜ |
| 6 | Zell-Parameter alt | ⬜ |
| 7 | Mehrere Zellen | ⬜ |
| 8 | Runtime-Parameter | ⬜ |
| 9 | Save Button | ⬜ |

---

## 🎯 Wichtigste Punkte

1. **Layout korrekt reorganisiert**
2. **Toggle-Button funktioniert**
3. **Simulations-Parameter wirken SOFORT auf Spielfläche**
   - Zellanzahl → neue Zellen sichtbar
   - Futter → mehr/weniger Punkte sichtbar
   - Generations-Dauer → Geschwindigkeit ändert sich
4. Alle Parameter zur Laufzeit änderbar
5. Alte Funktionalität intakt

