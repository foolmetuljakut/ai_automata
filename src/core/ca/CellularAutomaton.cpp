#include "CellularAutomaton.h"

namespace autom {
namespace core {
namespace ca {

CellularAutomaton::CellularAutomaton()
    : m_x(0.0), m_y(0.0), m_vx(0.0), m_vy(0.0), m_score(0), m_id(0) {
}

void CellularAutomaton::SetPosition(double x, double y) {
    m_x = x;
    m_y = y;
}

void CellularAutomaton::GetPosition(double& x, double& y) const {
    x = m_x;
    y = m_y;
}

void CellularAutomaton::SetVelocity(double vx, double vy) {
    m_vx = vx;
    m_vy = vy;
}

void CellularAutomaton::GetVelocity(double& vx, double& vy) const {
    vx = m_vx;
    vy = m_vy;
}

void CellularAutomaton::SetScore(std::size_t score) {
    m_score = score;
}

std::size_t CellularAutomaton::GetScore() const {
    return m_score;
}

void CellularAutomaton::SetId(std::size_t id) {
    m_id = id;
}

std::size_t CellularAutomaton::GetId() const {
    return m_id;
}

} // namespace ca
} // namespace core
} // namespace autom

