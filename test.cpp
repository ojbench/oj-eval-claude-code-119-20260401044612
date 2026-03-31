#include "src.hpp"
#include <vector>
#include <iostream>

int main() {
    LinearScanRegisterAllocator allocator(2);
    std::vector<LiveInterval> intervalList;
    LiveInterval interval;
    interval.startpoint = 1;
    interval.endpoint = 4;
    intervalList.push_back(interval);
    interval.startpoint = 2;
    interval.endpoint = 5;
    intervalList.push_back(interval);
    interval.startpoint = 3;
    interval.endpoint = 9;
    intervalList.push_back(interval);
    interval.startpoint = 5;
    interval.endpoint = 8;
    intervalList.push_back(interval);
    interval.startpoint = 6;
    interval.endpoint = 7;
    intervalList.push_back(interval);
    allocator.linearScanRegisterAllocate(intervalList);
    for (int i = 0; i < intervalList.size(); i++)
        std::cout << "interval " << i+1 << ": " << intervalList[i].location->show() << "\n";
}
