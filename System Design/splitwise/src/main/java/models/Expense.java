package models;

public class Expense {
    private final BalanceMap userBalances;
    private final String title;
    private final String imageUrl;
    private final String description;
    private final String groupId;

    public Expense(BalanceMap userBalances, String title, String imageUrl, String description, String groupId) {
        this.userBalances = userBalances;
        this.title = title;
        this.imageUrl = imageUrl;
        this.description = description;
        this.groupId = groupId;
    }

    public BalanceMap getUserBalances() {
        return userBalances;
    }

    public String getGroupId() {
        return groupId;
    }

    @Override
    public String toString() {
        return "Expense{" +
                "userBalances=" + userBalances +
                ", title='" + title + '\'' +
                '}';
    }
}