#include "Command.h"
#include "Geom.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <string>
struct MaxSeqState {
    Polygon target;
    int current_streak=0;
    int max_streak=0;
    MaxSeqState(const Polygon& t):target(t) {}
};
void cmdArea(const std::vector<Polygon>& polygons, const std::string& arg) {
    if (arg=="EVEN" || arg=="ODD") {
        bool calcEven=(arg=="EVEN");
        double sum=std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [calcEven](double acc, const Polygon& p) {
                return (p.points.size()%2==(calcEven?0:1))?acc+getPolygonArea(p):acc;
            });
        std::cout<<sum<<"\n";
    }
    else if (arg=="MEAN") {
        if (polygons.empty()) {
            std::cout<< "<Invalid command>\n";
            return;
        }
        double sum=std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double acc, const Polygon& p) { return acc+getPolygonArea(p); });
        std::cout<<sum/polygons.size()<<"\n";
    }
    else {
        try {
            size_t vertexCount=std::stoull(arg);
            double sum=std::accumulate(polygons.begin(), polygons.end(), 0.0,
                [vertexCount](double acc, const Polygon& p) {
                    return p.points.size()==vertexCount?acc+getPolygonArea(p):acc;
                });
            std::cout<<sum<<"\n";
        }
        catch (...) {
            std::cout<<"<Invalid command>\n";
        }
    }
}
void cmdMaxMin(const std::vector<Polygon>& polygons, const std::string& arg, bool isMax) {
    if (polygons.empty()) {
        std::cout<<"<Invalid command>\n";
        return;
    }
    if (arg=="AREA") {
        auto cmp=[](const Polygon& a, const Polygon& b) {
            return getPolygonArea(a)<getPolygonArea(b);
        };
        auto it=isMax?std::max_element(polygons.begin(), polygons.end(), cmp)
                     : std::min_element(polygons.begin(), polygons.end(), cmp);
        std::cout<<getPolygonArea(*it)<<"\n";
    }
    else if (arg=="VERTEXES") {
        auto cmp=[](const Polygon& a, const Polygon& b) {
            return a.points.size()<b.points.size();
        };
        auto it=isMax?std::max_element(polygons.begin(), polygons.end(), cmp)
                     : std::min_element(polygons.begin(), polygons.end(), cmp);
        std::cout<<it->points.size()<<"\n";
    }
    else {
        std::cout<<"<Invalid command>\n";
    }
}
void cmdCount(const std::vector<Polygon>& polygons, const std::string& arg) {
    if (arg=="EVEN" || arg=="ODD") {
        bool countEven=(arg=="EVEN");
        size_t c=std::count_if(polygons.begin(), polygons.end(),
            [countEven](const Polygon& p) {
                return (p.points.size()%2==(countEven?0:1));
            });
        std::cout<<c<<"\n";
    }
    else {
        try {
            size_t vertexCount=std::stoull(arg);
            size_t c=std::count_if(polygons.begin(), polygons.end(),
                [vertexCount](const Polygon& p) {
                    return p.points.size()==vertexCount;
                });
            std::cout<<c<<"\n";
        }
        catch (...) {
            std::cout<<"<Invalid command>\n";
        }
    }
}
void processCommands(const std::vector<Polygon>& polygons) {
    std::string cmd;
    std::cout<<std::fixed<<std::setprecision(1);
    while (std::cin>>cmd) {
        if (cmd=="AREA") {
            std::string arg;
            std::cin>>arg;
            cmdArea(polygons, arg);
        }
        else if (cmd=="MAX" || cmd=="MIN") {
            std::string arg;
            std::cin>>arg;
            cmdMaxMin(polygons, arg, cmd=="MAX");
        }
        else if (cmd=="COUNT") {
            std::string arg;
            std::cin>>arg;
            cmdCount(polygons, arg);
        }
        else if (cmd=="RECTS") {
            size_t c=std::count_if(polygons.begin(), polygons.end(), isRectangle);
            std::cout<<c<<"\n";
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
                MaxSeqState result=std::accumulate(
                    polygons.begin(), polygons.end(),
                    MaxSeqState(target),
                    updateMaxSeq
                );
                std::cout<<result.max_streak<<"\n";
            }
            else {
                std::cout<<"<Invalid command>\n";
                std::cin.clear();
            }
        }
        else {
            std::cout<<"<Invalid command>\n";
            std::string dump;
            std::getline(std::cin, dump);
        }
    }
}
