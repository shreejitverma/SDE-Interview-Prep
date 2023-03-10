import services.ExpenseService;
import services.GroupService;

public class Splitwise {
    private final GroupService groupService;
    private final ExpenseService expenseService;

    public Splitwise(GroupService groupService, ExpenseService expenseService) {
        this.groupService = groupService;
        this.expenseService = expenseService;
    }
}
