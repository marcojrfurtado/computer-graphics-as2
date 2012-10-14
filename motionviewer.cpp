#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif





#include "joint.hpp"
#include <cstdio>
#include <cstring>

#define EXIT_ERROR_STATUS -1


// OpenGL Window attributes
#define WIN_SIZE 1000
#define WIN_POS 100


// Root element in the hierarchy
Joint root(true);

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y) {

	FILE *fp;
	switch(key) {
		case 'w':
			fp = fopen("output.bvh","w");
			fprintf(fp,"HIERARCHY\n");
			root.print(fp);
			fclose(fp);
			break;
		case 'q':
			exit(1);
			break;
	}
}



int main(int argc, char *argv[]) {

	FILE *bvh_fp;
	char next_string[100];


	if ( argc < 2 ) {
		fprintf(stderr,"Wrong number of arguments.\nCorrect usage: ./modelviewer <BVH filename>\n");
		return EXIT_ERROR_STATUS;
	}


	if ( ! (bvh_fp = fopen(argv[1],"r") ) ) {
		fprintf(stderr,"Problem opening file %s.\n",argv[1]);
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
	} else if ( !strcmp(next_string,"MOTION"  ) ) {
		printf("To be done");
	} else {
		fprintf(stderr,"Wrong file header.\n");
		return EXIT_ERROR_STATUS;
	}

	fclose(bvh_fp);


	//INIT GLUT PROCEDURES

	glutInit( &argc, argv );
	glutInitWindowSize( WIN_SIZE, WIN_SIZE );
	glutInitWindowPosition(WIN_POS, WIN_POS );

	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE  );

	glutCreateWindow("Motion Viewer");

	glutKeyboardFunc(keyInput);



	glutMainLoop();

	return 0;
}
