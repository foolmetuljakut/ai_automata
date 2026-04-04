#pragma once

#include <memory>
#include <Eigen/Dense>
#include "CellularAutomaton.h"

namespace autom {
namespace core {
namespace anns {
class INeuralNetwork;
}  // namespace anns
}  // namespace core
}  // namespace autom

namespace autom {
namespace core {
namespace ca {

/**
 * @brief Controller für Zellen-Verhalten.
 *
 * Koordiniert:
 * - Sensorik-Update basierend auf Umgebung
 * - NN Forward-Pass
 * - Velocity Update
 * - Backpropagation bei Lernbedingungen
 * - Flexibilität-Reduktion
 */
class CellularAutomatonController {
public:
    /**
     * @brief Konstruktor.
     * @param cell Die zu kontrollierende Zelle
     * @param network Das NN für diese Zelle
     */
    CellularAutomatonController(
        std::shared_ptr<ICellularAutomaton> cell,
        std::shared_ptr<autom::core::anns::INeuralNetwork> network
    );

    /**
     * @brief Haupt-Update-Methode.
     *
     * Führt durch:
     * 1. Sensorik Update (basierend auf targetPoint)
     * 2. NN Forward Pass
     * 3. Velocity Update
     * 4. Rotation Update
     * 5. Optional Backpropagation (wenn Bedingungen erfüllt)
     * 6. Flexibilität Reduktion
     *
     * @param targetPoint Position eines sichtbaren Punktes (z.B. Futter)
     * @param deltaTime Zeit seit letztem Update
     * @param sensorRange Reichweite der Sensoren
     */
    void Update(
        const Eigen::Vector2d& targetPoint,
        double deltaTime,
        float sensorRange = 100.0f
    );

    /**
     * @brief Setze den zu lernenden Target für Backprop.
     *
     * Target sollte [0, 0, 1, 0, 0] sein (mittlerer Sensor).
     */
    void SetLearningTarget(const Eigen::VectorXd& target);

    /**
     * @brief Prüfe ob Backprop aktiv werden sollte.
     *
     * Aktiv wenn:
     * - Futter sichtbar (in mindestens einem Sensor)
     * - Futter NICHT in mittlerem Sensor
     * - Sichtbarkeits-Counter > Threshold
     */
    bool ShouldLearn() const;

    /**
     * @brief Reset für neue Epoche.
     */
    void Reset();

private:
    std::shared_ptr<ICellularAutomaton> m_cell;
    std::shared_ptr<autom::core::anns::INeuralNetwork> m_network;

    // Lern-Parameter
    Eigen::VectorXd m_learningTarget;
    int m_foodVisibleFrames = 0;
    const int FOOD_VISIBLE_THRESHOLD = 5;  // Minimum Frames bevor Backprop

    // Explorer-Parameter (NEU für Smooth Exploration)
    Eigen::Vector2d m_explorationDirection = Eigen::Vector2d::Zero();
    int m_explorationCounter = 0;
    const int EXPLORATION_DURATION = 200;  // Frames bis neue Richtung (war 20, zu schnell!)

    // Persistente Bewegungs-Richtung (NEU für Smooth Direction Changes)
    double m_currentAngle = 0.0;  // Aktuelle Bewegungs-Richtung in Radians
    const double ANGLE_INTERPOLATION = 0.05;  // 5% pro Frame = smooth curve

    // NN Output für Geschwindigkeit (für velocity magnitude berechnung)
    Eigen::VectorXd m_lastNNOutput = Eigen::VectorXd::Zero(2);  // Speichert letzten NN-Output


    /**
     * @brief Generiere neue zufällige Explorations-Richtung
     */
    void GenerateNewExplorationDirection();

    /**
     * @brief (Schritt 1) Aktualisiere Sensor-Inputs basierend auf Ziel-Position.
     *
     * @param targetPoint Position des Ziels
     * @param sensorRange Reichweite der Sensoren
     * @param outSensorInputs Gefüllter Vektor der Sensor-Eingaben
     * @param outClosestSensor Index des nächsten Sensors (-1 wenn keiner sichtbar)
     */
    void UpdateSensors(
        const Eigen::Vector2d& targetPoint,
        float sensorRange,
        Eigen::VectorXd& outSensorInputs,
        int& outClosestSensor
    );

    /**
     * @brief (Schritt 2-3) Berechne Ziel-Winkel aus NN-Output oder Exploration.
     *
     * @param nnOutput Output des Neuronalen Netzwerks
     * @param closestSensor Index des nächsten Sensors
     * @return Ziel-Winkel in Radians
     */
    double ComputeTargetAngle(
        const Eigen::VectorXd& nnOutput,
        int closestSensor
    );

    /**
     * @brief (Schritt 3) Interpoliere aktuellen Winkel zum Ziel-Winkel.
     *
     * Erstellt smooth curves statt abrupter Richtungswechsel.
     *
     * @param targetAngle Gewünschter Winkel
     */
    void UpdateAngleInterpolation(double targetAngle);

    /**
     * @brief (Schritt 4) Aktualisiere Velocity basierend auf aktueller Richtung.
     */
    void UpdateVelocity();

    /**
     * @brief (Schritt 5) Führe optionales Backpropagation durch.
     *
     * Nur wenn ShouldLearn() true ist.
     */
    void UpdateLearning();

    /**
     * @brief (Schritt 6) Reduziere Flexibilität der Zelle.
     */
    void ReduceFlexibility();

    /**
     * @brief Berechne welcher Sensor "sieht" den Punkt.
     *
     * Gibt den nächsten Sensor-Index zurück (-1 wenn außer Range).
     */
    int ComputeClosestSensor(
        const Eigen::Vector2d& targetPoint,
        const Eigen::Vector2d& cellPos,
        double rotation,
        float sensorRange
    ) const;

    /**
     * @brief Normalisiere Abstand zu [0, 1] Range.
     */
    double NormalizeDistance(double distance, float sensorRange) const;

    /**
     * @brief Rotiere NN Output zu Velocity basierend auf Zell-Rotation.
     */
    Eigen::Vector2d ComputeVelocityFromOutput(
        const Eigen::VectorXd& nnOutput,
        double cellRotation
    ) const;
};

} // namespace ca
} // namespace core
} // namespace autom


