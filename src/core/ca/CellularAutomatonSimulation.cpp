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

void CellularAutomatonSimulation::Update(double deltaTime) {
    // Nur wenn Simulation läuft, aktualisiere die Zellen
    if (m_state != State::RUNNING) {
        return;
    }

    // Aktualisiere alle Zellen in m_cells
    for (auto& cell : m_cells) {
        if (!cell) continue;

        double x, y;
        cell->GetPosition(x, y);

        double vx, vy;
        cell->GetVelocity(vx, vy);

        // Neue Position = Alte Position + Velocity * DeltaTime
        double newX = x + vx * deltaTime;
        double newY = y + vy * deltaTime;

        // === BOUNDARY WRAPPING (Toroidal World) ===
        // Wenn Zelle über Grenze hinausgeht, kommt sie von der anderen Seite wieder heraus
        if (newX < 0.0) {
            newX += m_playgroundWidth;
        } else if (newX > m_playgroundWidth) {
            newX -= m_playgroundWidth;
        }

        if (newY < 0.0) {
            newY += m_playgroundHeight;
        } else if (newY > m_playgroundHeight) {
            newY -= m_playgroundHeight;
        }

        cell->SetPosition(newX, newY);

        // === FOOD COLLECTION ===
        // Prüfe, ob Zelle Futter-Punkte erreicht hat (innerhalb eines Radiusses von ~15 Pixeln)
        const double foodCollectionRadius = 15.0;

        // Iteriere durch Futter und sammle auf, wenn Zelle in Reichweite ist
        for (auto it = m_foodItems.begin(); it != m_foodItems.end(); ) {
            double foodX = it->x;
            double foodY = it->y;

            double dx = newX - foodX;
            double dy = newY - foodY;
            double distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= foodCollectionRadius) {
                // Futter aufgesammelt!
                std::size_t currentScore = cell->GetScore();
                cell->SetScore(currentScore + 1);

                // Entferne Futter von der Liste
                it = m_foodItems.erase(it);

                // Spawne neues Futter an zufälliger Position
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_real_distribution<> disX(20.0, m_playgroundWidth - 20.0);
                std::uniform_real_distribution<> disY(20.0, m_playgroundHeight - 20.0);

                m_foodItems.emplace_back(disX(gen), disY(gen));
            } else {
                ++it;
            }
        }
    }
}

void CellularAutomatonSimulation::InitializeCells(std::size_t count) {
    m_cells.clear();

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> disX(50.0, m_playgroundWidth - 50.0);
    std::uniform_real_distribution<> disY(50.0, m_playgroundHeight - 50.0);
    std::uniform_real_distribution<> disVel(-50.0, 50.0);

    // Wenn count == 1 und m_cell existiert bereits, nutze die original cell
    if (count == 1 && m_cell) {
        RandomizeCell(m_cell, 0, gen, disX, disY, disVel);
        m_cells.push_back(m_cell);
        return;
    }

    for (std::size_t i = 0; i < count; ++i) {
        auto cell = std::make_shared<CellularAutomaton>();
        RandomizeCell(cell, i, gen, disX, disY, disVel);
        m_cells.push_back(cell);
    }
}

void CellularAutomatonSimulation::RandomizeCell(
std::shared_ptr<ICellularAutomaton> cell,
std::size_t i,
std::mt19937& gen,
std::uniform_real_distribution<>& disX,
std::uniform_real_distribution<>& disY,
std::uniform_real_distribution<>& disVel) {
    cell->SetId(i);
    cell->SetPosition(disX(gen), disY(gen));
    cell->SetVelocity(disVel(gen), -std::abs(disVel(gen)) / 2.0);  // Y negativ = oben
    cell->SetScore(0);
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

void CellularAutomatonSimulation::SetState(State state) {
    m_state = state;
}

CellularAutomatonSimulation::State CellularAutomatonSimulation::GetState() const {
    return m_state;
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

} // namespace ca
} // namespace core
} // namespace autom

