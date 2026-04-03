#include "MainWindow.h"
#include <imgui.h>
#include <Eigen/Dense>
#include <memory>
#include <cmath>

// Abhaengigkeit injiziert zum Testen der Integration -> Newspaper Prinzip
#include "math/NeuralMath.h"
#include "core/ca/CellularAutomaton.h"
#include "core/ca/CellularAutomatonSimulation.h"

namespace autom {
namespace gui {

MainWindow::MainWindow() {
    // Erstelle eine Zelle
    m_cell = std::make_shared<autom::core::ca::CellularAutomaton>();
    m_cell->SetPosition(m_playgroundWidth / 2.0, m_playgroundHeight / 2.0);
    // WICHTIG: Y-Komponente ist negativ weil ImGui Y=0 OBEN hat, Y wächst nach UNTEN
    // Negative Y = nach oben in ImGui
    m_cell->SetVelocity(50.0, -30.0);  // Rechts-oben statt rechts-unten
    m_cell->SetScore(0);
    m_cell->SetId(0);

    // Erstelle Simulation mit Zelle
    m_simulation = std::make_shared<autom::core::ca::CellularAutomatonSimulation>(m_cell);
}

void MainWindow::Render() {
    // Newspaper Principle: Top-level struktur erst, Details dann
    ImGui::Begin("AI Automata - Dashboard");

    ImGui::Text("Zellulaere Automaten Simulation");
    ImGui::Separator();

    // Layout: Links Spielfläche, Rechts Dashboard (mit stabiler Group-Technik)
    ImGui::BeginGroup();
    {
        DrawPlayground();
    }
    ImGui::EndGroup();

    ImGui::SameLine(0.0f, 20.0f);  // Etwas Abstand zwischen den Spalten

    ImGui::BeginGroup();
    {
        DrawDashboard();
        ImGui::Spacing();
        DrawControlButtons();
    }
    ImGui::EndGroup();

    ImGui::End();
}

void MainWindow::DrawPlayground() {
    ImGui::Text("Spielflaeche:");

    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize(m_playgroundWidth, m_playgroundHeight);

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Hintergrund der Spielfläche
    drawList->AddRectFilled(
        canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(50, 50, 50, 255)
    );

    // Rahmen
    drawList->AddRect(
        canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(200, 200, 200, 255)
    );

    // Zeichne die Zelle BEVOR Dummy() aufgerufen wird
    if (m_cell) {
        DrawCell(m_cell, canvasPos);
    }

    // Dummy-Ausrichtung für ImGui-Layout (NACH dem Zeichnen!)
    ImGui::Dummy(canvasSize);
}

void MainWindow::DrawCell(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos) {
    if (!cell) {
        return;
    }

    double x, y;
    cell->GetPosition(x, y);

    // Canvas-Position wird jetzt übergeben statt neu berechnet!
    ImVec2 cellScreenPos(
        canvasPos.x + static_cast<float>(x),
        canvasPos.y + static_cast<float>(y)
    );

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const float cellRadius = 10.0f;
    const ImU32 cellColor = IM_COL32(0, 200, 100, 255);

    // Zeichne Zellenkörper
    drawList->AddCircleFilled(cellScreenPos, cellRadius, cellColor);

    // Zeichne Sensorik wenn aktiviert
    if (m_showSensors) {
        DrawCellSensors(cell, canvasPos);
    }

    // Zeichne Motorik wenn aktiviert
    if (m_showMotors) {
        DrawCellMotors(cell, canvasPos);
    }

    // Debug-Info
    ImGui::Text("ID: %zu | Pos: (%.1f, %.1f)", cell->GetId(), x, y);
}

void MainWindow::DrawCellSensors(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos) {
    if (!cell) {
        return;
    }

    double x, y;
    cell->GetPosition(x, y);

    // Canvas-Position wird übergeben
    ImVec2 cellScreenPos(
        canvasPos.x + static_cast<float>(x),
        canvasPos.y + static_cast<float>(y)
    );

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Zeichne Sicht-Keile (keilförmige Sensorzellen)
    // Für jetzt: Drei Keile (links, oben, rechts)
    const float sensorRange = 50.0f;
    const float sensorAngle = 30.0f;  // Grad pro Seite

    // Funktioniert ähnlich wie ein Kuchenstück
    int numRays = 5;
    for (int i = 0; i < numRays; ++i) {
        float angle = (i - numRays / 2) * (2.0f * sensorAngle / numRays) * 3.14159f / 180.0f;
        float endX = cellScreenPos.x + sensorRange * std::cos(angle);
        float endY = cellScreenPos.y + sensorRange * std::sin(angle);

        drawList->AddLine(
            cellScreenPos,
            ImVec2(endX, endY),
            IM_COL32(255, 100, 0, 200),  // Orange, halb transparent
            1.0f
        );
    }
}

void MainWindow::DrawCellMotors(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos) {
    if (!cell) {
        return;
    }

    double x, y;
    cell->GetPosition(x, y);

    double vx, vy;
    cell->GetVelocity(vx, vy);

    // Canvas-Position wird übergeben
    ImVec2 cellScreenPos(
        canvasPos.x + static_cast<float>(x),
        canvasPos.y + static_cast<float>(y)
    );

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Zeichne Velocity-Vektor
    ImVec2 velocityEndPos(
        cellScreenPos.x + vx * 0.5f,  // Scale für Visualisierung
        cellScreenPos.y + vy * 0.5f
    );

    drawList->AddLine(cellScreenPos, velocityEndPos, IM_COL32(255, 200, 0, 255), 2.0f);

    // Pfeilspitze
    float vx_norm = static_cast<float>(vx);
    float vy_norm = static_cast<float>(vy);
    float len = std::sqrt(vx_norm * vx_norm + vy_norm * vy_norm);
    if (len > 0.1f) {
        vx_norm /= len;
        vy_norm /= len;

        const float arrowSize = 8.0f;
        ImVec2 arrow1(velocityEndPos.x - arrowSize * vx_norm + arrowSize * vy_norm,
                      velocityEndPos.y - arrowSize * vy_norm - arrowSize * vx_norm);
        ImVec2 arrow2(velocityEndPos.x - arrowSize * vx_norm - arrowSize * vy_norm,
                      velocityEndPos.y - arrowSize * vy_norm + arrowSize * vx_norm);

        drawList->AddLine(velocityEndPos, arrow1, IM_COL32(255, 200, 0, 255), 2.0f);
        drawList->AddLine(velocityEndPos, arrow2, IM_COL32(255, 200, 0, 255), 2.0f);
    }
}

void MainWindow::DrawDashboard() {
    ImGui::Text("Parameter:");
    ImGui::Separator();

    // Limitiere nur die Slider-Breite
    ImGui::PushItemWidth(180.0f);

    // Sensor Range
    ImGui::SliderFloat("Sensor Range", &m_sensorRange, 10.0f, 200.0f);

    // Sensor Angle
    ImGui::SliderFloat("Sensor Angle", &m_sensorAngle, 10.0f, 180.0f);

    // Hidden Neurons
    ImGui::SliderInt("Hidden Neurons", &m_hiddenNeurons, 2, 32);

    ImGui::PopItemWidth();  // Beende die Breiten-Begrenzung

    ImGui::Spacing();

    // Toggles für Visualisierung
    ImGui::Text("Visualisierung:");
    ImGui::Checkbox("Zeige Sensorik", &m_showSensors);
    ImGui::Checkbox("Zeige Motorik", &m_showMotors);

    ImGui::Spacing();

    // Simulation Status
    ImGui::Text("Status: %s",
                m_simulation->GetState() == autom::core::ca::CellularAutomatonSimulation::State::RUNNING
                    ? "RUNNING"
                    : "STOPPED");
}

void MainWindow::DrawControlButtons() {
    ImGui::Text("Kontrolle:");
    ImGui::Separator();

    // Start Button
    if (ImGui::Button("Start##btn1", ImVec2(55, 30))) {
        m_simulation->SetState(autom::core::ca::CellularAutomatonSimulation::State::RUNNING);
        m_simulationRunning = true;
    }

    ImGui::SameLine(0.0f, 8.0f);

    // Stop Button
    if (ImGui::Button("Stop##btn2", ImVec2(55, 30))) {
        m_simulation->SetState(autom::core::ca::CellularAutomatonSimulation::State::STOPPED);
        m_simulationRunning = false;
    }

    ImGui::SameLine(0.0f, 8.0f);

    // Save Button (noch ohne Funktion)
    if (ImGui::Button("Save##btn3", ImVec2(55, 30))) {
        // TODO: Implementiere Speicherfunktion
    }
}

void MainWindow::UpdateSimulation(double deltaTime) {
    if (!m_simulation) {
        return;
    }

    m_simulation->Update(deltaTime);
}

void MainWindow::SetShowSensors(bool show) {
    m_showSensors = show;
}

void MainWindow::SetShowMotors(bool show) {
    m_showMotors = show;
}

} // namespace gui
} // namespace autom

