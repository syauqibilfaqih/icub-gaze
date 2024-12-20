# set up our program
add_executable(icub-gaze)

# declare our source files
target_sources(icub-gaze PRIVATE main.cpp)

# link with YARP libraries
target_link_libraries(icub-gaze PRIVATE YARP::YARP_os
                                          YARP::YARP_init
                                          YARP::YARP_sig)
