#include "MockCellularAutomaton.h"

namespace autom {
namespace test {
namespace mocks {

MockCellularAutomaton::MockCellularAutomaton()
    : m_x(0.0), m_y(0.0), m_vx(0.0), m_vy(0.0), m_score(0), m_id(0),
      m_setPositionCallCount(0), m_getPositionCallCount(0) {
}

void MockCellularAutomaton::SetPosition(double x, double y) {
    m_setPositionCallCount++;
    m_x = x;
    m_y = y;
}

void MockCellularAutomaton::GetPosition(double& x, double& y) const {
    m_getPositionCallCount++;
    x = m_x;
    y = m_y;
}

void MockCellularAutomaton::SetVelocity(double vx, double vy) {
    m_vx = vx;
    m_vy = vy;
}

void MockCellularAutomaton::GetVelocity(double& vx, double& vy) const {
    vx = m_vx;
    vy = m_vy;
}

void MockCellularAutomaton::SetScore(std::size_t score) {
    m_score = score;
}

std::size_t MockCellularAutomaton::GetScore() const {
    return m_score;
}

void MockCellularAutomaton::SetId(std::size_t id) {
    m_id = id;
}

std::size_t MockCellularAutomaton::GetId() const {
    return m_id;
}

} // namespace mocks
} // namespace test
} // namespace autom

