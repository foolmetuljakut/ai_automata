#pragma once

#include <Eigen/Dense>

namespace autom {
namespace math {

/**
 * @brief Eine Basis-Klasse fuer mathematische Operationen auf Neuronalen Netzen.
 *
 * Demonstriert die Anwendung des Single Responsibility Principles.
 * Berechnet beispielsweise die Linearkombination aus Gewichten und Eingaben (W * x + b).
 */
class NeuralMath {
public:
    /**
     * @brief Berechnet den Output eines Layers ohne Aktivierungsfunktion.
     *
     * @param weights Matrix der Gewichte (M x N).
     * @param inputs Vektor der Eingaben (N).
     * @param bias Vektor der Fehlerabweichungen (M).
     * @return Berechnetes Ergebnis als Vektor (M).
     */
    static Eigen::VectorXd ComputeLayerOutput(const Eigen::MatrixXd& weights,
                                              const Eigen::VectorXd& inputs,
                                              const Eigen::VectorXd& bias);
};

} // namespace math
} // namespace autom

