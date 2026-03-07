#include "shapes.h"
int main() {
    Composite comp;
    comp.add(std::make_unique<Rectangle>(Point{ 0,0 }, 4.0, 2.0));
    comp.add(std::make_unique<Circle>(Point{ 5,1 }, 1.0));
    comp.add(std::make_unique<RightTrapezoid>(Point{ 2,0 }, 4.0, 2.0, 3.0));

    std::cout << comp.info();

    comp.scale(2.0);
    std::cout << "After scale x2:" << comp.info() ;
}