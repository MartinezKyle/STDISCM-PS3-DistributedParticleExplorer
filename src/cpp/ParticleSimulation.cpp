#include "ParticleSimulation.hpp"

ParticleSimulation::ParticleSimulation() : simulationPanel() {
    simulationPanel.setPosition(50, 50);
}

void ParticleSimulation::updateSimulationLoop() {
    while (isRunning) {
        simulationPanel.updateSimulation();
        
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }
}

void ParticleSimulation::run() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Simulation Panel");
    window.clear(sf::Color::Black);
    float borderThickness = 500.0f;
    sf::RectangleShape borderRect;
    borderRect.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    borderRect.setFillColor(sf::Color::Transparent);
    borderRect.setOutlineColor(sf::Color::Black);
    borderRect.setOutlineThickness(borderThickness);
    borderRect.setPosition(0, 0);

    window.clear();
    window.draw(simulationPanel);
    window.draw(borderRect);

    // Display the contents of the RenderWindow
    window.display();
    std::shared_ptr<Explorer> explorer = nullptr;

    while (window.isOpen() && isRunning) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                isRunning = false;

            }
            else if (event.type == sf::Event::MouseButtonPressed && simulationPanel.getExplorer() == nullptr) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    auto mousePos = sf::Mouse::getPosition(window);
                    simulationPanel.addExplorer(this->ID, mousePos.x, mousePos.y);

                    explorer = simulationPanel.getExplorer();
                    std::cout << "Added explorer at: " << mousePos.x << ", " << mousePos.y << std::endl;
                }
            }
            else if (event.type == sf::Event::KeyPressed && explorer != nullptr) {
                if (explorer){
                    if (event.key.code == sf::Keyboard::W) {
                        std::cout << "W pressed" << std::endl;
                        explorer->moveUp();
                    }
                    else if (event.key.code == sf::Keyboard::S) {
                        std::cout << "S pressed" << std::endl;
                        explorer->moveDown();
                    }
                    else if (event.key.code == sf::Keyboard::A) {
                        std::cout << "A pressed" << std::endl;
                        explorer->moveLeft();
                    }
                    else if (event.key.code == sf::Keyboard::D) {
                        std::cout << "D pressed" << std::endl;
                        explorer->moveRight();
                    }
                }
            }

            if (explorer != nullptr && explorer->getMove()){
                applyZoomAndCenter(window, explorer->getXCoord(), explorer->getYCoord());
            }
        }

        window.clear(sf::Color::White);
        window.draw(simulationPanel);
        window.draw(borderRect);
        window.display();
    }
}

void ParticleSimulation::applyZoomAndCenter(sf::RenderWindow& window, double x, double y) {
    sf::Vector2u windowSize = window.getSize();
    sf::View view(sf::FloatRect(0, 0, windowSize.x, windowSize.y));
    
    view.setCenter(sf::Vector2f(x + 10, y + 10));    
    view.zoom(1 / zoomFactor);
    window.setView(view);
}

void ParticleSimulation::setID(const json& jsonData) { 
    if (!jsonData.is_object()) {
        std::cerr << "Expected jsonData to be an object, got: " << jsonData.type_name() << std::endl;
        return;
    }

    if (jsonData.contains("clientID") && jsonData["clientID"].is_number()) {
        this->ID = jsonData["clientID"];
    } else {
        std::cerr << "Missing or invalid 'clientID' in jsonData." << std::endl;
    }
}

void ParticleSimulation::addParticle(const json& jsonData, long elapsedTime) {
    simulationPanel.parseJSONToParticles(jsonData, elapsedTime);
}

void ParticleSimulation::addOtherExplorer(const json& jsonData){
    simulationPanel.parseJSONToExplorers(jsonData, "add");
}

void ParticleSimulation::removeExplorer(const json& jsonData){
    simulationPanel.parseJSONToExplorers(jsonData, "remove");
}

void ParticleSimulation::setIsRunning(){
    isRunning = false;
}

bool ParticleSimulation::getIsRunning() const {
    return isRunning.load();
}

SimulationPanel& ParticleSimulation::getSimulationPanel(){
    return simulationPanel;
}


