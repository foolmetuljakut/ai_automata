#include <core/ga/GeneticAlgorithm.h>
#include <core/ga/GeneticOperations.h>
#include <core/anns/NeuralNetwork.h>
#include <random>
#include <algorithm>
#include <limits>

namespace autom {
namespace core {
namespace ga {

using autom::core::anns::NeuralNetwork;
using autom::core::anns::INeuralNetwork;

// Static random generator
static std::mt19937& GetRNG() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

GeneticAlgorithm::GeneticAlgorithm(
    std::size_t populationSize,
    std::size_t inputCount,
    std::size_t hiddenCount,
    std::size_t outputCount,
    double mutationRate)
    : m_populationSize(populationSize),
      m_inputCount(inputCount),
      m_hiddenCount(hiddenCount),
      m_outputCount(outputCount),
      m_operations(std::make_unique<GeneticOperations>(mutationRate)) {

    InitializePopulation();
    m_currentFitness.assign(populationSize, 0.0);
    m_previousFitness.assign(populationSize, 0.0);
}

void GeneticAlgorithm::InitializePopulation() {
    m_population.clear();

    // Random distribution for weight initialization
    std::normal_distribution<double> weightDist(0.0, 0.1);

    for (std::size_t i = 0; i < m_populationSize; ++i) {
        auto nn = std::make_shared<NeuralNetwork>(
            m_inputCount, m_hiddenCount, m_outputCount);

        // Initialize weights randomly
        Eigen::MatrixXd w_ih(m_hiddenCount, m_inputCount);
        for (int row = 0; row < w_ih.rows(); ++row) {
            for (int col = 0; col < w_ih.cols(); ++col) {
                w_ih(row, col) = weightDist(GetRNG());
            }
        }

        Eigen::VectorXd b_h(m_hiddenCount);
        for (int j = 0; j < b_h.size(); ++j) {
            b_h(j) = weightDist(GetRNG());
        }

        Eigen::MatrixXd w_ho(m_outputCount, m_hiddenCount);
        for (int row = 0; row < w_ho.rows(); ++row) {
            for (int col = 0; col < w_ho.cols(); ++col) {
                w_ho(row, col) = weightDist(GetRNG());
            }
        }

        Eigen::VectorXd b_o(m_outputCount);
        for (int j = 0; j < b_o.size(); ++j) {
            b_o(j) = weightDist(GetRNG());
        }

        nn->SetInputToHiddenWeights(w_ih);
        nn->SetHiddenBias(b_h);
        nn->SetHiddenToOutputWeights(w_ho);
        nn->SetOutputBias(b_o);

        m_population.push_back(nn);
    }
}

void GeneticAlgorithm::SetIndividualFitness(std::size_t individualIndex, double score) {
    if (individualIndex >= m_currentFitness.size()) {
        throw std::out_of_range("Individual index out of range");
    }
    m_currentFitness[individualIndex] = score;
}

double GeneticAlgorithm::GetIndividualFitness(std::size_t individualIndex) const {
    if (individualIndex >= m_currentFitness.size()) {
        throw std::out_of_range("Individual index out of range");
    }
    return m_currentFitness[individualIndex];
}

double GeneticAlgorithm::GetBestFitness() const {
    if (m_currentFitness.empty()) {
        return 0.0;
    }
    return *std::max_element(m_currentFitness.begin(), m_currentFitness.end());
}

double GeneticAlgorithm::GetAverageFitness() const {
    if (m_currentFitness.empty()) {
        return 0.0;
    }

    double sum = 0.0;
    for (double fitness : m_currentFitness) {
        sum += fitness;
    }
    return sum / m_currentFitness.size();
}

std::size_t GeneticAlgorithm::GetPopulationSize() const {
    return m_population.size();
}

const std::vector<double>& GeneticAlgorithm::GetFitnessHistory() const {
    return m_fitnessHistory;
}

void GeneticAlgorithm::SetMutationRate(double rate) {
    if (m_operations) {
        m_operations->SetMutationRate(rate);
    }
}

double GeneticAlgorithm::GetMutationRate() const {
    if (m_operations) {
        return m_operations->GetMutationRate();
    }
    return 1e-3;
}

std::shared_ptr<INeuralNetwork> GeneticAlgorithm::GetIndividualNetwork(std::size_t index) {
    if (index >= m_population.size()) {
        throw std::out_of_range("Individual index out of range");
    }
    return m_population[index];
}

std::vector<std::size_t> GeneticAlgorithm::Selection() {
    std::vector<std::size_t> selected;

    // Indices of individuals with fitness improvement
    for (std::size_t i = 0; i < m_currentFitness.size(); ++i) {
        // Improvement: current > previous
        if (m_currentFitness[i] > m_previousFitness[i]) {
            selected.push_back(i);
        }
    }

    // Elitism: keep best individual even if no improvement
    if (selected.empty()) {
        auto bestIdx = std::distance(
            m_currentFitness.begin(),
            std::max_element(m_currentFitness.begin(), m_currentFitness.end()));
        selected.push_back(bestIdx);
    }

    return selected;
}

void GeneticAlgorithm::Reproduction(const std::vector<std::size_t>& selectedIndices, std::size_t targetSize) {
    if (selectedIndices.empty()) {
        return;
    }

    std::vector<std::shared_ptr<INeuralNetwork>> newPopulation;

    // Copy selected individuals to new population
    for (std::size_t idx : selectedIndices) {
        newPopulation.push_back(m_population[idx]);
    }

    // Generate offspring via crossover + mutation until we reach target size
    std::uniform_int_distribution<std::size_t> parentDist(0, selectedIndices.size() - 1);

    while (newPopulation.size() < targetSize) {
        // Select two random parents from selected
        std::size_t parent1Idx = selectedIndices[parentDist(GetRNG())];
        std::size_t parent2Idx = selectedIndices[parentDist(GetRNG())];

        auto parent1 = m_population[parent1Idx];
        auto parent2 = m_population[parent2Idx];

        // Linearize both parents
        Eigen::VectorXd p1_linear = GeneticOperations::Linearize(
            m_inputCount, m_hiddenCount, m_outputCount,
            parent1->GetInputToHiddenWeights(), parent1->GetHiddenBias(),
            parent1->GetHiddenToOutputWeights(), parent1->GetOutputBias());

        Eigen::VectorXd p2_linear = GeneticOperations::Linearize(
            m_inputCount, m_hiddenCount, m_outputCount,
            parent2->GetInputToHiddenWeights(), parent2->GetHiddenBias(),
            parent2->GetHiddenToOutputWeights(), parent2->GetOutputBias());

        // Crossover
        Eigen::VectorXd offspring_linear = m_operations->Crossover(p1_linear, p2_linear);

        // Mutation
        m_operations->Mutate(offspring_linear);

        // Deserialize back to NN
        Eigen::MatrixXd w_ih(m_hiddenCount, m_inputCount);
        Eigen::VectorXd b_h(m_hiddenCount);
        Eigen::MatrixXd w_ho(m_outputCount, m_hiddenCount);
        Eigen::VectorXd b_o(m_outputCount);

        GeneticOperations::Deseriialize(
            offspring_linear, m_inputCount, m_hiddenCount, m_outputCount,
            w_ih, b_h, w_ho, b_o);

        // Create new NN with offspring weights
        auto offspring_nn = std::make_shared<NeuralNetwork>(
            m_inputCount, m_hiddenCount, m_outputCount);

        offspring_nn->SetInputToHiddenWeights(w_ih);
        offspring_nn->SetHiddenBias(b_h);
        offspring_nn->SetHiddenToOutputWeights(w_ho);
        offspring_nn->SetOutputBias(b_o);

        newPopulation.push_back(offspring_nn);
    }

    // Truncate to target size if needed
    if (newPopulation.size() > targetSize) {
        newPopulation.resize(targetSize);
    }

    m_population = newPopulation;
}

void GeneticAlgorithm::UpdateFitnessStatistics() {
    double bestFitness = GetBestFitness();
    m_fitnessHistory.push_back(bestFitness);
}

void GeneticAlgorithm::ExecuteGenerationStep() {
    // Update fitness statistics before step
    UpdateFitnessStatistics();

    // Store pre-selection size for debugging
    m_lastPreSelectionSize = m_population.size();

    // Selection
    std::vector<std::size_t> selected = Selection();

    m_lastPostSelectionSize = selected.size();

    // Reproduction to restore population size
    Reproduction(selected, m_populationSize);

    // Update fitness for next generation
    m_previousFitness = m_currentFitness;
    m_currentFitness.assign(m_populationSize, 0.0);
}

std::size_t GeneticAlgorithm::GetLastGenerationPreSelectionSize() const {
    return m_lastPreSelectionSize;
}

std::size_t GeneticAlgorithm::GetLastGenerationPostSelectionSize() const {
    return m_lastPostSelectionSize;
}

}  // namespace ga
}  // namespace core
}  // namespace autom


