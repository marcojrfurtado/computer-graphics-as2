LOCAL_LIBRARY=joint.cpp camera.cpp
all:
	g++ -o motionviewer motionviewer.cpp $(LOCAL_LIBRARY)  -lglut -lGLEW -lGL -lGLU -lX11 -lm
