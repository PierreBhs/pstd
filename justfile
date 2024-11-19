project_dir := justfile_directory()
build_dir := project_dir + "/build"

build:
    cmake -S . -B build
    cmake --build {{build_dir}}

run:
    {{build_dir}}/pstd
    
