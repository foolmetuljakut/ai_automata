#pragma once

#include <string>
#include <vector>
#include <memory>

namespace autom {
namespace core {
namespace ca {
class ICellularAutomaton;
}  // namespace ca
}  // namespace core
}  // namespace autom

namespace autom {
namespace gui {

/**
 * @brief Eine Wrapper-Klasse, die das UI-Element darstellt.
 *
 * Versteckt die Komplexitaet von ImGui hinter einer minimalen Schnittstelle (Kapselung).
 * Es wird hier kein Code zur Berechnung von Mathe/Netzwerken stehen (Schichtentrennung).
 * Die GUI ruft nur Methoden auf und visualisiert die Ergebnisse (aufrufender Code).
 */
class MainWindow {
public:
    MainWindow();

    void Render();

private:
    void DrawCalculationsSection();
    void DrawPlayground();
    void DrawCell(const std::shared_ptr<core::ca::ICellularAutomaton>& cell);

    // Spielfläche Parameter
    float m_playgroundWidth = 800.0f;
    float m_playgroundHeight = 600.0f;

    // Dummy Zelle für Anzeige
    std::shared_ptr<core::ca::ICellularAutomaton> m_cell;
};

} // namespace gui
} // namespace autom

