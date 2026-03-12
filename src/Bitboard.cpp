#include "../include/Bitboard.h"
Bitboard::Bitboard(){
    board.resize(64, 0);
}


uint64_t Bitboard::getRow(int y) const{
    return board[y];
}


const uint64_t Bitboard::operator[](int y) const{
    return board[y];
}
uint64_t& Bitboard::operator[](int y){
    return board[y];
}

Bitboard& Bitboard::operator=(const Bitboard& other){
    this->board = other.board;
    return *this;
}
void Bitboard::setRow(int y, uint64_t row){
    this->board[y] = row;
}
void Bitboard::setAlive(int x, int y){
    board[y] |= (1ULL << x);
}

void Bitboard::setDead(int x, int y){
    board[y] &= ~(1ULL << x);
}

bool Bitboard::isAlive(int x, int y){
    return ( board[y] & (1ULL << x) ) != 0 ;
}

void Bitboard::draw(RenderWindow& window, float cell_size, float offset_x, float offset_y){

    for ( int row = 0 ; row < size; row++){
        for ( int col =0 ; col < size; col++){
            
            if ( this->isAlive(col, row) ){
                RectangleShape cell(Vector2f(cell_size, cell_size));
                float x_pos = offset_x + (col * cell_size);
                float y_pos = offset_y + (row * cell_size);
                
                cell.setPosition(x_pos, y_pos);
                cell.setFillColor(Color::Green);
                window.draw(cell);
            }
        }
    }
    return;
}

void Bitboard::restart(){
    for ( int i = 0 ; i < size; i++){
        this->board[i] = 0ULL;
    }
}

void Bitboard::save(const string& filename){
    ofstream file(filename, ios::in);
    if (file.is_open()){
        for ( int i = 0 ; i < size; i++){
            file << board[i] << endl;
        }
        file.close();
    } else {
        cerr << "Error opening file for writing: " << filename << endl;
    }
}

void Bitboard::load(const string& filename){
    ifstream file(filename);
    if (file.is_open()){
        for ( int i = 0 ; i < size; i++){
            file >> board[i];
        }
        file.close();
    } else {
        cerr << "Error opening file for reading: " << filename << endl;
    }
}
