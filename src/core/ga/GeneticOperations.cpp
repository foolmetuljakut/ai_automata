#include <core/ga/GeneticOperations.h>
#include <random>
#include <cmath>

namespace autom {
namespace core {
namespace ga {

// Static random generator (thread-safe via static initialization)
static std::mt19937& GetRNG() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

GeneticOperations::GeneticOperations(double mutationRate)
    : m_mutationRate(mutationRate) {
}

// ============================================================================
// LINEARIZATION
// ============================================================================

Eigen::VectorXd GeneticOperations::Linearize(
    std::size_t inputCount,
    std::size_t hiddenCount,
    std::size_t outputCount,
    const Eigen::MatrixXd& w_ih,
    const Eigen::VectorXd& b_h,
    const Eigen::MatrixXd& w_ho,
    const Eigen::VectorXd& b_o) {

    std::size_t size = CalculateVectorSize(inputCount, hiddenCount, outputCount);
    Eigen::VectorXd linear(size);

    std::size_t idx = 0;

    // 1. InputToHidden weights (row-major: row by row)
    // w_ih is (hidden × input), so hiddenCount rows, inputCount cols
    for (std::size_t row = 0; row < hiddenCount; ++row) {
        for (std::size_t col = 0; col < inputCount; ++col) {
            linear(idx++) = w_ih(row, col);
        }
    }

    // 2. Hidden bias
    for (std::size_t i = 0; i < hiddenCount; ++i) {
        linear(idx++) = b_h(i);
    }

    // 3. HiddenToOutput weights (row-major)
    // w_ho is (output × hidden), so outputCount rows, hiddenCount cols
    for (std::size_t row = 0; row < outputCount; ++row) {
        for (std::size_t col = 0; col < hiddenCount; ++col) {
            linear(idx++) = w_ho(row, col);
        }
    }

    // 4. Output bias
    for (std::size_t i = 0; i < outputCount; ++i) {
        linear(idx++) = b_o(i);
    }

    return linear;
}

void GeneticOperations::Deseriialize(
    const Eigen::VectorXd& vector,
    std::size_t inputCount,
    std::size_t hiddenCount,
    std::size_t outputCount,
    Eigen::MatrixXd& w_ih,
    Eigen::VectorXd& b_h,
    Eigen::MatrixXd& w_ho,
    Eigen::VectorXd& b_o) {

    w_ih.resize(hiddenCount, inputCount);
    b_h.resize(hiddenCount);
    w_ho.resize(outputCount, hiddenCount);
    b_o.resize(outputCount);

    std::size_t idx = 0;

    // 1. InputToHidden weights (row-major)
    for (std::size_t row = 0; row < hiddenCount; ++row) {
        for (std::size_t col = 0; col < inputCount; ++col) {
            w_ih(row, col) = vector(idx++);
        }
    }

    // 2. Hidden bias
    for (std::size_t i = 0; i < hiddenCount; ++i) {
        b_h(i) = vector(idx++);
    }

    // 3. HiddenToOutput weights (row-major)
    for (std::size_t row = 0; row < outputCount; ++row) {
        for (std::size_t col = 0; col < hiddenCount; ++col) {
            w_ho(row, col) = vector(idx++);
        }
    }

    // 4. Output bias
    for (std::size_t i = 0; i < outputCount; ++i) {
        b_o(i) = vector(idx++);
    }
}

std::size_t GeneticOperations::CalculateVectorSize(
    std::size_t inputCount,
    std::size_t hiddenCount,
    std::size_t outputCount) {

    return (hiddenCount * inputCount)      // w_ih
         + hiddenCount                      // b_h
         + (outputCount * hiddenCount)      // w_ho
         + outputCount;                     // b_o
}

// ============================================================================
// CROSSOVER
// ============================================================================

Eigen::VectorXd GeneticOperations::Crossover(
    const Eigen::VectorXd& parent1,
    const Eigen::VectorXd& parent2) const {

    if (parent1.size() != parent2.size()) {
        throw std::invalid_argument("Parents must have same size");
    }

    std::size_t size = parent1.size();

    if (size == 0) {
        return parent1;
    }

    // Random cut point
    std::uniform_int_distribution<std::size_t> dist(0, size);
    std::size_t cutPoint = dist(GetRNG());

    Eigen::VectorXd offspring(size);

    // Copy from parent1 [0..cutPoint)
    for (std::size_t i = 0; i < cutPoint; ++i) {
        offspring(i) = parent1(i);
    }

    // Copy from parent2 [cutPoint..end)
    for (std::size_t i = cutPoint; i < size; ++i) {
        offspring(i) = parent2(i);
    }

    return offspring;
}

Eigen::VectorXd GeneticOperations::CrossoverTwoPoint(
    const Eigen::VectorXd& parent1,
    const Eigen::VectorXd& parent2) const {

    if (parent1.size() != parent2.size()) {
        throw std::invalid_argument("Parents must have same size");
    }

    std::size_t size = parent1.size();

    if (size <= 1) {
        return parent1;
    }

    // Random two cut points
    std::uniform_int_distribution<std::size_t> dist(0, size - 1);
    std::size_t cut1 = dist(GetRNG());
    std::size_t cut2 = dist(GetRNG());

    // Ensure cut1 < cut2
    if (cut1 > cut2) {
        std::swap(cut1, cut2);
    }

    Eigen::VectorXd offspring(size);

    // Copy from parent1 [0..cut1)
    for (std::size_t i = 0; i < cut1; ++i) {
        offspring(i) = parent1(i);
    }

    // Copy from parent2 [cut1..cut2)
    for (std::size_t i = cut1; i < cut2; ++i) {
        offspring(i) = parent2(i);
    }

    // Copy from parent1 [cut2..end)
    for (std::size_t i = cut2; i < size; ++i) {
        offspring(i) = parent1(i);
    }

    return offspring;
}

// ============================================================================
// MUTATION
// ============================================================================

double GeneticOperations::GaussianRandom() {
    // Box-Muller Transform
    static std::normal_distribution<double> dist(0.0, 1.0);
    return dist(GetRNG());
}

void GeneticOperations::Mutate(Eigen::VectorXd& vector, double sigma) const {
    std::uniform_real_distribution<double> probDist(0.0, 1.0);

    for (int i = 0; i < vector.size(); ++i) {
        if (probDist(GetRNG()) < m_mutationRate) {
            // Add Gaussian noise (unbounded)
            double noise = GaussianRandom() * sigma;
            vector(i) += noise;
        }
    }
}

// ============================================================================
// MUTATION RATE
// ============================================================================

void GeneticOperations::SetMutationRate(double rate) {
    m_mutationRate = rate;
}

double GeneticOperations::GetMutationRate() const {
    return m_mutationRate;
}

}  // namespace ga
}  // namespace core
}  // namespace autom

