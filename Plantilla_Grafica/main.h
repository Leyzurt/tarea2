#pragma once

#include "GLSLProgram.h"
#include "../include/glm/glm/glm.hpp"
#include "../include/glm/glm/gtx/quaternion.hpp"
#include "../include/glm/glm/gtc/matrix_transform.hpp"
#include "../include/glm/glm/gtc/type_ptr.hpp"
#include "../include/glfw3.h"
#include "../include/GL/freeglut.h"
#include "../include/AntTweakBar.h"
#include <time.h>
#include "TransferFunc.h"
/*#include <ft2build.h>
#include FT_FREETYPE_H  */
#include "Camera.h"
#include "VolumeRender.h"
#include "Texture.h"

Transfer *transfer_func;
GLuint texture_depth;
GLFWwindow* gWindow;
GLuint g_frameBuffer;
GLuint texture_coord;
int gWidth = 1400;
int gHeight = 900;
glm::mat4x4 mProjMatrix, mModelViewMatrix;
glm::mat4 depthBiasMVP;
CGLSLProgram *glslProgramVolume, *glslProgramBackFace, *glslProgramTexture;
//CGLSLProgram *glslProgramTexture, *glslProgramTexture_vertex, *glslProgramSkybox, *glslProgramShadow, *texture_program;
using namespace std;
using std::vector;
GLuint glVBO_dir;
vector<GLfloat> glVBO;
GLuint glVBO_indexes_dir;
vector<GLuint> glVBO_indexes;
vector<GLint> *ID = new vector<GLint>(20, 0);