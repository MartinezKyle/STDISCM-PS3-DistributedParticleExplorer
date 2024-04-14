#ifndef EXPLORER_H
#define EXPLORER_H

#include <SFML/Graphics.hpp>

class Explorer : public sf::Drawable {
public:
    Explorer(int clientID, double x, double y);

    double getXCoord() const;
    double getYCoord() const;
    double getID() const;
    bool getMove();

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    
    void revertMove();
    void updateCoords(double x, double y);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    double x_coord;
    double y_coord;
    sf::CircleShape shape;
    std::atomic<bool> moved;
    int clientID;
    int radius = 10;
};

#endif // EXPLORER_H
