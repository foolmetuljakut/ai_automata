#pragma once

#include <Eigen/Dense>
#include <cstddef>
#include <memory>

namespace autom {
namespace core {
namespace ga {

/**
 * @brief Interface für genetische Operationen (Crossover, Mutation, Linearisierung).
 *
 * Diese Klasse kapselt die Primitive-Operationen des Genetischen Algorithmus:
 * - Linearisierung: NN-Gewichte in einen 1D-Vektor umwandeln
 * - Deserialisierung: Vektor zurück zu NN-Gewichten
 * - Crossover: zwei Vektoren kombinieren
 * - Mutation: zufällige Wertänderungen
 */
class GeneticOperations {
public:
    /**
     * @brief Konstruktor.
     * @param mutationRate Wahrscheinlichkeit pro Wert pro Generation (default: 1e-3)
     */
    explicit GeneticOperations(double mutationRate = 1e-3);

    // ========== LINEARISIERUNG ==========

    /**
     * @brief Linearisiere ein NN zu einem 1D-Vektor.
     *
     * Reihenfolge (zeilenweise):
     * [InputToHidden_weights (zeile für zeile), HiddenBias, HiddenToOutput_weights, OutputBias]
     *
     * @param inputCount Anzahl Input-Neuronen
     * @param hiddenCount Anzahl Hidden-Neuronen
     * @param outputCount Anzahl Output-Neuronen
     * @param w_ih InputToHiddenWeights Matrix (hidden × input)
     * @param b_h Hidden Bias Vector (hidden)
     * @param w_ho HiddenToOutputWeights Matrix (output × hidden)
     * @param b_o Output Bias Vector (output)
     * @return Linearisierter Vektor
     */
    static Eigen::VectorXd Linearize(
        std::size_t inputCount,
        std::size_t hiddenCount,
        std::size_t outputCount,
        const Eigen::MatrixXd& w_ih,
        const Eigen::VectorXd& b_h,
        const Eigen::MatrixXd& w_ho,
        const Eigen::VectorXd& b_o);

    /**
     * @brief Deserialisiere einen 1D-Vektor zu NN-Komponenten.
     *
     * @param vector Linearisierter Vektor
     * @param inputCount Anzahl Input-Neuronen
     * @param hiddenCount Anzahl Hidden-Neuronen
     * @param outputCount Anzahl Output-Neuronen
     * @param w_ih [OUT] InputToHiddenWeights Matrix
     * @param b_h [OUT] Hidden Bias Vector
     * @param w_ho [OUT] HiddenToOutputWeights Matrix
     * @param b_o [OUT] Output Bias Vector
     */
    static void Deseriialize(
        const Eigen::VectorXd& vector,
        std::size_t inputCount,
        std::size_t hiddenCount,
        std::size_t outputCount,
        Eigen::MatrixXd& w_ih,
        Eigen::VectorXd& b_h,
        Eigen::MatrixXd& w_ho,
        Eigen::VectorXd& b_o);

    // ========== CROSSOVER ==========

    /**
     * @brief 1-Point Crossover: kombiniere zwei Parent-Vektoren.
     *
     * Wählt einen zufälligen Cut-Point und kombiniert:
     * Offspring = [Parent1[0..cut], Parent2[cut..end]]
     *
     * @param parent1 Erster Parent-Vektor
     * @param parent2 Zweiter Parent-Vektor
     * @return Offspring-Vektor (gleiche Länge wie Parents)
     */
    Eigen::VectorXd Crossover(
        const Eigen::VectorXd& parent1,
        const Eigen::VectorXd& parent2) const;

    /**
     * @brief 2-Point Crossover: kombiniere zwei Parents mit zwei Cut-Points.
     *
     * Offspring = [Parent1[0..cut1], Parent2[cut1..cut2], Parent1[cut2..end]]
     *
     * @param parent1 Erster Parent-Vektor
     * @param parent2 Zweiter Parent-Vektor
     * @return Offspring-Vektor
     */
    Eigen::VectorXd CrossoverTwoPoint(
        const Eigen::VectorXd& parent1,
        const Eigen::VectorXd& parent2) const;

    // ========== MUTATION ==========

    /**
     * @brief Mutiere einen Vektor mit der konfigurierten Mutationsrate.
     *
     * Für jeden Wert: mit Wahrscheinlichkeit m_mutationRate
     * wird ein zufälliger Offset hinzugefügt (Gaussian Distribution, unbegrenzt).
     *
     * @param vector Vektor zum Mutieren (wird IN-PLACE geändert)
     * @param sigma Standard-Abweichung der Mutation (default: 0.1)
     */
    void Mutate(Eigen::VectorXd& vector, double sigma = 0.1) const;

    /**
     * @brief Setze die Mutationsrate.
     * @param rate Neue Mutationsrate (z.B. 1e-3 für 0.1%)
     */
    void SetMutationRate(double rate);

    /**
     * @brief Hole die aktuelle Mutationsrate.
     */
    double GetMutationRate() const;

    /**
     * @brief Berechne die erwartete Größe des linearisierten Vektors.
     *
     * @param inputCount Anzahl Input-Neuronen
     * @param hiddenCount Anzahl Hidden-Neuronen
     * @param outputCount Anzahl Output-Neuronen
     * @return Größe des Vektors
     */
    static std::size_t CalculateVectorSize(
        std::size_t inputCount,
        std::size_t hiddenCount,
        std::size_t outputCount);

private:
    double m_mutationRate;

    /**
     * @brief Gaussian Random Number (Box-Muller Transform).
     */
    static double GaussianRandom();
};

}  // namespace ga
}  // namespace core
}  // namespace autom

