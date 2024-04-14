cmake_minimum_required(VERSION 3.29)

# Set your project name and specify the language (CXX for C++)
project(DistributedParticleExplorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED COMPONENTS system thread filesystem)
find_package(ZLIB REQUIRED)
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/hpp 
    $ENV{JAVA_HOME}/include
    ${Boost_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)

if(WIN32)
    include_directories($ENV{JAVA_HOME}/include/win32)
elseif(UNIX AND NOT APPLE)
    include_directories($ENV{JAVA_HOME}/include/linux)
elseif(APPLE)
    include_directories($ENV{JAVA_HOME}/include/darwin)
endif()

add_executable(ClientServer 
    src/cpp/Explorer.cpp
    src/cpp/Particle.cpp
    src/cpp/SimulationPanel.cpp
    src/cpp/ParticleSimulation.cpp
    src/cpp/ClientServer.cpp
)

target_link_libraries(ClientServer PRIVATE 
    Boost::system
    Boost::thread
    Boost::filesystem
    ZLIB::ZLIB
    sfml-system sfml-network sfml-graphics sfml-window
    nlohmann_json::nlohmann_json
)
