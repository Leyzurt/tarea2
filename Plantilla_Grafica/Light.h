#pragma once
#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "../include/glm/glm/glm.hpp"

using glm::vec3;

class Light {
private:
	vec3 position;	/*Position of the light*/
	vec3 ambient;	/*Values of the refraction of the light*/
	vec3 diffuse;
	vec3 specular;
	vec3 dir;
public:
	Light();
	Light(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular, vec3 dir);
	~Light();

	/*Getters*/
	vec3 getPosition();
	vec3 getAmbient();
	vec3 getDiffuse();
	vec3 getSpecular();
	vec3 getDir();

	/*Setters*/
	void setPosition(vec3 position);
	void set(vec3 ambient, vec3 diffuse, vec3 specular);
	void setAmbient(vec3 ambient);
	void setDiffuse(vec3 diffuse);
	void setSpecular(vec3 specular);
	void setDir(vec3 dir);

	/*Adders*/
	void addPosition(vec3 position);
};

#endif