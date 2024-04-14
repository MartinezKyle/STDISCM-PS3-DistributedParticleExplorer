import java.awt.*;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.awt.geom.AffineTransform;

import javax.swing.JPanel;
import javax.swing.SwingUtilities;

public class SimulationPanel extends JPanel{
    public List<Particle> particles = Collections.synchronizedList(new ArrayList<Particle>());
    private final int SIMULATION_WIDTH = 1280;
    private final int SIMULATION_HEIGHT = 720;
    private final int THREAD_COUNT = 8;

    private final ExecutorService executorService = Executors.newWorkStealingPool();

    public static int frameCount = 0;
    public static int previousFPS = 0;
    public long lastFPSCheck = 0;

    public List<Explorer> explorers = Collections.synchronizedList(new ArrayList<Explorer>());

    private ParticleSimulationServer server;

    public SimulationPanel(){
        setBounds(50, 50, SIMULATION_WIDTH, SIMULATION_HEIGHT);
        setBackground(Color.WHITE);
        setFocusable(true);
        executorService.execute(() -> {
            while (true) {
                updateSimulation();
                
                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public void setServer(ParticleSimulationServer server){
        this.server = server;
    }

    public void opt1Add(double x, double y, double angle, double velocity){
        Particle particle = new Particle(x, y, velocity, angle);
        this.particles.add(particle);
        particle.setBounds(0,0, 1280,720);
        this.add(particle);
        
        if (server != null && !server.clientHandlers.isEmpty()){
            server.broadcastParticle(particle);
        }
    }

    public void opt2Add(int n, double x1, double y1, double x2, double y2, double angle, double velocity){
        double xDifference = (x2 - x1) / (n - 1);
        double yDifference = (y2 - y1) / (n - 1);
        for (int i = 0; i < n; i++){
            opt1Add((x1 + (i * xDifference)), (y1 + (i * yDifference)), angle, velocity);
        }
    }

    public void opt3Add(int n, double x, double y, double angle1, double angle2, double velocity){
        double angleDifference = (angle2 - angle1) / (n - 1);
        for (int i = 0; i < n; i++){
            opt1Add(x, y, (angle1 + (i * angleDifference)), velocity);
        }
    }

    public void opt4Add(int n, double x, double y, double angle, double velocity1, double velocity2){
        double velocityDifference = (velocity2 - velocity1) / (n - 1);
        for (int i = 0; i < n; i++){
            opt1Add(x, y, angle, (velocity1 + (i * velocityDifference)));
        }
    }

    public void addExplorer(int clientID, double x, double y){
        explorers.add(new Explorer(clientID, x, y));
        SwingUtilities.invokeLater(this::repaint);
    }

    public int explorerExist(int clientID){
        for (int i = 0; i < explorers.size(); i++){
            if (clientID == explorers.get(i).getClientID()){
                return i;
            }
        }
        return -1;
    }

    public void updateExplorer(int id, double x, double y) {
        for (Explorer explorer : explorers) {
            if (explorer.getClientID() == id) {
                explorer.updateCoords(x, y);
                break; 
            }
        }
    }    

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
    
        // Graphics2D g3 = (Graphics2D) g.create();
    
        // if (!isDevMode && explorer != null) {
        //     setBackground(Color.BLACK);
        //     Graphics2D g2 = (Graphics2D) g;

        //     double zoomFactor = 1.94;  
        //     applyZoomAndCenter(g2, explorer.x_coord, explorer.y_coord, zoomFactor);
            
        //     g2.setColor(Color.WHITE);
        //     g2.fillRect(0, 0, getWidth(), getHeight());

        //     for (Particle particle : particles) {
        //         particle.paintComponent(g2);
        //     }
    
        //     explorer.paintComponent(g2);
        // } else {
        //     setBackground(Color.WHITE);
        //     for (Particle particle : particles) {
        //         particle.paintComponent(g);
        //     }

        //     if (explorer != null) {
        //         explorer.paintComponent(g);
        //     }
        // }

        setBackground(Color.WHITE);

        for (Particle particle : particles) {
            particle.paintComponent(g);
        }

        for (Explorer explorer : explorers) {
            explorer.paintComponent(g);
        }

        drawFPSInfo(g);
    }
    
    private void applyZoomAndCenter(Graphics2D g2d, double x, double y, double zoomFactor) {
        int centerX = SIMULATION_WIDTH / 2;
        int centerY = SIMULATION_HEIGHT / 2;
    
        AffineTransform transform = AffineTransform.getTranslateInstance(centerX, centerY);
        transform.scale(zoomFactor, zoomFactor);
        transform.translate(-x, -y);
    
        g2d.setTransform(transform);
    }
    
    private void drawFPSInfo(Graphics g) {
        g.setColor(Color.GREEN);
        g.setFont(new Font("Arial", Font.BOLD, 12));
        long currentTime = System.currentTimeMillis();
        if (currentTime - lastFPSCheck >= 500) {
            frameCount = (int) (frameCount / ((currentTime - lastFPSCheck) / 1000.0));
            g.drawString("FPS: " + frameCount, 10, 20);
            previousFPS = frameCount;
            frameCount = 0;
            lastFPSCheck = currentTime;
        } else {
            g.drawString("FPS: " + previousFPS, 10, 20);
        }
    }
    
    public void updateSimulation(){
        synchronized (this.particles){
            for (Particle particle : this.particles) {
                particle.updatePosition(0.1);
            }
        }
        
        SwingUtilities.invokeLater(this::repaint);

        frameCount++;
    }

    public void removeExplorerById(int idToRemove) {
        synchronized (explorers) { 
            Iterator<Explorer> iterator = explorers.iterator();
            while (iterator.hasNext()) {
                Explorer explorer = iterator.next();
                if (explorer.getClientID() == idToRemove) { 
                    iterator.remove();
                    break; 
                }
            }
        }
    }
    

}
