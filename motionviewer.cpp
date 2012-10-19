#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif



#include "joint.hpp"
#include "motion.hpp"
#include "camera.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#define EXIT_ERROR_STATUS -1


// OpenGL Window attributes
#define WIN_SIZE 1000
#define WIN_POS 100

// Default orientation
#define X_LEFT -1.0
#define X_RIGHT 1.0
#define Y_LEFT -1.0
#define Y_RIGHT 1.0
#define NEAR 1.0
#define FAR 100.0

using namespace std;


static std::vector< Motion::frame_data >::const_iterator current_frame_it ;



// Initial value for FPS
#define FPS_DEFAULT 120

// The frame rate defined in the BVH file is ignored
int fps = FPS_DEFAULT;

// Used to start/stop the animation
bool modePlay = false;

// Root element in the hierarchy
Joint root(true);
Motion mt;

// Object representing the camera
Camera cam;

glm::vec3 pos(0,0,0);

void restore() {

	// Restart object to its original position

//	root.restore();

	current_frame_it = mt.get_frame_set().begin();

}

// REcursivefunction used to draw lines in a joint hierarchy
void draw_lines(Joint *j, glm::vec3 origin) {

/*	if ( origin == 0 )i
		origin = j->get_offset();*/
		

	vector<Joint *>::const_iterator it;
	for ( it = j->get_children().begin(); it != j->get_children().end() ; it++ ) {

		glm::vec3 joint_offset = (*it)->get_offset();
		joint_offset.x+=origin.x;
		joint_offset.y+=origin.y;
		joint_offset.z+=origin.z;

		if ( (*it)->has_children()  ) {
			draw_lines(*it,joint_offset);
		}

		glBegin(GL_LINES);
		glVertex3f(origin.x,origin.y,origin.z);
		glVertex3f(joint_offset.x,joint_offset.y,joint_offset.z);
		glEnd();



	}
}


// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y) {

	FILE *fp;
	switch(key) {
		case 'w':
			fp = fopen("output.bvh","w");
			fprintf(fp,"HIERARCHY\n");
			root.print(fp);
			fprintf(fp,"MOTION\n");
			mt.print(fp);
			fclose(fp);
			break;
		case 'q':
			exit(0);
			break;
		case 'T':
	       		cam.rotate(-10.0,0.0,0.0);
       			break;
		case 't':
			cam.rotate(10.0,0.0,0.0);
			break;
		case 'A':
			cam.rotate(0.0,-10.0,0.0);
			break;
		case 'a':
			cam.rotate(0.0,10.0,0.0);
			break;
		case 'C':
			cam.rotate(0.0,0.0,-10.0);
			break;
		case 'c':
			cam.rotate(0.0,0.0,10.0);
			break;
	 	// CAMERA TRANSLATIONS
		case 'I':
	       		cam.translate(0.0,0.0,0.1);      
			break;
	    	case 'i':
		       	cam.translate(0.0,0.0,-0.1);
			break; 
		// ANIMATION CONTROL
		case 'p':
			modePlay=true;
			break;
		case 'P':
			modePlay=false;
			break;
		case 's':
			modePlay=false;
			restore();
			fps=FPS_DEFAULT;
			break;
		case '+':
			fps+=10;
			break;
		case '-':
			fps-=10;
			break;
	 // Switch between projection types
		default:
			break;
	}
   // This avoids changing the animation speed if afer keystrokes
	if( !modePlay )
		glutPostRedisplay();
}


// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{ 
   if(key == GLUT_KEY_UP) cam.translate(0.0,0.1,0.0);
   else if(key == GLUT_KEY_DOWN) cam.translate(0.0,-0.1,0.0);
   else if(key == GLUT_KEY_LEFT) cam.translate(-0.1,0.0,0.0);
   else if(key == GLUT_KEY_RIGHT) cam.translate(0.1,0.0,0.0);
   
   // This avoids changing the animation speed if afer keystrokes
   if ( !modePlay )
	   glutPostRedisplay();
}

int process_file(const char *filename) {

	FILE *bvh_fp;
	char next_string[100];
	
	if ( ! (bvh_fp = fopen(filename,"r") ) ) {
		fprintf(stderr,"Problem opening file %s.\n",filename);
		return EXIT_ERROR_STATUS;
	}


	// Test HIERARCHY keyword
	fscanf(bvh_fp," %s",next_string);


	// Check if we are reading hierarchy description or motion description

	if ( !strcmp(next_string,"HIERARCHY") ) {
		fscanf(bvh_fp," %s",next_string);
		if ( strcmp(next_string,"ROOT" ) ) {
			fprintf(stderr,"Hierarchy descriptions must start from ROOT element.\n");
			return EXIT_ERROR_STATUS;
		}
		root.process(bvh_fp);
	} else {
		fprintf(stderr,"Wrong hierarchy header.\n");
		return EXIT_ERROR_STATUS;
	} 
	
	// Test HIERARCHY keyword
	fscanf(bvh_fp," %s",next_string);

	
	
	if ( !strcmp(next_string,"MOTION"  ) ) {
		mt.set_frame_data_size(root.count_hierarchy_channels());
		mt.process(bvh_fp);

		// Animation is set to start at the first frame
		current_frame_it = mt.get_frame_set().begin();
	} else {
		fprintf(stderr,"Wrong motion header.\n");
		return EXIT_ERROR_STATUS;
	} 

	fclose(bvh_fp);

	return 0;

}

void setCamera() {

	glLoadIdentity();
	// set the camera
	gluLookAt(cam.eye.x,cam.eye.y,cam.eye.z,cam.center.x,cam.center.y,cam.center.z,cam.up.x,cam.up.y,cam.up.z);
	
	
	// Rotates on the camera
	//glRotatef(cam.angle.z, 0.0, 0.0, 1.0);
	//glRotatef(cam.angle.y, 0.0, 1.0, 0.0);
	//glRotatef(cam.angle.x, 1.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	gluPerspective(90,1 , 1.0, 100);
	glFrustum(X_LEFT,X_RIGHT,Y_LEFT,Y_RIGHT,NEAR,FAR);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);

}


// Small timer function, which tells glut to redisplay the object
void Timer( int id) {

	glutPostRedisplay();
}





void processNextFrame() {





	root.motion_transformation(*current_frame_it );

	// Go to the next frame, if we are playing
	if  ( modePlay ) {

		if ( fps > 0 )
			current_frame_it++;
		else
			current_frame_it--;
	}

	// If we reached the end of the animation, we should go back ( unless we are reversing  it)
	if ( current_frame_it == mt.get_frame_set().end() )
		restore();
	else if ( ( fps < 0  )  && ( current_frame_it  == mt.get_frame_set().begin()   ) ) {
		current_frame_it = mt.get_frame_set().end();
		current_frame_it--;
	}




}


// Drawing (display) routine.
void drawScene(void) {


	// Clear Screen to background color.
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


	setCamera();


	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	
	glColor3f(0.0,0.0,0.0);

	glTranslatef(pos.x,pos.y,pos.z);
	
	processNextFrame();

//	draw_lines(&root,root.get_offset());
	

	glutSwapBuffers();

	// Computes the frame time, based on the fps
	int frametime = 1000/FPS_DEFAULT;
	glutTimerFunc( frametime , Timer, 0);
}

int setup(const char *filename) {
	
	// Set background (or clearing) color.
	glClearColor(1.0, 1.0, 1.0, 0.0);	

	glEnableClientState(GL_VERTEX_ARRAY);
	
	if ( process_file(filename)== EXIT_ERROR_STATUS  )
		return EXIT_ERROR_STATUS;


	cam.translate(0.0,0.0,60.0);


//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(3, GL_FLOAT, 0, movedVertexArray);

	return 0;
	
}



int main(int argc, char *argv[]) {



	if ( argc < 2 ) {
		fprintf(stderr,"Wrong number of arguments.\nCorrect usage: ./modelviewer <BVH filename>\n");
		return EXIT_ERROR_STATUS;
	}



	//INIT GLUT PROCEDURES
	glutInit( &argc, argv );
	
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE  );

	glutInitWindowSize( WIN_SIZE, WIN_SIZE );
	
	glutInitWindowPosition(WIN_POS, WIN_POS );


	glutCreateWindow("Motion Viewer");
	
	if ( setup(argv[1]) == EXIT_ERROR_STATUS )
		return EXIT_ERROR_STATUS;
	
	// Register the callback function for non-ASCII key entry.
	glutSpecialFunc(specialKeyInput);

	glutKeyboardFunc(keyInput);
	glutDisplayFunc(drawScene);

	glutMainLoop();

	return 0;
}
