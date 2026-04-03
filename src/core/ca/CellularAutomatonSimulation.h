#pragma once

#include <memory>
#include <vector>
#include "CellularAutomaton.h"

namespace autom {
namespace core {
namespace ca {

/**
 * @brief Struktur für Futter-Punkte.
 */
struct FoodItem {
    double x;
    double y;

    FoodItem(double x_, double y_) : x(x_), y(y_) {}
};

/**
 * @brief Simulationsklasse für zelluläre Automaten.
 *
 * Verwaltet:
 * - Zustand der Simulation (RUNNING/STOPPED)
 * - Mehrere Zellen
 * - Futter-Punkte
 * - Update-Logik
 *
 * Single Responsibility Principle: Nur Simulationslogik, keine GUI-Details.
 */
class ICellularAutomatonSimulation {
public:
    virtual ~ICellularAutomatonSimulation() = default;

    enum class State { STOPPED, RUNNING };

    virtual void SetState(State state) = 0;
    virtual State GetState() const = 0;

    virtual void Update(double deltaTime) = 0;

    // Single Cell (Legacy, für Abwärtskompatibilität)
    virtual std::shared_ptr<ICellularAutomaton> GetCell() const = 0;

    // Multiple Cells (neu)
    virtual void SetCellCount(std::size_t count) = 0;
    virtual std::size_t GetCellCount() const = 0;
    virtual std::shared_ptr<ICellularAutomaton> GetCellAt(std::size_t index) const = 0;

    // Food Items (neu)
    virtual void SetFoodCount(std::size_t count) = 0;
    virtual std::size_t GetFoodCount() const = 0;
    virtual const std::vector<FoodItem>& GetFoodItems() const = 0;

    // Simulation Parameters (neu)
    virtual void SetGenerationDuration(double duration) = 0;
    virtual double GetGenerationDuration() const = 0;
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

    // Multiple Cells
    void SetCellCount(std::size_t count) override;
    std::size_t GetCellCount() const override;
    std::shared_ptr<ICellularAutomaton> GetCellAt(std::size_t index) const override;

    // Food Items
    void SetFoodCount(std::size_t count) override;
    std::size_t GetFoodCount() const override;
    const std::vector<FoodItem>& GetFoodItems() const override;

    // Simulation Parameters
    void SetGenerationDuration(double duration) override;
    double GetGenerationDuration() const override;

private:
    State m_state = State::STOPPED;
    std::shared_ptr<ICellularAutomaton> m_cell;  // Legacy, für Abwärtskompatibilität

    // Multiple Cells
    std::vector<std::shared_ptr<ICellularAutomaton>> m_cells;

    // Food Items
    std::vector<FoodItem> m_foodItems;

    // Simulation Parameters
    double m_generationDuration = 1.0;  // Sekunden

    // Playground boundaries (für Zell-Positionierung)
    const double m_playgroundWidth = 600.0;
    const double m_playgroundHeight = 400.0;

    /**
     * @brief Erstelle neue Zellen mit zufälligen Positionen.
     */
    void InitializeCells(std::size_t count);

    /**
     * @brief Erstelle Futter-Punkte mit zufälligen Positionen.
     */
    void InitializeFood(std::size_t count);
};

} // namespace ca
} // namespace core
} // namespace autom

