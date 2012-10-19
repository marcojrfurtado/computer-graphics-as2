#ifndef QUATERNION_H
 #define QUATERNION_H

#include <glm/glm.hpp>

class Quaternion{
public:
	double w;
	glm::vec3 u;

	inline void Multiply(const Quaternion q)
		{
			Quaternion tmp;
			tmp.u.x = ((w * q.u.x) + (u.x * q.w) + (u.y * q.u.z) - (u.z * q.u.y));
			tmp.u.y = ((w * q.u.y) - (u.x * q.u.z) + (u.y * q.w) + (u.z * q.u.x));
			tmp.u.z = ((w * q.u.z) + (u.x * q.u.y) - (u.y * q.u.x) + (u.z * q.w));
			tmp.w = ((w * q.w) - (u.x * q.u.x) - (u.y * q.u.y) - (u.z * q.u.z));
			*this = tmp;
		}

	inline double Norm()
	{return sqrt(u.x*u.x+u.y*u.y+u.z*u.z+w*w);}


	inline void Normalize()
	{
	double norm=Norm();
	u.x/=norm;u.y/=norm;u.z/=norm;
	}
}

#endif
