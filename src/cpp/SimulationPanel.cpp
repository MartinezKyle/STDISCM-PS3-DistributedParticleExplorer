#include "SimulationPanel.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <boost/thread.hpp>
#include <mutex>
#include <memory>
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>

#include "Particle.hpp"
#include "Explorer.hpp"
#include <corecrt_math_defines.h>

using json = nlohmann::json;

SimulationPanel::SimulationPanel() {
    explorer = nullptr;
    frameCount = 0;
    previousFPS = 0;
    lastFPSCheck = std::chrono::high_resolution_clock::now();
    particles.reserve(1000);
    explorers.reserve(5);

    if (!font.loadFromFile("../../lib/calibri.ttf")) {
        std::cerr << "Failed to load font file" << std::endl;
    }
}

void SimulationPanel::parseJSONToParticles(const json& jsonData , long elapsedTime) {
    if (jsonData.is_array()) {
        for (const auto& obj : jsonData) {
            double angle = obj.at("angle").get<double>();
            double velocity = obj.at("velocity").get<double>();
            double xcoord = obj.at("xcoord").get<double>();
            double ycoord = obj.at("ycoord").get<double>();

            particles.push_back(std::make_shared<Particle>(xcoord, ycoord, velocity, angle));
        }
    } else {
        double angle = jsonData["angle"];
        double velocity = jsonData["velocity"];
        double xcoord = jsonData["xcoord"];
        double ycoord = jsonData["ycoord"];

        double angleRadians = angle * M_PI / 180;
        double Time = static_cast<double>(elapsedTime) / 1000;
        double displacement = velocity * Time;
        double NewX = xcoord + (displacement * cos(angleRadians));
        double NewY = ycoord + (displacement * sin(angleRadians));

        std::cout << "Elapsed Time: " << Time << std::endl;
        std::cout << "NewX: " << NewX << " NewY: " << NewY << std::endl;

        particles.push_back(std::make_shared<Particle>(NewX, NewY, velocity, angle));
    }
}

void SimulationPanel::parseJSONToExplorers(const json& jsonData, const std::string& type) {
    auto updateOrAddExplorer = [this](int id, double xcoord, double ycoord) {
        auto it = std::find_if(explorers.begin(), explorers.end(),
            [id](const std::shared_ptr<Explorer>& explorer) { return explorer->getID() == id; });

        if (it != explorers.end()) {
            (*it)->updateCoords(xcoord, ycoord);
        } else {
            explorers.push_back(std::make_shared<Explorer>(id, xcoord, ycoord));
        }
    };

    auto removeExplorer = [this](int id) {
        auto it = std::find_if(explorers.begin(), explorers.end(),
            [id](const std::shared_ptr<Explorer>& explorer) { return explorer->getID() == id; });

        if (it != explorers.end()) {
            explorers.erase(it);
        } 
    };

    if (type == "add"){
        if (jsonData.is_array()) {
            for (const auto& obj : jsonData) {
                int id = obj.at("clientID").get<int>();
                double xcoord = obj.at("xcoord").get<double>();
                double ycoord = obj.at("ycoord").get<double>();

                updateOrAddExplorer(id, xcoord, ycoord);
            }
        } else {
            int id = jsonData["clientID"].get<int>();
            double xcoord = jsonData["xcoord"].get<double>();
            double ycoord = jsonData["ycoord"].get<double>();

            updateOrAddExplorer(id, xcoord, ycoord);
        }

        std::cout << "Explorers size: " << explorers.size() << std::endl;
    } else {
        if (jsonData.is_array()) {
            for (const auto& obj : jsonData) {
                int id = obj.at("clientID").get<int>();

                removeExplorer(id);
            }
        } else {
            int id = jsonData["clientID"].get<int>();

            removeExplorer(id);
        }
    }
}

void SimulationPanel::addExplorer(int ID, double x, double y) {
    explorer = std::make_shared<Explorer>(ID, x, y);
    std::cout << "explorer move: " << explorer->getMove() << std::endl;
}

const std::shared_ptr<Explorer>& SimulationPanel::getExplorer() const {
    return explorer;
}

void SimulationPanel::updateSimulation() {
    for (auto& particle : particles) {
        particle->updatePosition(0.1);
    }

    frameCount++;
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFPSCheck).count();

    if (timeDiff >= 500) {
        previousFPS = frameCount / (timeDiff / 1000.0);
        frameCount = 0;
        lastFPSCheck = currentTime; 
    }
}

void SimulationPanel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::View originalView = target.getView();
    
    target.clear(sf::Color::White);

    for (const auto& particle : particles) {
        target.draw(*particle);
    }

    for (const auto& others : explorers) {
        target.draw(*others);
    }

    if (explorer) {
        target.draw(*explorer);
    }
    
    target.setView(target.getDefaultView());
    
    drawFPSInfo(target);

    target.setView(originalView);   
}


void SimulationPanel::drawFPSInfo(sf::RenderTarget& target) const {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::Green);
    text.setPosition(10, 20);

    text.setString("FPS: " + std::to_string(static_cast<int>(previousFPS)));

    target.draw(text);
}

