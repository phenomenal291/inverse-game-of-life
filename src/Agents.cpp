#include "../include/Agents.h"

Agents::Agents(){
    population_size = 0;
    this->cell_size = 10;
}

Agents::Agents(int population_size, int cell_size){
    this->population_size = population_size;
    this->cell_size = cell_size;
    for ( int i = 0; i < population_size; i++){
        agents.push_back(GameOfLife(cell_size));
    }
}

int Agents::get_max_fitness_score() const{
    return this->max_fitness_score;
}

void Agents::update(){
    for ( auto &agent : agents){
        agent.update();
    }
}

void Agents::randomize(){
   for ( auto &agent : agents){
        agent.randomize();
    }
}

int Agents::compute_fitness(const Bitboard &desired_grid){
    this->max_fitness_score = 0;
    for ( GameOfLife &agent : agents){
        this->max_fitness_score = max(agent.fitness(desired_grid), this->max_fitness_score);
    }
    return this->max_fitness_score;
}

void Agents::breed(const int& BEST_GRID){

    vector<GameOfLife> new_agents;
    
    sort(agents.begin(), agents.end(), [](const GameOfLife &a, const GameOfLife &b){
        return a.get_fitness_score() > b.get_fitness_score();
    });

    for ( int i = 0; i < BEST_GRID; i++){
        new_agents.push_back(agents[i]);
    }

    auto run_tournament = [&]() -> GameOfLife {
        int best_gladiator_index = rand() % population_size;
        
        for(int k = 0; k < 3; k++) { 
            int challenger = rand() % population_size;
            if(agents[challenger].get_fitness_score() > agents[best_gladiator_index].get_fitness_score()) {
                best_gladiator_index = challenger;
            }
        }
        return agents[best_gladiator_index];
    };

    for ( int i = 0; i < population_size - BEST_GRID; i++){
        
        GameOfLife parentA = run_tournament();
        GameOfLife parentB = run_tournament();
        
        GameOfLife child = parentA.crossover(parentB);
        child.mutate(0.05f); 
        
        new_agents.push_back(child);
    }

    this->agents = new_agents;
}

void Agents::draw(RenderTarget& window,const int& WIDTH, const int& LENGTH){

    float padding = 5.0f;
    int columns = WIDTH / (64* cell_size + padding);
    for (int i = 0; i < population_size; i++){

        int col = i % columns;
        int row = i / columns;

        float offset_x = col * (64* cell_size + padding);
        float offset_y = row * (64* cell_size + padding);

        agents[i].draw(window, offset_x, offset_y);
    }
}

void Agents::restart(){
    for ( auto &agent : agents){
        agent.restart();
    }
}
void Agents::save(const string& filename){
    if ( agents.empty()) return;
    agents[0].save(filename);
}
