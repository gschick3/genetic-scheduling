#pragma once

#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <fstream>

#include "Schedule.h"
#include "Genome.h"

double scoreActivity(const Item& item, const std::vector<Item>& schedule) {
    double score = 0;

    // Preferred Facilitator
    if (item.activity.prefFac.count(item.facilitator)) score += 0.5;
    else if (item.activity.otherFac.count(item.facilitator)) score += 0.2;
    else score -= 0.1;

    // Room Size
    if (item.room.capacity < item.activity.enrollment) score -= 0.5;
    else if (item.room.capacity > item.activity.enrollment * 3) score -= 0.2;
    else if (item.room.capacity > item.activity.enrollment * 6) score -= 0.4;
    else score += 0.3;

    std::unordered_multiset<int> facTimes; // times that faculty member is scheduled
    facTimes.insert(item.time);

    for (const Item& item2 : schedule) {
        if (item.activity.id == item2.activity.id) continue;

        // Double classes (A/B)
        if (item2.activity.id.substr(0, item2.activity.id.length() - 1)
            == item.activity.id.substr(0, item.activity.id.length() - 1)) {
            if (abs(item.time - item2.time) > 4) score += 0.5;
            else if (item.time == item2.time) score -= 0.5;
        } // 101 and 191
        else if ((item.activity.id.at(item.activity.id.length() - 1) == 'A'
            || item.activity.id.at(item.activity.id.length() - 1) == 'B')
            && (item2.activity.id.at(item2.activity.id.length() - 1) == 'A'
                || item2.activity.id.at(item2.activity.id.length() - 1) == 'B')) {
            if (abs(item.time - item2.time) == 1) {
                // Location is too far
                if ((item.room.building == "Roman" || item.room.building == "Beach")
                    && (item2.room.building != "Roman" && item2.room.building != "Beach"))
                    score -= 0.4;
                else score += 0.5;
            }
            else if (abs(item.time - item2.time) == 2) score += 0.25;
            else if (item.time == item2.time) score -= 0.25;
        }

        // Conflicting rooms
        if (item.time == item2.time
            && item.room.building == item2.room.building
            && item.room.num == item2.room.num)
            score -= 0.5;

        // Faculty times
        if (item.facilitator == item2.facilitator) {
            facTimes.insert(item2.time);
            if (abs(item.time - item2.time) == 1) {
                // Location is too far
                if ((item.room.building == "Roman" || item.room.building == "Beach")
                    && (item2.room.building != "Roman" && item2.room.building != "Beach"))
                    score -= 0.4;
                else score += 0.5;
            }
        }
    }

    // Teacher double scheduled?
    if (facTimes.count(item.time) > 1) score -= 0.5; // CHANGED: from -0.2 to -0.5, since this is very bad
    else score += 0.2;

    // Department head
    if (item.facilitator == "Tyler") {
        if (facTimes.size() > 2) // CHANGED: separate overload level from other faculty, but use same score
            score -= 0.5;
    } else {
        if (facTimes.size() > 4) 
            score -= 0.5;
        else if (facTimes.size() == 1 || facTimes.size() == 2) // Underloaded only for non-department head
            score -= 0.4;
    }

    return score;
}

Genome fillGenome() {
    // Create genome with start and end times
    Genome genome(11, 15);

    // Add facilitators
    std::string facilitators[10] = { "Lock", "Glen", "Banks", "Richards", "Shaw",
                                    "Singer", "Uther", "Tyler", "Numen", "Zeldin" };
    for (const std::string& facilitator : facilitators)
        genome.addFacilitator(facilitator);

    // Add activities from CSV file
    std::ifstream in("../data/activities.csv");
    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);

        std::string id;
        std::string enrollment;
        std::unordered_set<std::string> prefProfs;
        std::unordered_set<std::string> otherProfs;

        std::getline(ss, id, ',');
        std::getline(ss, enrollment, ',');

        std::string prof;
        for (int i = 0; i < 4; i++) {
            std::getline(ss, prof, ',');
            if (!prof.empty())
                prefProfs.insert(prof);
        }

        while (std::getline(ss, prof, ','))
            otherProfs.insert(prof);

        genome.addActivity(id, std::stoi(enrollment), prefProfs, otherProfs);
    }
    in.close();

    // Add rooms from CSV file
    in.open("../data/rooms.csv");
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string building;
        int roomNum;
        int capacity;

        std::string temp;
        std::getline(ss, building, ',');
        std::getline(ss, temp, ',');
        roomNum = std::stoi(temp);
        std::getline(ss, temp, ',');
        capacity = std::stoi(temp);

        genome.addRoom(building, roomNum, capacity);
    }
    return genome;
}