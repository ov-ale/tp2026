#include "Command.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <stdexcept>
namespace {
    void skipLine() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    bool expectLineEnd() {
        while (std::cin.peek()==' ' || std::cin.peek()=='\t') {
            std::cin.get();
        }
        return std::cin.peek()=='\n' || std::cin.peek()==EOF;
    }
}
double AccumulateArea::operator()(double acc, const Polygon& p) const {
    return acc+getPolygonArea(p);
}
AccumulateAreaEvenOdd::AccumulateAreaEvenOdd(bool even):is_even(even) {}
double AccumulateAreaEvenOdd::operator()(double acc, const Polygon& p) const {
    if ((p.points.size()%2==0)==is_even) {
        return acc + getPolygonArea(p);
    }
    return acc;
}
AccumulateAreaVertex::AccumulateAreaVertex(size_t cnt):count(cnt) {}
double AccumulateAreaVertex::operator()(double acc, const Polygon& p) const {
    if (p.points.size()==count) {
        return acc+getPolygonArea(p);
    }
    return acc;
}
bool IsEvenVertexCount::operator()(const Polygon& p) const {
    return p.points.size()%2==0;
}
bool IsOddVertexCount::operator()(const Polygon& p) const {
    return p.points.size()%2!=0;
}
HasVertexCount::HasVertexCount(size_t n):count(n) {}
bool HasVertexCount::operator()(const Polygon& p) const {
    return p.points.size()==count;
}
UpdateMaxSeq::UpdateMaxSeq(const Polygon& tgt):target(tgt) {}
MaxSeqState UpdateMaxSeq::operator()(MaxSeqState state, const Polygon& p) const {
    if (p==target) {
        ++state.current_streak;
        state.max_streak=std::max(state.max_streak, state.current_streak);
    }
    else {
        state.current_streak=0;
    }
    return state;
}
void processCommands(const std::vector<Polygon>& polygons) {
    std::string cmd;
    std::cout<<std::fixed<<std::setprecision(1);
    while (std::cin>>cmd) {
        if (cmd=="AREA") {
            std::string arg;
            std::cin>>arg;
            if (!expectLineEnd()) {
                std::cout<<"<INVALID COMMAND>\n";
                skipLine();
                continue;
            }
            if (arg=="EVEN") {
                std::cout<<std::accumulate(polygons.begin(), polygons.end(),
                                             0.0, AccumulateAreaEvenOdd(true))<<"\n";
            }
            else if (arg=="ODD") {
                std::cout<<std::accumulate(polygons.begin(), polygons.end(),
                                             0.0, AccumulateAreaEvenOdd(false))<<"\n";
            }
            else if (arg=="MEAN") {
                if (polygons.empty()) {
                    std::cout<<"<INVALID COMMAND>\n";
                }
                else {
                    double sum=std::accumulate(polygons.begin(), polygons.end(),
                                                 0.0, AccumulateArea());
                    std::cout<<sum/static_cast<double>(polygons.size())<<"\n";
                }
            }
            else {
                try {
                    size_t vertexCount=std::stoull(arg);
                    if (vertexCount<3) {
                        throw std::invalid_argument("invalid");
                    }
                    std::cout<<std::accumulate(polygons.begin(), polygons.end(),
                                                 0.0, AccumulateAreaVertex(vertexCount))<<"\n";
                }
                catch (...) {
                    std::cout<<"<INVALID COMMAND>\n";
                }
            }
        }
        else if (cmd=="MAX" || cmd=="MIN") {
            std::string arg;
            std::cin>>arg;
            if (!expectLineEnd()) {
                std::cout<<"<INVALID COMMAND>\n";
                skipLine();
                continue;
            }
            if (polygons.empty()) {
                std::cout<<"<INVALID COMMAND>\n";
                continue;
            }
            const bool isMax=(cmd=="MAX");
            if (arg=="AREA") {
                auto compArea=[](const Polygon& a, const Polygon& b) {
                    return getPolygonArea(a)<getPolygonArea(b);
                };
                auto it=isMax?std::max_element(polygons.begin(), polygons.end(), compArea)
                             :std::min_element(polygons.begin(), polygons.end(), compArea);
                std::cout<<getPolygonArea(*it)<<"\n";
            }
            else if (arg=="VERTEXES") {
                auto compVert=[](const Polygon& a, const Polygon& b) {
                    return a.points.size()<b.points.size();
                };
                auto it=isMax?std::max_element(polygons.begin(), polygons.end(), compVert)
                             :std::min_element(polygons.begin(), polygons.end(), compVert);
                std::cout<<it->points.size()<<"\n";
            }
            else {
                std::cout<<"<INVALID COMMAND>\n";
            }
        }
        else if (cmd=="COUNT") {
            std::string arg;
            std::cin>>arg;
            if (!expectLineEnd()) {
                std::cout<<"<INVALID COMMAND>\n";
                skipLine();
                continue;
            }
            if (arg=="EVEN") {
                std::cout<<std::count_if(polygons.begin(), polygons.end(),
                                           IsEvenVertexCount())<<"\n";
            }
            else if (arg=="ODD") {
                std::cout<<std::count_if(polygons.begin(), polygons.end(),
                                           IsOddVertexCount())<<"\n";
            }
            else {
                try {
                    size_t vertexCount=std::stoull(arg);
                    if (vertexCount<3) {
                        throw std::invalid_argument("invalid");
                    }
                    std::cout<<std::count_if(polygons.begin(), polygons.end(),
                                               HasVertexCount(vertexCount))<<"\n";
                }
                catch (...) {
                    std::cout<<"<INVALID COMMAND>\n";
                }
            }
        }
        else if (cmd=="RECTS") {
            if (!expectLineEnd()) {
                std::cout<<"<INVALID COMMAND>\n";
                skipLine();
                continue;
            }
            std::cout<<std::count_if(polygons.begin(), polygons.end(), isRectangle)<<"\n";
        }
        else if (cmd=="MAXSEQ") {
            Polygon target;
            if (std::cin>>target && expectLineEnd()) {
                MaxSeqState initial{0, 0};
                MaxSeqState result=std::accumulate(polygons.begin(), polygons.end(),
                                                     initial, UpdateMaxSeq(target));
                std::cout<<result.max_streak<<"\n";
            }
            else {
                std::cout<<"<INVALID COMMAND>\n";
                skipLine();
            }
        }
        else {
            std::cout<<"<INVALID COMMAND>\n";
            skipLine();
        }
    }
}
