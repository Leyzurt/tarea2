#pragma once

#include "../include/glew/glew.h"
#include "../include/glm/glm/glm.hpp"
#include "../include/glm/glm/gtc/matrix_transform.hpp"
#include "../include/glm/glm/gtc/type_ptr.hpp"
#include "../include/assimp/Importer.hpp"
#include "../include/assimp/scene.h"
#include "../include/assimp/postprocess.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "Texture.h"
#include "Transformation.h"
#include "Light.h"

using glm::vec3;
using std::string;
using std::vector;


class Model
{
protected:
	/*Array to VBO*/
	vector<GLfloat> glVBO; /*v1x v1y v1z vn1x vn1y vn1z vt1 vt2 vt3*/
	GLuint glVBO_dir;
	GLuint glVBO_indexes_dir;
	vector<GLuint> glVBO_indexes;
	GLuint glVBO_indexes_size;
	/*Arrays to load a model*/
	vector<GLfloat*> vertexes;
	vector<GLfloat*> normal;
	vector<GLfloat*> texture_;
	int vertexesLenght;
	/*Structures*/
	
	//BoundingBox* boundingBox;
	Texture* texture;
	/*Light values*/
	bool texturize = false;
	float shininess;
	float roughness;
	float fresnel;
	float albedo;
	float ratioRefract;
	float type_interpol, one_nayar, cook_torrance, texture_active, phong_active;
	string ruta_textura;
	/*Actives*/
	float specular;
	float difuse;
	float activeRefract;
	float activeReflect;
	GLuint textureCubeID;
	/*Uniforms IDs*/
	vector<GLint> *ID;
private:
	int index;
public:
	Texture* texture_normal;
	string ruta_textura_normal;
	int size_tangents;
	Light* material; /*Values of the refraction of the light*/
	Transformation* transformation;
	glm::vec4 min, max;
	glm::vec4 min_aux, max_aux;
	Model(string rutaTextura, string rutaTextura_normal);
	void Leer(string routes);
	void Normalize_vec();
	void agregarFace(vector<string> Faces);
	void Max_min();
	void render(GLuint shader_id);

	/*Getters*/
	float getActiveReflect();
	GLuint getTextureCube();
	glm::vec4 Model::getCenter();
	glm::vec4 Model::getCenter_2();
	vector<GLfloat> getGLVBO();
	Transformation* getTransformation();
	Texture* getTexture();
	GLuint getGLVBO_dir();
	GLuint getGLVBO_indexes_dir();
	vector<GLuint> getGLVBO_indexes();
	GLuint getGLVBO_indexes_size();
	float getShininess();
	void setTransformation(Transformation* transformation);
	void setTexture(Texture* texture);
	float getActiveRefract();
	float getRatioRefract();
	float getRoughness();
	float getFresnel();
	float getAlbedo();
	/*Functions*/
	
	void setActiveReflect(float reflect);
	void setTextureCube(GLuint textureID);
	void initGLDataBinding();
	void processNode(aiNode * node, const aiScene * scene);	
	void setParameters(float matShininess, float roughness, float fresnel, float albedo);
	void setSettings(float type_interpol, float one_nayar, float cook_torrance, float texture_active, float phong_active);
	void setSettings(float specular, float difuse, float texture_active);
	void setMaterial(float matAmbientReflectances[3], float matDiffuseReflectances[3], float matSpecularReflectances[3]);
	void setActiveRefract(float active);
	void setRatioRefract(float refract);

	void render_shadow(GLuint shader_id);

};
