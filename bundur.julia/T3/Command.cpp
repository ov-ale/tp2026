#include "Command.h"
#include "Geom.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <string>
struct MaxSeqState {
    Polygon target;
    int current_streak=0;
    int max_streak=0;
    MaxSeqState(const Polygon& t):target(t) {}
};
void processCommands(const std::vector<Polygon>& polygons) {
    std::string cmd;
    std::cout<<std::fixed<<std::setprecision(1);
    while (std::cin>>cmd) {
        if (cmd=="AREA") {
            std::string arg;
            std::cin>>arg;
            if (arg=="EVEN") {
                double sum=std::accumulate(polygons.begin(), polygons.end(), 0.0,
                    [](double acc, const Polygon& p) {
                        return (p.points.size()%2==0)?acc+getPolygonArea(p):acc;
                    });
                std::cout<<sum<<"\n";
            }
            else if (arg=="ODD") {
                double sum=std::accumulate(polygons.begin(), polygons.end(), 0.0,
                    [](double acc, const Polygon& p) {
                        return (p.points.size()%2!=0)?acc+getPolygonArea(p):acc;
                    });
                std::cout<<sum<<"\n";
            }
            else if (arg=="MEAN") {
                if (polygons.empty()) {
                    std::cout<<"<INVALID COMMAND>\n";
                }
                else {
                    double sum=std::accumulate(polygons.begin(), polygons.end(), 0.0,
                        [](double acc, const Polygon& p) { return acc + getPolygonArea(p); });
                    std::cout<<sum/polygons.size()<<"\n";
                }
            }
            else {
                try {
                    size_t vertexCount=std::stoull(arg);
                    double sum=std::accumulate(polygons.begin(), polygons.end(), 0.0,
                        [vertexCount](double acc, const Polygon& p) {
                            return (p.points.size()==vertexCount)?acc+getPolygonArea(p):acc;
                        });
                    std::cout<<sum<<"\n";
                }
                catch (...) {
                    std::cout<<"<INVALID COMMAND>\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
        }
        else if (cmd=="MAX" || cmd=="MIN") {
            std::string arg;
            std::cin>>arg;
            if (polygons.empty()) {
                std::cout<<"<INVALID COMMAND>\n";
                continue;
            }
            bool isMax=(cmd=="MAX");
            if (arg=="AREA") {
                auto cmp=[](const Polygon& a, const Polygon& b) {
                    return getPolygonArea(a) < getPolygonArea(b);
                };
                auto it=isMax?std::max_element(polygons.begin(), polygons.end(), cmp)
                            :std::min_element(polygons.begin(), polygons.end(), cmp);
                std::cout<<getPolygonArea(*it)<<"\n";
            }
            else if (arg=="VERTEXES") {
                auto cmp=[](const Polygon& a, const Polygon& b) {
                    return a.points.size()<b.points.size();
                };
                auto it=isMax?std::max_element(polygons.begin(), polygons.end(), cmp)
                             :std::min_element(polygons.begin(), polygons.end(), cmp);
                std::cout<<it->points.size()<<"\n";
            }
            else {
                std::cout<<"<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        else if (cmd=="COUNT") {
            std::string arg;
            std::cin>>arg;
            if (arg=="EVEN") {
                std::cout<<std::count_if(polygons.begin(), polygons.end(),
                    [](const Polygon& p) { return p.points.size()%2==0; })<<"\n";
            }
            else if (arg=="ODD") {
                std::cout<<std::count_if(polygons.begin(), polygons.end(),
                    [](const Polygon& p) { return p.points.size()%2!=0; })<<"\n";
            }
            else {
                try {
                    size_t vertexCount=std::stoull(arg);
                    std::cout<<std::count_if(polygons.begin(), polygons.end(),
                        [vertexCount](const Polygon& p) { return p.points.size()==vertexCount; })<<"\n";
                }
                catch (...) {
                    std::cout<<"<INVALID COMMAND>\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
        }
        else if (cmd=="RECTS") {
            std::cout<<std::count_if(polygons.begin(), polygons.end(), isRectangle)<<"\n";
        }
        else if (cmd=="MAXSEQ") {
            Polygon target;
            if (std::cin>>target) {
                auto updateMaxSeq=[](MaxSeqState state, const Polygon& p) {
                    if (p==state.target) {
                        state.current_streak++;
                        state.max_streak=std::max(state.max_streak, state.current_streak);
                    }
                    else {
                        state.current_streak=0;
                    }
                    return state;
                };
                MaxSeqState result=std::accumulate(polygons.begin(), polygons.end(), MaxSeqState(target), updateMaxSeq);
                std::cout<<result.max_streak<<"\n";
            }
            else {
                std::cout<<"<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        else {
            std::cout<<"<INVALID COMMAND>\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}
