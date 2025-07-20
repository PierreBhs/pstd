project_dir := justfile_directory()
build_root := project_dir + "/build"
default_preset := "conan-release"

conan-setup: conan-setup-release conan-setup-debug

conan-setup-release:
    conan install . --build=missing -s build_type=Release

conan-setup-debug:
    conan install . --build=missing -s build_type=Debug

# --- Build Commands ---
build: build-release build-debug

build-release:
    cmake --preset conan-release
    cmake --build --preset conan-release

build-debug:
    cmake --preset conan-debug
    cmake --build --preset conan-debug

# --- Test Commands ---
test: test-release

test-release:
    #!/usr/bin/env bash
    set -euxo pipefail
    cmake --build --preset conan-release --target pstd_tests
    ctest --preset conan-release --output-on-failure

test-debug:
    #!/usr/bin/env bash
    set -euxo pipefail
    cmake --build --preset conan-debug --target pstd_tests
    ctest --preset conan-debug --output-on-failure

test-rerun: test-rerun-release

test-rerun-release:
    #!/usr/bin/env bash
    set -euxo pipefail
    ctest --preset conan-release --rerun-failed --output-on-failure

test-rerun-debug:
    #!/usr/bin/env bash
    set -euxo pipefail
    ctest --preset conan-debug --rerun-failed --output-on-failure

# --- Performance Test Commands ---
perf: perf-release

perf-release:
    #!/usr/bin/env bash
    set -euxo pipefail
    cmake --preset conan-release -DPSTD_BUILD_PERF_TESTS=ON
    cmake --build --preset conan-release --target pstd_perf
    "{{build_root}}/Release/perf/pstd_perf" \
        --benchmark_out="{{project_dir}}/perf/benchmark_results_release.json" \
        --benchmark_out_format=json

perf-debug:
    #!/usr/bin/env bash
    set -euxo pipefail
    cmake --preset conan-debug -DPSTD_BUILD_PERF_TESTS=ON
    cmake --build --preset conan-debug --target pstd_perf
    "{{build_root}}/Debug/perf/pstd_perf" \
        --benchmark_out="{{project_dir}}/perf/benchmark_results_debug.json" \
        --benchmark_out_format=json

# --- Plotting Command ---
python-env:
    #!/usr/bin/env bash
    set -euxo pipefail
    uv venv
    uv pip install --upgrade pip && \
    uv pip install conan pandas matplotlib seaborn

plot: conan-setup-release build-release perf-release plot-release

# Plot using the virtualenv
plot-release:
    .venv/bin/python3 {{project_dir}}/perf/plot_results.py {{project_dir}}/perf/benchmark_results_release.json --output-dir {{project_dir}}/perf_plots_release

plot-debug:
    .venv/bin/python3 {{project_dir}}/perf/plot_results.py {{project_dir}}/perf/benchmark_results_debug.json --output-dir {{project_dir}}/perf_plots_debug

# --- Installation ---
install: install-release

install-release:
    #!/usr/bin/env bash
    set -euxo pipefail
    cmake --build --preset conan-release
    cmake --install {{build_root}}/Release --prefix ./install_dir
    
# --- Clean Command ---
clean:
    rm -rf {{build_root}}
    rm -rf {{project_dir}}/perf_plots_release
    rm -rf {{project_dir}}/perf_plots_debug
    rm -f {{project_dir}}/perf/benchmark_results_release.json
    rm -f {{project_dir}}/perf/benchmark_results_debug.json
    rm -rf .venv
    rm -rf ./install_dir
