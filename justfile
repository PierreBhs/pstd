project_dir := justfile_directory()
build_dir := project_dir + "/build"

conan-setup:
    conan install . -u -b missing
    . build/Release/generators/conanbuild.sh


# Check how to change name later
build:
    cmake --preset conan-release
    cmake --build --preset conan-release

test:
    ctest --preset conan-release

test-rerun:
    ctest --preset conan-release --rerun-failed --output-on-failure

run:
    {{build_dir}}/Release/pstd
    
