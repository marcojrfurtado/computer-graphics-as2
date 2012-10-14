#ifndef JOINT_H
#define JOINT_H


#include <vector>
#include <string>
#include <cstdio>



class Joint {

	public:
		Joint(bool root = false, bool end = false) {
			is_root=root;
			is_end=end;
			this->o.x = this->o.y = this->o.z = 0;
		}

		// Reads a joint from the FILE fp.
		// As joint descriptions are recursive, this is used to process the joint-to-joint connections and offsets for
		// the sampled motion data. 
		//Starts reading after a joint keyword ( ROOT, JOINT or End)
		// FILE fp must have previously opened.
		// In case of failure in the parsing process, returns false.
		bool process(FILE *fp);

		// class setters
		// Sets the name of this joint
		void set_name(const char *new_name) { name = new_name;  }
		void set_offset(const double x, const double y, const double z) { 
			this->o.x = x;
			this->o.y = y;
			this->o.z = z;
		}

		const char * get_name() { return name.c_str(); }

		// Writes the joint, and its hierachy, to the file *out_fp
		void print(FILE *out_fp,const char *offset = "\0" );

		void pretty_print(const char *offset);

	private:
		// OFFSET structure
		struct offset {
			double x, y , z;
		};
		offset o;

		// CHANNEL TYPES
		typedef enum {
			X_POSITION = 0,
			Y_POSITION,
			Z_POSITION,
			X_ROTATION,
			Y_ROTATION,
			Z_ROTATION
		} ChannelType;

		// ChannelVector
		std::vector<ChannelType> channels;

		// Read channels description from the file
		void read_channels(FILE *fp);

		
		std::vector<Joint> subjoints;
		std::string name;

		bool is_root;
		bool is_end;


};




#endif
