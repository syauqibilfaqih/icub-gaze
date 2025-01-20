# icub-gaze
![spliticub](https://github.com/user-attachments/assets/28324395-c0bc-4e24-ab44-7a03ab153f8f)

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
5. Alternatively, the program can be ran using a single line, make sure the terminal is in icub-gaze directory, then run:
```
. run-plotter.sh
```
It will run both the robot program and the plotter to show the graph of the camera errors, eye positions, head positions, for two iterations.

