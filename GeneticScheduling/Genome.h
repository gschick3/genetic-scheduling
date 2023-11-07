#pragma once

#include <random>
#include <iostream>
#include <utility>

struct Activity {
    std::string id;
    int enrollment;
    std::unordered_set<std::string> prefFac;
    std::unordered_set<std::string> otherFac;
};

struct Room {
    std::string building;
    int num;
    int capacity;
};

class Genome {
public:
    Genome(int startOfDay, int endOfDay) {
        beg = startOfDay;
        end = endOfDay;
    }

    void addActivity(std::string id, int enrollment,
        std::unordered_set<std::string> prefFac, std::unordered_set<std::string> otherFac) {
        activities.push_back(Activity{ std::move(id), enrollment, std::move(prefFac), std::move(otherFac) });
    }

    void addRoom(std::string building, int num, int capacity) {
        rooms.push_back(Room{ std::move(building), num, capacity });
    }

    void addFacilitator(const std::string& fac) {
        facilitators.push_back(fac);
    }

    void setTimes(int startOfDay, int endOfDay) {
        beg = startOfDay;
        end = endOfDay;
    }

    [[nodiscard]] std::vector<Activity> getActivities() const {
        return activities;
    }

    [[nodiscard]] Room getRandomRoom() const {
        return rooms.at(getRandomInt(0, (int)rooms.size()-1));
    }

    [[nodiscard]] std::string getRandomFacilitator() const {
        return facilitators.at(getRandomInt(0, (int)facilitators.size()-1));
    }

    [[nodiscard]] int getRandomTime() const {
        return getRandomInt(beg, end);
    }

    void printGenome() {
        std::cout << "Facilitators:\n";
        for (const std::string& fac : facilitators)
            std::cout << fac << std::endl;
        std::cout << "\nActivities:\n";
        for (const Activity& a : activities) {
            std::cout << a.id << " (" << a.enrollment << ")\n";
            std::cout << "\tPreferred: ";
            for (const std::string& p : a.prefFac)
                std::cout << p << " ";
            std::cout << "\n\tOther: ";
            for (const std::string& p : a.otherFac)
                std::cout << p << " ";
            std::cout << "\n";
        }
        std::cout << "\nRooms:\n";
        for (const Room& r : rooms)
            std::cout << r.building << " " << r.num << " (" << r.capacity << ")\n";
    }

private:
    std::vector<Activity> activities;
    std::vector<Room> rooms;
    std::vector<std::string> facilitators;
    int beg, end;

    static int getRandomInt(int beg, int end) {
        // Beg and end are both inclusive
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(beg, end);
        return dist(mt);
    }
};