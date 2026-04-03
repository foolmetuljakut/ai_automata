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
class CellularAutomatonController;  // NEU
}  // namespace ca
namespace anns {
class INeuralNetwork;  // NEU
}  // namespace anns
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

    // Neural Networks für jede Zelle (NEU für Epic 4)
    std::vector<std::shared_ptr<autom::core::anns::INeuralNetwork>> m_networks;
    std::vector<std::shared_ptr<autom::core::ca::CellularAutomatonController>> m_controllers;

    // Render-Loop Parameter
    bool m_showSensors = false;
    bool m_showMotors = true;
    bool m_simulationRunning = false;
    double m_deltaTime = 0.016;  // ~60 FPS default

    // Spielfläche Parameter
    float m_playgroundWidth = 600.0f;
    float m_playgroundHeight = 400.0f;

    // Simulations-Parameter (NEU für Epic 3)
    int m_cellCount = 1;
    int m_foodCount = 20;
    float m_generationDuration = 1.0f;

    // Dashboard Parameter (für später)
    float m_sensorRange = 100.0f;
    float m_sensorAngle = 60.0f;
    int m_hiddenNeurons = 8;

    // Draw-Funktionen (Schichtentrennung)
    void DrawPlayground();
    void DrawCell(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos);
    void DrawCellSensors(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos);
    void DrawCellMotors(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos);
    void DrawFood(ImVec2 canvasPos);
    void DrawSimulationParameters();
    void DrawControlButtons();
    void DrawCellParameters();
    void DrawDashboard();

    // Neural Network Initialization (NEU Epic 4)
    void InitializeNeuronsAndControllers(std::size_t count);
};

} // namespace gui
} // namespace autom

