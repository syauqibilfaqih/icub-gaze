#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <cstdlib> 
#include <ctime>   
#include <time.h>
#include <iostream>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace std;

int iter;

class IcubThread: public PeriodicThread{
	public:
        bool isMovementDone;
		IcubThread(double period):PeriodicThread(period){
			eyePort.open("/img");
            simPort.open("/simPortSender");

			yarp.connect("/icubSim/cam/left", "/img");
            // yarp.waitConnection("/simPortSender", "/simPortReceiver");
            yarp.connect("/simPortSender", "/simPortReceiver");

		    //creating the connection, associating names of the port to keywords
			prop.put("device", "remote_controlboard");
			prop.put("local", "/thread");
			prop.put("remote", "/icubSim/head");
			
            //starting the polydriver
			pd.open(prop);
			
            //opening the controllers to be used
			pd.view(ipc);
			pd.view(enc);	
            pd.view(ivc);
            pd.view(icm);

            icm->setControlMode(0,VOCAB_CM_POSITION);
            icm->setControlMode(2,VOCAB_CM_POSITION);
            icm->setControlMode(3,VOCAB_CM_POSITION);
            icm->setControlMode(4,VOCAB_CM_POSITION);
            ipc->positionMove(0,0);
            ipc->positionMove(2,0);
            ipc->positionMove(3,0);
            ipc->positionMove(4,0);

            Time::delay(2);

            icm->setControlMode(0,VOCAB_CM_VELOCITY);
            icm->setControlMode(2,VOCAB_CM_VELOCITY);
            // icm->setControlMode(3,VOCAB_CM_VELOCITY);
            // icm->setControlMode(4,VOCAB_CM_VELOCITY);
            ivc->velocityMove(0,0);
            ivc->velocityMove(2,0);
            // ivc->velocityMove(3,0);
            // ivc->velocityMove(4,0);

		};

	private:		
		Network yarp;
		Property prop;
		PolyDriver pd;
		IPositionControl * ipc;
        IVelocityControl * ivc;
        IControlMode * icm;
		IEncoders *enc;
		BufferedPort<ImageOf<PixelRgb>> eyePort;
        BufferedPort<Bottle> simPort;
		
		double eyeTiltPosition, eyeYawPosition=0.0;
    	double eyeEncoderTiltPosition, eyeEncoderYawPosition=0.0;
        int errX, errY; 
        int lastErrX, lastErrY = 0;
        double Kp = 0.2;
        double Kd = 0;

	protected:
		void run() override {
			ImageOf<PixelRgb> *image= eyePort.read();
			int pixelMeanX,pixelMeanY = 0;
			int numofpix = 0;
			for(int x=0; x<image->width(); x++){
			  for(int y=0; y<image->height(); y++){
			  	PixelRgb& pixel =image->pixel(x,y);
			  	if(pixel.r>2*pixel.g && pixel.r>2*pixel.b){
			  	  pixelMeanX = pixelMeanX+x;
                  pixelMeanY = pixelMeanY+y;
			  	  numofpix++;
			  	}
			  }
			}
			pixelMeanX = pixelMeanX/numofpix;   
            // cout<<"Pixel mean X is: "<<pixelMeanX<<" while the image width is: "<<image->width()<<endl;
            pixelMeanY = pixelMeanY/numofpix;
            // cout<<"Pixel mean Y is: "<<pixelMeanY<<" while the image height is: "<<image->height()<<endl;
            // cout<<" "<<endl;

			errX=pixelMeanX-(image->width()/2);
            errY=pixelMeanY-(image->height()/2);
            // cout<<"error X is: "<<errX<<endl;
            // cout<<"error Y is: "<<errY<<endl;
            // cout<<" "<<endl;

            Bottle& simMessage = simPort.prepare();
            simMessage.clear();
            simMessage.addInt64(errX);
            simMessage.addInt64(errY);
            simMessage.addInt64(iter);
            simPort.write();


			double degX = errX*Kp + (lastErrX-errX)*Kd;
            double degY = errY*Kp + (lastErrY-errY)*Kd;
            // cout<<"Deg X is: "<<degX<<endl;
            // cout<<"Deg Y is: "<<degY<<endl;
            // cout<<" "<<endl;

			enc->getEncoder(4, &eyeEncoderYawPosition);
            enc->getEncoder(3, &eyeEncoderTiltPosition);

            // cout<<"Encoder Position X: "<<eyeEncoderHorPosition<<endl;
            // cout<<"Encoder Position Y: "<<eyeEncoderVerPosition<<endl;
            // cout<<" "<<endl;

		 	eyeYawPosition=eyeEncoderYawPosition;
            eyeTiltPosition=eyeEncoderTiltPosition;

			eyeYawPosition+=degX;
            eyeTiltPosition-=degY;
            // cout<<"New Eye Position X: "<<eyeHorPosition<<endl;
            // cout<<"New Eye Position Y: "<<eyeVerPosition<<endl;

			ipc->positionMove(4, eyeYawPosition);	
            ipc->positionMove(3, eyeTiltPosition);

            if(abs(eyeEncoderYawPosition) != 0 && abs(eyeEncoderTiltPosition) != 0){
                ivc->velocityMove(0,(eyeTiltPosition)*1.2);
                ivc->velocityMove(2,-(eyeYawPosition)*1.2);
            }
            else{
                ivc->velocityMove(0,0);
                ivc->velocityMove(2,0);
            }

            lastErrX = errX;
            lastErrY = errY;

            isMovementDone = (abs(errX)<1 && abs(errY)<1 && abs(eyeEncoderTiltPosition)<0.2 && abs(eyeEncoderYawPosition)<0.2) ? true : false;
		}
		void threadRelease() override {
            ivc->velocityMove(0,0);
            ivc->velocityMove(2,0);
			pd.close();
		}
};


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

    // start the thread
    IcubThread it(0.5);
    it.start();

    // MyThread mt(0.5);
    // mt.start();

    Time::delay(1);

    // to generate random position of sphere, where it should be located 25 pixel far from the centroid of left eye,
    // it is probably in the range of (centroid+0.3,centroid+0.4) or (centroid-0.3,centroid-0.4)  

    // variable of last centroid point
    float lastCentrX = 0.0; 
    float lastCentrY = 0.9;

    for(iter=0; iter<6; iter++){
        cout<<"Number of iteration: "<<iter+1<<endl;        
        // to change position of the sphere, the command is world set ssph 1 value_x value_y value_z
        sphereReq.clear();
        sphereReq.addString("world");
        sphereReq.addString("set");
        sphereReq.addString("ssph");
        sphereReq.addInt32(1);


        float newCentrX = 0.4, newCentrY=0.0;
        cout<<"Generating new random position..."<<endl;

        while(newCentrX>0.3 || newCentrX<-0.3){
            // getting random for each iteration time
            srand(time(0));

            // generate random number in range -0.2 to 0.2
            float randomRangeX = -0.1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.1+0.1)));
            
            // 0.2 to make sure that it's 25 pixel away
            newCentrX = (randomRangeX<0) ? lastCentrX+randomRangeX-0.2 : lastCentrX+randomRangeX+0.2;
            // cout<<"New Center is (x,y): ("<<newCentrX<<", "<<newCentrY<<")"<<endl;
        }
        while(newCentrY>1.1 || newCentrY<0.6){
            // getting random for each iteration time
            srand(time(0));

            // generate random number in range -0.2 to 0.2
            float randomRangeY = -0.1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.1+0.1)));

            // 0.2 to make sure that it's 25 pixel away
            newCentrY = (randomRangeY<0) ? lastCentrY+randomRangeY-0.2 : lastCentrY+randomRangeY+0.2;
            // cout<<"New Center is (x,y): ("<<newCentrX<<", "<<newCentrY<<")"<<endl;
        }

        cout<<"New random position is generated!"<<endl;

        sphereReq.addFloat64(newCentrX);
        sphereReq.addFloat64(newCentrY);
        sphereReq.addFloat64(0.8);

        it.isMovementDone = false;

        portRpc.write(sphereReq);

        lastCentrX = newCentrX;
        lastCentrY = newCentrY;

        while(!it.isMovementDone){}
        cout<<"System reached the desired position!"<<endl;
        cout<<"------------------------------------"<<endl;
    }

    portRpc.close(); // closing the port after its finished

    return 0;
}