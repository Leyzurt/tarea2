#include "Points.h"

points::points(GLfloat x, GLfloat y, GLfloat r, GLfloat g, GLfloat b, bool select) {
	this->x = x;
	this->y = y;
	this->r = r;
	this->g = g;
	this->b = b;
	this->selected = select;
}

void points::setPoints(points *p)
{
	this->x = p->x;
	this->y = p->y;
	this->r = p->r;
	this->g = p->g;
	this->b = p->b;
	this->selected = p->selected;
}
