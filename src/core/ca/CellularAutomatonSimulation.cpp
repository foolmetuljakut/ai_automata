#include "CellularAutomatonSimulation.h"
#include <random>
#include <cmath>

namespace autom {
namespace core {
namespace ca {

CellularAutomatonSimulation::CellularAutomatonSimulation(std::shared_ptr<ICellularAutomaton> cell)
    : m_state(State::STOPPED), m_cell(cell) {
    // Initialisiere mit 1 Zelle und 20 Futter-Punkten
    InitializeCells(1);
    InitializeFood(20);
}

void CellularAutomatonSimulation::SetState(State state) {
    m_state = state;
}

CellularAutomatonSimulation::State CellularAutomatonSimulation::GetState() const {
    return m_state;
}

void CellularAutomatonSimulation::Update(double deltaTime) {
    // Nur wenn Simulation läuft, aktualisiere die Zellen
    if (m_state != State::RUNNING) {
        return;
    }

    // Aktualisiere alle Zellen in m_cells
    // TODO: Später mit generationDuration skalieren für variable Geschwindigkeit
    for (auto& cell : m_cells) {
        if (!cell) continue;

        double x, y;
        cell->GetPosition(x, y);

        double vx, vy;
        cell->GetVelocity(vx, vy);

        // Neue Position = Alte Position + Velocity * DeltaTime
        double newX = x + vx * deltaTime;
        double newY = y + vy * deltaTime;

        cell->SetPosition(newX, newY);
    }
}

std::shared_ptr<ICellularAutomaton> CellularAutomatonSimulation::GetCell() const {
    return m_cell;
}

void CellularAutomatonSimulation::SetCellCount(std::size_t count) {
    InitializeCells(count);
}

std::size_t CellularAutomatonSimulation::GetCellCount() const {
    return m_cells.size();
}

std::shared_ptr<ICellularAutomaton> CellularAutomatonSimulation::GetCellAt(std::size_t index) const {
    if (index < m_cells.size()) {
        return m_cells[index];
    }
    return nullptr;
}

void CellularAutomatonSimulation::SetFoodCount(std::size_t count) {
    InitializeFood(count);
}

std::size_t CellularAutomatonSimulation::GetFoodCount() const {
    return m_foodItems.size();
}

const std::vector<FoodItem>& CellularAutomatonSimulation::GetFoodItems() const {
    return m_foodItems;
}

void CellularAutomatonSimulation::SetGenerationDuration(double duration) {
    if (duration > 0.0) {
        m_generationDuration = duration;
    }
}

double CellularAutomatonSimulation::GetGenerationDuration() const {
    return m_generationDuration;
}

void CellularAutomatonSimulation::InitializeCells(std::size_t count) {
    m_cells.clear();

    // Wenn count == 1 und m_cell existiert bereits, nutze die original cell
    if (count == 1 && m_cell) {
        m_cells.push_back(m_cell);
        return;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> disX(50.0, m_playgroundWidth - 50.0);
    std::uniform_real_distribution<> disY(50.0, m_playgroundHeight - 50.0);
    std::uniform_real_distribution<> disVel(-50.0, 50.0);

    for (std::size_t i = 0; i < count; ++i) {
        auto cell = std::make_shared<CellularAutomaton>();
        cell->SetId(i);
        cell->SetPosition(disX(gen), disY(gen));
        cell->SetVelocity(disVel(gen), -std::abs(disVel(gen)) / 2.0);  // Y negativ = oben
        cell->SetScore(0);
        m_cells.push_back(cell);
    }
}

void CellularAutomatonSimulation::InitializeFood(std::size_t count) {
    m_foodItems.clear();

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> disX(20.0, m_playgroundWidth - 20.0);
    std::uniform_real_distribution<> disY(20.0, m_playgroundHeight - 20.0);

    for (std::size_t i = 0; i < count; ++i) {
        m_foodItems.emplace_back(disX(gen), disY(gen));
    }
}

} // namespace ca
} // namespace core
} // namespace autom

