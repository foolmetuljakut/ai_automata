#include "NeuralMath.h"

namespace autom {
namespace math {

Eigen::VectorXd NeuralMath::ComputeLayerOutput(const Eigen::MatrixXd& weights,
                                               const Eigen::VectorXd& inputs,
                                               const Eigen::VectorXd& bias) {
    // Einfache lineare Algebra fuer Neuoronale Netzwerke.
    // SRP und klare Schichtentrennung: Reines Crunchen von Zahlen,
    // keine Abhaengigkeiten zur UI oder anderer Logik.
    return (weights * inputs) + bias;
}

} // namespace math
} // namespace autom

