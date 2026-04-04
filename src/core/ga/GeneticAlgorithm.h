#pragma once

#include <Eigen/Dense>
#include <cstddef>
#include <memory>
#include <vector>
#include <core/ga/GeneticOperations.h>

namespace autom {
namespace core {
namespace anns {
class INeuralNetwork;
}  // namespace anns

namespace ca {
class ICellularAutomaton;
}  // namespace ca

namespace ga {

/**
 * @brief Interface für den Genetischen Algorithmus.
 *
 * Verwaltet eine Population von Individuen (mit NN), führt Selection,
 * Crossover und Mutation durch, um die Population zu verbessern.
 */
class IGeneticAlgorithm {
public:
    virtual ~IGeneticAlgorithm() = default;

    /**
     * @brief Führe einen Generations-Schritt aus.
     *
     * Schritte:
     * 1. Evaluiere Fitness aller Individuen (muss extern gesetzt werden via SetIndividualFitness)
     * 2. Selection: Behalte nur Individuen mit Score-Verbesserung
     * 3. Reproduction: Erzeuge neue Individuen durch Crossover + Mutation
     * 4. Population auf Target-Größe bringen
     */
    virtual void ExecuteGenerationStep() = 0;

    /**
     * @brief Setze die Fitness eines Individuums.
     *
     * @param individualIndex Index des Individuums in der Population
     * @param score Neuer Score
     */
    virtual void SetIndividualFitness(std::size_t individualIndex, double score) = 0;

    /**
     * @brief Hole die Fitness eines Individuums.
     */
    virtual double GetIndividualFitness(std::size_t individualIndex) const = 0;

    /**
     * @brief Hole die beste Fitness der aktuellen Population.
     */
    virtual double GetBestFitness() const = 0;

    /**
     * @brief Hole die durchschnittliche Fitness der aktuellen Population.
     */
    virtual double GetAverageFitness() const = 0;

    /**
     * @brief Hole die Population-Größe.
     */
    virtual std::size_t GetPopulationSize() const = 0;

    /**
     * @brief Hole die Fitness-Historie (für Tracking über Generationen).
     */
    virtual const std::vector<double>& GetFitnessHistory() const = 0;

    /**
     * @brief Setze die Mutationsrate.
     */
    virtual void SetMutationRate(double rate) = 0;

    /**
     * @brief Hole die aktuelle Mutationsrate.
     */
    virtual double GetMutationRate() const = 0;

    /**
     * @brief Hole das NN eines Individuums.
     */
    virtual std::shared_ptr<anns::INeuralNetwork> GetIndividualNetwork(std::size_t index) = 0;
};

/**
 * @brief Konkrete Implementierung des Genetischen Algorithmus.
 *
 * Verwaltet eine Population von NeuralNetworks und optimiert sie via Selection/Crossover/Mutation.
 */
class GeneticAlgorithm : public IGeneticAlgorithm {
public:
    /**
     * @brief Konstruktor.
     *
     * @param populationSize Zielgröße der Population
     * @param inputCount Anzahl Input-Neuronen für alle NN
     * @param hiddenCount Anzahl Hidden-Neuronen für alle NN
     * @param outputCount Anzahl Output-Neuronen für alle NN
     * @param mutationRate Mutationswahrscheinlichkeit pro Wert (default: 1e-3)
     */
    GeneticAlgorithm(
        std::size_t populationSize,
        std::size_t inputCount,
        std::size_t hiddenCount,
        std::size_t outputCount,
        double mutationRate = 1e-3);

    // ========== IGeneticAlgorithm INTERFACE ==========

    void ExecuteGenerationStep() override;
    void SetIndividualFitness(std::size_t individualIndex, double score) override;
    double GetIndividualFitness(std::size_t individualIndex) const override;
    double GetBestFitness() const override;
    double GetAverageFitness() const override;
    std::size_t GetPopulationSize() const override;
    const std::vector<double>& GetFitnessHistory() const override;
    void SetMutationRate(double rate) override;
    double GetMutationRate() const override;
    std::shared_ptr<anns::INeuralNetwork> GetIndividualNetwork(std::size_t index) override;

    // ========== EXTRA METHODEN ==========

    /**
     * @brief Hole die Größe der Population vor Selection (für Testing).
     */
    std::size_t GetLastGenerationPreSelectionSize() const;

    /**
     * @brief Hole die Größe der Population nach Selection (für Testing).
     */
    std::size_t GetLastGenerationPostSelectionSize() const;

private:
    std::size_t m_populationSize;
    std::size_t m_inputCount;
    std::size_t m_hiddenCount;
    std::size_t m_outputCount;

    std::unique_ptr<GeneticOperations> m_operations;

    // Population: NeuralNetworks
    std::vector<std::shared_ptr<anns::INeuralNetwork>> m_population;

    // Fitness-Tracking
    std::vector<double> m_currentFitness;     // Fitness der aktuellen Generation
    std::vector<double> m_previousFitness;    // Fitness der vorherigen Generation (für Verbesserung)
    std::vector<double> m_fitnessHistory;     // Best Fitness pro Generation

    // Debug Info
    std::size_t m_lastPreSelectionSize = 0;
    std::size_t m_lastPostSelectionSize = 0;

    /**
     * @brief Initialisiere die Population mit zufälligen NN.
     */
    void InitializePopulation();

    /**
     * @brief Selektiere Individuen basierend auf Fitness-Verbesserung.
     *
     * @return Indices der ausgewählten Individuen
     */
    std::vector<std::size_t> Selection();

    /**
     * @brief Erzeuge neue Individuen durch Crossover + Mutation.
     *
     * @param selectedIndices Indices der ausgewählten Individuen
     * @param targetSize Gewünschte Population-Größe
     */
    void Reproduction(const std::vector<std::size_t>& selectedIndices, std::size_t targetSize);

    /**
     * @brief Berechne Fitness-Statistiken.
     */
    void UpdateFitnessStatistics();
};

}  // namespace ga
}  // namespace core
}  // namespace autom



