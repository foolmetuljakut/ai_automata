#include <catch2/catch.hpp>
#include "core/anns/NeuralNetwork.h"
#include <Eigen/Dense>

TEST_CASE("NeuralNetwork: Initialization", "[anns]") {
    autom::core::anns::NeuralNetwork nn(4, 8, 2);

    SECTION("Dimensions are correct") {
        REQUIRE(nn.GetInputCount() == 4);
        REQUIRE(nn.GetHiddenCount() == 8);
        REQUIRE(nn.GetOutputCount() == 2);
    }

    SECTION("Weight matrices have correct dimensions") {
        REQUIRE(nn.GetInputToHiddenWeights().rows() == 8);
        REQUIRE(nn.GetInputToHiddenWeights().cols() == 4);

        REQUIRE(nn.GetHiddenToOutputWeights().rows() == 2);
        REQUIRE(nn.GetHiddenToOutputWeights().cols() == 8);
    }

    SECTION("Bias vectors are initialized to zero") {
        REQUIRE(nn.GetHiddenBias().sum() == 0.0);
        REQUIRE(nn.GetOutputBias().sum() == 0.0);
    }
}

TEST_CASE("NeuralNetwork: Weight and Bias Updates", "[anns]") {
    autom::core::anns::NeuralNetwork nn(2, 3, 1);

    SECTION("SetInputToHiddenWeights updates weights") {
        Eigen::MatrixXd newWeights = Eigen::MatrixXd::Random(3, 2);
        nn.SetInputToHiddenWeights(newWeights);
        REQUIRE(nn.GetInputToHiddenWeights().isApprox(newWeights));
    }

    SECTION("SetHiddenBias updates bias") {
        Eigen::VectorXd newBias = Eigen::VectorXd::Random(3);
        nn.SetHiddenBias(newBias);
        REQUIRE(nn.GetHiddenBias().isApprox(newBias));
    }

    SECTION("SetHiddenToOutputWeights updates weights") {
        Eigen::MatrixXd newWeights = Eigen::MatrixXd::Random(1, 3);
        nn.SetHiddenToOutputWeights(newWeights);
        REQUIRE(nn.GetHiddenToOutputWeights().isApprox(newWeights));
    }

    SECTION("SetOutputBias updates bias") {
        Eigen::VectorXd newBias = Eigen::VectorXd::Random(1);
        nn.SetOutputBias(newBias);
        REQUIRE(nn.GetOutputBias().isApprox(newBias));
    }
}


