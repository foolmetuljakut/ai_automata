#include <catch2/catch.hpp>
#include "math/NeuralMath.h"

using namespace autom::math;

Eigen::MatrixXd CreateTestWeights() {
    Eigen::MatrixXd W(2, 2);
    W <<  1.0, 2.0,
         -1.0, 0.5;
    return W;
}

Eigen::VectorXd CreateTestInputs() {
    Eigen::VectorXd x(2);
    x << 1.0, 2.0;
    return x;
}

Eigen::VectorXd CreateTestBias() {
    Eigen::VectorXd b(2);
    b << 0.5, -0.5;
    return b;
}

/**
 * @brief Unit Tests für die NeuralMath Klasse
 *
 * Verifiziert das SRP-Konzept anhand einfacher Matrix/Vektor-Transformation.
 * Struktur nach dem Arrange-Act-Assert Pattern.
 */
TEST_CASE("NeuralMath: ComputeLayerOutput rechnet Vektoren fehlerfrei", "[math]") {

    // Kapselung von Komplexitaet - Arrange Phase
    auto W = CreateTestWeights();
    auto x = CreateTestInputs();
    auto b = CreateTestBias();

    SECTION("Lineare Vorwaerts-Transformation (W*x + b)") {
        // Act
        Eigen::VectorXd y = NeuralMath::ComputeLayerOutput(W, x, b);

        // Assert (Fehler klar behandeln / Sprechende Namen der Tests helfen dabei)
        // Erwartet:
        // y_0 = (1.0*1.0 + 2.0*2.0) +  0.5 = 5.5
        // y_1 = (-1.0*1.0 + 0.5*2.0) - 0.5 = -0.5
        REQUIRE(y(0) == Catch::Detail::Approx(5.5).margin(1e-4));
        REQUIRE(y(1) == Catch::Detail::Approx(-0.5).margin(1e-4));
    }

    SECTION("Transformation reagiert korrekt auf Null-Eingaben") {
        // Arrange
        Eigen::VectorXd zeroInputs = Eigen::VectorXd::Zero(2);

        // Act
        Eigen::VectorXd y_zero = NeuralMath::ComputeLayerOutput(W, zeroInputs, b);

        // Assert: Bei Input 0 bleibt nur der Bias übrig
        REQUIRE(y_zero(0) == Catch::Detail::Approx(0.5).margin(1e-4));
        REQUIRE(y_zero(1) == Catch::Detail::Approx(-0.5).margin(1e-4));
    }
}
