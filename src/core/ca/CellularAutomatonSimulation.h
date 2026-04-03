#pragma once

#include <memory>
#include "CellularAutomaton.h"

namespace autom {
namespace core {
namespace ca {

/**
 * @brief Simulationsklasse für zelluläre Automaten.
 *
 * Verwaltet den Zustand der Simulation (RUNNING/STOPPED) und aktualisiert
 * die Zellen bei jedem Update-Aufruf.
 *
 * Single Responsibility Principle: Diese Klasse ist nur für die Simulationslogik zuständig.
 * GUI-Details gehören nicht hierher.
 */
class ICellularAutomatonSimulation {
public:
    virtual ~ICellularAutomatonSimulation() = default;

    enum class State { STOPPED, RUNNING };

    virtual void SetState(State state) = 0;
    virtual State GetState() const = 0;

    virtual void Update(double deltaTime) = 0;

    virtual std::shared_ptr<ICellularAutomaton> GetCell() const = 0;
};

/**
 * @brief Konkrete Implementierung der Zellulärer-Automaten-Simulation.
 */
class CellularAutomatonSimulation : public ICellularAutomatonSimulation {
public:
    /**
     * @brief Konstruktor.
     * @param cell Die zu simulierende Zelle.
     */
    explicit CellularAutomatonSimulation(std::shared_ptr<ICellularAutomaton> cell);

    void SetState(State state) override;
    State GetState() const override;

    /**
     * @brief Aktualisiert die Simulation um deltaTime Sekunden.
     *
     * Wenn die Simulation läuft (State == RUNNING), wird die Zellposition
     * basierend auf ihrer Velocity aktualisiert.
     *
     * @param deltaTime Zeitdelta seit dem letzten Update (in Sekunden).
     */
    void Update(double deltaTime) override;

    std::shared_ptr<ICellularAutomaton> GetCell() const override;

private:
    State m_state = State::STOPPED;
    std::shared_ptr<ICellularAutomaton> m_cell;
};

} // namespace ca
} // namespace core
} // namespace autom

