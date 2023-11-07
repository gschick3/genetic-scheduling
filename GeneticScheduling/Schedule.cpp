#include "Schedule.h"

Schedule::Schedule(const Genome genome) {
    fitnessScore = 0;

    for (const Activity& activity : genome.getActivities())
        schedule.emplace_back(genome, activity);
}

Schedule::Schedule(const Genome genome, const Schedule& parent1, const Schedule& parent2,
    double splicePoint, double mutationChance) {
    fitnessScore = 0;

    // Get random number from 0-1
    std::random_device rd;
    std::mt19937 mt(rd());
    
    // Choose a random splice point (will be different location every time, but will stay at proper size)
    std::uniform_int_distribution<int> startDist(0, (int)(parent1.schedule.size() * splicePoint));
    int startPoint = startDist(mt);

    for (int i = 0; i < startPoint; i++) {
        std::uniform_int_distribution<int> mutDist(0, (int)(1 / mutationChance));
        int mut = mutDist(mt);
        if (mut == 0)
            schedule.push_back(Item(genome, parent1.schedule.at(i).activity));
        else
            schedule.push_back(parent1.schedule.at(i));
    }
    for (int i = startPoint; i < parent2.schedule.size(); i++) {
        std::uniform_int_distribution<int> mutDist(0, (int)(1 / mutationChance));
        int mut = mutDist(mt);
        if (mut == 0)
            schedule.push_back(Item(genome, parent1.schedule.at(i).activity));
        else
            schedule.push_back(parent2.schedule.at(i));
    }

}

void Schedule::scoreActivities(double(*scoringFunc)(const Item&, const std::vector<Item>&)) {
    // scoringFunc gives an activity score for a given item using the item and its schedule
    fitnessScore = 0;
    for (Item& a : schedule)
        fitnessScore += scoringFunc(a, schedule);
}

void Schedule::printSchedule(std::ostream& stream) const {
    for (const Item& i : schedule) {
        stream << i.activity.id << "\t" << i.room.building << " " << i.room.num
            << "\t" << i.time << "\t" << i.facilitator << "\n";
    }
    stream << "Score: " << fitnessScore << "\n";
}

bool Schedule::operator>(const Schedule& rhs) const {
    // This apparently corresponds with std::greater
    return fitnessScore < rhs.fitnessScore;
}