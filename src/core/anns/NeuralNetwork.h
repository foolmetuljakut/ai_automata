#pragma once

#include <cstddef>
#include <Eigen/Dense>

namespace autom {
namespace core {
namespace anns {

/**
 * @brief Interface für ein neuronales Netzwerk.
 *
 * Dies ist nur die Datenstruktur ohne Logik. Interfaces erlauben Mock-basiertes Testen.
 * Das Netzwerk speichert Gewichte und Strukturinformationen.
 */
class INeuralNetwork {
public:
    virtual ~INeuralNetwork() = default;

    /// Anzahl der Input-Neuronen
    virtual std::size_t GetInputCount() const = 0;

    /// Anzahl der Hidden-Neuronen
    virtual std::size_t GetHiddenCount() const = 0;

    /// Anzahl der Output-Neuronen
    virtual std::size_t GetOutputCount() const = 0;

    /// Gewichte zwischen Input und Hidden Layer
    virtual const Eigen::MatrixXd& GetInputToHiddenWeights() const = 0;
    virtual void SetInputToHiddenWeights(const Eigen::MatrixXd& weights) = 0;

    /// Bias für Hidden Layer
    virtual const Eigen::VectorXd& GetHiddenBias() const = 0;
    virtual void SetHiddenBias(const Eigen::VectorXd& bias) = 0;

    /// Gewichte zwischen Hidden und Output Layer
    virtual const Eigen::MatrixXd& GetHiddenToOutputWeights() const = 0;
    virtual void SetHiddenToOutputWeights(const Eigen::MatrixXd& weights) = 0;

    /// Bias für Output Layer
    virtual const Eigen::VectorXd& GetOutputBias() const = 0;
    virtual void SetOutputBias(const Eigen::VectorXd& bias) = 0;
};

/**
 * @brief Konkrete Implementierung eines neuronalen Netzwerks.
 *
 * Speichert Gewichte und Bias für ein einfaches Feed-Forward Netzwerk.
 * Struktur: Input -> Hidden -> Output
 */
class NeuralNetwork : public INeuralNetwork {
public:
    /**
     * @brief Konstruktor.
     * @param inputCount Anzahl der Input-Neuronen.
     * @param hiddenCount Anzahl der Hidden-Neuronen.
     * @param outputCount Anzahl der Output-Neuronen.
     */
    NeuralNetwork(std::size_t inputCount, std::size_t hiddenCount, std::size_t outputCount);

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

private:
    std::size_t m_inputCount;
    std::size_t m_hiddenCount;
    std::size_t m_outputCount;

    Eigen::MatrixXd m_inputToHiddenWeights;
    Eigen::VectorXd m_hiddenBias;
    Eigen::MatrixXd m_hiddenToOutputWeights;
    Eigen::VectorXd m_outputBias;
};

} // namespace anns
} // namespace core
} // namespace autom

