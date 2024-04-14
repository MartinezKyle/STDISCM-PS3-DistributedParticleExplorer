public class ExplorerState {
    private double x_coord;
    private double y_coord;
    private int client_id;

    public ExplorerState(int client_id, double x, double y) {
        this.client_id = client_id;
        this.x_coord = x;
        this.y_coord = y;
    }

    public double getXCoord() {
        return x_coord;
    }

    public double getYCoord() {
        return y_coord;
    }

    public int getClientID(){ return client_id; }

}
