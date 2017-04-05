#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "Points.h"

using namespace std;
class Transfer {
public:
	vector<points*> puntos;
	GLuint texture_id;
	GLfloat *data;
	Transfer();
	void Interpolation();
	void OrderPoints();
	GLuint BindTexture();

};