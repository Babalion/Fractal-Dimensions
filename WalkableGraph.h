//
// Created by chris on 15.05.21.
//
#pragma once

#include <fstream>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

/*
 * A WalkableGraph represents an abstract graph structure on which a walker can travel.
 * This class abstracts all graph-structures where every node has the same number of neighbours.
 * With the help of this class, any such graph can be served in a unified form.
 *
 * You can perform a random walk with specified parameters on a WalkableGraph with `stepsToReturn`
 * and save results to a .tsv file.
 *
 * For a new graph-structure you only have to implement `hopToNeighbour`,
 * where you specify how one can travel from one node to the randomly chosen neighbour n
 */
class WalkableGraph {

public:
    //hops to the neighbour with index n
    // 0 <= n < amountOfNeighbours
    virtual void hopToNeighbour(unsigned int n) = 0;

    //Creates a abstract graph with a given amountOfNeighbours and startLocation
    WalkableGraph(unsigned int amountOfNeighbours, const std::vector<int> &startLoc,std::string filename);

    void resetCoordinates() {
        loc = startLoc;
    }

    [[maybe_unused]] [[nodiscard]] unsigned int getAmountOfNeighbours() const {
        return amountOfNeighbours;
    }

    //generates a large .tsv file with number of steps until return for each random-walk
    void stepsToReturn(uint_fast64_t maxIterations, unsigned int maxSteps);

protected:
    std::vector<int> loc;
    std::string filename;

    [[nodiscard]] const std::vector<int> &getCoordinates() const {
        return loc;
    }

private:
    unsigned int amountOfNeighbours;
    std::vector<int> startLoc;
    std::random_device rd;
    std::mt19937 rng;
    std::uniform_int_distribution<unsigned int> u;

    //let the walker walk until come back
    //returns number of steps until comeback, will break when reached maxSteps
    //returns 0, if never and sets location to start
    uint_fast64_t walkUntilComeBack(uint_fast64_t maxSteps);
};


WalkableGraph::WalkableGraph(unsigned int amountOfNeighbours, const std::vector<int> &startLoc,
                             std::string filename) :
        filename(std::move(filename)), amountOfNeighbours(amountOfNeighbours), startLoc(startLoc), rng(rd()),
        u(0, amountOfNeighbours - 1) {
    loc = startLoc;//copy the initial position
}

uint_fast64_t WalkableGraph::walkUntilComeBack(uint_fast64_t maxSteps) {
    for (uint_fast64_t i = 0; i < maxSteps; ++i) {
        hopToNeighbour(u(rng));
        if (loc == startLoc) {//we're back!
            return static_cast<int>(i);
        }
    }
    resetCoordinates();
    return 0;//walker escaped and has not come back
}

void WalkableGraph::stepsToReturn(uint_fast64_t maxIterations, unsigned int maxSteps) {
    std::ofstream file;
    file.open(filename);
    file << "steps-until-comeback\n";
    for (uint_fast64_t i = 0; i < maxIterations; i++) {
        if ((i + 1) % 5000 == 0) {
            std::cout << "iteration: (" << i + 1 << "/" << maxIterations << ")" << std::endl;
        }
        uint_fast64_t steps = walkUntilComeBack(maxSteps);
        //steps is 0 if walker escaped
        file << steps << std::endl;
    }
    file.close();
}