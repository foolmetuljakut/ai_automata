#include "CellularAutomatonSimulation.h"

namespace autom {
namespace core {
namespace ca {

CellularAutomatonSimulation::CellularAutomatonSimulation(std::shared_ptr<ICellularAutomaton> cell)
    : m_state(State::STOPPED), m_cell(cell) {
}

void CellularAutomatonSimulation::SetState(State state) {
    m_state = state;
}

CellularAutomatonSimulation::State CellularAutomatonSimulation::GetState() const {
    return m_state;
}

void CellularAutomatonSimulation::Update(double deltaTime) {
    // Nur wenn Simulation läuft, aktualisiere die Zelle
    if (m_state != State::RUNNING || !m_cell) {
        return;
    }

    // Aktualisiere Position basierend auf Velocity
    double x, y;
    m_cell->GetPosition(x, y);

    double vx, vy;
    m_cell->GetVelocity(vx, vy);

    // Neue Position = Alte Position + Velocity * DeltaTime
    double newX = x + vx * deltaTime;
    double newY = y + vy * deltaTime;

    m_cell->SetPosition(newX, newY);
}

std::shared_ptr<ICellularAutomaton> CellularAutomatonSimulation::GetCell() const {
    return m_cell;
}

} // namespace ca
} // namespace core
} // namespace autom

