#include <catch2/catch.hpp>
#include <core/ga/GeneticAlgorithm.h>
#include <core/anns/NeuralNetwork.h>

/**
 * @brief Integration test für GA-Nutzung wie in MainWindow
 *
 * Simuliert den Generationswechsel:
 * 1. Erstelle GA mit N Zellen
 * 2. Setze Fitness für alle Zellen
 * 3. Führe ExecuteGenerationStep durch
 * 4. Bekomme neue Networks
 */
TEST_CASE("GeneticAlgorithm: Generation Switch Cycle", "[ga][integration]") {
    const std::size_t populationSize = 5;
    const std::size_t inputCount = 5;   // 5 Sensoren
    const std::size_t hiddenCount = 8;
    const std::size_t outputCount = 2;  // Velocity X, Y
    const double mutationRate = 1e-3;

    auto ga = std::make_shared<autom::core::ga::GeneticAlgorithm>(
        populationSize, inputCount, hiddenCount, outputCount, mutationRate);

    SECTION("Initial population is correct size") {
        REQUIRE(ga->GetPopulationSize() == populationSize);
    }

    SECTION("SetIndividualFitness works for all individuals") {
        for (std::size_t i = 0; i < populationSize; ++i) {
            // Simuliere Score einer Zelle (random zwischen 0-10)
            double score = static_cast<double>(rand() % 10);
            REQUIRE_NOTHROW(ga->SetIndividualFitness(i, score));
            REQUIRE(ga->GetIndividualFitness(i) == score);
        }
    }

    SECTION("SetIndividualFitness throws for out of range") {
        REQUIRE_THROWS_AS(ga->SetIndividualFitness(populationSize + 5, 10.0),
                          std::out_of_range);
    }

    SECTION("ExecuteGenerationStep maintains population size") {
        // 1. Setze Fitness für Generation 1
        for (std::size_t i = 0; i < populationSize; ++i) {
            ga->SetIndividualFitness(i, static_cast<double>(i + 1));  // 1, 2, 3, 4, 5
        }

        // 2. Führe Generation-Step durch
        ga->ExecuteGenerationStep();

        // 3. Population sollte noch gleich groß sein
        REQUIRE(ga->GetPopulationSize() == populationSize);

        // 4. Sollte neue Networks bekommen können
        for (std::size_t i = 0; i < populationSize; ++i) {
            auto network = ga->GetIndividualNetwork(i);
            REQUIRE(network != nullptr);
        }
    }

    SECTION("Multiple generation cycles work") {
        for (int generation = 0; generation < 3; ++generation) {
            // Setze Fitness
            for (std::size_t i = 0; i < populationSize; ++i) {
                double score = static_cast<double>(rand() % 20);
                ga->SetIndividualFitness(i, score);
            }

            // Step
            ga->ExecuteGenerationStep();

            // Validate
            REQUIRE(ga->GetPopulationSize() == populationSize);

            // Und alle Networks sollten zugreifbar sein
            for (std::size_t i = 0; i < populationSize; ++i) {
                auto network = ga->GetIndividualNetwork(i);
                REQUIRE(network != nullptr);
            }
        }
    }

    SECTION("Changing population size fails correctly") {
        // Erst setz fitness für current size
        for (std::size_t i = 0; i < populationSize; ++i) {
            ga->SetIndividualFitness(i, 1.0);
        }

        // Versuche fitness für größeren Index zu setzen (vor ExecuteGenerationStep)
        REQUIRE_THROWS_AS(ga->SetIndividualFitness(populationSize + 10, 5.0),
                          std::out_of_range);
    }
}

/**
 * @brief Test für die MainWindow-Scenario: GA bei Cell Count Änderung
 */
TEST_CASE("GeneticAlgorithm: Cell Count Change Scenario", "[ga][integration]") {
    SECTION("Initial GA with 2 cells") {
        auto ga = std::make_shared<autom::core::ga::GeneticAlgorithm>(2, 5, 8, 2, 1e-3);
        REQUIRE(ga->GetPopulationSize() == 2);

        // Setze fitness
        ga->SetIndividualFitness(0, 5.0);
        ga->SetIndividualFitness(1, 3.0);

        // Step
        ga->ExecuteGenerationStep();

        REQUIRE(ga->GetPopulationSize() == 2);
    }

    SECTION("GA recreated with 5 cells (like slider change)") {
        // Original GA mit 2 Zellen
        auto ga = std::make_shared<autom::core::ga::GeneticAlgorithm>(2, 5, 8, 2, 1e-3);

        // User ändert Slider auf 5 Zellen → GA wird neu erstellt
        ga = std::make_shared<autom::core::ga::GeneticAlgorithm>(5, 5, 8, 2, 1e-3);

        REQUIRE(ga->GetPopulationSize() == 5);

        // Jetzt sollte SetIndividualFitness für 0-4 funktionieren
        for (std::size_t i = 0; i < 5; ++i) {
            REQUIRE_NOTHROW(ga->SetIndividualFitness(i, static_cast<double>(i)));
        }

        // Und für 5+ sollte es fehlen
        REQUIRE_THROWS_AS(ga->SetIndividualFitness(5, 1.0), std::out_of_range);
    }
}

