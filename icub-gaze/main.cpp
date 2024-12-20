#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <time.h>
#include <iostream>

using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char *argv[]) {
    Network yarp;
    Port p;
    BufferedPort<Bottle> bp;
    VectorOf<int> v; // v.resize(20)

    bp.open("/bpsender");
    bool ok  = p.open("/sender");
    if (!ok) {
        std::cerr << "Failed to open port" << std::endl;
        return 1;
    }

    yarp.waitConnection("/sender", "/receiver");
    yarp.waitConnection("/bpsender", "/bpreceiver");
    Bottle b;
    b.addString("Hello");
       
    // send the message through the port
    std::cout << "Sending Hello message " << std::endl;
    p.write(b);

    Bottle& output = bp.prepare();

    srand(time(NULL));
    for(int i=0; i<20; i++){
        v.push_back(rand()%30+1);
    }
    for(int i=0; i<20; i++){
        std::cout<<i+1<<". "<<v[i]<<"\n";
    }
    output.clear();
    output.addList().read(v);
    output.addString("median");   
    bp.write();

    p.close();
    bp.close();
}
