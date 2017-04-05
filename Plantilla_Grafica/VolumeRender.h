#pragma once
#include "../include/glew/glew.h"
#include "../include/glm/glm/glm.hpp"
#include "../include/glm/glm/gtc/matrix_transform.hpp"
#include "../include/glm/glm/gtc/type_ptr.hpp"
#include "Transformation.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;
using glm::mat4;
using glm::vec3;
class Volume {
private:
	GLuint g_vao;
	GLuint g_volTexObj;
	GLint g_angle = 0;		
public:
	GLuint ttf1DTex;
	GLuint g_frameBuffer;
	Transformation *Transform;
	GLuint backFace2DTex;
	Volume();
	void initVBO();
	void initVol3DTex(const char* filename, GLuint width, GLuint height, GLuint depth);
	void initFace2DTex(GLuint texWidth, GLuint texHeight);
	void Render(GLenum glFaces, GLuint shader_id);
	void initTFF1DTex(const char* filename);
	void initFrameBuffer(GLuint texWidth, GLuint texHeight);
};