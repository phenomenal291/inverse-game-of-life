#pragma once
#include "main.h"
class Bitboard{
    vector<uint64_t> board;
public:
    const int size = 64;
    Bitboard();
    Bitboard(const Bitboard& other) : board(other.board) {}
    uint64_t getRow(int y) const;
    void setRow(int y, uint64_t row);
    const uint64_t operator[](int y) const;
    uint64_t& operator[](int y);
    Bitboard& operator=(const Bitboard& other);
    void setAlive(int x, int y);
    void setDead(int x, int y);
    bool isAlive(int x, int y);
    void draw(RenderWindow& window, float cell_size, float offset_x = 0.0f, float offset_y = 0.0f);
    void restart();

    void save(const string& filename);
    void load(const string& filename);
};

