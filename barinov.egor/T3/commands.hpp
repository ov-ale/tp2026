#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "polygon.hpp"
#include <vector>
#include <string>

bool isEven(int n);
bool isOdd(int n);

double getAreaEven(const std::vector<Polygon>& polygons);
double getAreaOdd(const std::vector<Polygon>& polygons);
double getAreaMean(const std::vector<Polygon>& polygons);
double getAreaByVertexCount(const std::vector<Polygon>& polygons, size_t vertexCount);

double getMaxArea(const std::vector<Polygon>& polygons);
size_t getMaxVertexes(const std::vector<Polygon>& polygons);
double getMinArea(const std::vector<Polygon>& polygons);
size_t getMinVertexes(const std::vector<Polygon>& polygons);

size_t countEven(const std::vector<Polygon>& polygons);
size_t countOdd(const std::vector<Polygon>& polygons);
size_t countByVertexCount(const std::vector<Polygon>& polygons, size_t vertexCount);

size_t echoCommand(std::vector<Polygon>& polygons, const Polygon& target);
bool inframeCommand(const std::vector<Polygon>& polygons, const Polygon& target);

#endif