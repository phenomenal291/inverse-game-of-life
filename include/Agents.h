#pragma once
#include "GameOfLife.h"
#include "main.h"
class Agents{

public: 
    vector<GameOfLife> agents;
    
    int population_size;
    int max_fitness_score = 0;
    int cell_size = 10;
    Agents();
    Agents(int population_size, int cell_size);
    int get_max_fitness_score() const;
    void update();
    void randomize();
    void breed(const int& BEST_GRID);
    int compute_fitness(const Bitboard &desired_grid);
    void draw(RenderTarget& window,const int& WIDTH, const int& LENGTH);
    void restart();
    void save(const string& filename);
};
