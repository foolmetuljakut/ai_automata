#include "MainWindow.h"
#include <imgui.h>
#include <Eigen/Dense>
#include <memory>
#include <cmath>

#include "math/NeuralMath.h"
#include "core/ca/CellularAutomaton.h"
#include "core/ca/CellularAutomatonSimulation.h"

namespace autom {
namespace gui {

MainWindow::MainWindow() {
    m_cell = std::make_shared<autom::core::ca::CellularAutomaton>();
    m_cell->SetPosition(m_playgroundWidth / 2.0, m_playgroundHeight / 2.0);
    m_cell->SetVelocity(50.0, -30.0);
    m_cell->SetScore(0);
    m_cell->SetId(0);

    m_simulation = std::make_shared<autom::core::ca::CellularAutomatonSimulation>(m_cell);
    m_simulation->SetCellCount(m_cellCount);
    m_simulation->SetFoodCount(m_foodCount);
    m_simulation->SetGenerationDuration(m_generationDuration);
}

void MainWindow::Render() {
    ImGui::Begin("AI Automata - Dashboard");

    ImGui::Text("Zellulaere Automaten Simulation");
    ImGui::Separator();

    ImGui::BeginGroup();
    { DrawPlayground(); }
    ImGui::EndGroup();

    ImGui::SameLine(0.0f, 20.0f);

    ImGui::BeginGroup();
    {
        DrawSimulationParameters();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        DrawControlButtons();
        ImGui::Spacing();
        DrawCellParameters();
    }
    ImGui::EndGroup();

    ImGui::End();
}

void MainWindow::DrawPlayground() {
    ImGui::Text("Spielflaeche:");

    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize(m_playgroundWidth, m_playgroundHeight);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    drawList->AddRectFilled(
        canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(50, 50, 50, 255)
    );

    drawList->AddRect(
        canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(200, 200, 200, 255)
    );

    DrawFood(canvasPos);

    if (m_simulation) {
        for (std::size_t i = 0; i < m_simulation->GetCellCount(); ++i) {
            auto cell = m_simulation->GetCellAt(i);
            if (cell) {
                DrawCell(cell, canvasPos);
            }
        }
    }

    ImGui::Dummy(canvasSize);
}

void MainWindow::DrawCell(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos) {
    if (!cell) return;

    double x, y;
    cell->GetPosition(x, y);

    ImVec2 cellScreenPos(
        canvasPos.x + static_cast<float>(x),
        canvasPos.y + static_cast<float>(y)
    );

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const float cellRadius = 10.0f;
    const ImU32 cellColor = IM_COL32(0, 200, 100, 255);

    drawList->AddCircleFilled(cellScreenPos, cellRadius, cellColor);

    if (m_showSensors) {
        DrawCellSensors(cell, canvasPos);
    }

    if (m_showMotors) {
        DrawCellMotors(cell, canvasPos);
    }

    ImGui::Text("ID: %zu | Pos: (%.1f, %.1f)", cell->GetId(), x, y);
}

void MainWindow::DrawCellSensors(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos) {
    if (!cell) return;

    double x, y;
    cell->GetPosition(x, y);

    ImVec2 cellScreenPos(
        canvasPos.x + static_cast<float>(x),
        canvasPos.y + static_cast<float>(y)
    );

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    // Nutze aktuelle Slider-Werte statt hardcodiert
    float sensorRange = m_sensorRange;
    float sensorAngle = m_sensorAngle;
    int numRays = 5;

    for (int i = 0; i < numRays; ++i) {
        float angle = (i - numRays / 2) * (2.0f * sensorAngle / numRays) * 3.14159f / 180.0f;
        float endX = cellScreenPos.x + sensorRange * std::cos(angle);
        float endY = cellScreenPos.y + sensorRange * std::sin(angle);

        drawList->AddLine(
            cellScreenPos,
            ImVec2(endX, endY),
            IM_COL32(255, 100, 0, 200),
            1.0f
        );
    }
}

void MainWindow::DrawCellMotors(const std::shared_ptr<autom::core::ca::ICellularAutomaton>& cell, ImVec2 canvasPos) {
    if (!cell) return;

    double x, y;
    cell->GetPosition(x, y);

    double vx, vy;
    cell->GetVelocity(vx, vy);

    ImVec2 cellScreenPos(
        canvasPos.x + static_cast<float>(x),
        canvasPos.y + static_cast<float>(y)
    );

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImVec2 velocityEndPos(
        cellScreenPos.x + vx * 0.5f,
        cellScreenPos.y + vy * 0.5f
    );

    drawList->AddLine(cellScreenPos, velocityEndPos, IM_COL32(255, 200, 0, 255), 2.0f);

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

void MainWindow::DrawFood(ImVec2 canvasPos) {
    if (!m_simulation) return;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const float foodRadius = 3.0f;
    const ImU32 foodColor = IM_COL32(255, 200, 0, 255);

    for (const auto& food : m_simulation->GetFoodItems()) {
        ImVec2 foodScreenPos(
            canvasPos.x + static_cast<float>(food.x),
            canvasPos.y + static_cast<float>(food.y)
        );
        drawList->AddCircleFilled(foodScreenPos, foodRadius, foodColor);
    }
}

void MainWindow::DrawSimulationParameters() {
    ImGui::Text("Simulations-Parameter:");
    ImGui::PushItemWidth(180.0f);

    int oldCellCount = m_cellCount;
    int oldFoodCount = m_foodCount;
    float oldGenDuration = m_generationDuration;

    ImGui::SliderInt("Zellanzahl##cells", &m_cellCount, 1, 10);
    ImGui::SliderInt("Futter-Stueckchen##food", &m_foodCount, 10, 100);
    ImGui::SliderFloat("Generations-Dauer##duration", &m_generationDuration, 0.5f, 10.0f);

    ImGui::PopItemWidth();

    // Sofort Simulation updaten wenn Parameter ändern
    if (oldCellCount != m_cellCount && m_simulation) {
        m_simulation->SetCellCount(m_cellCount);
    }
    if (oldFoodCount != m_foodCount && m_simulation) {
        m_simulation->SetFoodCount(m_foodCount);
    }
    if (oldGenDuration != m_generationDuration && m_simulation) {
        m_simulation->SetGenerationDuration(m_generationDuration);
    }
}

void MainWindow::DrawControlButtons() {
    ImGui::Text("Kontrolle:");

    static bool isRunning = false;
    std::string buttonText = isRunning ? "Pause##toggle" : "Start##toggle";

    if (ImGui::Button(buttonText.c_str(), ImVec2(150, 30))) {
        isRunning = !isRunning;
        if (isRunning) {
            m_simulation->SetState(autom::core::ca::CellularAutomatonSimulation::State::RUNNING);
        } else {
            m_simulation->SetState(autom::core::ca::CellularAutomatonSimulation::State::STOPPED);
        }
    }

    ImGui::SameLine(0.0f, 8.0f);

    if (ImGui::Button("Save##btn", ImVec2(50, 30))) {
        // TODO
    }
}

void MainWindow::DrawCellParameters() {
    ImGui::Text("Zell-Parameter:");
    ImGui::PushItemWidth(180.0f);

    ImGui::SliderFloat("Sensor Range", &m_sensorRange, 10.0f, 200.0f);
    ImGui::SliderFloat("Sensor Angle", &m_sensorAngle, 10.0f, 180.0f);
    ImGui::SliderInt("Hidden Neurons", &m_hiddenNeurons, 2, 32);

    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::Text("Visualisierung:");
    ImGui::Checkbox("Zeige Sensorik", &m_showSensors);
    ImGui::Checkbox("Zeige Motorik", &m_showMotors);

    ImGui::Spacing();

    ImGui::Text("Status: %s",
                m_simulation->GetState() == autom::core::ca::CellularAutomatonSimulation::State::RUNNING
                    ? "RUNNING"
                    : "STOPPED");
}

void MainWindow::DrawDashboard() {
    DrawSimulationParameters();
    ImGui::Spacing();
    DrawControlButtons();
    ImGui::Spacing();
    DrawCellParameters();
}

void MainWindow::UpdateSimulation(double deltaTime) {
    if (!m_simulation) return;
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

