#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <time.h>
#include <iostream>

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char *argv[]) {
    Network yarp; // initialize the network
    RpcClient portRpc; // initialize Rpc port as client

    portRpc.open("/sphere"); // open a new port for sphere
    yarp.connect("/sphere","/icubSim/world"); // connect to the head's port

    // create a new sphere on the window
    
    // create a new bottle for producing sphere and change its position
    Bottle sphereReq;

    // clear every entities in case it's exist in the previous run
    sphereReq.addString("world");
    sphereReq.addString("del");
    sphereReq.addString("all");
    portRpc.write(sphereReq);

    sphereReq.clear();

    // send the command  `world mk ssph (radius) (three params for pos) (three params for colour)` through bottle
    // with values world mk ssph 0.04 0.0 0.9 0.8 1 0 0 
    sphereReq.addString("world");
    sphereReq.addString("mk");
    sphereReq.addString("ssph");
    sphereReq.addFloat64(0.04);
    sphereReq.addFloat64(0.0);
    sphereReq.addFloat64(0.9);
    sphereReq.addFloat64(0.8);
    sphereReq.addFloat64(1.0);
    sphereReq.addFloat64(0);
    sphereReq.addFloat64(0);
    portRpc.write(sphereReq);

    // Sending command to move the robot's head in a loop
    // for(int i=-30; i<31; i++){
    //     for(int j=0; j<6; j++){
    //         Bottle req; // using Bottle as the message's data type
    //         req.addString("set");
    //         req.addString("pos");
    //         req.addInt8(j);
    //         req.addInt8(i);
    //         port.write(req);
    //     }
    //     Time::delay(0.1);
    // }

    

    portRpc.close(); // closing the port after its finished

    return 0;
}