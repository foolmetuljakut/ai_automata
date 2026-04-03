#pragma once

#include <string>
#include <vector>
#include <memory>
#include <imgui.h>  // Für ImVec2

namespace autom {
namespace core {
namespace ca {
class ICellularAutomaton;
class ICellularAutomatonSimulation;
}  // namespace ca
}  // namespace core
}  // namespace autom

namespace autom {
namespace gui {

/**
 * @brief Hauptfenster der Anwendung.
 *
 * Verwaltet:
 * - 2D Spielfläche für Zellenvisualisierung
 * - Dashboard mit Parametern
 * - Control Buttons (Start, Stop, Save)
 * - Toggle für Sensorik und Motorik Visualisierung
 * - Render-Schleife für kontinuierliche Updates
 *
 * Newspaper Principle: Wichtige Interfaces oben, Details unten.
 */
class MainWindow {
public:
    MainWindow();

    /**
     * @brief Rendert das Hauptfenster für diesen Frame.
     *
     * Wird jedes Frame aufgerufen von main().
     */
    void Render();

    /**
     * @brief Setzt die Sichtbarkeit der Sensorik (Sicht-Keile).
     */
    void SetShowSensors(bool show);
    bool GetShowSensors() const { return m_showSensors; }

    /**
     * @brief Setzt die Sichtbarkeit der Motorik (Richtungsvektor).
     */
    void SetShowMotors(bool show);
    bool GetShowMotors() const { return m_showMotors; }

    /**
     * @brief Aktualisiert die Simulation (wird vom Render-Loop aufgerufen).
     */
    void UpdateSimulation(double deltaTime);

private:
    // Simulation
    std::shared_ptr<autom::core::ca::ICellularAutomatonSimulation> m_simulation;
    std::shared_ptr<autom::core::ca::ICellularAutomaton> m_cell;

    // Render-Loop Parameter
    bool m_showSensors = false;
    bool m_showMotors = true;
    bool m_simulationRunning = false;
    double m_deltaTime = 0.016;  // ~60 FPS default

    // Spielfläche Parameter
    float m_playgroundWidth = 600.0f;  // Reduziert von 800 für besseres Layout
    float m_playgroundHeight = 400.0f; // Reduziert von 600 für besseres Layout

    // Dashboard Parameter (für später)
    float m_sensorRange = 100.0f;
    float m_sensorAngle = 60.0f;
    int m_hiddenNeurons = 8;

    // Draw-Funktionen (Schichtentrennung)
    void DrawPlayground();
    void DrawCell(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos);
    void DrawCellSensors(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos);
    void DrawCellMotors(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos);
    void DrawDashboard();
    void DrawControlButtons();
};

} // namespace gui
} // namespace autom

