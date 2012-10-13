#ifndef JOINT_H
#define JOINT_H


#include <vector>
#include <string>
#include <cstdio>



class Joint {

	public:
		Joint(bool root = false) {
			is_root=root;
		}

		// Reads a joint from the FILE fp.
		// As joint descriptions are recursive, this is used to process the joint-to-joint connections and offsets for
		// the sampled motion data. 
		//Starts reading after a joint keyword ( ROOT, JOINT or End)
		// FILE fp must have previously opened.
		// In case of failure in the parsing process, returns false.
		bool process(FILE *fp);

		// Sets the name of this joint
		void set_name(const char *new_name) { name = new_name;  }

		const char * get_name() { return name.c_str(); }

		void pretty_print(const char *offset);

	private:
		std::vector<Joint> subjoints;
		std::string name;

		bool is_root;


};




#endif
