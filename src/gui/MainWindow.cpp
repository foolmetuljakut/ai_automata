#include "MainWindow.h"
#include <imgui.h>
#include <Eigen/Dense>
#include <memory>
#include <cmath>
#include <random>
#include <limits>

#include "math/NeuralMath.h"
#include "core/ca/CellularAutomaton.h"
#include "core/ca/CellularAutomatonSimulation.h"
#include "core/ca/CellularAutomatonController.h"
#include "core/anns/NeuralNetwork.h"
#include "core/ga/GeneticAlgorithm.h"
#include "core/ga/GeneticOperations.h"

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

    // Initialisiere NNs und Controller für Zellen (NEU Epic 4)
    InitializeNeuronsAndControllers(m_cellCount);

    // Initialisiere Genetischen Algorithmus (NEU Epic 5)
    m_geneticAlgorithm = std::make_shared<autom::core::ga::GeneticAlgorithm>(
        m_cellCount,
        5,  // Input: 5 Sensoren
        m_hiddenNeurons,
        2,  // Output: 2 (Velocity X, Y)
        m_mutationRate
    );
}

void MainWindow::InitializeNeuronsAndControllers(std::size_t count) {
    m_networks.clear();
    m_controllers.clear();

    for (std::size_t i = 0; i < count; ++i) {
        // Erstelle NN: 5 Input (Sensorik), 8 Hidden, 2 Output (Velocity)
        auto network = std::make_shared<autom::core::anns::NeuralNetwork>(5, m_hiddenNeurons, 2);

        // WICHTIG: Initialisierung, die Sensorik-Position direkt zu Output-Richtung mappt!
        // Sensor 0 (links -90°) → Output [-1, 0]
        // Sensor 1 (links-oben -45°) → Output [-0.7, 0.7]
        // Sensor 2 (mitte 0°) → Output [1, 0]
        // Sensor 3 (rechts-oben +45°) → Output [0.7, 0.7]
        // Sensor 4 (rechts +90°) → Output [0, 1]

        // Input → Hidden: Jeder Sensor wird direkt an einen Hidden durchgeleitet
        Eigen::MatrixXd w_ih = Eigen::MatrixXd::Zero(m_hiddenNeurons, 5);
        for (int h = 0; h < m_hiddenNeurons && h < 5; ++h) {
            w_ih(h, h) = 1.0;  // Direkter Durchgang
        }
        // Zusätzliche Hidden: Random Kombinationen
        for (int h = 5; h < m_hiddenNeurons; ++h) {
            for (int s = 0; s < 5; ++s) {
                w_ih(h, s) = (std::rand() % 100 - 50) / 100.0 * 0.2;
            }
        }

        // Hidden → Output: WICHTIG - mappt Sensor-Position zu Richtung
        // Output 0 (X): Positiv wenn Sensor 2 (mitte) oder Sensor 3-4 (rechts)
        // Output 1 (Y): Positiv wenn Sensor 0-1 (oben) oder Sensor 3-4 (oben)
        Eigen::MatrixXd w_ho = Eigen::MatrixXd::Zero(2, m_hiddenNeurons);

        // Output 0 (X-Richtung):
        // Sensor 0 (links) → -1
        // Sensor 2 (mitte) → +1
        // Sensor 4 (rechts) → +0
        if (m_hiddenNeurons > 0) w_ho(0, 0) = -1.0;  // Sensor 0 → links (-X)
        if (m_hiddenNeurons > 1) w_ho(0, 1) = -0.5;  // Sensor 1 → links-oben
        if (m_hiddenNeurons > 2) w_ho(0, 2) = 1.0;   // Sensor 2 → rechts (+X)
        if (m_hiddenNeurons > 3) w_ho(0, 3) = 0.5;   // Sensor 3 → rechts-oben
        if (m_hiddenNeurons > 4) w_ho(0, 4) = 0.0;   // Sensor 4 → neutral

        // Output 1 (Y-Richtung):
        // Sensor 0 (links/oben) → +Y
        // Sensor 2 (mitte) → 0
        // Sensor 4 (rechts/unten) → -Y
        if (m_hiddenNeurons > 0) w_ho(1, 0) = 0.5;   // Sensor 0 → links-oben
        if (m_hiddenNeurons > 1) w_ho(1, 1) = 1.0;   // Sensor 1 → oben (+Y)
        if (m_hiddenNeurons > 2) w_ho(1, 2) = 0.0;   // Sensor 2 → neutral
        if (m_hiddenNeurons > 3) w_ho(1, 3) = -1.0;  // Sensor 3 → unten (-Y)
        if (m_hiddenNeurons > 4) w_ho(1, 4) = -0.5;  // Sensor 4 → rechts-unten

        network->SetInputToHiddenWeights(w_ih);
        network->SetHiddenToOutputWeights(w_ho);
        network->SetHiddenBias(Eigen::VectorXd::Zero(m_hiddenNeurons));
        network->SetOutputBias(Eigen::VectorXd::Zero(2));

        m_networks.push_back(network);

        // Erstelle Controller für diese Zelle
        auto cell = (i == 0) ? m_cell : m_simulation->GetCellAt(i);
        if (cell) {
            auto controller = std::make_shared<autom::core::ca::CellularAutomatonController>(cell, network);
            m_controllers.push_back(controller);
        }
    }
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

    // Zeichne Zellenkörper (Kreis)
    drawList->AddCircleFilled(cellScreenPos, cellRadius, cellColor);

    // NEU: Zeichne Richtungsmarkierung basierend auf Rotation und Geschwindigkeit
    double rotation = cell->GetRotation();

    // Berechne Geschwindigkeit Betrag
    double vx, vy;
    cell->GetVelocity(vx, vy);
    double velocityMagnitude = std::sqrt(vx * vx + vy * vy);

    // Pfeil-Länge proportional zur Geschwindigkeit
    // Min: 2px (bei v=0), Max: 50px (bei hoher Geschwindigkeit)
    // Faktor 0.3 macht die Variation sichtbarer
    float arrowLength = 2.0f + static_cast<float>(velocityMagnitude) * 0.3f;
    if (arrowLength > 50.0f) arrowLength = 50.0f;  // Cap at 50px

    ImVec2 arrowTip(
        cellScreenPos.x + arrowLength * std::cos(rotation),
        cellScreenPos.y + arrowLength * std::sin(rotation)
    );
    drawList->AddLine(cellScreenPos, arrowTip, IM_COL32(255, 255, 255, 255), 2.0f);

    // Zeichne Sensorik wenn aktiviert
    if (m_showSensors) {
        DrawCellSensors(cell, canvasPos);
    }

    // Zeichne Motorik wenn aktiviert
    if (m_showMotors) {
        DrawCellMotors(cell, canvasPos);
    }

    // Debug-Info
    ImGui::Text("ID: %zu | Score: %zu", cell->GetId(), cell->GetScore());
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

    // WICHTIG: Hole Zell-Rotation und drehe Sensoren damit! (FIX für Epic 4)
    double rotation = cell->GetRotation();

    for (int i = 0; i < numRays; ++i) {
        float angle = (i - numRays / 2) * (2.0f * sensorAngle / numRays) * 3.14159f / 180.0f;

        // Drehe angle um Zell-Rotation
        float rotatedAngle = angle + static_cast<float>(rotation);

        float endX = cellScreenPos.x + sensorRange * std::cos(rotatedAngle);
        float endY = cellScreenPos.y + sensorRange * std::sin(rotatedAngle);

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

    // WICHTIG: Nutze Rotation für Pfeil-Richtung (FIX für Epic 4)
    double rotation = cell->GetRotation();

    ImVec2 velocityEndPos(
        cellScreenPos.x + 50.0f * std::cos(rotation),  // Scale für Visualisierung
        cellScreenPos.y + 50.0f * std::sin(rotation)
    );

    drawList->AddLine(cellScreenPos, velocityEndPos, IM_COL32(255, 200, 0, 255), 2.0f);

    // Pfeilspitze
    float vx_norm = std::cos(static_cast<float>(rotation));
    float vy_norm = std::sin(static_cast<float>(rotation));

    const float arrowSize = 8.0f;
    ImVec2 arrow1(velocityEndPos.x - arrowSize * vx_norm + arrowSize * vy_norm,
                  velocityEndPos.y - arrowSize * vy_norm - arrowSize * vx_norm);
    ImVec2 arrow2(velocityEndPos.x - arrowSize * vx_norm - arrowSize * vy_norm,
                  velocityEndPos.y - arrowSize * vy_norm + arrowSize * vx_norm);

    drawList->AddLine(velocityEndPos, arrow1, IM_COL32(255, 200, 0, 255), 2.0f);
    drawList->AddLine(velocityEndPos, arrow2, IM_COL32(255, 200, 0, 255), 2.0f);
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

    ImGui::SliderInt("Zellanzahl##cells", &m_cellCount, 2, 10);  // Minimum: 2 Zellen
    ImGui::SliderInt("Futter-Stueckchen##food", &m_foodCount, 10, 100);
    ImGui::SliderFloat("Generations-Dauer (s)##duration", &m_generationDuration, 5.0f, 120.0f);  // 5-120 Sekunden

    ImGui::PopItemWidth();

    // Textfeld für verbleibende Zeit
    double remainingTime = m_generationDuration - m_currentGenerationElapsedTime;
    if (remainingTime < 0.0) remainingTime = 0.0;
    ImGui::Text("Verbleibende Zeit: %.2f / %.1f s", remainingTime, m_generationDuration);

    // Textfeld für aktuelle Generation
    ImGui::Text("Generation: %d", m_currentGeneration);

    // Sofort Simulation updaten wenn Parameter ändern
    if (oldCellCount != m_cellCount && m_simulation) {
        m_simulation->SetCellCount(m_cellCount);
        InitializeNeuronsAndControllers(m_cellCount);  // NEU: Reinitialisiere NNs

        // NEU für Epic 5: Reinitialisiere GA bei Cell Count Änderung
        m_geneticAlgorithm = std::make_shared<autom::core::ga::GeneticAlgorithm>(
            m_cellCount,
            5,  // Input: 5 Sensoren
            m_hiddenNeurons,
            2,  // Output: 2 (Velocity X, Y)
            m_mutationRate
        );
    }
    if (oldFoodCount != m_foodCount && m_simulation) {
        m_simulation->SetFoodCount(m_foodCount);
    }
    if (oldGenDuration != m_generationDuration && m_simulation) {
        m_simulation->SetGenerationDuration(m_generationDuration);
        m_currentGenerationElapsedTime = 0.0;  // Reset timer wenn Duration ändert
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

    ImGui::Spacing();

    ImGui::Text("Genetische Algorithmus:");
    // Mutation Rate als logarithmischer Slider (1e-5 bis 1e-1)
    float logMutationRate = static_cast<float>(std::log10(m_mutationRate));
    ImGui::SliderFloat("Mutation Rate##ga", &logMutationRate, -5.0f, -1.0f, "%.1e");
    m_mutationRate = std::pow(10.0, static_cast<double>(logMutationRate));

    if (m_geneticAlgorithm) {
        m_geneticAlgorithm->SetMutationRate(m_mutationRate);
    }

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

    // Syncronisiere m_simulationRunning mit Simulation State
    m_simulationRunning = (m_simulation->GetState() == autom::core::ca::CellularAutomatonSimulation::State::RUNNING);

    // Tracking der Generation-Zeit
    if (m_simulationRunning) {
        m_currentGenerationElapsedTime += deltaTime;

        // Wenn Generation vorbei ist: Generationswechsel durchführen
        if (m_currentGenerationElapsedTime >= m_generationDuration) {
            m_currentGenerationElapsedTime = 0.0;
            m_currentGeneration++;  // Inkrementiere Generation Counter

            // === GENERATION SWITCH mit GENETISCHEM ALGORITHMUS ===

            // 1. Setze Fitness (Score) in GA für alle Zellen
            // WICHTIG: Nur für Zellen iterieren, die auch in der GA sind!
            std::size_t gaPopSize = m_geneticAlgorithm ? m_geneticAlgorithm->GetPopulationSize() : 0;
            std::size_t cellCount = m_simulation->GetCellCount();

            // Prüfe Konsistenz
            if (gaPopSize != cellCount) {
                // GA nicht synchronisiert - das sollte nicht passieren!
                // Reinitialize GA mit aktueller Cell Count
                m_geneticAlgorithm = std::make_shared<autom::core::ga::GeneticAlgorithm>(
                    cellCount, 5, m_hiddenNeurons, 2, m_mutationRate
                );
                gaPopSize = cellCount;
            }

            for (std::size_t i = 0; i < gaPopSize && i < cellCount; ++i) {
                auto cell = m_simulation->GetCellAt(i);
                if (cell) {
                    double score = static_cast<double>(cell->GetScore());
                    m_geneticAlgorithm->SetIndividualFitness(i, score);
                }
            }

            // 2. Führe GA-Generationsschritt durch:
            //    - Selection: Behalte nur Zellen mit Score-Verbesserung
            //    - Reproduction: Erzeuge neue durch Crossover + Mutation
            m_geneticAlgorithm->ExecuteGenerationStep();

            // 3. Update NN in m_networks mit neuen GA-Ergebnissen
            for (std::size_t i = 0; i < gaPopSize; ++i) {
                if (i >= m_networks.size() || i >= m_controllers.size()) {
                    // Größe wurde geändert, sollte nicht passieren aber defensive Programmierung
                    break;
                }

                auto newNetwork = m_geneticAlgorithm->GetIndividualNetwork(i);
                if (newNetwork) {
                    m_networks[i] = newNetwork;

                    // Update auch den Controller mit neuem Network
                    auto cell = m_simulation->GetCellAt(i);
                    if (cell) {
                        m_controllers[i] = std::make_shared<autom::core::ca::CellularAutomatonController>(
                            cell, newNetwork
                        );
                    }
                }
            }

            // 4. Reset Zellen für neue Generation
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> disX(50.0, m_playgroundWidth - 50.0);
            std::uniform_real_distribution<> disY(50.0, m_playgroundHeight - 50.0);
            std::uniform_real_distribution<> disVel(-50.0, 50.0);

            for (std::size_t i = 0; i < m_simulation->GetCellCount(); ++i) {
                auto cell = m_simulation->GetCellAt(i);
                if (!cell) continue;

                // Neue zufällige Position
                cell->SetPosition(disX(gen), disY(gen));

                // Neue zufällige Velocity
                double vx = disVel(gen);
                double vy = -std::abs(disVel(gen)) / 2.0;  // Y negativ = oben
                cell->SetVelocity(vx, vy);

                // WICHTIG: Score zurücksetzen für neue Generation
                cell->SetScore(0);

                // Reset Controller
                if (i < m_controllers.size()) {
                    m_controllers[i]->Reset();
                }
            }
        }
    }

    // Erste: Update Controller für jede Zelle (NEU Epic 4)
    if (m_simulationRunning && !m_controllers.empty()) {
        const auto& foodItems = m_simulation->GetFoodItems();

        for (std::size_t i = 0; i < m_simulation->GetCellCount(); ++i) {
            if (i >= m_controllers.size()) break;

            auto cell = m_simulation->GetCellAt(i);
            auto controller = m_controllers[i];

            if (!cell || !controller) continue;

            // Finde nächstes Futter für diese Zelle
            Eigen::Vector2d closestFood(0, 0);
            double closestDist = std::numeric_limits<double>::max();
            bool foodFound = false;

            double cellX, cellY;
            cell->GetPosition(cellX, cellY);
            Eigen::Vector2d cellPos(cellX, cellY);

            for (const auto& food : foodItems) {
                Eigen::Vector2d foodPos(food.x, food.y);
                double dist = (foodPos - cellPos).norm();

                if (dist < closestDist) {
                    closestDist = dist;
                    closestFood = foodPos;
                    foodFound = true;
                }
            }

            // Update Controller mit nächstem Futter
            if (foodFound) {
                controller->Update(closestFood, deltaTime, m_sensorRange);
            }
        }
    }

    // Dann: Update Simulation Position
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

