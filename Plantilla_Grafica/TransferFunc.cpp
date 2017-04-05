#include "TransferFunc.h"


Transfer::Transfer() {

	//Puntos de control:
	//Vec2 que contiene en .x el vec2 de x,y y en .y el vec3 de rgb
	this->puntos.push_back(new points(0.0f, 0.0f, 0.0f, 0.0f, 0.0f,false));
	this->puntos.push_back(new points(1.0f, 1.0f, 1.0f, 1.0f, 1.0f,false));
	//this->puntos.push_back(new points(0.5f, 0.5f, 0.0f, 1.0f, 0.0f, false));
	glGenTextures(1, &this->texture_id);
	glBindTexture(GL_TEXTURE_1D, this->texture_id);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, 256, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_1D, 0);

	this->data = new GLfloat[256 * 4];
	this->OrderPoints();
	this->Interpolation();
}

void Transfer::Interpolation()
{
	int length = 256;
	double displacement = double(1) / double(length);
	int partial_length;
	unsigned int step, step_acum = 0, index = 0;
	double r_multy, g_multy, b_multy, a_multy;
	for (int i = 0; i < this->puntos.size()-1; i++)
	{
		partial_length = puntos[i + 1]->x - puntos[i]->x;
		step = unsigned int(length * partial_length);
		step_acum += step;
		if (i == puntos.size() - 2) 
			step += length - step_acum;
		
		r_multy = puntos[i]->r + (puntos[i + 1]->r - puntos[i]->r) / (puntos[i + 1]->x - puntos[i]->x);
		g_multy = puntos[i]->g + (puntos[i + 1]->g - puntos[i]->g) / (puntos[i + 1]->x - puntos[i]->x);
		b_multy = puntos[i]->b + (puntos[i + 1]->b - puntos[i]->b) / (puntos[i + 1]->x - puntos[i]->x);
		a_multy = puntos[i]->y + (puntos[i + 1]->y - puntos[i]->y) / (puntos[i + 1]->x - puntos[i]->x);

		for (unsigned int j = 0;  j < step && index < 256 ; j++, index++)
		{
			
			data[index * 4 + 0] = float(r_multy * (double(index)*displacement - puntos[i]->x));
			data[index * 4 + 1] = float(g_multy * (double(index)*displacement - puntos[i]->x));
			data[index * 4 + 2] = float(b_multy * (double(index)*displacement - puntos[i]->x));
			data[index * 4 + 3] = float(a_multy * (double(index)*displacement - puntos[i]->x));
		}
	}

	/*for (unsigned int i = 0; i < length; i++) {
	cout << "R :: " << data[i * 4 + 0];
	cout << " G :: " << data[i * 4 + 1];
	cout << " B :: " << data[i * 4 + 2];
	cout << " A :: " << data[i * 4 + 3] << endl;
	}
	system("pause");*/

}

void Transfer::OrderPoints()
{
	for (int i = 0; i < puntos.size(); i++)
	{
		for (int j = 0; j < puntos.size() - 1 ; j++)
		{
			if (puntos[j]->x > puntos[j + 1]->x) {
				points* p = new points(puntos[j]->x, puntos[j]->y, puntos[j]->r, puntos[j]->g, puntos[j]->b, puntos[j]->selected);
				puntos[j]->setPoints(puntos[j + 1]);
				puntos[j + 1]->setPoints(p);
			}
		}
	}
}

GLuint Transfer::BindTexture()
{
	glBindTexture(GL_TEXTURE_1D, this->texture_id);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, 256, 0, GL_RGBA, GL_FLOAT, data);
	return this->texture_id;
}
