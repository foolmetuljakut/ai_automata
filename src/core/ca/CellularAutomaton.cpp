#include "CellularAutomaton.h"
#include <cmath>

namespace autom {
namespace core {
namespace ca {

CellularAutomaton::CellularAutomaton()
    : m_x(0.0), m_y(0.0), m_vx(0.0), m_vy(0.0), m_rotation(0.0),
      m_sensorInputs(Eigen::VectorXd::Zero(5)), m_flexibility(1.0), m_score(0), m_id(0) {
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

    // Berechne Rotation basierend auf Velocity
    // atan2(vy, vx) gibt Winkel in Radians
    m_rotation = std::atan2(vy, vx);
}

void CellularAutomaton::GetVelocity(double& vx, double& vy) const {
    vx = m_vx;
    vy = m_vy;
}

void CellularAutomaton::SetRotation(double angle) {
    m_rotation = angle;
}

double CellularAutomaton::GetRotation() const {
    return m_rotation;
}

void CellularAutomaton::SetSensorInputs(const Eigen::VectorXd& inputs) {
    // Kopiere nur wenn Größe passt
    if (inputs.size() == 5) {
        m_sensorInputs = inputs;
    }
}

const Eigen::VectorXd& CellularAutomaton::GetSensorInputs() const {
    return m_sensorInputs;
}

void CellularAutomaton::SetFlexibility(double flex) {
    // Clamp zu [0, 1]
    m_flexibility = std::max(0.0, std::min(1.0, flex));
}

double CellularAutomaton::GetFlexibility() const {
    return m_flexibility;
}

void CellularAutomaton::ReduceFlexibility(double rate) {
    m_flexibility -= rate;
    if (m_flexibility < 0.0) {
        m_flexibility = 0.0;
    }
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

