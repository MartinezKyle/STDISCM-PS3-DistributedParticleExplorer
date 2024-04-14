public class ParticleState {
    private double x_coord;
    private double y_coord;
    private double velocity;
    private double angle;

    public ParticleState(double x, double y, double velocity, double angle) {
        this.x_coord = x;
        this.y_coord = y;
        this.velocity = velocity;
        this.angle = angle;
    }

    // Getters
    public double getXCoord() {
        return x_coord;
    }

    public double getYCoord() {
        return y_coord;
    }

    public double getVelocity() {
        return velocity;
    }

    public double getAngle() {
        return angle;
    }

    // You might want setters if you plan on modifying the state after creation
}
