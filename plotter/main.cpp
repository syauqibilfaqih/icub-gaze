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

    vector<double> timePlot={};

    // start time
    double startTime = Time::now();
    double timeNow = 0;
    bool flag = true;
    int iter = 0;

    while(iter<2){
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
        plt::title("The Saccade Plot, Number of Iteration = "+std::to_string(iter+1));

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
            flag=false;
        }

        Bottle *simMessage = simPort.read();
        if(simMessage != nullptr){
            iter = simMessage->get(2).asInt64();
            if(iter<2){
                errorXPlot.push_back(simMessage->get(0).asInt64());
                errorYPlot.push_back(simMessage->get(1).asInt64());
            }
            else{
                timePlot.pop_back();
            }
        }
        
        // Create a secondary plot for the error and iteration number
        plt::subplot(2, 1, 2);
        
        plt::named_plot("error x",timePlot,errorXPlot,"m");
        plt::named_plot("error y",timePlot,errorYPlot,"c");

        plt::xlabel("Time (s)");
        plt::ylabel("Error (px)");
 
        plt::legend();

        plt::pause(0.001);
    }
    // Show the plot
    plt::show();
    simPort.close();
    return 0;
}