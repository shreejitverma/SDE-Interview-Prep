import models.*;
import org.junit.Assert;
import org.junit.Test;
import services.ExpenseService;
import services.GroupService;

import java.util.ArrayList;
import java.util.HashMap;

public class GroupPaymentGraphTest {
    @Test
    public void defaultTest() throws IllegalAccessException {
        final ExpenseService expenseService = constructExpenseService();
        final HashMap<String, Group> groups = getGroups();
        GroupService groupService = new GroupService(expenseService, groups);
        final var balances = groupService.getBalances("123", "A");
        final var balanceMap = balances.getBalances();
        Assert.assertEquals(balanceMap.get("A").getAmount(), 50.0, 0.0001);
        Assert.assertEquals(balanceMap.get("B").getAmount(), 30.0, 0.0001);
        Assert.assertEquals(balanceMap.get("C").getAmount(), -80.0, 0.0001);
    }

    @Test
    public void paymentGraphTest() throws IllegalAccessException {
        final ExpenseService expenseService = constructExpenseService2();
        final HashMap<String, Group> groups = getGroups();
        GroupService groupService = new GroupService(expenseService, groups);
        final var balances = groupService.getGroupPaymentGraph("123", "A");
        final var graph = balances.getGraph();
        Assert.assertEquals(graph.get("A").getBalances().get("E").getAmount(), 90.0, 0.0001);
        Assert.assertEquals(graph.get("D").getBalances().get("B").getAmount(), 70.0, 0.0001);
        Assert.assertEquals(graph.get("F").getBalances().get("C").getAmount(), 40.0, 0.0001);
        Assert.assertEquals(graph.get("D").getBalances().get("E").getAmount(), 10.0, 0.0001);
        System.out.println(graph);
    }

    private HashMap<String, Group> getGroups() {
        final var groups = new HashMap<String, Group>();
        final var userList = new ArrayList<String>();
        userList.add("A");
        userList.add("B");
        userList.add("C");
        groups.put("123", new Group("Europe", "Euro trip", "www.interviewready.io",
                userList));
        return groups;
    }

    private ExpenseService constructExpenseService() {
        final var expenseService = new ExpenseService();
        final var firstExpense = new BalanceMap();
        firstExpense.getBalances().put("A", new Amount(Currency.USD, 10));
        firstExpense.getBalances().put("B", new Amount(Currency.USD, 20));
        firstExpense.getBalances().put("C", new Amount(Currency.USD, -30));
        expenseService.addExpense(new Expense(firstExpense,
                "outing1", "www.interviewready.io", "outing 1", "123"));
        final var secondExpense = new BalanceMap();
        secondExpense.getBalances().put("A", new Amount(Currency.USD, -50));
        secondExpense.getBalances().put("B", new Amount(Currency.USD, 10));
        secondExpense.getBalances().put("C", new Amount(Currency.USD, 40));
        expenseService.addExpense(new Expense(secondExpense,
                "outing2", "www.interviewready.io", "outing 2", "123"));
        final var thirdExpense = new BalanceMap();
        thirdExpense.getBalances().put("A", new Amount(Currency.USD, 90));
        thirdExpense.getBalances().put("C", new Amount(Currency.USD, -90));
        expenseService.addExpense(new Expense(thirdExpense,
                "outing3", "www.interviewready.io", "outing 3", "123"));
        return expenseService;
    }

    private ExpenseService constructExpenseService2() {
        final var expenseService = new ExpenseService();
        final var firstExpense = new BalanceMap();
        firstExpense.getBalances().put("A", new Amount(Currency.USD, 90));
        firstExpense.getBalances().put("B", new Amount(Currency.USD, -70));
        firstExpense.getBalances().put("C", new Amount(Currency.USD, -40));
        firstExpense.getBalances().put("D", new Amount(Currency.USD, 80));
        firstExpense.getBalances().put("E", new Amount(Currency.USD, -100));
        firstExpense.getBalances().put("F", new Amount(Currency.USD, 40));
        expenseService.addExpense(new Expense(firstExpense,
                "outing1", "www.interviewready.io", "outing 1", "123"));
        return expenseService;
    }
}
