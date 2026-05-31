#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>
#include <string>
#include <iostream>
#include "polygon.h"

void doArea(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out);
void doMax(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out);
void doMin(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out);
void doCount(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out);

void doInFrame(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out);
void doRightShapes(const std::vector<Polygon>& polygons, std::ostream& out);

void printError(std::ostream& out);

#endif
