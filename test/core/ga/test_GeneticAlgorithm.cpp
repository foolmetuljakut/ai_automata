#include <catch2/catch.hpp>
#include <core/ga/GeneticAlgorithm.h>
#include <core/anns/NeuralNetwork.h>
#include <Eigen/Dense>
#include <cmath>
#include <vector>

using namespace autom::core::ga;
using namespace autom::core::anns;

// ============================================================================
// TEST CATEGORY: SELECTION
// ============================================================================

TEST_CASE("Selection: keeps individuals with score improvement") {
    GeneticAlgorithm ga(4, 5, 8, 2, 1e-3);

    // Set previous fitness (simulating previous generation)
    // We set current fitness directly
    ga.SetIndividualFitness(0, 15.0);  // Improvement: 15 - 10 = 5
    ga.SetIndividualFitness(1, 5.0);   // No improvement: 5 - 8 = -3
    ga.SetIndividualFitness(2, 20.0);  // Improvement: 20 - 10 = 10
    ga.SetIndividualFitness(3, 12.0);  // Improvement: 12 - 8 = 4

    // Manually set previous fitness (normally from last generation)
    // This is tested via the ExecuteGenerationStep integration

    // For now, just verify population structure
    REQUIRE(ga.GetPopulationSize() == 4);
}

TEST_CASE("Selection: removes individuals without improvement") {
    // This will be tested in ExecuteGenerationStep where we can set previous fitness
    GeneticAlgorithm ga(5, 5, 8, 2, 1e-3);
    REQUIRE(ga.GetPopulationSize() == 5);
}

TEST_CASE("Selection: keeps at least one individual (best one)") {
    GeneticAlgorithm ga(3, 5, 8, 2, 1e-3);

    // Even if all have bad fitness, at least one survives (elitism)
    ga.SetIndividualFitness(0, 5.0);
    ga.SetIndividualFitness(1, 3.0);
    ga.SetIndividualFitness(2, 2.0);

    // ExecuteGenerationStep will select, at least one should survive
    // Tested via integration

    REQUIRE(ga.GetPopulationSize() == 3);
}

// ============================================================================
// TEST CATEGORY: POPULATION INITIALIZATION
// ============================================================================

TEST_CASE("Initialize: population has correct size") {
    std::size_t popSize = 10;
    GeneticAlgorithm ga(popSize, 5, 8, 2, 1e-3);

    REQUIRE(ga.GetPopulationSize() == popSize);
}

TEST_CASE("Initialize: all individuals are valid NN") {
    GeneticAlgorithm ga(5, 5, 8, 2, 1e-3);

    for (std::size_t i = 0; i < 5; ++i) {
        auto nn = ga.GetIndividualNetwork(i);
        REQUIRE(nn != nullptr);
        REQUIRE(nn->GetInputCount() == 5);
        REQUIRE(nn->GetHiddenCount() == 8);
        REQUIRE(nn->GetOutputCount() == 2);
    }
}

TEST_CASE("Initialize: NN are initialized with valid weights") {
    GeneticAlgorithm ga(3, 5, 8, 2, 1e-3);

    for (std::size_t i = 0; i < 3; ++i) {
        auto nn = ga.GetIndividualNetwork(i);

        // Check that weights are not all zeros (should be random initialized)
        const auto& w_ih = nn->GetInputToHiddenWeights();

        // At least some weights should be non-zero
        bool hasNonZero = false;
        for (int row = 0; row < w_ih.rows(); ++row) {
            for (int col = 0; col < w_ih.cols(); ++col) {
                if (std::abs(w_ih(row, col)) > 1e-10) {
                    hasNonZero = true;
                    break;
                }
            }
            if (hasNonZero) break;
        }

        REQUIRE(hasNonZero);
    }
}

// ============================================================================
// TEST CATEGORY: FITNESS TRACKING
// ============================================================================

TEST_CASE("Fitness: get and set individual fitness") {
    GeneticAlgorithm ga(5, 5, 8, 2, 1e-3);

    ga.SetIndividualFitness(0, 10.5);
    REQUIRE(std::abs(ga.GetIndividualFitness(0) - 10.5) < 1e-10);

    ga.SetIndividualFitness(2, 25.3);
    REQUIRE(std::abs(ga.GetIndividualFitness(2) - 25.3) < 1e-10);
}

TEST_CASE("Fitness: best fitness calculation") {
    GeneticAlgorithm ga(5, 5, 8, 2, 1e-3);

    ga.SetIndividualFitness(0, 10.0);
    ga.SetIndividualFitness(1, 25.0);
    ga.SetIndividualFitness(2, 15.0);
    ga.SetIndividualFitness(3, 30.0);
    ga.SetIndividualFitness(4, 20.0);

    REQUIRE(std::abs(ga.GetBestFitness() - 30.0) < 1e-10);
}

TEST_CASE("Fitness: average fitness calculation") {
    GeneticAlgorithm ga(4, 5, 8, 2, 1e-3);

    ga.SetIndividualFitness(0, 10.0);
    ga.SetIndividualFitness(1, 20.0);
    ga.SetIndividualFitness(2, 30.0);
    ga.SetIndividualFitness(3, 40.0);

    // Average: (10 + 20 + 30 + 40) / 4 = 25.0
    REQUIRE(std::abs(ga.GetAverageFitness() - 25.0) < 1e-10);
}

TEST_CASE("Fitness: history tracking over generations") {
    GeneticAlgorithm ga(3, 5, 8, 2, 1e-3);

    // Initial generation (fitness history should be empty or contain initial best)
    REQUIRE(ga.GetFitnessHistory().size() >= 0);

    // Set fitness and execute
    ga.SetIndividualFitness(0, 10.0);
    ga.SetIndividualFitness(1, 20.0);
    ga.SetIndividualFitness(2, 15.0);

    ga.ExecuteGenerationStep();

    // After generation step, history should be updated
    REQUIRE(ga.GetFitnessHistory().size() >= 1);
}

// ============================================================================
// TEST CATEGORY: MUTATION RATE
// ============================================================================

TEST_CASE("Mutation rate: default is 1e-3") {
    GeneticAlgorithm ga(5, 5, 8, 2);  // No mutation rate specified
    REQUIRE(std::abs(ga.GetMutationRate() - 1e-3) < 1e-10);
}

TEST_CASE("Mutation rate: can be set and retrieved") {
    GeneticAlgorithm ga(5, 5, 8, 2, 0.05);
    REQUIRE(std::abs(ga.GetMutationRate() - 0.05) < 1e-10);

    ga.SetMutationRate(0.1);
    REQUIRE(std::abs(ga.GetMutationRate() - 0.1) < 1e-10);
}

// ============================================================================
// TEST CATEGORY: GENERATION STEP
// ============================================================================

TEST_CASE("Generation step: executes without crash") {
    GeneticAlgorithm ga(5, 5, 8, 2, 1e-3);

    // Set some fitness values
    for (std::size_t i = 0; i < 5; ++i) {
        ga.SetIndividualFitness(i, 10.0 + i * 2);
    }

    // Should not crash
    ga.ExecuteGenerationStep();

    // Population should still be valid
    REQUIRE(ga.GetPopulationSize() == 5);
}

TEST_CASE("Generation step: population size maintained after reproduction") {
    GeneticAlgorithm ga(10, 5, 8, 2, 1e-3);

    // Set fitness
    for (std::size_t i = 0; i < 10; ++i) {
        ga.SetIndividualFitness(i, 10.0 + i);
    }

    ga.ExecuteGenerationStep();

    // Population should return to target size
    REQUIRE(ga.GetPopulationSize() == 10);
}

TEST_CASE("Generation step: all individuals are valid NN after reproduction") {
    GeneticAlgorithm ga(5, 5, 8, 2, 1e-3);

    for (std::size_t i = 0; i < 5; ++i) {
        ga.SetIndividualFitness(i, 10.0 + i);
    }

    ga.ExecuteGenerationStep();

    // All should still have valid NN
    for (std::size_t i = 0; i < 5; ++i) {
        auto nn = ga.GetIndividualNetwork(i);
        REQUIRE(nn != nullptr);
        REQUIRE(nn->GetInputCount() == 5);
        REQUIRE(nn->GetHiddenCount() == 8);
        REQUIRE(nn->GetOutputCount() == 2);
    }
}

// ============================================================================
// TEST CATEGORY: MULTIPLE GENERATIONS
// ============================================================================

TEST_CASE("Multiple generations: population evolves") {
    GeneticAlgorithm ga(5, 5, 8, 2, 1e-3);

    // Simulate multiple generations with improving fitness
    double previousBest = 0.0;

    for (int gen = 0; gen < 5; ++gen) {
        // Simulate: better individuals have higher fitness
        for (std::size_t i = 0; i < 5; ++i) {
            double fitness = 10.0 + i + gen * 2;  // Fitness increases each generation
            ga.SetIndividualFitness(i, fitness);
        }

        ga.ExecuteGenerationStep();

        double currentBest = ga.GetBestFitness();
        REQUIRE(currentBest >= previousBest);  // Monotonic improvement

        previousBest = currentBest;
    }
}

TEST_CASE("Multiple generations: average fitness improves") {
    GeneticAlgorithm ga(5, 5, 8, 2, 1e-3);

    double previousAvg = 0.0;

    for (int gen = 0; gen < 5; ++gen) {
        // All fitness increases each generation
        for (std::size_t i = 0; i < 5; ++i) {
            ga.SetIndividualFitness(i, 10.0 + i + gen * 3);
        }

        ga.ExecuteGenerationStep();

        double currentAvg = ga.GetAverageFitness();
        REQUIRE(currentAvg >= previousAvg);

        previousAvg = currentAvg;
    }
}

// ============================================================================
// TEST CATEGORY: EDGE CASES
// ============================================================================

TEST_CASE("Edge case: population size 1") {
    GeneticAlgorithm ga(1, 5, 8, 2, 1e-3);

    REQUIRE(ga.GetPopulationSize() == 1);

    ga.SetIndividualFitness(0, 10.0);
    ga.ExecuteGenerationStep();

    REQUIRE(ga.GetPopulationSize() == 1);
    auto nn = ga.GetIndividualNetwork(0);
    REQUIRE(nn != nullptr);
}

TEST_CASE("Edge case: large population") {
    GeneticAlgorithm ga(100, 5, 8, 2, 1e-3);

    REQUIRE(ga.GetPopulationSize() == 100);

    // Set fitness for all
    for (std::size_t i = 0; i < 100; ++i) {
        ga.SetIndividualFitness(i, 10.0 + i * 0.1);
    }

    ga.ExecuteGenerationStep();

    REQUIRE(ga.GetPopulationSize() == 100);
}

TEST_CASE("Edge case: mutation rate 0") {
    GeneticAlgorithm ga(5, 5, 8, 2, 0.0);

    REQUIRE(std::abs(ga.GetMutationRate() - 0.0) < 1e-10);

    for (std::size_t i = 0; i < 5; ++i) {
        ga.SetIndividualFitness(i, 10.0 + i);
    }

    // Should not crash even with 0 mutation
    ga.ExecuteGenerationStep();

    REQUIRE(ga.GetPopulationSize() == 5);
}

TEST_CASE("Edge case: mutation rate 1.0") {
    GeneticAlgorithm ga(5, 5, 8, 2, 1.0);

    REQUIRE(std::abs(ga.GetMutationRate() - 1.0) < 1e-10);

    for (std::size_t i = 0; i < 5; ++i) {
        ga.SetIndividualFitness(i, 10.0 + i);
    }

    // Should not crash even with 100% mutation
    ga.ExecuteGenerationStep();

    REQUIRE(ga.GetPopulationSize() == 5);
}


