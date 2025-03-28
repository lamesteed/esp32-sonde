# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(sonde_main)

# Define the main executable
add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_DIR}/main/sonde_main.cpp)

# Link libraries globally
target_link_libraries(${CMAKE_PROJECT_NAME}
    esp_idf
    libefuse
    # Add more libraries as needed
)
