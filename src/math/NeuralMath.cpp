#include "NeuralMath.h"
#include <cmath>

namespace autom {
namespace math {

Eigen::VectorXd NeuralMath::ComputeLayerOutput(const Eigen::MatrixXd& weights,
                                               const Eigen::VectorXd& inputs,
                                               const Eigen::VectorXd& bias) {
    // Berechne: output = weights * inputs + bias
    // SRP und klare Schichtentrennung: Reines Crunchen von Zahlen,
    // keine Abhaengigkeiten zur UI oder anderer Logik.
    return (weights * inputs) + bias;
}

Eigen::VectorXd NeuralMath::ApplyReLU(const Eigen::VectorXd& input) {
    // ReLU: max(0, x)
    return input.cwiseMax(0.0);
}

Eigen::VectorXd NeuralMath::ApplyTanh(const Eigen::VectorXd& input) {
    // Tanh: element-wise auf jedes Element anwenden
    return input.array().tanh().matrix();
}

} // namespace math
} // namespace autom

