#include "joint.hpp"
#include <cstdio>
#include <cstring>

#define EXIT_ERROR_STATUS -1


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

	Joint root(true);
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

	root.pretty_print("\0");


	fclose(bvh_fp);


	return 0;
}
