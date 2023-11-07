#pragma once

#include <unordered_map>
#include <unordered_set>
#include <random>
#include <iostream>
#include <utility>

#include "Genome.h"

struct Item {
    Activity activity;
    Room room;
    std::string facilitator;
    int time;
    double fitnessScore;

    Item(const Activity a, Room r, std::string f, int t) {
        activity = a;
        room = r;
        facilitator = f;
        time = t;
        fitnessScore = 0;
    }

    Item(const Genome& genome, Activity a) {
        activity = std::move(a);
        room = genome.getRandomRoom();
        time = genome.getRandomTime();
        facilitator = genome.getRandomFacilitator();
        fitnessScore = 0;
    }
    bool operator==(const Item& rhs) const {
        return activity.id == rhs.activity.id;
    }
};

class Schedule {
public:
    explicit Schedule(Genome genome);
    Schedule(Genome genome, const Schedule& parent1, const Schedule& parent2,
        double splicePoint, double mutationChance);
    [[nodiscard]] double getFitness() const { return fitnessScore; }
    void scoreActivities(double(*scoringFunc)(const Item&, const std::vector<Item>&));

    void printSchedule(std::ostream&) const;

    bool operator>(const Schedule& rhs) const;
private:
    std::vector<Item> schedule;
    double fitnessScore;
};