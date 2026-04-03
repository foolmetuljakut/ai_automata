#include "MockNeuralNetwork.h"

namespace autom {
namespace test {
namespace mocks {

MockNeuralNetwork::MockNeuralNetwork(std::size_t inputCount, std::size_t hiddenCount, std::size_t outputCount)
    : m_inputCount(inputCount),
      m_hiddenCount(hiddenCount),
      m_outputCount(outputCount),
      m_inputToHiddenWeights(hiddenCount, inputCount),
      m_hiddenBias(hiddenCount),
      m_hiddenToOutputWeights(outputCount, hiddenCount),
      m_outputBias(outputCount),
      m_setInputToHiddenWeightsCallCount(0) {
    // Initialize matrices with zeros
    m_inputToHiddenWeights.setZero();
    m_hiddenBias.setZero();
    m_hiddenToOutputWeights.setZero();
    m_outputBias.setZero();
}

std::size_t MockNeuralNetwork::GetInputCount() const {
    return m_inputCount;
}

std::size_t MockNeuralNetwork::GetHiddenCount() const {
    return m_hiddenCount;
}

std::size_t MockNeuralNetwork::GetOutputCount() const {
    return m_outputCount;
}

const Eigen::MatrixXd& MockNeuralNetwork::GetInputToHiddenWeights() const {
    return m_inputToHiddenWeights;
}

void MockNeuralNetwork::SetInputToHiddenWeights(const Eigen::MatrixXd& weights) {
    m_setInputToHiddenWeightsCallCount++;
    m_inputToHiddenWeights = weights;
}

const Eigen::VectorXd& MockNeuralNetwork::GetHiddenBias() const {
    return m_hiddenBias;
}

void MockNeuralNetwork::SetHiddenBias(const Eigen::VectorXd& bias) {
    m_hiddenBias = bias;
}

const Eigen::MatrixXd& MockNeuralNetwork::GetHiddenToOutputWeights() const {
    return m_hiddenToOutputWeights;
}

void MockNeuralNetwork::SetHiddenToOutputWeights(const Eigen::MatrixXd& weights) {
    m_hiddenToOutputWeights = weights;
}

const Eigen::VectorXd& MockNeuralNetwork::GetOutputBias() const {
    return m_outputBias;
}

void MockNeuralNetwork::SetOutputBias(const Eigen::VectorXd& bias) {
    m_outputBias = bias;
}

} // namespace mocks
} // namespace test
} // namespace autom

