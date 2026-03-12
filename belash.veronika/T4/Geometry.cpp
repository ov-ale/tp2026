#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include "Shape.hpp"
#include "Composite_shape.hpp"
#include "Ellipse.hpp"
#include "Rectangle.hpp"
#include "Right_trapezoid.hpp"

void printInfo(const std::vector<std::unique_ptr<Shape>>& shapes) {
    for (size_t i = 0; i < shapes.size(); ++i) {
        const std::unique_ptr<Shape>& shape = shapes[i];
        Point center = shape->getCenter();

        // Формат: [NAME,(x.xx, y.xx), area] - БЕЗ лишних пробелов!
        std::cout << "[" << shape->getName() << ",("
            << std::fixed << std::setprecision(2) << center.x << ", "
            << center.y << "), "
            << std::setprecision(2) << shape->getArea() << "]";

        if (shape->getName() == "COMPOSITE") {
            const CompositeShape* composite = dynamic_cast<const CompositeShape*>(shape.get());
            if (composite) {
                std::cout << ":";  // Двоеточие без пробела
                const std::vector<std::unique_ptr<Shape>>& subShapes = composite->getShapes();
                for (size_t j = 0; j < subShapes.size(); ++j) {
                    const std::unique_ptr<Shape>& subShape = subShapes[j];
                    Point subCenter = subShape->getCenter();
                    std::cout << "\n  [" << subShape->getName() << ",("
                        << std::fixed << std::setprecision(2) << subCenter.x << ", "
                        << subCenter.y << "), "
                        << std::setprecision(2) << subShape->getArea() << "]";
                }
            }
        }
        std::cout << '\n';
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::unique_ptr<Shape>> shapes;

    shapes.push_back(std::make_unique<Rectangle>(Point{ 0, 0 }, Point{ 2, 3 }));
    shapes.push_back(std::make_unique<Ellipse>(Point{ 5, 5 }, 2.0, 1.5));
    shapes.push_back(std::make_unique<RightTrapezoid>(Point{ 1, 1 }, 4.0, 2.0, 2.0));
    shapes.push_back(std::make_unique<Rectangle>(Point{ 3, 2 }, Point{ 5, 4 }));
    shapes.push_back(std::make_unique<Ellipse>(Point{ -2, -1 }, 1.0, 0.5));

    std::unique_ptr<CompositeShape> composite = std::make_unique<CompositeShape>();
    composite->addShape(std::make_unique<Rectangle>(Point{ 1, 2 }, Point{ 3, 5 }));
    composite->addShape(std::make_unique<Ellipse>(Point{ 4, 3 }, 1.0, 2.0));
    shapes.push_back(std::move(composite));

    bool fullMode = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--full") {
            fullMode = true;
            break;
        }
    }

    printInfo(shapes);

    if (!fullMode) {
        std::cerr << "Error: No input provided" << std::endl;
        return 1;
    }
    std::cout << "\nAFTER SCALING:\n";
    for (size_t i = 0; i < shapes.size(); ++i) {
        shapes[i]->scale(2.0);
    }
    printInfo(shapes);

    return 0;
}
