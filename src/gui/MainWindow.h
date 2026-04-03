#pragma once

#include <string>
#include <vector>

namespace autom {
namespace gui {

/**
 * @brief Eine Wrapper-Klasse, die as UI-Element darstellt.
 *
 * Versteckt die Komplexitaet von ImGui hinter einer minimalen Schnittstelle (Kapselung).
 * Es wird hier kein Code zur Berechnung von Mathe/Netzwerken stehen (Schichtentrennung).
 */
class MainWindow {
public:
    void Render();

private:
    void DrawCalculationsSection();
};

} // namespace gui
} // namespace autom

