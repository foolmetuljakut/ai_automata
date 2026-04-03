# 🧪 Manuelle Test-Anleitung für Epic 2

## 📋 Überblick

Diese Anleitung beschreibt, wie man die GUI-Funktionalität von Epic 2 **manuell** testet, ohne automatisierte ImGui-Tests zu schreiben.

---

## 🚀 Setup: Build und Start

### 1. **Build durchführen**
```bash
cd /home/devsr/ai_automata
./pipeline.sh cleanup
./pipeline.sh configure_cmake
./pipeline.sh execute_build
./pipeline.sh run_unit_tests
```

### 2. **Anwendung starten**
```bash
./build/ai_automata
```

Ein Fenster mit 1280x720 sollte sich öffnen mit dem Titel "AI Automata Default View".

---

## ✅ Test Case 1: Grundlegende GUI Layout

**Ziel:** Verifiziere, dass GUI korrekt layoutet ist.

### Schritte:
1. Starte die App
2. Schaue auf die GUI

### Erwartete Ergebnisse:
- ✅ Linke Seite: **Spielfläche** (800x600 px, dunkler Hintergrund mit Rahmen)
- ✅ Rechte Seite: **Dashboard** mit:
  - Text "Parameter:"
  - Slider für "Sensor Range"
  - Slider für "Sensor Angle"
  - Slider für "Hidden Neurons"
  - Checkboxes für "Zeige Sensorik" und "Zeige Motorik"
  - Status-Text "STOPPED"
- ✅ **Buttons**: "Start", "Stop", "Save" nebeneinander

---

## ✅ Test Case 2: Zelle wird angezeigt

**Ziel:** Verifiziere, dass die Zelle auf der Spielfläche gezeichnet wird.

### Schritte:
1. Schaue auf die Spielfläche (linke Seite)

### Erwartete Ergebnisse:
- ✅ **Grüner Kreis** in der Mitte der Spielfläche (Zelle)
- ✅ **Gelber Pfeil** der nach rechts-unten zeigt (Velocity Vector = Motorik)
- ✅ **Text unter Spielfläche**: "ID: 0 | Pos: (400.0, 300.0)"

---

## ✅ Test Case 3: Motorik Toggle (Velocity Vector)

**Ziel:** Verifiziere, dass der Motorik-Vektor (Pfeil) an- und ausschaltbar ist.

### Schritte:
1. Schaue auf die GUI
2. Das Checkbox "Zeige Motorik" sollte **bereits CHECKED sein** (default)
3. **Klicke auf das Checkbox** "Zeige Motorik" um es **auszuschalten**
4. Der gelbe Pfeil sollte **verschwinden**
5. **Klicke erneut** um es wieder **anzuschalten**
6. Der gelbe Pfeil sollte **wiederkommen**

### Erwartete Ergebnisse:
- ✅ Checkpoint bei "Zeige Motorik" ist **an** → Gelber Pfeil sichtbar
- ✅ Checkbox bei "Zeige Motorik" ist **aus** → Gelber Pfeil unsichtbar
- ✅ Checkbox bei "Zeige Motorik" ist **an** → Gelber Pfeil sichtbar

---

## ✅ Test Case 4: Sensorik Toggle (Vision Cones)

**Ziel:** Verifiziere, dass die Sensorik (Sicht-Keile) an- und ausschaltbar ist.

### Schritte:
1. Schaue auf das Checkbox "Zeige Sensorik" (sollte **UNCHECKED sein**)
2. **Klicke auf das Checkbox** "Zeige Sensorik" um es **anzuschalten**
3. Um die grüne Zelle sollten jetzt **orange Linien** (5 Stück) sichtbar sein - die Sensorik
4. **Klicke erneut** um es **auszuschalten**
5. Die orange Linien sollten **verschwinden**

### Erwartete Ergebnisse:
- ✅ Checkbox bei "Zeige Sensorik" ist **aus** → Keine orange Linien
- ✅ Checkbox bei "Zeige Sensorik" ist **an** → 5 orange Linien um die Zelle

---

## ✅ Test Case 5: Start Button - Zelle bewegt sich

**Ziel:** Verifiziere, dass der Start-Button die Simulation startet und die Zelle sich bewegt.

### Schritte:
1. **Beobachte die Zellposition** (Text unter Spielfläche: "Pos: (400.0, 300.0)")
2. **Klicke auf "Start" Button**
3. Status sollte sich ändern zu "RUNNING"
4. **Beobachte die Position** - sie sollte sich kontinuierlich ändern
5. Die Zelle sollte **diagonal nach rechts-oben bewegen** (wegen Velocity)
6. Nach ~10 Sekunden sollte die Zelle **rechts unten aus dem Fenster verschwinden**

### Erwartete Ergebnisse:
- ✅ Status ändert sich zu "RUNNING"
- ✅ Position Wert steigt kontinuierlich (Pos: 400 → 420 → 440 → ...)
- ✅ Grüner Kreis (Zelle) bewegt sich auf der Spielfläche
- ✅ Zelle verschwindet nach rechts-oben (Velocity: vx=50, vy=30)

---

## ✅ Test Case 6: Stop Button - Zelle stoppt

**Ziel:** Verifiziere, dass der Stop-Button die Simulation stoppt.

### Schritte:
1. **Klicke Start** (Zelle bewegt sich)
2. **Beobachte Position** (sollte sich ändern)
3. **Klicke Stop Button**
4. Status sollte sich ändern zu "STOPPED"
5. **Position sollte NICHT mehr ändern** - Zelle bleibt stehen

### Erwartete Ergebnisse:
- ✅ Nach Stop: Position bleibt gleich (z.B. "Pos: (580.5, 400.3)")
- ✅ Status zeigt "STOPPED"
- ✅ Zelle bewegt sich nicht mehr

---

## ✅ Test Case 7: Parameter Sliders (ohne Funktion)

**Ziel:** Verifiziere, dass Parameter-Sliders funktionieren (noch ohne Effekt auf Simulation).

### Schritte:
1. Bewege die Slider für:
   - "Sensor Range" (10 → 200)
   - "Sensor Angle" (10 → 180)
   - "Hidden Neurons" (2 → 32)
2. Die Slider sollten sich bewegen und die Werte ändern
3. **Noch kein visueller Effekt erwartet** (das kommt später)

### Erwartete Ergebnisse:
- ✅ Slider bewegen sich und Werte ändern sich
- ✅ Kein Crash

---

## ✅ Test Case 8: Sensorik + Motorik zusammen

**Ziel:** Verifiziere, dass beide Visualisierungen gleichzeitig arbeiten.

### Schritte:
1. **Schalte beide an**:
   - "Zeige Sensorik" → CHECKED
   - "Zeige Motorik" → CHECKED
2. Auf der Spielfläche sollte die Zelle jetzt haben:
   - Grüner Kreis (Zelle)
   - 5 orange Linien (Sensorik)
   - 1 gelber Pfeil (Motorik)
3. **Klicke Start**
4. Alles sollte zusammen **mitbewegen**
5. **Klicke Stop**
6. Alles sollte **stoppen**

### Erwartete Ergebnisse:
- ✅ Zelle + Sensorik + Motorik alle sichtbar
- ✅ Alle bewegen sich zusammen wenn Start geklickt
- ✅ Alle stoppen wenn Stop geklickt

---

## ⚠️ Test Case 9: Edge Cases / Fehlerszenarien

### Sub-Test 9a: Mehrfaches Klicken von Start
**Schritte:**
1. Klicke Start mehrmals hintereinander
2. Zelle sollte normal weiterlaufen (nicht schneller werden)

**Ergebnis:** ✅ Zelle läuft normal weiter

### Sub-Test 9b: Start → Stop → Start
**Schritte:**
1. Klicke Start (Zelle bewegt sich)
2. Klicke Stop (Zelle stoppt bei Position X)
3. Klicke Start wieder
4. Zelle sollte von Position X weitermachen (nicht resetten)

**Ergebnis:** ✅ Zelle setzt sich von alter Position fort

### Sub-Test 9c: Save Button
**Schritte:**
1. Klicke Save Button
2. App sollte **nicht crashen**
3. Nichts passiert (das ist OK, die Funktion ist noch nicht implementiert)

**Ergebnis:** ✅ Kein Crash

---

## 🔍 Debugging Tipps

Falls etwas nicht funktioniert:

### Problem: Fenster öffnet sich nicht
**Lösung:**
```bash
# Evtl. fehlende Dependencies
sudo dnf install glfw-devel libGL-devel
./pipeline.sh execute_build
```

### Problem: Zelle ist nicht sichtbar
**Lösung:**
- Prüfe, dass die Spielfläche nicht zu klein ist
- Prüfe die Koordinaten im Debug-Text unter der Spielfläche

### Problem: Buttons funktionieren nicht
**Lösung:**
- Stelle sicher, dass die Buttons clickbar sind (nicht überlagert)
- Prüfe den Status-Text ob sich der Zustand ändert

### Problem: Zelle bewegt sich nicht nach Start
**Lösung:**
- Prüfe, dass Velocity nicht (0, 0) ist (sollte (50, 30) sein)
- Prüfe, dass deltaTime > 0 ist
- Versuche eine höhere Velocity zu setzen (später: Dashboard Parameter)

---

## 📊 Test-Matrix zum Abhaken

| Test Case | Status | Notizen |
|-----------|--------|---------|
| 1. GUI Layout | ⬜ | |
| 2. Zelle angezeigt | ⬜ | |
| 3. Motorik Toggle | ⬜ | |
| 4. Sensorik Toggle | ⬜ | |
| 5. Start Button | ⬜ | |
| 6. Stop Button | ⬜ | |
| 7. Parameter Sliders | ⬜ | |
| 8. Sensorik + Motorik | ⬜ | |
| 9a. Mehrfaches Start | ⬜ | |
| 9b. Start → Stop → Start | ⬜ | |
| 9c. Save Button | ⬜ | |

**Alle Tests grün?** → Epic 2 ist fertig! 🎉

---

## 🎬 Finale Validierung

### Wenn alles funktioniert:
```bash
./pipeline.sh cleanup
./pipeline.sh configure_cmake
./pipeline.sh execute_build
./pipeline.sh run_unit_tests
./pipeline.sh code_analysis
./pipeline.sh create_coverage_report
./pipeline.sh create_documentation
./pipeline.sh create_rpm_package
```

### Alle grün? 🚀
- ✅ Build erfolgreich
- ✅ Unit Tests bestanden
- ✅ Code Analysis sauber
- ✅ Coverage Report generiert
- ✅ Dokumentation erstellt
- ✅ RPM Paket gebaut

**Dann ist Epic 2 KOMPLETT!**

