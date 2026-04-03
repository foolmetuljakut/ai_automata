#pragma once

#include "core/ca/CellularAutomaton.h"

namespace autom {
namespace test {
namespace mocks {

/**
 * @brief Mock-Implementierung von ICellularAutomaton für Unit-Tests.
 *
 * Erlaubt es, Verhalten zu spezifizieren und Aufrufe zu überwachen.
 */
class MockCellularAutomaton : public core::ca::ICellularAutomaton {
public:
    MockCellularAutomaton();

    void SetPosition(double x, double y) override;
    void GetPosition(double& x, double& y) const override;

    void SetVelocity(double vx, double vy) override;
    void GetVelocity(double& vx, double& vy) const override;

    void SetScore(std::size_t score) override;
    std::size_t GetScore() const override;

    void SetId(std::size_t id) override;
    std::size_t GetId() const override;

    // Mock-spezifische Methoden zur Überwachung
    int GetSetPositionCallCount() const { return m_setPositionCallCount; }
    int GetGetPositionCallCount() const { return m_getPositionCallCount; }

private:
    double m_x = 0.0;
    double m_y = 0.0;
    double m_vx = 0.0;
    double m_vy = 0.0;
    std::size_t m_score = 0;
    std::size_t m_id = 0;

    // Call counters für Tests (mutable um in const-Methoden inkrementiert zu werden)
    mutable int m_setPositionCallCount = 0;
    mutable int m_getPositionCallCount = 0;
};

} // namespace mocks
} // namespace test
} // namespace autom


