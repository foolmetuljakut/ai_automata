#include <catch2/catch.hpp>
#include <cmath>
#include "core/anns/NeuralNetwork.h"

TEST_CASE("NeuralNetwork: Forward Pass", "[anns][forward]") {
    // Input: 2, Hidden: 3, Output: 2
    autom::core::anns::NeuralNetwork nn(2, 3, 2);

    Eigen::VectorXd input(2);
    input << 1.0, 2.0;

    SECTION("Forward pass returns output vector") {
        Eigen::VectorXd output = nn.Forward(input);
        REQUIRE(output.size() == 2);
    }

    SECTION("Forward output is clamped to [-1, 1]") {
        // Set weights to produce large outputs
        Eigen::MatrixXd w_ih = Eigen::MatrixXd::Ones(3, 2) * 10.0;
        Eigen::MatrixXd w_ho = Eigen::MatrixXd::Ones(2, 3) * 10.0;
        nn.SetInputToHiddenWeights(w_ih);
        nn.SetHiddenToOutputWeights(w_ho);
        nn.SetHiddenBias(Eigen::VectorXd::Zero(3));
        nn.SetOutputBias(Eigen::VectorXd::Zero(2));

        Eigen::VectorXd output = nn.Forward(input);

        // All values should be in [-1, 1]
        for (int i = 0; i < output.size(); ++i) {
            REQUIRE(output(i) >= -1.0);
            REQUIRE(output(i) <= 1.0);
        }
    }

    SECTION("Forward pass caches activation for backward") {
        nn.Forward(input);
        Eigen::VectorXd cached = nn.GetOutput();
        REQUIRE(cached.size() == 2);
    }
}

TEST_CASE("NeuralNetwork: Backward Pass (Backpropagation)", "[anns][backward]") {
    autom::core::anns::NeuralNetwork nn(2, 3, 2);

    // Set specific weights for predictable behavior
    Eigen::MatrixXd w_ih(3, 2);
    w_ih << 0.5, 0.2,
            0.3, 0.4,
            0.1, 0.6;
    nn.SetInputToHiddenWeights(w_ih);
    nn.SetHiddenBias(Eigen::VectorXd::Zero(3));

    Eigen::MatrixXd w_ho(2, 3);
    w_ho << 0.2, 0.3, 0.1,
            0.4, 0.5, 0.2;
    nn.SetHiddenToOutputWeights(w_ho);
    nn.SetOutputBias(Eigen::VectorXd::Zero(2));

    Eigen::VectorXd input(2);
    input << 0.5, 0.5;

    Eigen::VectorXd target(2);
    target << 1.0, 0.0;

    SECTION("Backward pass updates weights") {
        Eigen::MatrixXd w_ih_before = nn.GetInputToHiddenWeights();

        nn.Forward(input);
        nn.Backward(target, 0.1);

        Eigen::MatrixXd w_ih_after = nn.GetInputToHiddenWeights();

        // Weights should have changed
        bool weightsChanged = !w_ih_before.isApprox(w_ih_after);
        REQUIRE(weightsChanged);
    }

    SECTION("Backward pass updates bias") {
        Eigen::VectorXd b_out_before = nn.GetOutputBias();

        nn.Forward(input);
        nn.Backward(target, 0.1);

        Eigen::VectorXd b_out_after = nn.GetOutputBias();

        // Bias should have changed
        bool biasChanged = !b_out_before.isApprox(b_out_after);
        REQUIRE(biasChanged);
    }

    SECTION("Higher learning rate -> bigger weight changes") {
        nn.Forward(input);
        Eigen::MatrixXd w_before = nn.GetInputToHiddenWeights();
        nn.Backward(target, 0.1);
        Eigen::MatrixXd w_after_lr01 = nn.GetInputToHiddenWeights();

        // Reset weights
        nn.SetInputToHiddenWeights(w_before);
        nn.SetHiddenBias(Eigen::VectorXd::Zero(3));

        nn.Forward(input);
        nn.Backward(target, 0.5);
        Eigen::MatrixXd w_after_lr05 = nn.GetInputToHiddenWeights();

        // Difference with higher LR should be larger
        double diff_01 = (w_after_lr01 - w_before).norm();
        double diff_05 = (w_after_lr05 - w_before).norm();

        REQUIRE(diff_05 > diff_01);
    }
}

TEST_CASE("NeuralNetwork: Forward then Backward integration", "[anns][integration]") {
    autom::core::anns::NeuralNetwork nn(2, 3, 2);

    // Random initialization mit kleinen Werten
    Eigen::MatrixXd w_ih = Eigen::MatrixXd::Random(3, 2) * 0.1;
    Eigen::MatrixXd w_ho = Eigen::MatrixXd::Random(2, 3) * 0.1;
    nn.SetInputToHiddenWeights(w_ih);
    nn.SetHiddenToOutputWeights(w_ho);
    nn.SetHiddenBias(Eigen::VectorXd::Zero(3));
    nn.SetOutputBias(Eigen::VectorXd::Zero(2));

    Eigen::VectorXd input(2);
    input << 0.5, 0.5;

    Eigen::VectorXd target(2);
    target << 1.0, 0.0;

    SECTION("Multiple forward-backward iterations reduce error") {
        // Forward pass and compute initial error
        Eigen::VectorXd output1 = nn.Forward(input);
        double error1 = (output1 - target).squaredNorm();

        // Multiple backward passes
        for (int i = 0; i < 5; ++i) {
            nn.Forward(input);
            nn.Backward(target, 0.1);
        }

        Eigen::VectorXd output_final = nn.Forward(input);
        double error_final = (output_final - target).squaredNorm();

        // Error should decrease with learning
        REQUIRE(error_final < error1);
    }
}

TEST_CASE("NeuralNetwork: Output range", "[anns][output]") {
    autom::core::anns::NeuralNetwork nn(5, 8, 2);

    Eigen::VectorXd input = Eigen::VectorXd::Random(5);

    Eigen::VectorXd output = nn.Forward(input);

    SECTION("Output values in [-1, 1]") {
        for (int i = 0; i < output.size(); ++i) {
            REQUIRE(output(i) >= -1.0);
            REQUIRE(output(i) <= 1.0);
        }
    }
}

