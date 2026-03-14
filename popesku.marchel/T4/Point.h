#ifndef WORKOUT_POINT__H
#define WORKOUT_POINT__H
class Point {
public:
    double x_ = 0.0;
    double y_ = 0.0;

    Point() = default;
    Point(double x_val, double y_val):
    x_(x_val),
    y_(y_val)
    {}
};
#endif //WORKOUT_POINT__H