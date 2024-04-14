import java.awt.CardLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import java.awt.FlowLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;
import javax.swing.JToggleButton;

public class ParticleSimulation extends JFrame{
    //private final ExecutorService executorService;
    public final SimulationPanel simulationPanel;
    private final JPanel particleControlPanel = new JPanel(null);
    private final JPanel particleFieldControlPanel = new JPanel(null);
    private final JPanel controlPanel = new JPanel(null);
    private JComboBox<String> addParticleDropdown;
    private JButton particleAddButton;
    private JTextField pX1 = new JTextField();
    private JTextField pY1 = new JTextField();
    private JTextField pV1 = new JTextField();
    private JTextField pA1 = new JTextField();
    private JTextField pX2 = new JTextField();
    private JTextField pY2 = new JTextField();
    private JTextField pV2 = new JTextField();
    private JTextField pA2 = new JTextField();
    private JTextField pN = new JTextField();
    private JCheckBox opt2 = new JCheckBox();
    private JCheckBox opt3 = new JCheckBox();
    private JCheckBox opt4 = new JCheckBox();
    
    public ParticleSimulation(){
        super("Particle Simulation");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1680, 920);
        setResizable(false);
        setBackground(Color.BLACK);
        setLayout(null);
        //executorService = Executors.newFixedThreadPool(THREAD_COUNT);
        simulationPanel = new SimulationPanel();
        add(simulationPanel);
        createControlPanel();
        add(controlPanel);
    }

    public void createControlPanel(){
        controlPanel.setBounds(1380, 50, 250, 720);
        controlPanel.setLayout(new BoxLayout(controlPanel, BoxLayout.Y_AXIS));

        particleControlPanel.setLayout(new BoxLayout(particleControlPanel, BoxLayout.Y_AXIS));
        particleFieldControlPanel.setLayout(new GridLayout(0,2));
        particleFieldControlPanel.add(new JLabel("X1:"));
        particleFieldControlPanel.add(pX1);
        particleFieldControlPanel.add(new JLabel("Y1:"));
        particleFieldControlPanel.add(pY1);
        particleFieldControlPanel.add(new JLabel("Angle:"));
        particleFieldControlPanel.add(pA1);
        particleFieldControlPanel.add(new JLabel("Velocity:"));
        particleFieldControlPanel.add(pV1);
        particleFieldControlPanel.add(new JLabel("No. of Particles"));
        pN.setEnabled(false);
        particleFieldControlPanel.add(pN);
        particleFieldControlPanel.add(new JLabel(String.format("<html><body style=\"text-align: left;  \">%s</body></html>","Multiple Particle, Varying Start:")));
        opt2.setSelected(false);
        opt2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                disableMultiples();
                if (opt2.isSelected()){
                    opt3.setSelected(false);
                    opt4.setSelected(false);
                }
                pN.setEnabled(opt2.isSelected());
                pX2.setEnabled(opt2.isSelected());
                pY2.setEnabled(opt2.isSelected());
            }
        });
        particleFieldControlPanel.add(opt2);
        particleFieldControlPanel.add(new JLabel("X2:"));
        pX2.setEnabled(false);
        particleFieldControlPanel.add(pX2);
        particleFieldControlPanel.add(new JLabel("Y2:"));
        pY2.setEnabled(false);
        particleFieldControlPanel.add(pY2);

        particleFieldControlPanel.add(new JLabel(String.format("<html><body style=\"text-align: left;  \">%s</body></html>","Multiple Particle, Varying Angle")));
        opt3.setSelected(false);
        opt3.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                disableMultiples();
                if (opt3.isSelected()){
                    opt2.setSelected(false);
                    opt4.setSelected(false);
                }
                pN.setEnabled(opt3.isSelected());
                pA2.setEnabled(opt3.isSelected());
            }
        });
        particleFieldControlPanel.add(opt3);
        particleFieldControlPanel.add(new JLabel("Angle2:"));
        pA2.setEnabled(false);
        particleFieldControlPanel.add(pA2);

        particleFieldControlPanel.add(new JLabel(String.format("<html><body style=\"text-align: left;  \">%s</body></html>","Multiple Particle, Varying Velocity")));
        opt4.setSelected(false);
        opt4.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                disableMultiples();
                if (opt4.isSelected()){
                    opt2.setSelected(false);
                    opt3.setSelected(false);
                }
                pN.setEnabled(opt4.isSelected());
                pV2.setEnabled(opt4.isSelected());
            }
        });
        particleFieldControlPanel.add(opt4);
        particleFieldControlPanel.add(new JLabel("Velocity2:"));
        pV2.setEnabled(false);
        particleFieldControlPanel.add(pV2);
        
        particleAddButton = new JButton("Add Particle");
        particleAddButton.setSize(230, 20);
        particleAddButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(java.awt.event.ActionEvent e){
                double x1 = Double.parseDouble(pX1.getText());
                double y1 = Double.parseDouble(pY1.getText());
                double a1 = Double.parseDouble(pA1.getText());
                double v1 = Double.parseDouble(pV1.getText());
                if (!opt2.isSelected()&&!opt3.isSelected()&&!opt4.isSelected()){
                    simulationPanel.opt1Add(x1, y1, a1, v1);
                }
                else{
                    int n = Integer.parseInt(pN.getText());
                    if(opt2.isSelected()){
                        double x2 = Double.parseDouble(pX2.getText());
                        double y2 = Double.parseDouble(pY2.getText());
                        simulationPanel.opt2Add(n, x1, y1, x2, y2, a1, v1);
                    }
                    else if(opt3.isSelected()){
                        double a2 = Double.parseDouble(pA2.getText());
                        simulationPanel.opt3Add(n, x1, y1, a1, a2, v1);
                    }
                    else if(opt4.isSelected()){
                        double v2 = Double.parseDouble(pV2.getText());
                        simulationPanel.opt4Add(n, x1, y1, a1, v1, v2);
                    }
                }
                clearAllParticleFields();
            }
        });
        particleControlPanel.add(particleFieldControlPanel);
        particleControlPanel.add(particleAddButton);

        controlPanel.add(particleControlPanel);
    }

    public void disableMultiples(){
        pX2.setEnabled(false);
        pY2.setEnabled(false);
        pA2.setEnabled(false);
        pV2.setEnabled(false);
        pN.setEnabled(false);
    }

    public void clearAllParticleFields(){
        pX1.setText("");
        pY1.setText("");
        pA1.setText("");
        pV1.setText("");
        pX2.setText("");
        pY2.setText("");
        pA2.setText("");
        pV2.setText("");
        pN.setText("");
    }
}
