#pragma once
#include "../include/glew/glew.h"
#include "../include/glm/glm/glm.hpp"
#include "../include/glm/glm/gtc/matrix_transform.hpp"
#include "../include/glm/glm/gtc/type_ptr.hpp"
class points {

public:
	GLfloat x, y, r, g, b;
	bool selected;
	points(GLfloat x, GLfloat y, GLfloat r, GLfloat g, GLfloat b, bool select);
	void setPoints(points *p);
};