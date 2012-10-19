#include "joint.hpp"
#include <numeric>
#include <cstring>
#include <cstdlib>

#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

// Used for our spherical linear interpolation
#include "quaternion.hpp"

using namespace std;

bool Joint::process(FILE *fp) {	

	char next_str[100];

	// Sequential id used for the joints to identify their unique channel offset
	static int seq_id = 0;
	
	// Read identification for this joint
	fscanf(fp," %s {",next_str);


	//printf("%s\n",next_str);

	set_name(next_str);

	// Compute the channel offset here
	if ( this->parent && !is_end )  {
		this->channel_offset += 6 + (  3 * seq_id++ );
	}

	
	while ( fscanf(fp," %s",next_str) != EOF ) {

		if ( !strcmp(next_str,"JOINT" )  ) {
			Joint *child = new Joint(false,false,this);
			child->process(fp);
			this->subjoints.push_back(child);
		}	
		else if (  !strcmp(next_str,"End")  ) {
			Joint *child = new Joint(false,true,this);
			child->process(fp);
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

	return true;

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

	std::vector<Joint *>::iterator it;

	for( it = subjoints.begin(); it != subjoints.end(); it++ ) {
		(*it)->pretty_print(next_offset);
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

	fprintf(out_fp,"%schannel offset: %d\n",offset,this->channel_offset);

	fprintf(out_fp,"%sOFFSET %.5lf %.5lf %.5lf\n",next_offset,original.x,original.y,original.z);

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

	vector<Joint *>::iterator itj;

	for(itj=subjoints.begin(); itj !=subjoints.end() ; itj++ ) {
		(*itj)->print(out_fp,next_offset);
	}
	fprintf(out_fp,"%s}\n",offset);

}

int Joint::count_hierarchy_channels() {
	int channel_count = 0;

	std::vector<Joint *>::iterator it;

	for( it = subjoints.begin(); it != subjoints.end() ; it++ ) 
		channel_count+=(*it)->count_hierarchy_channels();

	channel_count+=channels.size();

	return channel_count;
}

void Joint::render_transformation( const Motion::frame_data & data ) {
	render_transformation(  data, data, 0 );
}

void Joint::render_transformation( const Motion::frame_data & data, const Motion::frame_data & data2 , const float lambda ) {

	if ( this->channels.size() == 0 )
		return;
	
	glPushMatrix();


	if ( this->is_root  ) {
		// Interpolate between data and data2
		// If lambda == 0, we display only data 
		glTranslatef(((1-lambda)*data[0] ) + (lambda*data2[0]) ,((1-lambda)*data[1]) + (lambda*data2[1]),((1-lambda)*data[2] + (lambda*data2[2])));

	} else {
		glTranslatef(this->o.x,this->o.y,this->o.z);
	}
	

#ifndef JOINT_DISABLE_ROTATIONS
	
	std::vector<ChannelType>::iterator it_channels;
	
	// Iterate on the channels
	int i = 0;
	for( it_channels = channels.begin() ; it_channels != channels.end(); it_channels++ ) {
		

		if ( *it_channels == X_ROTATION ) {
			glRotatef(data[this->channel_offset + i ],1.0f,0.0f,0.0f);
//			printf("rotating [%s.x]: %lf\n",get_name(),data[this->channel_offset + i]);
		}
		else if ( *it_channels == Y_ROTATION ) {
			glRotatef(data[this->channel_offset + i ],0.0f,1.0f,0.0f);
//			printf("rotating [%s.y]: %lf\n",get_name(),data[this->channel_offset + i]);
		}
		else if ( *it_channels == Z_ROTATION ) {
			glRotatef(data[this->channel_offset + i ],0.0f,0.0f,1.0f);
//			printf("rotating [%s.z]: %lf\n",get_name(),data[this->channel_offset + i]);
		}

		i++;
	}
#endif

	std::vector< Joint * >::iterator it_sub;

	glm::vec3 origin(0.0,0.0,0.0);
	if ( subjoints.size() == 1 ) {
		render_bone(origin, (*subjoints.begin())->get_offset() );
	} else {

		glm::vec3 center = get_center();

		render_bone(origin,center);


		for( it_sub = subjoints.begin() ; it_sub != subjoints.end() ; it_sub++ ) {
			render_bone( center,  (*it_sub)->get_offset() );
		}
	}
	

	for( it_sub = subjoints.begin() ; it_sub != subjoints.end() ; it_sub++ ) {
		(*it_sub)->render_transformation(data,data2,lambda);
	}
	
	glPopMatrix();

}

void Joint::render_bone(glm::vec3 p1, glm::vec3 p2) {
	
	glBegin(GL_LINES);
		glVertex3f(p1.x,p1.y,p1.z);
		glVertex3f(p2.x,p2.y,p2.z);
	glEnd();

//	RenderBone(p1.x,p1.y,p1.z,p2.x,p2.y,p2.z);




}

void Joint::restore() {

	o.x = original.x;
	o.y = original.y;
	o.z = original.z;

	std::vector<Joint *>::iterator it;
	for( it = subjoints.begin() ; it != subjoints.end(); it++ ) {
		(*it)->restore();
	}
}

glm::vec3 Joint::get_center() {

	glm::vec3 ret(0.0,0.0,0.0);

	std::vector<Joint *>::iterator it;
	for( it = subjoints.begin() ; it != subjoints.end() ; it++ ) {
		glm::vec3 next = (*it)->get_offset();
		ret.x+= next.x;
		ret.y+= next.y;
		ret.z+= next.z;
	}

	ret.x /= subjoints.size() + 1;
	ret.y /= subjoints.size() + 1;
	ret.z /= subjoints.size() + 1;

	return ret;
}

void Joint::slerp(Quaternion q1, Quaternion q2, Quaternion &qr , double lambda) 
{
	float dotproduct = q1.u.x * q2.u.x + q1.u.y * q2.u.y + q1.u.z * q2.u.z + q1.w * q2.w;
	float theta, st, sut, sout, coeff1, coeff2;

	// algorithm adapted from Shoemake's paper
 lambda=lambda/2.0;

	theta = (float) acos(dotproduct);
	if (theta<0.0) theta=-theta;
	
	st = (float) sin(theta);
	sut = (float) sin(lambda*theta);
	sout = (float) sin((1-lambda)*theta);
	coeff1 = sout/st;
	coeff2 = sut/st;

	qr.u.x = coeff1*q1.u.x + coeff2*q2.u.x;
	qr.u.y = coeff1*q1.u.y + coeff2*q2.u.y;
	qr.u.z = coeff1*q1.u.z + coeff2*q2.u.z;
	qr.w = coeff1*q1.w + coeff2*q2.w;

	qr.Normalize();
}


void  Joint::RenderBone( float x0, float y0, float z0, float x1, float y1, float z1 )
{
	// ^¦çê½Q_ðÔ~ð`æ

	// ~ÌQ[_Ìîñð´_Eü«E·³ÌîñÉÏ·
	GLdouble  dir_x = x1 - x0;
	GLdouble  dir_y = y1 - y0;
	GLdouble  dir_z = z1 - z0;
	GLdouble  bone_length = sqrt( dir_x*dir_x + dir_y*dir_y + dir_z*dir_z );

	// `æp^ÌÝè
	static GLUquadricObj *  quad_obj = NULL;
	if ( quad_obj == NULL )
		quad_obj = gluNewQuadric();
	gluQuadricDrawStyle( quad_obj, GLU_FILL );
	gluQuadricNormals( quad_obj, GLU_SMOOTH );

	glPushMatrix();

	// ½sÚ®ðÝè
	glTranslated( x0, y0, z0 );

	// ÈºA~Ìñ]ð\·sñðvZ

	// ²ðPÊxNgÉ³K»
	double  length;
	length = sqrt( dir_x*dir_x + dir_y*dir_y + dir_z*dir_z );
	if ( length < 0.0001 ) { 
		dir_x = 0.0; dir_y = 0.0; dir_z = 1.0;  length = 1.0;
	}
	dir_x /= length;  dir_y /= length;  dir_z /= length;

	// îÆ·é²Ìü«ðÝè
	GLdouble  up_x, up_y, up_z;
	up_x = 0.0;
	up_y = 1.0;
	up_z = 0.0;

	// ²Æ²ÌOÏ©ç²Ìü«ðvZ
	double  side_x, side_y, side_z;
	side_x = up_y * dir_z - up_z * dir_y;
	side_y = up_z * dir_x - up_x * dir_z;
	side_z = up_x * dir_y - up_y * dir_x;

	// ²ðPÊxNgÉ³K»
	length = sqrt( side_x*side_x + side_y*side_y + side_z*side_z );
	if ( length < 0.0001 ) {
		side_x = 1.0; side_y = 0.0; side_z = 0.0;  length = 1.0;
	}
	side_x /= length;  side_y /= length;  side_z /= length;

	// ²Æ²ÌOÏ©ç²Ìü«ðvZ
	up_x = dir_y * side_z - dir_z * side_y;
	up_y = dir_z * side_x - dir_x * side_z;
	up_z = dir_x * side_y - dir_y * side_x;

	// ñ]sñðÝè
	GLdouble  m[16] = { side_x, side_y, side_z, 0.0,
	                    up_x,   up_y,   up_z,   0.0,
	                    dir_x,  dir_y,  dir_z,  0.0,
	                    0.0,    0.0,    0.0,    1.0 };
	glMultMatrixd( m );

	// ~ÌÝè
	GLdouble radius= 0.01; // ~Ì¾³
	GLdouble slices = 8.0; // ~ÌúËóÌ×ªiftHg12j
	GLdouble stack = 3.0;  // ~ÌÖØèÌ×ªiftHgPj

	// ~ð`æ
	gluCylinder( quad_obj, radius, radius, bone_length, slices, stack ); 

	glPopMatrix();
}
