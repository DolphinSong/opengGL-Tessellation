#ifndef INCLUDED
#define INCLUDED

#include <glm/glm.hpp>

class MathHelper
{
public:
	MathHelper(void);
	//Returns a translated matrix
	glm::mat4 translate(glm::vec3 trans);
	
	//Returns a rotated matrix
	glm::mat4 rotate(float* angles, bool* axis);
	
	//Returns a perspective matrix
	glm::mat4 setPerspective(float fovy, float aspect, float zNear, float zFar);

	//Returns a view matrix, set the camera
	glm::mat4 setCamera(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 camup0);

	
	
};

#endif