import java.awt.Color;
import java.awt.Graphics;
import java.util.List;
import java.util.ArrayList;
import java.math.BigDecimal;
import java.math.RoundingMode;

import javax.swing.JComponent;
import javax.swing.SwingUtilities;

public class Particle extends JComponent{
    private double x_coord;
    private double y_coord;
    private double velocity;
    private double angle;

    public Particle(double x, double y, double velocity, double angle){
        this.x_coord = x;
        this.y_coord = y;
        this.angle = angle;
        this.velocity = velocity;
        repaint();
    }

    public void updatePosition(double time){
        double x2 = x_coord + getVelocityX() * time;
        double y2 = y_coord + getVelocityY() * time;
        if (x2 <= 0 || x2 >= 1280){
            this.angle = 180 - angle; 
        }
        if (y2 <= 0 || y2 >= 720){
            this.angle = - angle;
        }
        x_coord += getVelocityX() * time;
        y_coord += getVelocityY() * time;
        repaint();
    }

    public double getXCoord(){
        return x_coord;
    }

    public double getYCoord() {
        return y_coord;
    }

    public double getAngle() {
        return angle;
    }

    public double getVelocity() {
        return velocity;
    }

    public double getVelocityX() {
        double result = velocity * Math.cos(Math.toRadians(angle));
        return BigDecimal.valueOf(result)
                         .setScale(4, RoundingMode.HALF_UP)
                         .doubleValue();
    }
    
    public double getVelocityY() {
        double result = velocity * Math.sin(Math.toRadians(angle));
        return BigDecimal.valueOf(result)
                         .setScale(4, RoundingMode.HALF_UP)
                         .doubleValue();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.setColor(Color.RED);
        g.fillOval((int) x_coord - 5, 720 - (int)y_coord - 5, 10, 10);
    }
}
