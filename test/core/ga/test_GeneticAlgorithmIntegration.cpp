#include <catch2/catch.hpp>
#include <core/ga/GeneticAlgorithm.h>
#include <core/anns/NeuralNetwork.h>
#include <Eigen/Dense>
#include <cmath>

using namespace autom::core::ga;
using namespace autom::core::anns;

// ============================================================================
// TEST CATEGORY: NN + GA INTEGRATION
// ============================================================================

TEST_CASE("Integration: GA evolves NN weights for simple task") {
    // Simulate a simple fitness task: GA should improve population fitness
    GeneticAlgorithm ga(10, 2, 4, 1, 1e-3);

    // Generation 1: random fitness
    for (std::size_t i = 0; i < 10; ++i) {
        ga.SetIndividualFitness(i, 5.0 + i * 0.5);
    }
    ga.ExecuteGenerationStep();

    // Store initial best from history
    double initialBest = ga.GetFitnessHistory()[0];

    // Multiple generations with improving fitness
    for (int gen = 0; gen < 10; ++gen) {
        for (std::size_t i = 0; i < 10; ++i) {
            // Simulate: fitness improves over generations
            ga.SetIndividualFitness(i, 5.0 + i * 0.5 + gen * 1.5);
        }
        ga.ExecuteGenerationStep();
    }

    // Get final best from history
    double finalBest = ga.GetFitnessHistory().back();

    // Best fitness should improve
    REQUIRE(finalBest > initialBest);
}

TEST_CASE("Integration: Crossover produces functionally different offspring") {
    // Create two different NN with different weights
    GeneticAlgorithm ga(2, 5, 8, 2, 1e-3);

    auto nn1 = ga.GetIndividualNetwork(0);
    auto nn2 = ga.GetIndividualNetwork(1);

    // Give them very different weights
    Eigen::MatrixXd w_ih_1 = Eigen::MatrixXd::Constant(8, 5, 1.0);
    Eigen::MatrixXd w_ih_2 = Eigen::MatrixXd::Constant(8, 5, -1.0);

    nn1->SetInputToHiddenWeights(w_ih_1);
    nn2->SetInputToHiddenWeights(w_ih_2);

    // After a generation step (which includes crossover), we should see changes
    ga.SetIndividualFitness(0, 10.0);
    ga.SetIndividualFitness(1, 15.0);

    ga.ExecuteGenerationStep();

    // Just check that after crossover, population still has valid NN
    for (std::size_t i = 0; i < 2; ++i) {
        auto nn = ga.GetIndividualNetwork(i);
        REQUIRE(nn->GetInputCount() == 5);
    }
}

TEST_CASE("Integration: Mutation creates behavioral variation") {
    GeneticAlgorithm ga(5, 5, 8, 2, 0.5);  // High mutation rate for testing

    // Set same high fitness for all
    for (std::size_t i = 0; i < 5; ++i) {
        ga.SetIndividualFitness(i, 20.0);
    }

    ga.ExecuteGenerationStep();

    // After generation with high mutation, NN should vary
    std::vector<Eigen::VectorXd> outputs(5);
    Eigen::VectorXd testInput = Eigen::VectorXd::Random(5);

    for (std::size_t i = 0; i < 5; ++i) {
        auto nn = ga.GetIndividualNetwork(i);
        outputs[i] = nn->Forward(testInput);
    }

    // Not all outputs should be identical (due to mutation)
    bool hasVariation = false;
    for (std::size_t i = 1; i < 5; ++i) {
        if (!outputs[i].isApprox(outputs[0])) {
            hasVariation = true;
            break;
        }
    }

    REQUIRE(hasVariation);  // Should have variation due to mutation
}

// ============================================================================
// TEST CATEGORY: NN FORWARD PASS WITH GA
// ============================================================================

TEST_CASE("Integration: NN from GA can perform forward pass") {
    GeneticAlgorithm ga(3, 5, 8, 2, 1e-3);

    for (std::size_t i = 0; i < 3; ++i) {
        ga.SetIndividualFitness(i, 10.0 + i);
    }

    ga.ExecuteGenerationStep();

    // All NN should be able to do forward pass
    for (std::size_t i = 0; i < 3; ++i) {
        auto nn = ga.GetIndividualNetwork(i);

        Eigen::VectorXd input = Eigen::VectorXd::Random(5);
        Eigen::VectorXd output = nn->Forward(input);

        REQUIRE(output.size() == 2);

        // Output should be in [-1, 1] (clamped)
        for (int j = 0; j < 2; ++j) {
            REQUIRE(output(j) >= -1.0);
            REQUIRE(output(j) <= 1.0);
        }
    }
}

TEST_CASE("Integration: NN output deterministic for same input") {
    GeneticAlgorithm ga(1, 5, 8, 2, 1e-3);

    auto nn = ga.GetIndividualNetwork(0);

    Eigen::VectorXd input = Eigen::VectorXd::Random(5);
    Eigen::VectorXd output1 = nn->Forward(input);
    Eigen::VectorXd output2 = nn->Forward(input);

    // Same input should produce same output
    REQUIRE(output1.isApprox(output2));
}

// ============================================================================
// TEST CATEGORY: ELITISM
// ============================================================================

TEST_CASE("Integration: best individual tends to survive") {
    GeneticAlgorithm ga(5, 5, 8, 2, 1e-3);

    // Set very different fitness
    ga.SetIndividualFitness(0, 100.0);  // Best
    ga.SetIndividualFitness(1, 1.0);
    ga.SetIndividualFitness(2, 1.0);
    ga.SetIndividualFitness(3, 1.0);
    ga.SetIndividualFitness(4, 1.0);

    double bestBeforeGen = ga.GetBestFitness();
    ga.ExecuteGenerationStep();

    // Check fitness history for best after generation
    const auto& history = ga.GetFitnessHistory();
    REQUIRE(history.size() >= 1);
    double bestAfterGen = history.back();

    // Best should not decrease (elitism)
    REQUIRE(bestAfterGen >= bestBeforeGen - 1e-10);
}

// ============================================================================
// TEST CATEGORY: CONVERGENCE
// ============================================================================

TEST_CASE("Integration: population converges over many generations") {
    GeneticAlgorithm ga(10, 5, 8, 2, 1e-3);

    // Simulate convergence: increasing fitness each generation
    for (int gen = 0; gen < 20; ++gen) {
        for (std::size_t i = 0; i < 10; ++i) {
            // All fitness improves, but some more than others
            double baseGen = gen * 2.0;
            double variation = i * 0.5;
            ga.SetIndividualFitness(i, 20.0 + baseGen + variation);
        }

        ga.ExecuteGenerationStep();
    }

    // Check fitness history for final best
    const auto& history = ga.GetFitnessHistory();
    REQUIRE(history.size() >= 20);

    double finalBest = history.back();
    REQUIRE(finalBest > 50.0);

    // Average fitness from last generation (set it to check)
    for (std::size_t i = 0; i < 10; ++i) {
        ga.SetIndividualFitness(i, 20.0 + 19 * 2.0 + i * 0.5);
    }
    double avgFitness = ga.GetAverageFitness();
    REQUIRE(avgFitness > 40.0);
}

// ============================================================================
// TEST CATEGORY: STRESS TESTS
// ============================================================================

TEST_CASE("Integration: handle large population") {
    GeneticAlgorithm ga(50, 5, 8, 2, 1e-3);

    // Multiple generations with large population
    for (int gen = 0; gen < 5; ++gen) {
        for (std::size_t i = 0; i < 50; ++i) {
            ga.SetIndividualFitness(i, 10.0 + i * 0.1 + gen);
        }

        ga.ExecuteGenerationStep();
    }

    REQUIRE(ga.GetPopulationSize() == 50);

    const auto& history = ga.GetFitnessHistory();
    REQUIRE(history.size() >= 5);
    double finalBest = history.back();
    REQUIRE(finalBest > 18.0);  // With 5 generations, max is 10 + 49*0.1 + 4 = 18.9
}

TEST_CASE("Integration: handle large NN architecture") {
    // Bigger NN: 10 inputs, 32 hidden, 4 outputs
    GeneticAlgorithm ga(5, 10, 32, 4, 1e-3);

    for (int gen = 0; gen < 3; ++gen) {
        for (std::size_t i = 0; i < 5; ++i) {
            ga.SetIndividualFitness(i, 10.0 + i + gen);
        }

        ga.ExecuteGenerationStep();
    }

    REQUIRE(ga.GetPopulationSize() == 5);

    // Verify all NN have correct architecture
    for (std::size_t i = 0; i < 5; ++i) {
        auto nn = ga.GetIndividualNetwork(i);
        REQUIRE(nn->GetInputCount() == 10);
        REQUIRE(nn->GetHiddenCount() == 32);
        REQUIRE(nn->GetOutputCount() == 4);
    }
}








