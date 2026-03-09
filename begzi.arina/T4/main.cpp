#include <vector>
#include <memory>
#include <iostream>
#include "Rectangle.h"
#include "Circle.h"
#include "Trapezoid.h"
#include "Composite_shape.h"

int main() {
    std::vector<std::unique_ptr<Shape>> scene;

    scene.push_back(std::make_unique<Rectangle>(Point{ 0,0 }, Point{ 2,2 }));
    scene.push_back(std::make_unique<Circle>(Point{ 5,5 }, 1.0));
    scene.push_back(std::make_unique<IsoscelesTrapezoid>(Point{ 10,0 }, 4, 2, 2));
    scene.push_back(std::make_unique<Circle>(Point{ -2,-2 }, 1.5));

    auto comp = std::make_unique<CompositeShape>();
    comp->add(std::make_unique<Rectangle>(Point{ 20,20 }, Point{ 22,22 }));
    comp->add(std::make_unique<Circle>(Point{ 21,21 }, 0.5));
    scene.push_back(std::move(comp));

    std::cout << "--- BEFORE SCALE ---\n";
    for (const auto& s : scene) { s->print(); std::cout << "\n"; }

    for (auto& s : scene) s->scale(2.0);

    std::cout << "\n--- AFTER SCALE (x2) ---\n";
    for (const auto& s : scene) { s->print(); std::cout << "\n"; }

    return 0;
}
