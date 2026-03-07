#include "ellipse.h"

Ellipse::Ellipse(const Point& cen, double rh, double rv) : centerE(cen), radiusHor(rh), radiusVer(rv) {}

double Ellipse::getArea() const
{
	return 3.1415 * radiusHor * radiusVer;
}

std::string Ellipse::getName() const
{
	return "ELLIPSE";
}

Point Ellipse::getCenter() const
{
	return centerE;
}

void Ellipse::move(double dx, double dy)
{
	centerE.x += dx;
	centerE.y += dy;
}

void Ellipse::scale(double factor)
{
	radiusVer = radiusVer * factor;
	radiusHor = radiusHor * factor;
}

void Ellipse::print(std::ostream& os) const
{
	os << std::fixed << std::setprecision(2);
	os << "[" << getName() << ", " << getCenter() << ", " << getArea() << "]";
}