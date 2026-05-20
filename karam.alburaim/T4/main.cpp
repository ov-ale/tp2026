#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include "rectangle.h"
#include "ring.h"
#include "square.h"
#include "compositeShape.h"

int main() {
    try {
        std::cout << std::fixed << std::setprecision(2);
        std::vector<std::unique_ptr<Shape>> shapes;

  
        shapes.push_back(std::make_unique<Rectangle>(Point(0,0), Point(2,2)));
        shapes.push_back(std::make_unique<Square>(Point(5,5), 3));
        shapes.push_back(std::make_unique<Ring>(Point(10,10), 5, 2));
        shapes.push_back(std::make_unique<Square>(Point(-5,-5), 4));
        shapes.push_back(std::make_unique<Rectangle>(Point(1,1), Point(4,6)));

        auto comp = std::make_unique<CompositeShape>();
        
     
        comp->addShape(std::make_unique<Square>(Point(0,0), 2));
        comp->addShape(std::make_unique<Rectangle>(Point(5,5), Point(7,7)));
        comp->addShape(std::make_unique<Ring>(Point(2,2), 4, 1));

        shapes.push_back(std::move(comp));

        std::cout << "Before Scaling:" << std::endl;
        for (const auto& s : shapes) {
            std::cout << "[" << s->getName() << ", (" << s->getCenter().x << ", " << s->getCenter().y << "), " << s->getArea() << "]" << std::endl;
        }

        for (auto& s : shapes) s->scale(2.0);

        std::cout << "\nAfter Scaling (x2):" << std::endl;
        for (const auto& s : shapes) {
            std::cout << "[" << s->getName() << ", (" << s->getCenter().x << ", " << s->getCenter().y << "), " << s->getArea() << "]" << std::endl;
        }

    } catch (const std::invalid_argument& e) {

        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
