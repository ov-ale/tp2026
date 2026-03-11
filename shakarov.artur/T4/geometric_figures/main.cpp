#include <iostream>
#include <iomanip>
#include "rectangle.h"
#include "circle.h"
#include "compositeshape.h"

using namespace std;

void printShape(Shape* s) {
    Point c = s->getCenter();
    cout << fixed << setprecision(2);
    cout << "[" << s->getName() << ", (" << c.x << ", " << c.y << "), " << s->getArea() << "]";
}

int main() {
    Shape* shapes[5];
    
    shapes[0] = new Rectangle(0, 0, 4, 3);
    shapes[1] = new Circle(5, 5, 2);
    shapes[2] = new Rectangle(-2, -1, 2, 1);
    shapes[3] = new Circle(-3, 4, 1.5);
    
    CompositeShape* comp = new CompositeShape();
    comp->addShape(new Rectangle(10, 10, 14, 13));
    comp->addShape(new Circle(12, 8, 1));
    comp->addShape(new Rectangle(8, 5, 16, 7));
    shapes[4] = comp;
    
    cout << "ДО МАСШТАБИРОВАНИЯ:\n";
    for (int i = 0; i < 5; i++) {
        if (shapes[i]->getName() == "COMPOSITE") {
            CompositeShape* cs = (CompositeShape*)shapes[i];
            Point c = cs->getCenter();
            cout << "[COMPOSITE, (" << c.x << ", " << c.y << "), " << cs->getArea() << ":\n";
            
            for (int j = 0; j < cs->getCount(); j++) {
                cout << "  ";
                printShape(cs->getShapes()[j]);
                if (j < cs->getCount() - 1) cout << ",";
                cout << "\n";
            }
            cout << "]\n";
        } else {
            printShape(shapes[i]);
            cout << "\n";
        }
        cout << "\n";
    }
    
    cout << "\nПОСЛЕ МАСШТАБИРОВАНИЯ (x2):\n";
    for (int i = 0; i < 5; i++) {
        shapes[i]->scale(2.0);
        
        if (shapes[i]->getName() == "COMPOSITE") {
            CompositeShape* cs = (CompositeShape*)shapes[i];
            Point c = cs->getCenter();
            cout << "[COMPOSITE, (" << c.x << ", " << c.y << "), " << cs->getArea() << ":\n";
            
            for (int j = 0; j < cs->getCount(); j++) {
                cout << "  ";
                printShape(cs->getShapes()[j]);
                if (j < cs->getCount() - 1) cout << ",";
                cout << "\n";
            }
            cout << "]\n";
        } else {
            printShape(shapes[i]);
            cout << "\n";
        }
        cout << "\n";
    }
    
    for (int i = 0; i < 5; i++) {
        delete shapes[i];
    }
    
    return 0;
}