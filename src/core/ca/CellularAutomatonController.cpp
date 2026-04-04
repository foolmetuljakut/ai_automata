#include "CellularAutomatonController.h"
#include "../anns/NeuralNetwork.h"
#include <cmath>
#include <algorithm>

namespace autom {
namespace core {
namespace ca {

CellularAutomatonController::CellularAutomatonController(
    std::shared_ptr<ICellularAutomaton> cell,
    std::shared_ptr<autom::core::anns::INeuralNetwork> network
)
    : m_cell(cell), m_network(network) {
    // Default Lern-Target: NN sollte Output geben, der in Richtung 0° (rechts) geht
    // Das ist ein 2D Target für den 2D Output!
    m_learningTarget = Eigen::VectorXd::Zero(2);
    m_learningTarget(0) = 1.0;  // Output[0] sollte positiv sein (rechts)
    m_learningTarget(1) = 0.0;  // Output[1] sollte neutral sein

    // Initialisiere Explorer-Richtung
    GenerateNewExplorationDirection();
}

void CellularAutomatonController::Update(
    const Eigen::Vector2d& targetPoint,
    double deltaTime,
    float sensorRange
) {
    if (!m_cell || !m_network) {
        return;
    }

    Eigen::VectorXd sensorInputs = Eigen::VectorXd::Zero(5);
    int closestSensor;
    UpdateSensors(targetPoint, sensorRange, sensorInputs, closestSensor);
    Eigen::VectorXd nnOutput = m_network->Forward(sensorInputs);
    double targetAngle = ComputeTargetAngle(nnOutput, closestSensor);
    UpdateAngleInterpolation(targetAngle);
    UpdateVelocity();
    UpdateLearning();
    ReduceFlexibility();
}

void CellularAutomatonController::UpdateSensors(
    const Eigen::Vector2d& targetPoint,
    float sensorRange,
    Eigen::VectorXd& outSensorInputs,
    int& outClosestSensor
) {
    // Hole aktuelle Position und Rotation der Zelle
    double cellX, cellY;
    m_cell->GetPosition(cellX, cellY);
    Eigen::Vector2d cellPos(cellX, cellY);
    double rotation = m_cell->GetRotation();

    // Berechne welcher Sensor den Target sieht
    outClosestSensor = ComputeClosestSensor(targetPoint, cellPos, rotation, sensorRange);

    // Aktualisiere Sensor-Werte und Food-Visibility-Counter
    if (outClosestSensor >= 0 && outClosestSensor < 5) {
        double distance = (targetPoint - cellPos).norm();
        double normalizedDistance = NormalizeDistance(distance, sensorRange);
        outSensorInputs(outClosestSensor) = normalizedDistance;
        m_foodVisibleFrames++;
    } else {
        m_foodVisibleFrames = 0;
    }

    // Schreibe Sensor-Inputs zur Zelle
    m_cell->SetSensorInputs(outSensorInputs);
}

double CellularAutomatonController::ComputeTargetAngle(
    const Eigen::VectorXd& nnOutput,
    int closestSensor
) {
    double targetAngle = 0.0;

    if (closestSensor >= 0) {
        // Futter sichtbar: Nutze NN-Output als Ziel-Richtung
        targetAngle = std::atan2(nnOutput(1), nnOutput(0));
    } else {
        // Futter nicht sichtbar: explorative Random-Bewegung
        m_explorationCounter++;
        if (m_explorationCounter >= EXPLORATION_DURATION) {
            GenerateNewExplorationDirection();
            m_explorationCounter = 0;
        }
        targetAngle = std::atan2(m_explorationDirection(1), m_explorationDirection(0));
    }

    return targetAngle;
}

void CellularAutomatonController::UpdateAngleInterpolation(double targetAngle) {
    // Graduell den aktuellen Winkel zum Ziel-Winkel verkippen
    // Dadurch: Smooth curves statt Oszillation!
    double angleDiff = targetAngle - m_currentAngle;

    // Normalisiere angleDiff zu [-π, π]
    while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2 * M_PI;

    // Interpoliere: ANGLE_INTERPOLATION % pro Frame zum Ziel
    m_currentAngle += angleDiff * ANGLE_INTERPOLATION;
}

void CellularAutomatonController::UpdateVelocity() {
    // Konvertiere aktuellen Winkel zu Velocity-Vektor
    Eigen::Vector2d directionVector(
        std::cos(m_currentAngle),
        std::sin(m_currentAngle)
    );

    // Velocity basierend auf persistenter Richtung
    // Scale: 50 Pixel/Sekunde
    Eigen::Vector2d newVelocity = directionVector * 50.0;
    m_cell->SetVelocity(newVelocity(0), newVelocity(1));
}

void CellularAutomatonController::UpdateLearning() {
    if (ShouldLearn()) {
        // Backpropagation mit aktueller Lernrate
        double learningRate = m_cell->GetFlexibility();
        m_network->Backward(m_learningTarget, learningRate);
    }
}

void CellularAutomatonController::ReduceFlexibility() {
    // Reduktion: 10% pro Frame, damit Zelle mit der Zeit "sicherer" wird
    m_cell->ReduceFlexibility(0.01 * m_cell->GetFlexibility());
}

void CellularAutomatonController::SetLearningTarget(const Eigen::VectorXd& target) {
    m_learningTarget = target;
}

bool CellularAutomatonController::ShouldLearn() const {
    if (m_foodVisibleFrames <= FOOD_VISIBLE_THRESHOLD) {
        return false;
    }

    // Prüfe ob Futter NICHT im mittleren Sensor ist
    const Eigen::VectorXd& sensors = m_cell->GetSensorInputs();
    if (sensors(2) > -0.5) {  // Mittlerer Sensor hat kaum Signal
        return true;
    }

    return false;
}

void CellularAutomatonController::Reset() {
    m_foodVisibleFrames = 0;
}

int CellularAutomatonController::ComputeClosestSensor(
    const Eigen::Vector2d& targetPoint,
    const Eigen::Vector2d& cellPos,
    double rotation,
    float sensorRange
) const {
    // Berechne Richtung vom Cell zum Target
    Eigen::Vector2d direction = targetPoint - cellPos;
    double distance = direction.norm();

    // Prüfe ob im Range
    if (distance > sensorRange || distance < 0.1) {
        return -1;  // Außerhalb oder zu nah
    }

    // Normalisiere
    direction /= distance;

    // Berechne Angle im globalen Koordinatensystem
    double targetAngle = std::atan2(direction(1), direction(0));

    // Normalisiere Rotation und targetAngle auf [-π, π]
    while (rotation > M_PI) rotation -= 2 * M_PI;
    while (rotation < -M_PI) rotation += 2 * M_PI;
    while (targetAngle > M_PI) targetAngle -= 2 * M_PI;
    while (targetAngle < -M_PI) targetAngle += 2 * M_PI;

    // Berechne Winkel-Differenz relativ zur Zell-Rotation
    double angleDiff = targetAngle - rotation;

    // Normalisiere auf [-π, π]
    while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2 * M_PI;

    // Prüfe ob im Sensorbereich (±π/2)
    if (std::abs(angleDiff) > M_PI / 2.0) {
        return -1;  // Außerhalb Sensorbereich
    }

    // Mapping: angleDiff [-π/2, π/2] → Sensor [0, 4]
    // -π/2 → 0
    // 0 → 2 (mitte)
    // π/2 → 4
    int sensorIndex = 2 + static_cast<int>((angleDiff / (M_PI / 4.0)));
    sensorIndex = std::max(0, std::min(4, sensorIndex));

    return sensorIndex;
}

double CellularAutomatonController::NormalizeDistance(double distance, float sensorRange) const {
    // Normalisiere zu [0, 1]
    // distance = 0 → 1.0 (sehr nah, starkes Signal)
    // distance = sensorRange → 0.0 (am Rand, schwaches Signal)
    // WICHTIG: Positiv, damit ReLU die Werte nicht zerstört!
    double normalized = 1.0 - (distance / sensorRange);
    return std::max(0.0, std::min(1.0, normalized));
}

Eigen::Vector2d CellularAutomatonController::ComputeVelocityFromOutput(
    const Eigen::VectorXd& nnOutput,
    double cellRotation
) const {
    // NN Output: [out0, out1] in Range [-1, 1]
    // Das ist die NEUE Richtung, nicht relativ zur aktuellen Rotation!

    double magnitude = std::sqrt(nnOutput(0) * nnOutput(0) + nnOutput(1) * nnOutput(1));

    Eigen::Vector2d velocity(0, 0);

    if (magnitude > 0.01) {  // Verhindere Division durch 0
        // Normalisiere zu Einheitsvektor
        velocity(0) = nnOutput(0) / magnitude;
        velocity(1) = nnOutput(1) / magnitude;


        // Scale zu realistischer Geschwindigkeit (z.B. 50 Pixel/Sekunde)
        velocity *= 50.0;
    }

    return velocity;
}

void CellularAutomatonController::GenerateNewExplorationDirection() {
    // Generiere neue zufällige Explorations-Richtung
    double angle = (std::rand() % 360) * M_PI / 180.0;
    m_explorationDirection(0) = std::cos(angle);
    m_explorationDirection(1) = std::sin(angle);
}

} // namespace ca
} // namespace core
} // namespace autom

