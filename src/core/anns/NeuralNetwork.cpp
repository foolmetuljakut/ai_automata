#include "NeuralNetwork.h"
#include <algorithm>
#include <cmath>

namespace autom {
namespace core {
namespace anns {

NeuralNetwork::NeuralNetwork(std::size_t inputCount, std::size_t hiddenCount, std::size_t outputCount)
    : m_inputCount(inputCount),
      m_hiddenCount(hiddenCount),
      m_outputCount(outputCount),
      m_inputToHiddenWeights(hiddenCount, inputCount),
      m_hiddenBias(hiddenCount),
      m_hiddenToOutputWeights(outputCount, hiddenCount),
      m_outputBias(outputCount),
      m_hiddenActivation(hiddenCount),
      m_hiddenLinear(hiddenCount),
      m_output(outputCount) {
    // Initialize matrices with zeros
    m_inputToHiddenWeights.setZero();
    m_hiddenBias.setZero();
    m_hiddenToOutputWeights.setZero();
    m_outputBias.setZero();
}

std::size_t NeuralNetwork::GetInputCount() const {
    return m_inputCount;
}

std::size_t NeuralNetwork::GetHiddenCount() const {
    return m_hiddenCount;
}

std::size_t NeuralNetwork::GetOutputCount() const {
    return m_outputCount;
}

const Eigen::MatrixXd& NeuralNetwork::GetInputToHiddenWeights() const {
    return m_inputToHiddenWeights;
}

void NeuralNetwork::SetInputToHiddenWeights(const Eigen::MatrixXd& weights) {
    m_inputToHiddenWeights = weights;
}

const Eigen::VectorXd& NeuralNetwork::GetHiddenBias() const {
    return m_hiddenBias;
}

void NeuralNetwork::SetHiddenBias(const Eigen::VectorXd& bias) {
    m_hiddenBias = bias;
}

const Eigen::MatrixXd& NeuralNetwork::GetHiddenToOutputWeights() const {
    return m_hiddenToOutputWeights;
}

void NeuralNetwork::SetHiddenToOutputWeights(const Eigen::MatrixXd& weights) {
    m_hiddenToOutputWeights = weights;
}

const Eigen::VectorXd& NeuralNetwork::GetOutputBias() const {
    return m_outputBias;
}

void NeuralNetwork::SetOutputBias(const Eigen::VectorXd& bias) {
    m_outputBias = bias;
}

Eigen::VectorXd NeuralNetwork::ReLU(const Eigen::VectorXd& x) const {
    return x.cwiseMax(0.0);  // max(0, x)
}

Eigen::VectorXd NeuralNetwork::ReLUDerivative(const Eigen::VectorXd& activated) const {
    // Derivative of ReLU: 1 wenn > 0, sonst 0
    return (activated.array() > 0.0).cast<double>();
}

Eigen::VectorXd NeuralNetwork::ClampOutput(const Eigen::VectorXd& output) const {
    // Clamp zu [-1, 1]
    return output.cwiseMax(-1.0).cwiseMin(1.0);
}

Eigen::VectorXd NeuralNetwork::Forward(const Eigen::VectorXd& input) {
    // Cache input für Backward Pass
    m_lastInput = input;

    // Hidden Layer: W_ih * x + b_h
    m_hiddenLinear = m_inputToHiddenWeights * input + m_hiddenBias;

    // ReLU Activation
    m_hiddenActivation = ReLU(m_hiddenLinear);

    // Output Layer: W_ho * hidden + b_o
    m_output = m_hiddenToOutputWeights * m_hiddenActivation + m_outputBias;

    // Clamp output zu [-1, 1]
    m_output = ClampOutput(m_output);

    return m_output;
}

const Eigen::VectorXd& NeuralNetwork::GetOutput() const {
    return m_output;
}

void NeuralNetwork::Backward(const Eigen::VectorXd& target, double learningRate) {
    // MSE Error: loss = 0.5 * (output - target)^2
    Eigen::VectorXd outputError = m_output - target;  // dL/dOutput

    // Output Layer Gradient: dL/dW_ho = outputError * hidden^T
    Eigen::MatrixXd dW_ho = outputError * m_hiddenActivation.transpose();

    // Hidden Layer Error (backprop): dL/dHidden = W_ho^T * outputError * ReLU'
    Eigen::VectorXd hiddenError = m_hiddenToOutputWeights.transpose() * outputError;
    Eigen::VectorXd reluDerivative = ReLUDerivative(m_hiddenActivation);
    hiddenError = hiddenError.cwiseProduct(reluDerivative);

    // Hidden Layer Gradient: dL/dW_ih = hiddenError * input^T
    Eigen::MatrixXd dW_ih = hiddenError * m_lastInput.transpose();

    // Weight Updates (nur 1 Iteration, wie spezifiziert)
    m_hiddenToOutputWeights -= learningRate * dW_ho;
    m_inputToHiddenWeights -= learningRate * dW_ih;

    // Bias Updates
    m_outputBias -= learningRate * outputError;
    m_hiddenBias -= learningRate * hiddenError;
}

} // namespace anns
} // namespace core
} // namespace autom

