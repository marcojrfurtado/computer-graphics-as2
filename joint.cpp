#include "joint.hpp"
#include <cstring>
#include <cstdlib>

using namespace std;

bool Joint::process(FILE *fp) {	

	char next_str[100];
	
	// Read identification for this joint
	fscanf(fp," %s {",next_str);


	//printf("%s\n",next_str);

	set_name(next_str);

	
	while ( fscanf(fp," %s",next_str) != EOF ) {

		if ( !strcmp(next_str,"JOINT" )  ) {
			Joint child;
			child.process(fp);
			this->subjoints.push_back(child);
		}	
		else if (  !strcmp(next_str,"End")  ) {
			Joint child(false,true);
			child.process(fp);
			this->subjoints.push_back(child);
		}	
		else if ( !strcmp(next_str,"OFFSET") ) {
			double x,y,z;
			fscanf(fp," %lf %lf %lf",&x,&y,&z);
			set_offset(x,y,z);
		}
		else if ( !strcmp(next_str,"CHANNELS") ) {

			read_channels(fp);

		}
		else if ( !strcmp(next_str,"}") )
			break;
		else 
			printf("%s\n",next_str);
	}
//	if ( ! strcmp(next_str,"")  ) 

}


void Joint::read_channels(FILE *fp) {


	int num_channels;
	fscanf(fp," %d",&num_channels);


	for(int i = 0; i < num_channels; i++ ) {

		char channel_type[50];
		fscanf(fp," %s",channel_type);

		if ( !strcmp(channel_type,"Xposition") ) 
			channels.push_back(X_POSITION);
		else if ( !strcmp(channel_type,"Yposition") )
			channels.push_back(Y_POSITION);
		else if ( !strcmp(channel_type,"Zposition") )
			channels.push_back(Z_POSITION);
		else if ( !strcmp(channel_type,"Xrotation") )
			channels.push_back(X_ROTATION);
		else if ( !strcmp(channel_type,"Yrotation") )
			channels.push_back(Y_ROTATION);
		else if ( !strcmp(channel_type,"Zrotation") )
			channels.push_back(Z_ROTATION);
		else {
			fprintf(stderr,"Error parsing channels.\n");
			exit(-1);
		}
	}
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

void Joint::print(FILE *out_fp,const char *offset ) {

	char next_offset[15] = "\t\0";

	strcat(next_offset,offset);

	if ( is_root ) {
		fprintf(out_fp,"ROOT %s\n",this->get_name());
	} else if  ( is_end ) {
		fprintf(out_fp,"%sEnd Site\n",offset);
	} else {
		fprintf(out_fp,"%sJOINT %s\n",offset,this->get_name());
	}
	fprintf(out_fp,"%s{\n",offset);

	fprintf(out_fp,"%sOFFSET %.5lf %.5lf %.5lf\n",next_offset,o.x,o.y,o.z);

	if ( channels.size() > 0 ) {

		fprintf(out_fp,"%sCHANNELS %lu",next_offset,channels.size());
		vector<ChannelType>::iterator it;

		for(it=channels.begin(); it != channels.end(); it++ ) {

			switch(*it) {

				case X_POSITION:
					fprintf(out_fp," Xposition");
					break;
				case Y_POSITION:
					fprintf(out_fp," Yposition");
					break;
				case Z_POSITION:
					fprintf(out_fp," Zposition");
					break;
				case X_ROTATION:
					fprintf(out_fp," Xrotation");
					break;
				case Y_ROTATION:
					fprintf(out_fp," Yrotation");
					break;
				case Z_ROTATION:
					fprintf(out_fp," Zrotation");
					break;
			}
		}
		fprintf(out_fp,"\n");

	}

	vector<Joint>::iterator itj;

	for(itj=subjoints.begin(); itj !=subjoints.end() ; itj++ ) {
		itj->print(out_fp,next_offset);
	}
	fprintf(out_fp,"%s}\n",offset);

}
