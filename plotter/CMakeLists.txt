# set up our program
add_executable(plotter)

# declare our source files
target_sources(plotter PRIVATE main.cpp)

target_include_directories(plotter PRIVATE ${PROJECT_SOURCE_DIR}/plotter)

# find package
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(Python3 COMPONENTS NumPy)

# link with YARP libraries
target_link_libraries(plotter PRIVATE   YARP::YARP_os
                                        YARP::YARP_init
                                        YARP::YARP_sig
                                        YARP::YARP_dev
                                        Python3::Python
                                        Python3::Module
                                        Python3::NumPy)
