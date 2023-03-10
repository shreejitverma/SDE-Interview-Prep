package models;

import java.util.HashMap;
import java.util.Map;

public class PaymentGraph {
    private final Map<String, BalanceMap> graph;

    public PaymentGraph(HashMap<String, BalanceMap> graph) {
        this.graph = graph;
    }

    public Map<String, BalanceMap> getGraph() {
        return graph;
    }
}
