#!/bin/bash
set -e

function cleanup() {
  echo ">> Cleaning build directory..."
  rm -rf build html latex build_rpm *.list 2>/dev/null || true
  mkdir build
}

function configure_cmake() {
  echo ">> Configuring CMake..."
  cmake -B build -S . \
    -DENABLE_SANITIZER=ON \
    -DENABLE_COVERAGE=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
}

function execute_build() {
  echo ">> Compiling Application and Unit Tests..."
  cmake --build build -j$(nproc)
}

function code_analysis() {
  echo ">> Running Clang-Tidy (Code Smells Detection)..."
  # Exclude test frameworks and external libs
  FILES_TO_LINT=$(find src test -name "*.cpp" -not -path "*/imgui/*")
  clang-tidy -p build $FILES_TO_LINT || echo "Clang-Tidy found warnings, please review."
}

function run_unit_tests() {
  echo ">> Running Unit Tests..."
  cd build
  ctest --output-on-failure
  cd ..
}

function create_coverage_report() {
  echo ">> Generating Coverage Report..."
  gcovr -r . build/ --html-details -o build/coverage.html -e "build/.*" -e "test/.*" -e "../imgui/.*"
  echo "Coverage report available at build/coverage.html"
}

function create_documentation() {
  echo ">> Generating Doxygen..."
  if [ ! -f Doxyfile ]; then
      doxygen -g Doxyfile > /dev/null
      sed -i 's/INPUT                  =/INPUT                  = src test/g' Doxyfile
      sed -i 's/RECURSIVE              = NO/RECURSIVE              = YES/g' Doxyfile
      sed -i 's/EXTRACT_ALL            = NO/EXTRACT_ALL            = YES/g' Doxyfile
      sed -i 's/GENERATE_LATEX         = NO/GENERATE_LATEX         = YES/g' Doxyfile
  fi
  doxygen Doxyfile > /dev/null
  echo ">> Compiling LaTeX Documentation to PDF..."
  if [ -d "latex" ]; then
      cd latex
      # Build pdf (use -j to speed it up if desired, standard latex Makefile is mostly single threaded)
      make > /dev/null
      mv refman.pdf ../build/ai_automata_documentation.pdf
      cd ..
      echo "PDF Documentation available in latex/refman.pdf"
  fi
  echo "HTML Documentation available in html/index.html"
}

function create_rpm_package() {
  echo ">> Generating RPM Package (In-Place)..."
  rpmbuild -bb --build-in-place ai_automata.spec \
      --define "_topdir $(pwd)/build/rpm" \
      --define "_rpmdir $(pwd)/build"
  echo "RPM available in: $(pwd)/build/x86_64/"
  mv *.list build/ 2>/dev/null || true # Moving RPM build artifacts from root to build folder...
}

function execute_pipeline() {
  cleanup
  configure_cmake
  execute_build
  code_analysis
  run_unit_tests
  create_coverage_report
  create_documentation
  create_rpm_package
}

case "$1" in
  cleanup) "$@"; exit 0;;
  configure_cmake) "$@"; exit 0;;
  execute_build) "$@"; exit 0;;
  code_analysis) "$@"; exit 0;;
  run_unit_tests) "$@"; exit 0;;
  create_coverage_report) "$@"; exit 0;;
  create_documentation) "$@"; exit 0;;
  create_rpm_package) "$@"; exit 0;;
  *) echo "Usage: $0 {<none>|cleanup|configure_cmake|execute_build|code_analysis|run_unit_tests|create_coverage_report|create_documentation|create_rpm_package}"; exit 1;;
esac

execute_pipeline
