#include "MainWindow.h"
#include <imgui.h>
#include <Eigen/Dense>

// Abhaengigkeit injiziert (minimal) zum Testen der Integration -> Newspaper Prinzip
#include "math/NeuralMath.h"

namespace autom {
namespace gui {

void MainWindow::Render() {
    ImGui::Begin("AI Automata - Dashboard");

    ImGui::Text("Willkommen in der C++ Pipeline fuer AI Automata.");
    ImGui::Separator();

    DrawCalculationsSection();

    ImGui::End();
}

void MainWindow::DrawCalculationsSection() {
    // Demonstration einer Berechnung in der GUI
    ImGui::Text("Generiere Live-Berechnung:");

    static Eigen::Vector2d input(1.0, 1.0);
    static Eigen::Matrix2d weights;
    static Eigen::Vector2d bias(0.0, 0.0);
    static bool initialized = false;

    if (!initialized) {
        weights << 0.5, 0.1, -0.2, 0.8;
        initialized = true;
    }

    ImGui::InputDouble("Input 1", &input(0));
    ImGui::InputDouble("Input 2", &input(1));

    Eigen::Vector2d result = math::NeuralMath::ComputeLayerOutput(weights, input, bias);

    ImGui::Text("Ergebnis der Matrixmultiplikation (ImGui + Eigen3):");
    ImGui::Text("Res = (%.2f, %.2f)", result(0), result(1));
}

} // namespace gui
} // namespace autom

