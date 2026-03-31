#ifndef LINEARSCAN_HPP
#define LINEARSCAN_HPP

// don't include other headfiles
#include <string>
#include <vector>
#include <set>
#include <stack>

class Location {
public:
    // return a string that represents the location
    virtual std::string show() const = 0;
    virtual int getId() const = 0;
};

class Register : public Location {
private:
    int id;
public:
    Register(int regId) : id(regId) {}

    virtual std::string show() const {
        return "reg" + std::to_string(id);
    }

    virtual int getId() const {
        return id;
    }
};

class StackSlot : public Location {
public:
    StackSlot() {}

    virtual std::string show() const {
        return "stack";
    }

    virtual int getId() const {
        return -1;
    }
};

struct LiveInterval {
    int startpoint;
    int endpoint;
    Location* location = nullptr;
};

// Comparator for active list (sorted by endpoint in ascending order)
struct EndpointComparator {
    bool operator()(const LiveInterval* a, const LiveInterval* b) const {
        return a->endpoint < b->endpoint;
    }
};

class LinearScanRegisterAllocator {
private:
    int numRegisters;
    std::stack<int> freeRegisters; // Stack for FILO allocation
    std::set<LiveInterval*, EndpointComparator> active; // Active intervals sorted by endpoint

    void expireOldIntervals(LiveInterval& i) {
        // Remove intervals that end before i.startpoint
        auto it = active.begin();
        while (it != active.end()) {
            LiveInterval* j = *it;
            if (j->endpoint >= i.startpoint) {
                // All remaining intervals are still active
                break;
            }
            // This interval has expired, free its register
            if (j->location != nullptr && j->location->getId() != -1) {
                freeRegisters.push(j->location->getId());
            }
            it = active.erase(it);
        }
    }

    void spillAtInterval(LiveInterval& i) {
        // Find the interval with the largest endpoint in active U {i}
        LiveInterval* spill = &i;

        if (!active.empty()) {
            // The last element in active has the largest endpoint
            auto it = active.end();
            --it;
            LiveInterval* last = *it;

            if (last->endpoint > i.endpoint) {
                // Spill the last interval instead
                spill = last;
                // Assign its register to i
                i.location = spill->location;
                active.erase(it);
                active.insert(&i);
            }
        }

        // Spill the chosen interval
        if (spill == &i) {
            spill->location = new StackSlot();
        } else {
            spill->location = new StackSlot();
        }
    }

public:
    LinearScanRegisterAllocator(int regNum) : numRegisters(regNum) {
        // Initialize free registers stack
        // Push in reverse order so smallest ID is on top (FILO)
        for (int i = regNum - 1; i >= 0; i--) {
            freeRegisters.push(i);
        }
    }

    void linearScanRegisterAllocate(std::vector<LiveInterval>& intervalList) {
        // Process each interval in order of increasing startpoint
        for (auto& i : intervalList) {
            // Expire old intervals
            expireOldIntervals(i);

            // Check if there's a free register
            if (!freeRegisters.empty()) {
                // Allocate a free register
                int regId = freeRegisters.top();
                freeRegisters.pop();
                i.location = new Register(regId);
                active.insert(&i);
            } else {
                // No free register, need to spill
                spillAtInterval(i);
            }
        }
    }
};

#endif
