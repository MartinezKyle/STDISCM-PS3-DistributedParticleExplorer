import javax.swing.JComponent;
import java.awt.*;

public class Explorer extends JComponent {
    public double x_coord;
    public double y_coord;
    public int clientID;

    public Explorer(int ID, double x, double y){
        this.clientID = ID;
        this.x_coord = x;
        this.y_coord = y;
        this.setSize(20, 20); 
        this.setBounds(0,0, 1280,720);
        repaint();
    }

    public double getXCoord(){
        return x_coord;
    }

    public double getYCoord() {
        return y_coord;
    }

    public int getClientID(){ return clientID; }

    public void updateCoords(double x, double y){
        x_coord = x;
        y_coord = y;
    }

    @Override
    protected void paintComponent(Graphics g){
        super.paintComponent(g);
        g.setColor(Color.BLUE);
        g.fillOval((int) x_coord, (int) y_coord, 20, 20);
    }
}
