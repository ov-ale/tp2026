#ifndef COMMAND_H
#define COMMAND_H
#include <vector>
#include <cstddef>
#include "Geom.h"
struct AccumulateAreaEvenOdd {
    bool is_even;
    explicit AccumulateAreaEvenOdd(bool even);
    double operator()(double acc, const Polygon& p) const;
};
struct AccumulateArea {
    double operator()(double acc, const Polygon& p) const;
};
struct AccumulateAreaVertex {
    size_t count;
    explicit AccumulateAreaVertex(size_t cnt);
    double operator()(double acc, const Polygon& p) const;
};
struct IsEvenVertexCount {
    bool operator()(const Polygon& p) const;
};
struct IsOddVertexCount {
    bool operator()(const Polygon& p) const;
};
struct HasVertexCount {
    size_t count;
    explicit HasVertexCount(size_t n);
    bool operator()(const Polygon& p) const;
};
struct MaxSeqState {
    size_t max_streak;
    size_t current_streak;
};
struct UpdateMaxSeq {
    const Polygon& target;
    explicit UpdateMaxSeq(const Polygon& tgt);
    MaxSeqState operator()(MaxSeqState state, const Polygon& p) const;
};
void processCommands(const std::vector<Polygon>& polygons);
#endif
