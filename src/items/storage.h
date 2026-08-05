#pragma once
#include <map>
#include <string>

using namespace std;

/**
 * Fixed-capacity resource container. Subclasses populate `limits` and `stored`
 * with the resource names they support; anything not named there is rejected
 * rather than throwing.
 */
class AbstractStorage{
public:
    map<string, int> limits;
    map<string, int> stored;

    virtual ~AbstractStorage() = default;

    // Store what fits. `remaining_amount` receives the overflow.
    void store(string resource, int amount, int & remaining_amount){
        auto limit_it = limits.find(resource);
        auto stored_it = stored.find(resource);
        if (limit_it == limits.end() or stored_it == stored.end()){  // Unsupported resource.
            remaining_amount = amount;
            return;
        }

        int & current = stored_it->second;
        const int space = limit_it->second - current;

        if (amount > space){
            current = limit_it->second;
            remaining_amount = amount - space;
        } else {
            current += amount;
            remaining_amount = 0;
        }
    };

    // Take out what is there. `needed_amount` receives the shortfall.
    void takeout(string resource, int amount, int & needed_amount){
        auto stored_it = stored.find(resource);
        if (stored_it == stored.end()){  // Unsupported resource.
            needed_amount = amount;
            return;
        }

        int & current = stored_it->second;

        if (amount > current){
            needed_amount = amount - current;
            current = 0;
        } else {
            needed_amount = 0;
            current -= amount;
        }
    };
};

class ColonyStorage : public AbstractStorage{
public:
    ColonyStorage(bool initial_storage = false){
        limits = {{"food", 1000}, {"wood", 200}, {"stone", 200}, {"bronze", 200}, {"iron", 200}};
        stored = {{"food", 0}, {"wood", 0}, {"stone", 0}, {"bronze", 0}, {"iron", 0}};

        if (initial_storage){
            stored["food"] = 200;
            stored["wood"] = 50;
        }
    };
};

class BasicBackpack : public AbstractStorage{
public:
    BasicBackpack(){
        limits = {{"food", 20}, {"wood", 10}, {"stone", 10}, {"bronze", 10}, {"iron", 10}};
        stored = {{"food", 0}, {"wood", 0}, {"stone", 0}, {"bronze", 0}, {"iron", 0}};
    };
};
