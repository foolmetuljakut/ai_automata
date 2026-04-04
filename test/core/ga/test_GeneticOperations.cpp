#include <catch2/catch.hpp>
#include <core/ga/GeneticOperations.h>
#include <Eigen/Dense>
#include <cmath>

using namespace autom::core::ga;

// ============================================================================
// TEST CATEGORY: SERIALIZATION / LINEARIZATION
// ============================================================================

TEST_CASE("Linearize: NN with 5 inputs, 8 hidden, 2 outputs") {
    std::size_t inputCount = 5;
    std::size_t hiddenCount = 8;
    std::size_t outputCount = 2;

    // Create simple NN matrices with known values
    Eigen::MatrixXd w_ih = Eigen::MatrixXd::Ones(hiddenCount, inputCount);
    Eigen::VectorXd b_h = Eigen::VectorXd::Constant(hiddenCount, 2.0);
    Eigen::MatrixXd w_ho = Eigen::MatrixXd::Constant(outputCount, hiddenCount, 3.0);
    Eigen::VectorXd b_o = Eigen::VectorXd::Constant(outputCount, 4.0);

    // Linearize
    Eigen::VectorXd linear = GeneticOperations::Linearize(
        inputCount, hiddenCount, outputCount,
        w_ih, b_h, w_ho, b_o);

    // Expected size: 5*8 + 8 + 8*2 + 2 = 40 + 8 + 16 + 2 = 66
    REQUIRE(linear.size() == 66);

    // Check structure: first 40 elements should be w_ih (row-major)
    for (int i = 0; i < 40; ++i) {
        REQUIRE(linear(i) == 1.0);
    }

    // Check b_h: next 8 elements
    for (int i = 40; i < 48; ++i) {
        REQUIRE(linear(i) == 2.0);
    }

    // Check w_ho: next 16 elements
    for (int i = 48; i < 64; ++i) {
        REQUIRE(linear(i) == 3.0);
    }

    // Check b_o: last 2 elements
    for (int i = 64; i < 66; ++i) {
        REQUIRE(linear(i) == 4.0);
    }
}

TEST_CASE("Deserialize: vector back to NN components") {
    std::size_t inputCount = 5;
    std::size_t hiddenCount = 8;
    std::size_t outputCount = 2;

    // Create original matrices
    Eigen::MatrixXd w_ih_orig = Eigen::MatrixXd::Ones(hiddenCount, inputCount);
    Eigen::VectorXd b_h_orig = Eigen::VectorXd::Constant(hiddenCount, 2.0);
    Eigen::MatrixXd w_ho_orig = Eigen::MatrixXd::Constant(outputCount, hiddenCount, 3.0);
    Eigen::VectorXd b_o_orig = Eigen::VectorXd::Constant(outputCount, 4.0);

    // Linearize
    Eigen::VectorXd linear = GeneticOperations::Linearize(
        inputCount, hiddenCount, outputCount,
        w_ih_orig, b_h_orig, w_ho_orig, b_o_orig);

    // Deserialize
    Eigen::MatrixXd w_ih_restored(hiddenCount, inputCount);
    Eigen::VectorXd b_h_restored(hiddenCount);
    Eigen::MatrixXd w_ho_restored(outputCount, hiddenCount);
    Eigen::VectorXd b_o_restored(outputCount);

    GeneticOperations::Deseriialize(
        linear, inputCount, hiddenCount, outputCount,
        w_ih_restored, b_h_restored, w_ho_restored, b_o_restored);

    // Check all components match
    REQUIRE(w_ih_restored.isApprox(w_ih_orig));
    REQUIRE(b_h_restored.isApprox(b_h_orig));
    REQUIRE(w_ho_restored.isApprox(w_ho_orig));
    REQUIRE(b_o_restored.isApprox(b_o_orig));
}

TEST_CASE("Linearization is invertible (NN → Vector → NN)") {
    std::size_t inputCount = 5;
    std::size_t hiddenCount = 8;
    std::size_t outputCount = 2;

    // Create random NN
    Eigen::MatrixXd w_ih = Eigen::MatrixXd::Random(hiddenCount, inputCount);
    Eigen::VectorXd b_h = Eigen::VectorXd::Random(hiddenCount);
    Eigen::MatrixXd w_ho = Eigen::MatrixXd::Random(outputCount, hiddenCount);
    Eigen::VectorXd b_o = Eigen::VectorXd::Random(outputCount);

    // Linearize → Deserialize → Compare
    Eigen::VectorXd linear = GeneticOperations::Linearize(
        inputCount, hiddenCount, outputCount,
        w_ih, b_h, w_ho, b_o);

    Eigen::MatrixXd w_ih_restored(hiddenCount, inputCount);
    Eigen::VectorXd b_h_restored(hiddenCount);
    Eigen::MatrixXd w_ho_restored(outputCount, hiddenCount);
    Eigen::VectorXd b_o_restored(outputCount);

    GeneticOperations::Deseriialize(
        linear, inputCount, hiddenCount, outputCount,
        w_ih_restored, b_h_restored, w_ho_restored, b_o_restored);

    REQUIRE(w_ih_restored.isApprox(w_ih));
    REQUIRE(b_h_restored.isApprox(b_h));
    REQUIRE(w_ho_restored.isApprox(w_ho));
    REQUIRE(b_o_restored.isApprox(b_o));
}

TEST_CASE("Calculate vector size for different architectures") {
    REQUIRE(GeneticOperations::CalculateVectorSize(5, 8, 2) == 66);     // 5*8 + 8 + 8*2 + 2 = 40 + 8 + 16 + 2
    REQUIRE(GeneticOperations::CalculateVectorSize(10, 16, 4) == 244);  // 10*16 + 16 + 16*4 + 4 = 160 + 16 + 64 + 4
    REQUIRE(GeneticOperations::CalculateVectorSize(1, 1, 1) == 4);      // 1*1 + 1 + 1*1 + 1
}

// ============================================================================
// TEST CATEGORY: CROSSOVER
// ============================================================================

TEST_CASE("Crossover: combines two parent vectors") {
    GeneticOperations ops;

    // Create two distinct parent vectors
    Eigen::VectorXd parent1 = Eigen::VectorXd::Zero(10);
    Eigen::VectorXd parent2 = Eigen::VectorXd::Ones(10);

    parent1.setZero();   // [0, 0, ..., 0]
    parent2.setOnes();   // [1, 1, ..., 1]

    // Do crossover multiple times (randomized cut point)
    bool foundCombination = false;
    for (int trial = 0; trial < 100; ++trial) {
        Eigen::VectorXd offspring = ops.Crossover(parent1, parent2);

        // Check size
        REQUIRE(offspring.size() == 10);

        // Offspring should have mix of 0s and 1s
        bool hasZero = false;
        bool hasOne = false;
        for (int i = 0; i < 10; ++i) {
            REQUIRE((offspring(i) == 0.0 || offspring(i) == 1.0));
            if (offspring(i) == 0.0) hasZero = true;
            if (offspring(i) == 1.0) hasOne = true;
        }

        if (hasZero && hasOne) {
            foundCombination = true;
            break;
        }
    }

    // At least one crossover should create a mix
    REQUIRE(foundCombination);
}

TEST_CASE("Crossover: preserves vector structure") {
    GeneticOperations ops;

    Eigen::VectorXd parent1 = Eigen::VectorXd::LinSpaced(50, 0.0, 49.0);
    Eigen::VectorXd parent2 = Eigen::VectorXd::LinSpaced(50, 100.0, 149.0);

    Eigen::VectorXd offspring = ops.Crossover(parent1, parent2);

    // Size should match
    REQUIRE(offspring.size() == 50);

    // All values should come from either parent
    for (int i = 0; i < 50; ++i) {
        bool fromParent1 = std::abs(offspring(i) - parent1(i)) < 1e-10;
        bool fromParent2 = std::abs(offspring(i) - parent2(i)) < 1e-10;
        REQUIRE((fromParent1 || fromParent2));
    }
}

TEST_CASE("Crossover: produces continuous segments from parents") {
    // Note: This test verifies that crossover doesn't shuffle individual elements,
    // but takes contiguous segments from parents.
    GeneticOperations ops;

    Eigen::VectorXd parent1 = Eigen::VectorXd::Zero(100);
    Eigen::VectorXd parent2 = Eigen::VectorXd::Ones(100);

    // Do multiple trials
    for (int trial = 0; trial < 50; ++trial) {
        Eigen::VectorXd offspring = ops.Crossover(parent1, parent2);

        // Check for contiguous segments
        // If offspring has multiple switches between parent values, check they're minimal
        int switchCount = 0;
        for (int i = 1; i < 100; ++i) {
            if (offspring(i) != offspring(i - 1)) {
                switchCount++;
            }
        }

        // For 1-point crossover, should have 0 or 1 switches
        REQUIRE(switchCount <= 1);
    }
}

TEST_CASE("TwoPointCrossover: combines segments from both parents") {
    GeneticOperations ops;

    Eigen::VectorXd parent1 = Eigen::VectorXd::Zero(100);
    Eigen::VectorXd parent2 = Eigen::VectorXd::Ones(100);

    Eigen::VectorXd offspring = ops.CrossoverTwoPoint(parent1, parent2);

    REQUIRE(offspring.size() == 100);

    // All values from either parent
    for (int i = 0; i < 100; ++i) {
        REQUIRE((offspring(i) == 0.0 || offspring(i) == 1.0));
    }
}

// ============================================================================
// TEST CATEGORY: MUTATION
// ============================================================================

TEST_CASE("Mutation: changes values with correct probability") {
    GeneticOperations ops(0.1);  // 10% mutation rate

    Eigen::VectorXd vector = Eigen::VectorXd::Zero(1000);
    Eigen::VectorXd original = vector;

    ops.Mutate(vector, 0.1);  // sigma = 0.1

    // Count changed elements
    int changedCount = 0;
    for (int i = 0; i < 1000; ++i) {
        if (std::abs(vector(i) - original(i)) > 1e-10) {
            changedCount++;
        }
    }

    // Expected ~100 changes (10% of 1000), allow 30% tolerance
    REQUIRE(changedCount > 50);    // At least 50 changes
    REQUIRE(changedCount < 150);   // At most 150 changes
}

TEST_CASE("Mutation: with rate 0.0 does not change vector") {
    GeneticOperations ops(0.0);

    Eigen::VectorXd vector = Eigen::VectorXd::Random(100);
    Eigen::VectorXd original = vector;

    ops.Mutate(vector, 0.5);

    REQUIRE(vector.isApprox(original));
}

TEST_CASE("Mutation: with rate 1.0 changes all values") {
    GeneticOperations ops(1.0);

    Eigen::VectorXd vector = Eigen::VectorXd::Zero(1000);
    Eigen::VectorXd original = vector;

    ops.Mutate(vector, 0.1);

    // ALL values should change
    for (int i = 0; i < 1000; ++i) {
        REQUIRE(std::abs(vector(i) - original(i)) > 1e-10);
    }
}

TEST_CASE("Mutation: respects sigma parameter") {
    GeneticOperations ops(1.0);

    Eigen::VectorXd vector = Eigen::VectorXd::Zero(1000);

    // Mutate with small sigma
    ops.Mutate(vector, 0.01);

    // Most values should be close to 0
    int closeToZero = 0;
    for (int i = 0; i < 1000; ++i) {
        if (std::abs(vector(i)) < 0.1) {
            closeToZero++;
        }
    }
    REQUIRE(closeToZero > 800);  // At least 80% should be small

    // Reset and mutate with large sigma
    vector.setZero();
    ops.Mutate(vector, 1.0);

    // More values should be farther from 0
    closeToZero = 0;
    for (int i = 0; i < 1000; ++i) {
        if (std::abs(vector(i)) < 0.1) {
            closeToZero++;
        }
    }
    REQUIRE(closeToZero < 400);  // Less than 40% should be small
}

TEST_CASE("Mutation: is unbounded (no clamping)") {
    GeneticOperations ops(1.0);

    Eigen::VectorXd vector = Eigen::VectorXd::Zero(1000);

    // Mutate with very large sigma
    ops.Mutate(vector, 10.0);

    // Should have some very large values
    bool hasLarge = false;
    for (int i = 0; i < 1000; ++i) {
        if (std::abs(vector(i)) > 5.0) {
            hasLarge = true;
            break;
        }
    }
    REQUIRE(hasLarge);  // Should have some values > 5.0
}

TEST_CASE("Default mutation rate is 1e-3") {
    GeneticOperations ops;  // No argument
    REQUIRE(std::abs(ops.GetMutationRate() - 1e-3) < 1e-10);
}

TEST_CASE("Set and get mutation rate") {
    GeneticOperations ops;

    ops.SetMutationRate(0.05);
    REQUIRE(std::abs(ops.GetMutationRate() - 0.05) < 1e-10);

    ops.SetMutationRate(1e-4);
    REQUIRE(std::abs(ops.GetMutationRate() - 1e-4) < 1e-10);
}

TEST_CASE("Mutation: with low rate (1e-3) changes few values per call") {
    GeneticOperations ops(1e-3);  // Very low mutation rate

    Eigen::VectorXd vector = Eigen::VectorXd::Zero(10000);
    Eigen::VectorXd original = vector;

    ops.Mutate(vector, 0.1);

    // Count changed elements
    int changedCount = 0;
    for (int i = 0; i < 10000; ++i) {
        if (std::abs(vector(i) - original(i)) > 1e-10) {
            changedCount++;
        }
    }

    // Expected ~10 changes (0.1% of 10000), allow 50% tolerance
    REQUIRE(changedCount > 0);
    REQUIRE(changedCount < 30);  // Most calls should change < 30 values
}


