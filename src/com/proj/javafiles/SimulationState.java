import java.io.Serializable;
import java.util.List;

public class SimulationState implements Serializable {
    private List<ParticleState> particleStates;
    private List<ExplorerState> explorerStates;

    public SimulationState(List<ParticleState> particleStates) {
        this.particleStates = particleStates;
        this.explorerStates = null; // Initialize explorerStates to null
    }

    public SimulationState(List<ExplorerState> explorerStates, boolean dummy) {
        this.particleStates = null; // Initialize particleStates to null
        this.explorerStates = explorerStates;
    }

    // Getters
    public List<ParticleState> getParticles() {
        return particleStates;
    }

    public List<ExplorerState> getExplorers() {
        return explorerStates;
    }
}
