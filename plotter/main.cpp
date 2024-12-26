#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <cstdlib> 
#include <ctime>   
#include <time.h>
#include <iostream>
#include <matplotlibcpp.h>


namespace plt = matplotlibcpp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace std;

/*
you need to retrieve data from the eye and head using IPositionControl, and plot it vs time
also, plot the error, get it from the icub-gaze
*/

int main(int argc, char *argv[]) {
    // clear the data
    plt::cla();

    // Create the main plot for head positions
    plt::subplot(2, 1, 1);
    plt::title("The Saccade Plot");
    // plt::plot(x, y1);
    plt::xlabel("Time (s)");
    plt::ylabel("Position (deg)");

    // Create a secondary plot for the error and iteration number
    plt::subplot(2, 1, 2);
    // plt::plot(x, y2, "r");
    plt::xlabel("Number of Iteration");
    plt::ylabel("Error (px)");

    // Show the plot
    plt::show();
    return 0;
}