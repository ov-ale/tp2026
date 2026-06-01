#include "composite_shape.h"
#include "point.h"
#include "rectangle.h"
#include "ring.h"
#include "shape.h"
#include "square.h"
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

void printShapes(const std::vector<std::unique_ptr<Shape>>& shapes)
{
  std::cout << std::fixed << std::setprecision(2);
  for (const auto& shape : shapes)
  {
    Point c = shape->getCenter();
    double a = shape->getArea();
    std::cout << "[" << shape->getName()
    << ", (" << c.x_ << ", " << c.y_ << "), "
    << a << "]";

    if (auto* comp = dynamic_cast<CompositeShape*>(shape.get()))
    {
      std::cout << ": ";
      comp->printContents(std::cout);
    }
    std::cout << "\n";
  }
}

int main()
{
  std::vector<std::unique_ptr<Shape>> shapes;

  shapes.emplace_back(std::make_unique<Rectangle>(Point(0.0, 0.0), Point(4.0, 3.0)));
  shapes.emplace_back(std::make_unique<Ring>(Point(1.0, 1.0), 3.0, 1.0));
  shapes.emplace_back(std::make_unique<Square>(Point(-1.0, -1.0), 2.0));

  auto composite = std::make_unique<CompositeShape>();
  composite->add(std::make_unique<Rectangle>(Point(5.0, 5.0), Point(7.0, 6.0)));
  composite->add(std::make_unique<Square>(Point(6.0, 5.5), 1.0));
  shapes.push_back(std::move(composite));

  shapes.emplace_back(std::make_unique<Square>(Point(10.0, 0.0), 1.0));

  std::cout << "Before scaling:\n";
  printShapes(shapes);

  std::cout << "\nScaling by 2\n";
  for (auto& shape : shapes)
  {
    shape->scale(2.0);
  }

  std::cout << "\nAfter scaling:\n";
  printShapes(shapes);

  return 0;
}
