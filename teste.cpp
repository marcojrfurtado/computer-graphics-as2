#include <cstdio>
#include "quaternion.hpp"


int main() {

	Quaternion id(1.0,0.0,0.0,0.0) , q1(-0.9,0.54,1.87,0.5);

	Quaternion t1;
	t1 = id * q1;
	printf("q1(%lf, %lf, %lf,%lf), *(%lf,%lf,%lf,%lf)\n",q1.w,q1.u.x,q1.u.y,q1.u.z,t1.w,t1.u.x,t1.u.y,t1.u.z);

	return 0;
}
