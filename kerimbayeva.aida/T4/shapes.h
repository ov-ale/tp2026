#pragma once

#include "point.h"
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <iostream>
#include <memory>

struct Shape {
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual Point center() const = 0;
    virtual void move(const Point& newCenter) = 0;
    virtual void scale(double factor) = 0;
    virtual std::string info() const = 0;
};

struct Rectangle : Shape {
    Point c;
    double width;
    double height;

    Rectangle(const Point& center_, double w, double h) : c(center_), width(w), height(h) {}

    double area() const override
    {
        return width * height;
    }
    Point center() const override
    {
        return c;
    }
    void move(const Point& newCenter) override
    {
        c = newCenter;
    }
    void scale(double factor) override {
        width *= factor;
        height *= factor;
    }
    std::string info() const override {
        std::ostringstream ss;
        ss << "Rectangle " << c << ", " << std::fixed << std::setprecision(2)
            << area();
        return ss.str();
    }
};

struct Circle : Shape {
    Point c;
    double radius;
    Circle(const Point& center_, double r) : c(center_), radius(r) {}
    double area() const override {
        const double pi = 3.14;
        return pi * radius * radius;
    }
    Point center() const override {
        return c;
    }
    void move(const Point& newCenter) override {
        c = newCenter;
    }
    void scale(double factor) override {
        radius *= factor;
    }
    std::string info() const override {
        std::ostringstream ss;
        ss << "Circle " << c << ", " << std::fixed << std::setprecision(2)
            << area();
        return ss.str();
    }
};

struct RightTrapezoid : Shape {
    Point lb;
    double b1;
    double b2;
    double h;

    RightTrapezoid(const Point& left_bottom, double bottom_base, double top_base, double height)
        : lb(left_bottom), b1(bottom_base), b2(top_base), h(height) {
    }

    double area() const override { return (b1 + b2) * h / 2.0; }

    Point center() const override {
        double cx = lb.x + (b1 + b2) / 4.0;
        double cy = lb.y + h / 2.0;
        return { cx, cy };
    }

    void move(const Point& newCenter) override {
        Point cur = center();
        Point delta = newCenter - cur;
        lb = lb + delta;
    }

    void scale(double factor) override {
        Point c = center();
        Point off = lb - c;
        off = off * factor;
        lb = c + off;
        b1 *= factor;
        b2 *= factor;
        h *= factor;
    }

    std::string info() const override {
        std::ostringstream ss;
        ss << "Trapezoid " << center() << ", " << std::fixed << std::setprecision(2)
            << area();
        return ss.str();
    }
};

struct Composite : Shape {
    std::vector<std::unique_ptr<Shape>> children;

    Composite() = default;

    void add(std::unique_ptr<Shape> s) {
        children.push_back(std::move(s));
    }

    double area() const override {
        double sum = 0;
        for (int i = 0; i < children.size(); i++) {
            sum += children[i]->area();
        }
        return sum;
    }

    Point center() const override {
        if (children.empty())
            return { 0,0 };

        double minx, miny, maxx, maxy;
        bool first = true;

        for (int i = 0; i < children.size(); i++) {
            Point p = children[i]->center();

            if (first) {
                minx = maxx = p.x;
                miny = maxy = p.y;
                first = false;
            }
            else {
                if (p.x < minx) minx = p.x;
                if (p.y < miny) miny = p.y;
                if (p.x > maxx) maxx = p.x;
                if (p.y > maxy) maxy = p.y;
            }
        }

        return { (minx + maxx) / 2.0, (miny + maxy) / 2.0 };
    }

    void move(const Point& newCenter) override {
        Point cur = center();
        Point delta = newCenter - cur;

        for (int i = 0; i < children.size(); i++) {
            Point childC = children[i]->center();
            children[i]->move(childC + delta);
        }
    }

    void scale(double factor) override {
        Point c = center();
        for (int i = 0; i < children.size(); i++) {
            Point childC = children[i]->center();
            Point vec = childC - c;
            Point newChildCenter = c + vec * factor;
            children[i]->move(newChildCenter);
            children[i]->scale(factor);
        }
    }

    std::string info() const override {
        std::ostringstream ss;

        ss << "Composite " << center() << ", "
            << std::fixed << std::setprecision(2) << area() << ":\n";

        for (int i = 0; i < children.size(); i++) {
            ss << children[i]->info() << "\n";
        }

        return ss.str();
    }
};