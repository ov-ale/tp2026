
#include "rectangle.h"
#include "ellipse.h"
#include <iostream>
#include "recttrapezoid.h"
#include "compositeshape.h"

int main()
{
    std::cout << "before scaling:\n";
    Point pr1(1, 1);
    Point pr2(-2, -2);
    Rectangle r1(pr1, pr2);
    Rectangle r2(Point(2, 3), Point(-3, -5));
    std::cout << r1 << " " << r2 << std::endl;

    Point pe(0, 0);
    Ellipse e1(pe, 3, 4);
    std::cout << e1 << std::endl;

    Point prt(1, 1);
    RectTrapezoid rt(prt, 4, 5, 3);
    std::cout << rt << std::endl;

    r1.scale(2);
    r2.scale(2);
    e1.scale(2);
    rt.scale(2);

    CompositeShape cs1;
    cs1.add(std::make_unique<RectTrapezoid>(Point(1,1), 4, 5, 3));
    cs1.add(std::make_unique<Ellipse>(Point(2, 2), 3, 4));
    cs1.add(std::make_unique<Rectangle>(Point(0, 0), Point(10, 10)));

    std::cout << cs1 << std::endl;

    cs1.scale(2); 
    std::cout << "after scaling:\n";

    std::cout << r1 << std::endl;
    std::cout << e1 << std::endl;
    std::cout << rt << std::endl;
    std::cout << cs1 << std::endl;
}