#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <time.h>
#include <iostream>

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char *argv[]) {
    Network yarp; // initialize the network
    RpcClient port; // initialize Rpc port as client

    port.open("/icub-sender"); // open a new port
    yarp.connect("/icub-sender","/icubSim/head/rpc:i"); // connect to the head's port


    // Sending command to move the robot's head in a loop
    for(int i=-30; i<31; i++){
        for(int j=0; j<6; j++){
            Bottle req; // using Bottle as the message's data type
            req.addString("set");
            req.addString("pos");
            req.addInt8(j);
            req.addInt8(i);
            port.write(req);
        }
        Time::delay(0.1);
    }

    

    port.close(); // closing the port after its finished

    return 0;
}