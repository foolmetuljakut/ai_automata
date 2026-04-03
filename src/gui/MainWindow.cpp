#include "MainWindow.h"
#include <imgui.h>
#include <Eigen/Dense>
#include <memory>

// Abhaengigkeit injiziert (minimal) zum Testen der Integration -> Newspaper Prinzip
#include "math/NeuralMath.h"
#include "core/ca/CellularAutomaton.h"

namespace autom {
namespace gui {

MainWindow::MainWindow() {
    // Erstelle eine Dummy-Zelle zum Anzeigen
    m_cell = std::make_shared<autom::core::ca::CellularAutomaton>();
    m_cell->SetPosition(m_playgroundWidth / 2.0, m_playgroundHeight / 2.0);
    m_cell->SetVelocity(0.1, 0.05);
    m_cell->SetScore(0);
    m_cell->SetId(0);
}

void MainWindow::Render() {
    ImGui::Begin("AI Automata - Dashboard");

    ImGui::Text("Willkommen in der C++ Pipeline fuer AI Automata.");
    ImGui::Separator();

    DrawPlayground();
    ImGui::Separator();
    DrawCalculationsSection();

    ImGui::End();
}

void MainWindow::DrawPlayground() {
    ImGui::Text("Spielflaeche:");

    // Canvas für die Spielfläche
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize(m_playgroundWidth, m_playgroundHeight);

    if (canvasSize.x < 50.0f)
        canvasSize.x = 50.0f;
    if (canvasSize.y < 50.0f)
        canvasSize.y = 50.0f;

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Zeichne den Hintergrund der Spielfläche
    drawList->AddRectFilled(
        canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(50, 50, 50, 255)
    );

    // Zeichne den Rahmen
    drawList->AddRect(
        canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(200, 200, 200, 255)
    );

    // Zeichne die Zelle
    if (m_cell) {
        DrawCell(m_cell);
    }

    // Dummy: Ausrichtung des ImGui-Layouts
    ImGui::Dummy(canvasSize);
}

void MainWindow::DrawCell(const std::shared_ptr<core::ca::ICellularAutomaton>& cell) {
    if (!cell) {
        return;
    }

    double x, y;
    cell->GetPosition(x, y);

    // Berechne die Bildschirmposition basierend auf der Spielflächenposition
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    canvasPos.x -= m_playgroundWidth;  // Korrigiere die Position (GetCursorScreenPos ist am Ende der Spielfläche)
    ImVec2 cellScreenPos(
        canvasPos.x + static_cast<float>(x),
        canvasPos.y + static_cast<float>(y)
    );

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const float cellRadius = 10.0f;
    const ImU32 cellColor = IM_COL32(0, 200, 100, 255);

    // Zeichne den Zellenkörper als Kreis
    drawList->AddCircleFilled(cellScreenPos, cellRadius, cellColor);

    // Zeichne den Bewegungsvektor als Linie
    double vx, vy;
    cell->GetVelocity(vx, vy);
    ImVec2 velocityEndPos(
        cellScreenPos.x + vx * 50.0f,
        cellScreenPos.y + vy * 50.0f
    );
    drawList->AddLine(cellScreenPos, velocityEndPos, IM_COL32(255, 200, 0, 255), 2.0f);

    // Debug-Ausgabe: Position und Score
    ImGui::Text("Cell ID: %zu | Pos: (%.1f, %.1f) | Vel: (%.2f, %.2f) | Score: %zu",
                cell->GetId(), x, y, vx, vy, cell->GetScore());
}

void MainWindow::DrawCalculationsSection() {
    // Demonstration einer Berechnung in der GUI
    ImGui::Text("Generiere Live-Berechnung:");

    static Eigen::Vector2d input(1.0, 1.0);
    static Eigen::Matrix2d weights;
    static Eigen::Vector2d bias(0.0, 0.0);
    static bool initialized = false;

    if (!initialized) {
        weights << 0.5, 0.1, -0.2, 0.8;
        initialized = true;
    }

    ImGui::InputDouble("Input 1", &input(0));
    ImGui::InputDouble("Input 2", &input(1));

    Eigen::Vector2d result = math::NeuralMath::ComputeLayerOutput(weights, input, bias);

    ImGui::Text("Ergebnis der Matrixmultiplikation (ImGui + Eigen3):");
    ImGui::Text("Res = (%.2f, %.2f)", result(0), result(1));
}

} // namespace gui
} // namespace autom

