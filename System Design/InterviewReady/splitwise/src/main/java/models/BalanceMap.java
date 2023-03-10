package models;

import java.util.HashMap;
import java.util.Map;

public class BalanceMap {
    private final Map<String, Amount> balances;

    public BalanceMap() {
        this.balances = new HashMap<>();
    }

    public BalanceMap(Map<String, Amount> resultBalances) {
        this.balances = resultBalances;
    }

    public Map<String, Amount> getBalances() {
        return balances;
    }

    @Override
    public String toString() {
        return "BalanceMap{" +
                "balances=" + balances +
                '}';
    }
}
