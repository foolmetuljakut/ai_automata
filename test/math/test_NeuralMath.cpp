#include <catch2/catch.hpp>
#include "math/NeuralMath.h"
#include <Eigen/Dense>
#include <cmath>

TEST_CASE("NeuralMath: ComputeLayerOutput", "[math]") {
    SECTION("Basic computation") {
        Eigen::MatrixXd weights(2, 3);
        weights << 0.5, 0.1, 0.2,
                   -0.3, 0.8, 0.4;

        Eigen::VectorXd inputs(3);
        inputs << 1.0, 2.0, -1.0;

        Eigen::VectorXd bias(2);
        bias << 0.5, -0.1;

        Eigen::VectorXd result = autom::math::NeuralMath::ComputeLayerOutput(weights, inputs, bias);

        // Expected: weights * inputs + bias
        // Row 1: 0.5*1 + 0.1*2 + 0.2*(-1) + 0.5 = 0.5 + 0.2 - 0.2 + 0.5 = 1.0
        // Row 2: -0.3*1 + 0.8*2 + 0.4*(-1) - 0.1 = -0.3 + 1.6 - 0.4 - 0.1 = 0.8
        REQUIRE(result.size() == 2);
        REQUIRE(result(0) == 1.0);
        REQUIRE(result(1) == 0.8);
    }

    SECTION("Zero input produces bias") {
        Eigen::MatrixXd weights = Eigen::MatrixXd::Random(3, 4);
        Eigen::VectorXd inputs = Eigen::VectorXd::Zero(4);
        Eigen::VectorXd bias(3);
        bias << 1.0, 2.0, 3.0;

        Eigen::VectorXd result = autom::math::NeuralMath::ComputeLayerOutput(weights, inputs, bias);
        REQUIRE(result.isApprox(bias));
    }
}

TEST_CASE("NeuralMath: ApplyReLU", "[math]") {
    SECTION("ReLU clamps negative values to zero") {
        Eigen::VectorXd input(4);
        input << -2.0, 0.5, -1.0, 3.0;

        Eigen::VectorXd result = autom::math::NeuralMath::ApplyReLU(input);

        REQUIRE(result.size() == 4);
        REQUIRE(result(0) == 0.0);
        REQUIRE(result(1) == 0.5);
        REQUIRE(result(2) == 0.0);
        REQUIRE(result(3) == 3.0);
    }
}

TEST_CASE("NeuralMath: ApplyTanh", "[math]") {
    SECTION("Tanh applies correctly") {
        Eigen::VectorXd input(3);
        input << 0.0, 1.0, -1.0;

        Eigen::VectorXd result = autom::math::NeuralMath::ApplyTanh(input);

        REQUIRE(result.size() == 3);
        REQUIRE(result(0) == 0.0);  // tanh(0) = 0
        REQUIRE(std::abs(result(1) - std::tanh(1.0)) < 1e-10);
        REQUIRE(std::abs(result(2) - std::tanh(-1.0)) < 1e-10);
    }
}


