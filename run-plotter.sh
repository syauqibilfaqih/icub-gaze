cd "$(find . -type d -name bin -print -quit)"
./icub-gaze & 
./plotter &

wait
cd ..
cd ..