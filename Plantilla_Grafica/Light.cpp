#include "Light.h"

Light::Light(){
	this->position = vec3(0.0f, 0.0f, 0.0f);
	this->ambient = vec3(0.6f, 0.1f, 0.3f);
	this->diffuse = vec3(0.2f, 0.6f, 1.0f);
	this->specular = vec3(0.3f, 0.5f, 0.0f);
	this->dir = vec3(0.0f, 0.0f, 0.0f);
}

Light::Light(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular, vec3 dir){
	this->position = position;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->dir = dir;
}

Light::~Light(){

}

vec3 Light::getPosition(){
	return this->position;
}

vec3 Light::getAmbient(){
	return this->ambient;
}

vec3 Light::getDiffuse(){
	return this->diffuse;
}

vec3 Light::getSpecular(){
	return this->specular;
}

vec3 Light::getDir()
{
	return this->dir;
}

void Light::setPosition(vec3 position){
	this->position = position;
}

void Light::set(vec3 ambient, vec3 diffuse, vec3 specular){
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

void Light::setAmbient(vec3 ambient){
	this->ambient = ambient;
}

void Light::setDiffuse(vec3 diffuse){
	this->diffuse = diffuse;
}

void Light::setSpecular(vec3 specular){
	this->specular = specular;
}

void Light::addPosition(vec3 position){
	this->position += position;
}

void Light::setDir(vec3 dir) {
	this->dir = dir;
}
