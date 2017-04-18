#include "MathHelper.h"

using glm::mat4;

MathHelper::MathHelper(void)
{
}

mat4 MathHelper::translate(glm::vec3 trans){
	mat4 result;
	result[3][0] = trans.x;
	result[3][1] = trans.y;
	result[3][2] = trans.z;
	return result;
}

mat4 MathHelper::rotate(float* angle, bool* axis){
	mat4 rotationX = mat4(1.0f);
	mat4 rotationY = mat4(1.0f);
	mat4 rotationZ = mat4(1.0f);
	float radian[]={angle[0]*3.14159265f/180.0f,angle[1]*3.14159265f/180.0f,angle[2]*3.14159265f/180.0f};

	if(axis[0] == true){
		rotationX[1][1] = rotationX[2][2] = glm::cos(radian[0]);
		rotationX[1][2] = glm::sin(radian[0]);
		rotationX[2][1] = -rotationX[1][2];
	}
	
	if(axis[1] == true){
		rotationY[0][0] = rotationY[2][2] = glm::cos(radian[1]);
		rotationY[2][0] = glm::sin(radian[1]);
		rotationY[0][2] = -rotationY[2][0];
	}
	
	if(axis[2] ==true){
		rotationZ[0][0] = rotationZ[1][1] = glm::cos(radian[2]);
		rotationZ[0][1] = glm::sin(radian[2]);
		rotationZ[1][0] = -rotationZ[0][1];
	}
	
	mat4 result = rotationZ*rotationY*rotationX;
	return result;
}
//the function to build the perspective projection matrix( got the idea from the gluPerspective() function lol!!)
/*fovy Specifies the field of view angle, in degrees, in the y direction.
aspect Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
zNear Specifies the distance from the viewer to the near clipping plane (always positive).
zFar Specifies the distance from the viewer to the far clipping plane (always positive).*/
mat4 MathHelper::setPerspective(float fovy, float aspect, float zNear, float zFar){
	mat4 result;
	fovy=fovy*3.14159265f/360.0f;
	float f = 1.0f/(glm::tan(fovy));
	result[0][0] = f/aspect;
	result[1][1] = f;
	result[2][2] = ((zFar+zNear)/(zNear-zFar));
	result[2][3] = -1.0;
	result[3][2] = ((2.0f*zFar*zNear)/(zNear-zFar));
	result[3][3] = 0.0;
	
	return result;
}

mat4 MathHelper::setCamera(glm::vec3 camPos, glm::vec3 lookAt, glm::vec3 camup0){
	mat4 result = mat4(1.0f);
	glm::vec3 camForward = (lookAt - camPos)/glm::length(lookAt - camPos);
	glm::vec3 camRight = glm::cross(camForward, camup0);
	glm::vec3 camUp = glm::cross(camRight, camForward);
	
	mat4 viewRotation = mat4(glm::vec4(camRight.x, camUp.x, -camForward.x, 0.0f),
							glm::vec4(camRight.y, camUp.y, -camForward.y, 0.0f),
							glm::vec4(camRight.z, camUp.z, -camForward.z, 0.0f),
							glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	
	mat4 viewTranslation = mat4(1.0f);
	viewTranslation[3][0] = -camPos.x;
	viewTranslation[3][1] = -camPos.y;
	viewTranslation[3][2] = -camPos.z;
	
	result = viewRotation*viewTranslation;
	return result;
}
