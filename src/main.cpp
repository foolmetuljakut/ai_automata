#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> // Includes OpenGL headers automatically
#include <iostream>
#include <stdexcept>

#include "gui/MainWindow.h"

// -----------------------------------------------------------------------------
// AddressSanitizer Suppressions
// Unterdrueckt bekannte False-Positives (Memory Leaks) im proprietiaeren NVIDIA-Treiber.
// -----------------------------------------------------------------------------
extern "C" const char* __lsan_default_suppressions() {
    return "leak:libnvidia-glcore.so\n"
           "leak:libnvidia-glsi.so\n"
           "leak:libGLX_nvidia.so\n";
}

// -----------------------------------------------------------------------------
// Newspaper Principle: Von oben nach unten lesen. Erst das Wesentliche (main),
// dann die detaillierten Hilfsfunktionen.
// -----------------------------------------------------------------------------

// Fehler-Callbacks (Fehler klar behandeln)
static void GlfwErrorCallback(int error, const char* description)
{
    std::cerr << "Glfw Error " << error << ": " << description << std::endl;
}

// Forward Declarations fuer kleine Funktionen (SRP & Kleine Funktionen)
GLFWwindow* InitializeWindow(int width, int height, const char* title);
void InitializeImGui(GLFWwindow* window, const char* glsl_version);
void RunMainLoop(GLFWwindow* window, autom::gui::MainWindow& mainWindow);
void Cleanup(GLFWwindow* window);

int main(int, char**)
{
    try {
        const char* glsl_version = "#version 130";

        // Kapselung von Komplexitaet in einzelne Schritte
        GLFWwindow* window = InitializeWindow(1280, 720, "AI Automata Default View");
        InitializeImGui(window, glsl_version);

        autom::gui::MainWindow mainWindow;

        RunMainLoop(window, mainWindow);

        Cleanup(window);
    }
    catch (const std::exception& e) {
        std::cerr << "Fataler Fehler: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

// -----------------------------------------------------------------------------
// Implementierungsdetails (Kleine Funktionen & SRP)
// -----------------------------------------------------------------------------

GLFWwindow* InitializeWindow(int width, int height, const char* title)
{
    glfwSetErrorCallback(GlfwErrorCallback);

#if defined(GLFW_PLATFORM)
    // Avoid Wayland shutdown crash by prioritizing X11 which works stably with ASan/GLFW on Fedora
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    if (!glfwInit()) {
        throw std::runtime_error("Fehler bei glfwInit()");
    }

    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        throw std::runtime_error("Fehler bei glfwCreateWindow()");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync aktivieren

    return window;
}

void InitializeImGui(GLFWwindow* window, const char* glsl_version)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Lade config aus dem Projektwurzel-Verzeichnis
    ImGuiIO& io = ImGui::GetIO();
#ifdef IMGUI_INI_PATH
    io.IniFilename = IMGUI_INI_PATH;
#endif

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void RunMainLoop(GLFWwindow* window, autom::gui::MainWindow& mainWindow)
{
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glfwPollEvents();

        // Starte den ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Aktualisiere Simulation vor Rendering
        mainWindow.UpdateSimulation(deltaTime);

        // UI Rendern (Trennung von UI und Business Logic)
        mainWindow.Render();

        // OpenGL Drawing
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void Cleanup(GLFWwindow* window)
{
    // Aufräumen (Boy-Scout-Rule)
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
