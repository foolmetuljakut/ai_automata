#pragma once

#include <Eigen/Dense>

namespace autom {
namespace math {

/**
 * @brief Wrapper-Klasse für neuronale Netzwerk-Berechnungen mit Eigen3.
 *
 * Kapselt Eigen3-Komplexität hinter einer sauberen Schnittstelle.
 * Ermöglicht die Propagierung von Input durch ein neuronales Netzwerk.
 */
class NeuralMath {
public:
    /**
     * @brief Berechnet den Output eines Layers ohne Aktivierungsfunktion.
     *
     * Berechnet: output = weights * input + bias
     *
     * @param weights Matrix der Gewichte (M x N).
     * @param inputs Vektor der Eingaben (N).
     * @param bias Vektor der Fehlerabweichungen (M).
     * @return Berechnetes Ergebnis als Vektor (M).
     */
    static Eigen::VectorXd ComputeLayerOutput(const Eigen::MatrixXd& weights,
                                              const Eigen::VectorXd& inputs,
                                              const Eigen::VectorXd& bias);

    /**
     * @brief Wendet eine ReLU-Aktivierungsfunktion an.
     *
     * @param input Eingabevektor.
     * @return Vektor mit angewendeter ReLU (max(0, x)).
     */
    static Eigen::VectorXd ApplyReLU(const Eigen::VectorXd& input);

    /**
     * @brief Wendet eine Tanh-Aktivierungsfunktion an.
     *
     * @param input Eingabevektor.
     * @return Vektor mit angewendeter Tanh.
     */
    static Eigen::VectorXd ApplyTanh(const Eigen::VectorXd& input);
};

} // namespace math
} // namespace autom

