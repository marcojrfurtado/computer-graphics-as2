#include "joint.hpp"
#include <cstring>

using namespace std;

bool Joint::process(FILE *fp) {	

	char next_str[100];
	fscanf(fp," %s {",next_str);


	// Read identification for this joint
	printf("%s\n",next_str);

	set_name(next_str);

	
	while ( fscanf(fp," %s",next_str) != EOF ) {

		if ( !strcmp(next_str,"JOINT" ) || !strcmp(next_str,"End")  ) {
			Joint child;
			child.process(fp);
			this->subjoints.push_back(child);
		}
		else if ( !strcmp(next_str,"}") )
			break;
		else 
			printf("%s\n",next_str);
	}
//	if ( ! strcmp(next_str,"")  ) 

}



void Joint::pretty_print(const char *offset){


	char next_offset[100] = " \0";
	strcat(next_offset,offset);

	printf("%s %s\n",offset, this->get_name() );

	std::vector<Joint>::iterator it;

	for( it = subjoints.begin(); it != subjoints.end(); it++ ) {
		it->pretty_print(next_offset);
	}



}
