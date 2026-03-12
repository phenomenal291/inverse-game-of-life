#include "../include/GameOfLife.h"
#include "../include/Bitboard.h"
#include <cstdlib>


inline void half_add(uint64_t& a, uint64_t& b, uint64_t& sum, uint64_t& carry){
    sum = a ^ b;
    carry = (a & b);

}

inline void full_add(uint64_t& a, uint64_t& b, uint64_t& carry_int, uint64_t& sum, uint64_t& carry_out){
    uint64_t s1 = a ^ b;
    uint64_t c1 = a & b;
    sum = s1 ^ carry_int;
    carry_out = c1 | (s1 & carry_int);
}

GameOfLife::GameOfLife() : size(2), generation(0), fitness_score(0), grid(Bitboard()), seed(Bitboard()) {}
GameOfLife::GameOfLife(int size) : size(size), grid(Bitboard()){}
int GameOfLife::get_cell_size() const{
    return this->size;
}

uint64_t GameOfLife::get_row(int y) const{
    return this->grid.getRow(y);
}

int GameOfLife::get_generation() const{
    return this->generation;
}

int GameOfLife::get_fitness_score() const{
    return this->fitness_score;
}

GameOfLife& GameOfLife::operator=(const GameOfLife &other){
        this->grid = other.grid;
        this->seed = other.seed;
        this->size = other.size;
        this->generation = other.generation;
        this->fitness_score = other.fitness_score;
        return *this;
    }                                     
void GameOfLife::spawn(int x, int y){
    this->grid.setAlive(x,y);
}
    
void GameOfLife::kill(int x, int y){
    this->grid.setDead(x, y);
}

void GameOfLife::update(){
    Bitboard old_grid(this->grid);

    for ( int i = 0 ; i < N; i++){

        uint64_t top_row = old_grid.getRow((i-1+N)%N);
        uint64_t mid_row = old_grid.getRow(i);
        uint64_t bot_row = old_grid.getRow((i+1)%N);

        uint64_t top_left = ( top_row >> 1), top_center = (top_row), top_right = (top_row << 1); 
        uint64_t mid_left = ( mid_row >> 1),                      mid_right = (mid_row << 1);
        uint64_t bot_left = ( bot_row >> 1), bot_center = (bot_row)    , bot_right = (bot_row << 1);

        uint64_t bit0, bit1, bit2, bit3, carry_sums; 

        uint64_t sum_top, carry_top, sum_mid, carry_mid, sum_bot, carry_bot;

        full_add(top_left, top_center, top_right, sum_top, carry_top);
        half_add(mid_left, mid_right, sum_mid, carry_mid);
        full_add(bot_left, bot_center, bot_right, sum_bot, carry_bot);


        full_add(sum_top, sum_mid, sum_bot, bit0, carry_sums);
        
        
        uint64_t sum_c1, carry_c1, carry_c2;
        full_add(carry_top, carry_mid, carry_bot, sum_c1, carry_c1);
        half_add(sum_c1, carry_sums, bit1, carry_c2);


        half_add(carry_c1, carry_c2, bit2, bit3);

        uint64_t exact_3 = bit0 & bit1 & ~bit2 & ~bit3;
        uint64_t exact_2 = ~bit0 & bit1 & ~bit2 & ~bit3;
    
        this->grid.setRow(i, (exact_3 | (exact_2 & mid_row)) );
    }
    this->generation++;
}

void GameOfLife::draw(RenderTarget& window, float offset_x, float offset_y){

    for ( int row = 0 ; row < N; row++){
        for ( int col =0 ; col < N; col++){
            
            if ( this->grid.isAlive(col, row) == 1){
                RectangleShape cell(Vector2f(this->size, this->size));
                float x_pos = offset_x + (col * this->size);
                float y_pos = offset_y + (row * this->size);
                
                cell.setPosition(x_pos, y_pos);
                cell.setFillColor(Color::Green);
                window.draw(cell);
            }
        }
    }
    return;
}

void GameOfLife::restart(){
    this->grid = this->seed;
    this->generation = 0;
}
void GameOfLife::randomize(){

    static random_device rd;
    static mt19937_64 generator(rd());
    uniform_int_distribution<uint64_t> distribution(0,UINT64_MAX);
    for ( int i = 0 ; i < N; i++){
        uint64_t random_row = distribution(generator) & distribution(generator) & distribution(generator);        
        this->grid.setRow(i, distribution(generator) );
        this->seed.setRow(i, this->grid.getRow(i));
    }
    return;
}

GameOfLife GameOfLife::crossover(const GameOfLife &other){
    
   GameOfLife child(this->size);
    int x_start = rand() % N;
    int y_start = rand() % N;
    int box_width = (rand() % 20) + 5;
    int box_height = (rand() % 20) + 5;
    if ( x_start + box_width >= N) box_width = N - x_start;
    if ( y_start + box_height >= N) box_height = N - y_start;
    //     this create consequence of '1': '011111..'
    uint64_t bitmask =  ((1ULL << box_width) - 1)         << x_start;

    for ( int i = 0 ; i < N; i++){
        if ( y_start <= i && i <= y_start + box_height){
            uint64_t spliced_row = (this->seed[i] & ~bitmask) | (other.seed.getRow(i) & bitmask);
            child.seed.setRow(i, spliced_row);
            child.grid.setRow(i, spliced_row);
        }
        else{
            child.seed.setRow(i, this->seed[i]);
            child.grid.setRow(i, this->seed[i]);
        }
    }
    return child;
}


void GameOfLife::mutate(float mutation_rate){
    
    for ( int i = 0 ; i < N; i ++){

        float rate = rand() / (float)RAND_MAX; 
        if ( rate < mutation_rate){
            int x  = rand() % N;
            uint64_t bitmask ( 1ULL << x);
            this->seed[i] ^= bitmask;
            this->grid[i] ^= bitmask;
        }
    }
}

int GameOfLife::fitness(const Bitboard &desired_grid) {
    int score = 0;
    
    for ( int i = 0 ; i < N; i++){
        uint64_t my_row = this->grid[i];
        uint64_t target_row = desired_grid[i];

        uint64_t true_positives  = my_row & target_row;
        uint64_t true_negatives  = ~my_row & ~target_row; 
        
        score += __builtin_popcountll(true_positives) * 500; 
        score += __builtin_popcountll(true_negatives) * 1;   
    }

    this->fitness_score = score; 
    return score;
}

float GameOfLife::IoU(const Bitboard& other){
    float iou = 0.0f;
    int intersection_count = 0, union_count = 0;
    for (int i = 0; i < N; i++){
        uint64_t overlap = this->grid[i] & other[i];
        uint64_t total = other[i] | this->grid[i];

        intersection_count += __builtin_popcountll(overlap);
        union_count += __builtin_popcountll(total);
    }
    if ( union_count == 0) return 1.0f;
    iou = (float)intersection_count / union_count;
    return iou;

}

void GameOfLife::save(const string& filename){
    this->seed.save(filename);
}
