#ifndef MOTION_H
 #define MOTION_H

#include <vector>
#include <cstdio>


class Motion {
	public:
		// Type definition
		typedef std::vector< double > frame_data;
		
		
		Motion() {
			frames= 0;
			frametime= 0;
			frame_data_size=0;
		}
		// Reas the motion data from FILE fp
		bool process(FILE *fp);

		void set_frame_data_size(const int n) { this->frame_data_size = n; }

		// Returns the frame rate in milliseconds
		int get_frame_rate() { return ( int ) frametime*1000; }

		void set_frame_rate( int time_ms ) {  frametime = time_ms / 1000;  }

		const std::vector< frame_data > & get_frame_set() { return frame_set; }

		// Print motion information to FILE out_fp
		void print(FILE *out_fp);

	private:
		

		int frames;

		// frame time in BVH format
		double frametime;
		int frame_data_size;

		std::vector< frame_data > frame_set;
		
};

#endif
