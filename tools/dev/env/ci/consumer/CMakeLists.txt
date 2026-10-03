# Consumer project for the install-and-consume CI job: build hello.c
# against an installed Motif through its CMake config package.
cmake_minimum_required(VERSION 3.16)
project(motif_consumer C)

# CONFIG: use Motif's own MotifConfig.cmake, not CMake's FindMotif module.
find_package(Motif REQUIRED CONFIG)
message(STATUS "Found Motif ${Motif_VERSION} in ${Motif_DIR}")

add_executable(hello hello.c)
target_compile_definitions(hello PRIVATE HELLO_MRM)
target_link_libraries(hello PRIVATE Motif::Xm Motif::Mrm)
