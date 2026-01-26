# SAGA Pattern (Distributed Transactions)

**Topic:** Handling transactions across microservices where ACID is impossible.

## The Problem
In a monolithic Database, you can use `BEGIN TRANSACTION ... COMMIT/ROLLBACK`. 
In Microservices, Service A (Order) and Service B (Payment) have different DBs. 
If Payment fails, Order must be cancelled. We cannot lock both DBs (2PC is too slow/blocking).

## The Solution: SAGA
A sequence of local transactions. If one fails, execute **Compensating Transactions** (Undo operations) in reverse order.

### Example: Booking a Trip
1.  **Book Flight** (Success)
2.  **Book Hotel** (Success)
3.  **Book Car** (Failed! No cars available) -> **Trigger Rollback**
4.  (Compensate) **Cancel Hotel**
5.  (Compensate) **Cancel Flight**

## Implementation Approaches

### 1. Choreography (Event Driven)
Services talk to each other via Events.
*   `Order Service`: Emits `OrderCreated` event.
*   `Payment Service`: Listens to `OrderCreated`. Processes payment. Emits `PaymentProcessed` OR `PaymentFailed`.
*   `Order Service`: Listens to `PaymentFailed` -> Cancels Order.

**Pros:** Decentralized, simple for few services.
**Cons:** Hard to track complex workflows (cyclic dependencies).

### 2. Orchestration (Command Driven)
A central **Orchestrator** (State Machine) tells services what to do.
*   `Saga Orchestrator`:
    1.  Call `OrderService.create()`
    2.  Call `PaymentService.charge()`
    3.  If Error -> Call `PaymentService.refund()` AND `OrderService.reject()`

**Pros:** Central logic, easy to debug.
**Cons:** Orchestrator can become a monolith logic bottleneck.

## Code Example (Python Orchestration Logic)

```python
class TripBookingSaga:
    def execute(self):
        try:
            self.flight_id = flight_service.book()
            self.hotel_id = hotel_service.book()
            self.car_id = car_service.book() # Fails here
        except Exception:
            self.compensate()

    def compensate(self):
        if hasattr(self, 'car_id'): car_service.cancel(self.car_id)
        if hasattr(self, 'hotel_id'): hotel_service.cancel(self.hotel_id)
        if hasattr(self, 'flight_id'): flight_service.cancel(self.flight_id)
```
