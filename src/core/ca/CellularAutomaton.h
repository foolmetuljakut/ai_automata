#pragma once

#include <cstddef>
#include <Eigen/Dense>

namespace autom {
namespace core {
namespace ca {

/**
 * @brief Dummy-Implementierung einer Zelle (Cellular Automaton).
 *
 * Dies ist nur die Datenstruktur ohne Logik. Interfaces erlauben Mock-basiertes Testen.
 * Die Zelle selbst ist vorerst nur eine passive Datenstruktur mit getter/setter.
 */
class ICellularAutomaton {
public:
    virtual ~ICellularAutomaton() = default;

    /// Position in der 2D-Umgebung
    virtual void SetPosition(double x, double y) = 0;
    virtual void GetPosition(double& x, double& y) const = 0;

    /// Velocity / Bewegungsrichtung (Motorik-Ausgang)
    virtual void SetVelocity(double vx, double vy) = 0;
    virtual void GetVelocity(double& vx, double& vy) const = 0;

    /// Score: Anzahl gefressener Futterstückchen
    virtual void SetScore(std::size_t score) = 0;
    virtual std::size_t GetScore() const = 0;

    /// ID für Identifikation
    virtual void SetId(std::size_t id) = 0;
    virtual std::size_t GetId() const = 0;
};

/**
 * @brief Konkrete Implementierung einer Zelle.
 */
class CellularAutomaton : public ICellularAutomaton {
public:
    CellularAutomaton();

    void SetPosition(double x, double y) override;
    void GetPosition(double& x, double& y) const override;

    void SetVelocity(double vx, double vy) override;
    void GetVelocity(double& vx, double& vy) const override;

    void SetScore(std::size_t score) override;
    std::size_t GetScore() const override;

    void SetId(std::size_t id) override;
    std::size_t GetId() const override;

private:
    double m_x = 0.0;
    double m_y = 0.0;
    double m_vx = 0.0;
    double m_vy = 0.0;
    std::size_t m_score = 0;
    std::size_t m_id = 0;
};

} // namespace ca
} // namespace core
} // namespace autom

