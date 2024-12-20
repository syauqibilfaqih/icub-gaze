#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <cstdlib> 
#include <ctime>   
#include <time.h>
#include <iostream>

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char *argv[]) {
    Network yarp; // initialize the network
    RpcClient portRpc; // initialize Rpc port as client

    portRpc.open("/sphere"); // open a new port for sphere
    yarp.connect("/sphere","/icubSim/world"); // connect to the head's port

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

    Time::delay(1);

    // to generate random position of sphere, where it should be located 25 pixel far from the centroid of left eye,
    // it is probably in the range of (centroid+0.3,centroid+0.4) or (centroid-0.3,centroid-0.4)  

    // variable of last centroid point
    float lastCentrX = 0.0; 
    float lastCentrY = 0.9;

    for(int iter=0; iter<6; iter++){        
        // getting random for each iteration time
        srand(time(0));

        // to change position of the sphere, the command is world set ssph 1 value_x value_y value_z
        sphereReq.clear();
        sphereReq.addString("world");
        sphereReq.addString("set");
        sphereReq.addString("ssph");
        sphereReq.addInt32(1);

        // generate random number in range -0.2 to 0.2
        float randomRangeX = -0.1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.1+0.1)));
        float randomRangeY = -0.1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.1+0.1)));

        // 0.2 to make sure that it's 25 pixel away
        float newCentrX = (randomRangeX<0) ? lastCentrX+randomRangeX-0.2 : lastCentrX+randomRangeX+0.2;
        float newCentrY = (randomRangeY<0) ? lastCentrY+randomRangeY-0.2 : lastCentrY+randomRangeY+0.2;

        sphereReq.addFloat64(newCentrX);
        sphereReq.addFloat64(newCentrY);
        sphereReq.addFloat64(0.8);

        portRpc.write(sphereReq);

        // lastCentrX = newCentrX;
        // lastCentrY = newCentrY;

        Time::delay(1);
    }

    portRpc.close(); // closing the port after its finished

    return 0;
}