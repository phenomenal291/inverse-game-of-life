#pragma once
#include "main.h"
#include "Bitboard.h"

class GameOfLife{
    
    Bitboard seed;
    Bitboard grid;
    const int N = 64;
    int size = 10;
    int generation = 0;
    int fitness_score = 0;
public:
    GameOfLife();
    GameOfLife(int size = 10);
    void spawn(int x, int y);
    int get_cell_size() const;
    int get_generation() const;
    int get_fitness_score() const;
    uint64_t get_row(int y) const;
    GameOfLife& operator=(const GameOfLife &other);
    void kill(int x, int y);
    void update();
    void draw(RenderTarget &window, float offset_x = 0.0f, float offset_y = 0.0f);
    void restart();
    void randomize();
    GameOfLife crossover(const GameOfLife &other);
    void mutate(float mutation_rate = 0.01f);
    int fitness(const Bitboard &desired_grid);
    float IoU(const Bitboard& other);
    void save(const string& filename);
};
