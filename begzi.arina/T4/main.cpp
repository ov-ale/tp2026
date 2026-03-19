#include <vector>
#include <memory>
#include <iostream>
#include <cstdlib>
#include "rectangle.h"
#include "circle.h"
#include "trapezoid.h"
#include "composite_shape.h"

int main(int argc, char* argv[]) {
    double scaleFactor = 2.0;

    if (argc > 1) {
        scaleFactor = std::atof(argv[1]);
        if (scaleFactor <= 0) {
            scaleFactor = 2.0;
        }
    }

    std::vector<std::unique_ptr<Shape>> scene;

    scene.push_back(std::make_unique<Rectangle>(Point{ 0, 0 }, Point{ 2, 2 }));
    scene.push_back(std::make_unique<Circle>(Point{ 5, 5 }, 1.0));
    scene.push_back(std::make_unique<IsoscelesTrapezoid>(Point{ 10, 0 }, 4, 2, 2));
    scene.push_back(std::make_unique<Circle>(Point{ -2, -2 }, 1.5));

    auto comp = std::make_unique<CompositeShape>();
    comp->add(std::make_unique<Rectangle>(Point{ 20, 20 }, Point{ 22, 22 }));
    comp->add(std::make_unique<Circle>(Point{ 21, 21 }, 0.5));
    scene.push_back(std::move(comp));

    std::cout << " BEFORE SCALE \n";
    for (size_t i = 0; i < scene.size(); i++) {
        scene[i]->print();
        std::cout << "\n";
    }

    for (size_t i = 0; i < scene.size(); i++) {
        scene[i]->scale(scaleFactor);
    }

    std::cout << "\n AFTER SCALE \n";
    for (size_t i = 0; i < scene.size(); i++) {
        scene[i]->print();
        std::cout << "\n";
    }

    return 0;
}
