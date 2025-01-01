#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <cstdlib> 
#include <ctime>   
#include <time.h>
#include <iostream>
#include <matplotlibcpp.h>
#include <vector>


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
    Network yarp;
	Property prop;
	PolyDriver pd;
    IEncoders *enc;
    BufferedPort<Bottle> simPort;
    simPort.open("/simPortReceiver");
    yarp.connect("/simPortSender", "/simPortReceiver");

    //creating the connection, associating names of the port to keywords
	prop.put("device", "remote_controlboard");
	prop.put("local", "/plotter");
	prop.put("remote", "/icubSim/head");
    
    //starting the polydriver
    pd.open(prop);

	//opening the controllers we want to use
	pd.view(enc);

    // vectors for plotting
    vector<double> neckPitchPlot={};
    vector<double> neckYawPlot={};
    vector<double> eyesTiltPlot={};
    vector<double> eyesYawPlot={};
    vector<int> errorXPlot={};
    vector<int> errorYPlot={};
    vector<int> iterPlot={};

    vector<double> timePlot={};

    // start time
    double startTime = Time::now();
    double timeNow = 0;
    bool flag = true;
    int iter = 0;

    while(iter<6){
        // getting desired data
        double neckPitch, neckYaw, eyesTilt, eyesYaw=0.0;
        enc->getEncoder(0,&neckPitch);
        enc->getEncoder(2,&neckYaw);
        enc->getEncoder(3,&eyesTilt);
        enc->getEncoder(4,&eyesYaw);

        // assign it to vectors
        neckPitchPlot.push_back(neckPitch);
        neckYawPlot.push_back(neckYaw);
        eyesTiltPlot.push_back(eyesTilt);
        eyesYawPlot.push_back(eyesYaw);


        // clear the data
        plt::cla();

        // Create the main plot for head positions
        plt::subplot(2, 1, 1);
        plt::title("The Saccade Plot");

        // measuring time
        timeNow = Time::now() - startTime;
        timePlot.push_back(timeNow);
        
        // plotting neck pitch
        plt::named_plot("neck pitch",timePlot,neckPitchPlot,"b");

        // plotting neck yaw
        plt::named_plot("neck yaw",timePlot,neckYawPlot,"r");

        // plotting eyes tilt
        plt::named_plot("eyes tilt",timePlot,eyesTiltPlot,"g");

        // plotting eyes yaw
        plt::named_plot("eyes yaw",timePlot,eyesYawPlot,"y");

        plt::xlabel("Time (s)");
        plt::ylabel("Position (deg)");

        if(flag == true){   
            plt::legend();
            // flag=false;
        }

        Bottle *simMessage = simPort.read();
        if(simMessage != nullptr){
            errorXPlot.push_back(simMessage->get(0).asFloat64());
            errorYPlot.push_back(simMessage->get(1).asFloat64());
            iter = simMessage->get(2).asInt64();
            iterPlot.push_back(iter);
        }
        
        // Create a secondary plot for the error and iteration number
        plt::subplot(2, 1, 2);
        
        plt::named_plot("error x",timePlot,errorXPlot,"r");
        plt::named_plot("error y",timePlot,errorYPlot,"g");
        plt::xlabel("Time (s)");
        plt::ylabel("Error (px)");

        if(flag == true){   
            plt::legend();
            flag=false;
        }

        plt::pause(0.001);
    }
    // Show the plot
    plt::show();
    simPort.close();
    plt::close();
    return 0;
}