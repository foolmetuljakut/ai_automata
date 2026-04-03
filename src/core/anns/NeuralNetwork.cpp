#include "NeuralNetwork.h"

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
      m_outputBias(outputCount) {
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

} // namespace anns
} // namespace core
} // namespace autom

