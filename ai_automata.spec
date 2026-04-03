Name:           ai_automata
Version:        1.0
Release:        1%{?dist}
Summary:        AI Automata Dashboard Application

License:        MIT
URL:            https://github.com/devsr/ai_automata

BuildRequires:  gcc-c++ cmake make glfw-devel mesa-libGL-devel eigen3-devel catch2-devel
Requires:       glfw mesa-libGL

%description
AI Automata ist eine native C++ Applikation mit ImGui-Oberflaeche
und Eigen3 zur schnellen Berechnung von linearen Operationen.

%prep
# Build-in-Place wird genutzt.
# Keine automatisierten %setup Aktionen

%build
# Wir erstellen einen sauberen Release-Build ohne Sanitizer,
# da diese die RPM-Performance negativ beeinflussen.
# Setze IMGUI_DIR explizit hoch auf das echte ImGui Verzeichnis.
cmake -B build_rpm -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DENABLE_SANITIZER=OFF \
    -DENABLE_COVERAGE=OFF \
    -DIMGUI_DIR=/home/devsr/imgui
cmake --build build_rpm -j$(nproc)

%install
rm -rf %{buildroot}
DESTDIR=%{buildroot} cmake --install build_rpm

%files
/usr/bin/ai_automata
