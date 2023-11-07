#include <iostream>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <sstream>
#include <queue>
#include <deque>

#include "Schedule.h"
#include "Genome.h"
#include "configure.h"

const int POP_SIZE = 500;
const int MIN_GENS = 500;
const double MUTATION_RATE = 0.1;

Genome fillGenome();
double scoreActivity(const Item& item, const std::vector<Item>& schedule);

int main() {

    // Fill genome
    Genome genome = fillGenome();

    std::vector<Schedule> population;
    for (int i = 0; i < POP_SIZE; i++)
        population.emplace_back(genome);

    int generation = 0;
    double spliceSize = 0.50;
    double lastBestScore = 0.0000000001;
    double bestScore = 1;

    Schedule bestSchedule = population.at(0);

    std::priority_queue<Schedule, std::vector<Schedule>, std::greater<>> popRanked; // highest bestScore at the front
    while ((bestScore - lastBestScore) / lastBestScore > 0.01 || generation < MIN_GENS) { 
        lastBestScore = bestScore;

        // Slowly reduce splicing size
        if (generation % 50 == 0)
            spliceSize *= 0.9;

        popRanked = std::priority_queue<Schedule, std::vector<Schedule>, std::greater<>>();

        // Calculate fitness
        for (Schedule& s : population) {
            s.scoreActivities(scoreActivity);
            popRanked.push(s);
        }

        bestScore = popRanked.top().getFitness();
        bestSchedule = popRanked.top();

        // Breed top performers by matching highest and lowest of breeding pool
        std::deque<Schedule> breedingPool;
        for (int i = 0; i < POP_SIZE / 2; i++) {
            breedingPool.push_back(popRanked.top());
            popRanked.pop();
        }
        population.clear();
        while (!breedingPool.empty()) {
            Schedule parent1 = breedingPool.front();
            Schedule parent2 = breedingPool.back();
            breedingPool.pop_front();
            breedingPool.pop_back();

            population.push_back(parent1);
            population.push_back(parent2);

            population.emplace_back(genome, parent1, parent2, spliceSize, MUTATION_RATE);
            population.emplace_back(genome, parent2, parent1, spliceSize, MUTATION_RATE);
        }

        std::cout << "Generation: " << generation << " | Best Score: " << bestScore << std::endl;
        generation++;
    }

    std::ofstream file("../data/schedule.txt");
    bestSchedule.printSchedule(file);

    return 0;
}