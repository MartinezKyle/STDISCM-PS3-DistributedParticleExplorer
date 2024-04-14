#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>

class Particle : public sf::Drawable {
public:
    Particle(double x, double y, double velocity, double angle);

    void updatePosition(double time);

    double getXCoord() const;
    double getYCoord() const;
    double getAngle() const;
    double getVelocity() const;
    double getVelocityX() const;
    double getVelocityY() const;
    
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    double x_coord;
    double y_coord;
    double velocity;
    double angle;
    sf::CircleShape shape;
};

#endif // PARTICLE_H
