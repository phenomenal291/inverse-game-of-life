#include "../include/main.h"
#include "../include/GameOfLife.h"
#include "../include/Agents.h"
#include <cmath>
void manual_simulator_test(){
        
    int LENGTH = 1280;
    RenderWindow window(VideoMode(LENGTH, LENGTH), "Game Of Life");
    window.setFramerateLimit(120);
    Font font;
    if (!font.loadFromFile("/home/phuc/fonts/0xProto/0xProtoNerdFont-Bold.ttf")){
        cout << "Can not load font";
        return; 
    }
    Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(24);
    statusText.setFillColor(Color::Magenta);
    statusText.setPosition(10,10);
    GameOfLife simulator = GameOfLife(LENGTH / 64);
    Event event;
    bool paused = false;

    while( window.isOpen()){
        window.clear(Color::Black);
        while(window.pollEvent(event)){

            if ( event.type == Event::Closed) window.close();

            if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

    
            if ( event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                paused = !paused;
            }

            if ( event.type == Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                simulator.restart();
            }
        }

        sf::Vector2i mouse = Mouse::getPosition(window);
        int x = mouse.x / simulator.get_cell_size();
        int y = mouse.y / simulator.get_cell_size();
        if ( Mouse::isButtonPressed(Mouse::Left)){
            simulator.spawn(x,y);
        }
        if ( Mouse::isButtonPressed(Mouse::Right)){
            simulator.kill(x,y);
        }
        if ( !paused)
            simulator.update();
        simulator.draw(window);
        statusText.setString(paused ? "PAUSE" : "RUNNING");
        window.draw(statusText);
        window.display();
        
    }

}

// adding the draw desired map feature, and fitness score display
void single_simulator_test(){

    int LENGTH = 1280;
    RenderWindow window(VideoMode(LENGTH, LENGTH), "Game Of Life");
    window.setFramerateLimit(120);
    Font font;
    if (!font.loadFromFile("/home/phuc/fonts/0xProto/0xProtoNerdFont-Bold.ttf")){
        cout << "Can not load font";
        return; 
    }
    Text statusText;

    statusText.setFont(font);
    statusText.setCharacterSize(24);
    statusText.setFillColor(Color::Magenta);
    statusText.setPosition(10,10);
    GameOfLife simulator = GameOfLife(LENGTH / 64);
    Event event;

    const int MAX_GENERATION = 50;
    bool paused = true;
    bool drawing_desired_map = false;
    Bitboard desired_map;
    while( window.isOpen()){

        string status = "";
        window.clear(Color::Black);

        while(window.pollEvent(event)){
            
            if ( event.type == Event::Closed) window.close();

            if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

    
            if ( event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                paused = !paused;
            }

            if ( event.type == Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                if ( drawing_desired_map){
                    desired_map.restart();
                }
                else
                    simulator.restart();
            }

            if ( event.type == Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                paused = true;
                drawing_desired_map ^= 1;
            }
            if ( event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                simulator.randomize();
            }

        }

        sf::Vector2i mouse = Mouse::getPosition(window);
        int x = mouse.x / simulator.get_cell_size();
        int y = mouse.y / simulator.get_cell_size();
        if ( Mouse::isButtonPressed(Mouse::Left)){
            if ( drawing_desired_map){
                desired_map.setAlive(x,y);
            }
        }
        if ( Mouse::isButtonPressed(Mouse::Right)){
            if ( drawing_desired_map){
                desired_map.setDead(x,y);
            }
        }
        if ( simulator.get_generation() >= MAX_GENERATION){
            paused = true;
            status += "REACHED MAX GENERATION \n";
            status += "FITNESS SCORE: " + to_string(simulator.fitness(desired_map)) + "\n";
        }
        
        if ( drawing_desired_map){
            status += "DRAWING DESIRED MAP \n";
            desired_map.draw(window, simulator.get_cell_size());
        }
        else{
            status += (paused ? "PAUSE" : "SIMULATING");
            status += " \nGENERATION: " + to_string(simulator.get_generation()) + "\n"; 
            simulator.draw(window);
        }
        

        statusText.setString(status);
        if ( !paused && !drawing_desired_map)
            simulator.update();
        window.draw(statusText);
        window.display();
        
    }
    return;
}

void multi_simulator_test(){
    int WIDTH = 1920;
    int LENGTH = 1080;
    RenderWindow window(VideoMode(WIDTH, LENGTH), "Game Of Life");
    window.setFramerateLimit(120);
    Font font;
    if (!font.loadFromFile("/home/phuc/fonts/0xProto/0xProtoNerdFont-Bold.ttf")){
        cout << "Can not load font";
        return; 
    }

    Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(24);
    statusText.setFillColor(Color::Magenta);
    statusText.setPosition(1130,900);

    Text keybindText;
    keybindText.setFont(font);
    keybindText.setCharacterSize(18);
    keybindText.setFillColor(Color::Cyan);
    keybindText.setPosition(1440, 875);
    string keybindStr = "SPACE: PAUSE/RESUME\nR: RESTART\nS: TOGGLE DRAW MAP MODE\nENTER: RANDOMIZE AGENTS";
    const int POPULATION_SIZE = 200;
    const int MAX_GENERATION =  5;
    const int BEST_GRID = 20;
    int max_fitness_score = -1;
    Agents agents = Agents(POPULATION_SIZE, 2);
    Bitboard desired_map;
    Event event;

    bool paused = true;
    bool draw_map = false;
    bool display = true;
    float exact_box_size = 512.0f; 
    float scaled_cell_size = exact_box_size / 64.0f;
    float start_x = 50.0f;
    float start_y = 50.0f;
    int generation = 0;
    int attemps_cnt = 0;
    RenderTexture agentTexture;
    agentTexture.create(WIDTH, LENGTH);
    Sprite agentSprite;

    float max_IoU = 0.0f;
    while( window.isOpen()){

        string status = "";
        window.clear(Color::Black);
        while(window.pollEvent(event)){
            if ( event.type == Event::Closed) window.close();

            if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

    
            if ( event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                paused = !paused;
            }
            if ( event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Y) {
                agents.save("best_agent.txt");
            }
            if ( paused && event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                agents.randomize();
                generation = 0;

                agents.draw(static_cast<sf::RenderTarget&>(agentTexture), WIDTH, LENGTH);
                agentTexture.display();
                agentSprite.setTexture(agentTexture.getTexture());
            }

            if ( draw_map && event.type == Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                desired_map.restart();
            }
            if ( draw_map && event.type == Event::KeyPressed && event.key.code == sf::Keyboard::L) {
                desired_map.load("desired_map.txt");
            }
            if ( event.type == Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                paused = true;
                if ( draw_map ){
                    desired_map.save("desired_map.txt");
                }
                draw_map = !draw_map;

            }
            if ( event.type == Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                display = !display;
            }
        }

        sf::Vector2i mouse = Mouse::getPosition(window);
        int x = (int)floor((mouse.x - start_x) / scaled_cell_size);
        int y = (int)floor((mouse.y - start_y) / scaled_cell_size);

        if (draw_map && Mouse::isButtonPressed(Mouse::Left)){
            if (x >= 0 && x < 64 && y >= 0 && y < 64) 
                desired_map.setAlive(x,y);
        }
        if ( draw_map && Mouse::isButtonPressed(Mouse::Right)){
            if (x >= 0 && x < 64 && y >= 0 && y < 64) 
                desired_map.setDead(x,y);
        }
     
        if ( generation >= MAX_GENERATION){
            if (display) {
                agentTexture.clear(Color::Black);
                agents.draw(static_cast<sf::RenderTarget&>(agentTexture), WIDTH, LENGTH);
                agentTexture.display();
                agentSprite.setTexture(agentTexture.getTexture());
            }
            float local_max_IoU = 0.0f;
            for ( auto& agent: agents.agents){
                local_max_IoU = max( local_max_IoU, agent.IoU(desired_map));
            }
            max_IoU = local_max_IoU;
            max_fitness_score = agents.compute_fitness(desired_map);
            agents.breed(BEST_GRID);
            generation = 0;
            agents.restart();
            attemps_cnt++;
            
        }

        if ( draw_map){
            status += "DRAWING DESIRED MAP";

            RectangleShape boundary(Vector2f(exact_box_size, exact_box_size));
            boundary.setFillColor(Color::Transparent);
            boundary.setOutlineColor(Color::White);
            boundary.setOutlineThickness(2);
            boundary.setPosition(start_x, start_y);
            window.draw(boundary);
            desired_map.draw(window, scaled_cell_size, start_x, start_y);
        }
        else{

            status += (paused ? "PAUSE\n" : "SIMULATING\n");
            status += "FITNESS SCORE: " + to_string(max_fitness_score) + "\n" ; 
            status += "IoU METRIC: " + to_string(max_IoU) + "\n";
            status += "ATTEMPS: " + to_string(attemps_cnt) + "\n";
            
            window.draw(agentSprite);
            if ( !paused ){
                agents.update();
                generation++;
                
            }

        }   
        
        if ( draw_map){
            keybindStr = "\nLEFT CLICK: SET CELL ALIVE\nRIGHT CLICK: SET CELL DEAD\nR: RESTART MAP\nL: LOAD MAP\nS: EXIT AND SAVE MAP";
        }
        else
            keybindStr = "\nSPACE: PAUSE/RESUME\nS: TOGGLE DRAW MAP MODE\nD: FREEZE DISPLAY\nENTER: RANDOMIZE\nY: SAVE BEST AGENT";
        keybindText.setString(keybindStr);
        statusText.setString(status);
        window.draw(keybindText);
        window.draw(statusText);
        window.display();

    }
}

int main(){
    // manual_simulator_test();
    // single_simulator_test();
    multi_simulator_test();
}
