#pragma once

#include "core/anns/NeuralNetwork.h"

namespace autom {
namespace test {
namespace mocks {

/**
 * @brief Mock-Implementierung von INeuralNetwork für Unit-Tests.
 *
 * Erlaubt es, Verhalten zu spezifizieren und Aufrufe zu überwachen.
 */
class MockNeuralNetwork : public core::anns::INeuralNetwork {
public:
    MockNeuralNetwork(std::size_t inputCount, std::size_t hiddenCount, std::size_t outputCount);

    std::size_t GetInputCount() const override;
    std::size_t GetHiddenCount() const override;
    std::size_t GetOutputCount() const override;

    const Eigen::MatrixXd& GetInputToHiddenWeights() const override;
    void SetInputToHiddenWeights(const Eigen::MatrixXd& weights) override;

    const Eigen::VectorXd& GetHiddenBias() const override;
    void SetHiddenBias(const Eigen::VectorXd& bias) override;

    const Eigen::MatrixXd& GetHiddenToOutputWeights() const override;
    void SetHiddenToOutputWeights(const Eigen::MatrixXd& weights) override;

    const Eigen::VectorXd& GetOutputBias() const override;
    void SetOutputBias(const Eigen::VectorXd& bias) override;

    // Mock-spezifische Methoden zur Überwachung
    int GetSetInputToHiddenWeightsCallCount() const { return m_setInputToHiddenWeightsCallCount; }

private:
    std::size_t m_inputCount;
    std::size_t m_hiddenCount;
    std::size_t m_outputCount;

    Eigen::MatrixXd m_inputToHiddenWeights;
    Eigen::VectorXd m_hiddenBias;
    Eigen::MatrixXd m_hiddenToOutputWeights;
    Eigen::VectorXd m_outputBias;

    // Call counters für Tests (mutable um in const-Methoden inkrementiert zu werden)
    mutable int m_setInputToHiddenWeightsCallCount = 0;
};

} // namespace mocks
} // namespace test
} // namespace autom


