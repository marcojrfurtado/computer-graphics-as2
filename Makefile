all:
	g++ -o motionviewer motionviewer.cpp joint.cpp -lglut -lGLEW -lGL -lGLU -lX11
