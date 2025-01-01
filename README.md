# icub gaze
A repository on fast gaze shift performed in iCub Simulator.

# Requirements
1. Ubuntu 22.04
2. CMake 3.22.1 (tested)
3. icub-main 1.25.0

# Installation
1. Download this repository, and create a build folder under icub-gaze (not under icub-gaze/icub-gaze)
```
mkdir build
```
2. Run cmake to initialize the the project folder configuration
```
cmake ../
```
3. Compile the project 
```
make
```

# Execution
1. Run yarpserver on one terminal:
```
yarpserver
```
2. Open a new terminal, and run iCub_SIM:
```
iCub_SIM
```
3. In a new terminal, open the yarpview to see the camera view:
```
yarpview --name /viewer &
```
it would be empty, thus we need to connect it to the left eye, in a new terminal, run:
```
yarp connect /icubSim/cam/left /viewer
```
4. Open another terminal, and go to the bin directory:
```
cd <your directory>/icub-gaze/build/bin
```
then run the `icub-gaze` binary file
```
./icub-gaze
```
# Issues 
- [ ] Plot stuck
- [ ] No legend on subplot 2
- [ ] High overdamp
- [ ] Slow head response
 
