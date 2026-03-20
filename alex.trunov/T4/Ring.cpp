#include "Ring.h"

double Ring::getArea() const {
    double sq = 3.1415 * (maxRad_ * maxRad_ - minRad_ * minRad_);
    return sq;
}

Point Ring::getCenter() const {
    return center_;
}

void Ring::move(double x, double y) {
    center_.x_ += x;
    center_.y_ += y;
}

void Ring::scale(double factor) {
    maxRad_ *= factor;
    minRad_ *= factor;
}

std::string Ring::getName() const {
    return "Ring";
}
