#include "polygon.hpp"
#include "commands.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <limits>

bool readPolygonsFromFile(const std::string& filename, std::vector<Polygon>& polygons)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    return false;
  }
  std::string line;
  while (std::getline(file, line))
  {
    if (line.empty())
    {
      continue;
    }
    std::istringstream iss(line);
    Polygon polygon;
    if (iss >> polygon)
    {
      polygons.push_back(polygon);
    }
  }
  return true;
}

bool parsePolygon(const std::vector<std::string>& tokens, size_t start, Polygon& polygon)
{
  if (start >= tokens.size())
  {
    return false;
  }
  size_t vertexCount = 0;
  std::istringstream countStream(tokens[start]);
  countStream >> vertexCount;
  if (vertexCount < 3 || start + 1 + vertexCount > tokens.size())
  {
    return false;
  }
  polygon.points.clear();
  for (size_t i = 0; i < vertexCount; ++i)
  {
    Point p;
    std::istringstream pointStream(tokens[start + 1 + i]);
    if (!(pointStream >> p))
    {
      return false;
    }
    polygon.points.push_back(p);
  }
  return true;
}

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }
  std::vector<Polygon> polygons;
  if (!readPolygonsFromFile(argv[1], polygons))
  {
    std::cerr << "Error: cannot read file" << std::endl;
    return 1;
  }
  std::cout << std::fixed << std::setprecision(1);
  std::string line;
  while (std::getline(std::cin, line))
  {
    if (line.empty())
    {
      continue;
    }
    std::istringstream iss(line);
    std::string command;
    iss >> command;
    if (command == "AREA")
    {
      std::string param;
      iss >> param;
      if (param == "EVEN")
      {
        std::cout << getAreaEven(polygons) << std::endl;
      }
      else if (param == "ODD")
      {
        std::cout << getAreaOdd(polygons) << std::endl;
      }
      else if (param == "MEAN")
      {
        if (polygons.empty())
        {
          std::cout << "<INVALID COMMAND>" << std::endl;
        }
        else
        {
          std::cout << getAreaMean(polygons) << std::endl;
        }
      }
      else
      {
        size_t vertexCount = 0;
        std::istringstream countStream(param);
        countStream >> vertexCount;
        if (vertexCount >= 3)
        {
          std::cout << getAreaByVertexCount(polygons, vertexCount) << std::endl;
        }
        else
        {
          std::cout << "<INVALID COMMAND>" << std::endl;
        }
      }
    }
    else if (command == "MAX")
    {
      std::string param;
      iss >> param;
      if (param == "AREA")
      {
        if (polygons.empty())
        {
          std::cout << "<INVALID COMMAND>" << std::endl;
        }
        else
        {
          std::cout << getMaxArea(polygons) << std::endl;
        }
      }
      else if (param == "VERTEXES")
      {
        if (polygons.empty())
        {
          std::cout << "<INVALID COMMAND>" << std::endl;
        }
        else
        {
          std::cout << getMaxVertexes(polygons) << std::endl;
        }
      }
      else
      {
        std::cout << "<INVALID COMMAND>" << std::endl;
      }
    }
    else if (command == "MIN")
    {
      std::string param;
      iss >> param;
      if (param == "AREA")
      {
        if (polygons.empty())
        {
          std::cout << "<INVALID COMMAND>" << std::endl;
        }
        else
        {
          std::cout << getMinArea(polygons) << std::endl;
        }
      }
      else if (param == "VERTEXES")
      {
        if (polygons.empty())
        {
          std::cout << "<INVALID COMMAND>" << std::endl;
        }
        else
        {
          std::cout << getMinVertexes(polygons) << std::endl;
        }
      }
      else
      {
        std::cout << "<INVALID COMMAND>" << std::endl;
      }
    }
    else if (command == "COUNT")
    {
      std::string param;
      iss >> param;
      if (param == "EVEN")
      {
        std::cout << countEven(polygons) << std::endl;
      }
      else if (param == "ODD")
      {
        std::cout << countOdd(polygons) << std::endl;
      }
      else
      {
        size_t vertexCount = 0;
        std::istringstream countStream(param);
        countStream >> vertexCount;
        if (vertexCount >= 3)
        {
          std::cout << countByVertexCount(polygons, vertexCount) << std::endl;
        }
        else
        {
          std::cout << "<INVALID COMMAND>" << std::endl;
        }
      }
    }
    else if (command == "ECHO")
    {
      std::vector<std::string> tokens;
      std::string token;
      std::istringstream tokenStream(line);
      while (tokenStream >> token)
      {
        tokens.push_back(token);
      }
      if (tokens.size() < 2)
      {
        std::cout << "<INVALID COMMAND>" << std::endl;
        continue;
      }
      Polygon target;
      if (!parsePolygon(tokens, 1, target))
      {
        std::cout << "<INVALID COMMAND>" << std::endl;
        continue;
      }
      size_t added = echoCommand(polygons, target);
      std::cout << added << std::endl;
    }
    else if (command == "INFRAME")
    {
      std::vector<std::string> tokens;
      std::string token;
      std::istringstream tokenStream(line);
      while (tokenStream >> token)
      {
        tokens.push_back(token);
      }
      if (tokens.size() < 2)
      {
        std::cout << "<INVALID COMMAND>" << std::endl;
        continue;
      }
      Polygon target;
      if (!parsePolygon(tokens, 1, target))
      {
        std::cout << "<INVALID COMMAND>" << std::endl;
        continue;
      }
      bool result = inframeCommand(polygons, target);
      std::cout << (result ? "<TRUE>" : "<FALSE>") << std::endl;
    }
    else
    {
      std::cout << "<INVALID COMMAND>" << std::endl;
    }
  }
  return 0;
}