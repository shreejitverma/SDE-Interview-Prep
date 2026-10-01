---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L06
tags: [cs6210, cs6210/practice]
---

# Practice L06

Original exam-style questions for [L06a](../Part-3-Distributed-Systems/L06a-Spring-Operating-System.md), [L06b](../Part-3-Distributed-Systems/L06b-Java-RMI.md), [L06c](../Part-3-Distributed-Systems/L06c-Enterprise-Java-Beans.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

## Spring Operating System

> [!question]- Q1. The Spring operating system contrasts with traditional procedural operating systems by relying heavily on strongly typed interfaces. How does this object-based design affect the implementation of the microkernel? Provide one example of how it enables flexibility. (concepts: L06a-01, L06a-02, P-Spring-Overview)
> In a procedural OS, the kernel is a monolithic set of services.
> In Spring's object-based design, the microkernel only implements fundamental abstractions like domains and threads, while all other OS services are provided as objects with strongly typed interfaces implemented in user space.
> This enables high flexibility because any service can be completely replaced or extended simply by providing a new object that conforms to the expected interface, without altering the kernel itself.

> [!question]- Q2. Trace the invocation path of a Spring object that resides on a remote server. Start from the client domain holding a capability and include the roles of doors, door tables, and network proxies. How does the Nucleus ensure secure object invocation using front objects? (concepts: L06a-03, L06a-04, L06a-05)
> The client domain invokes a method on an object reference, which maps to an entry in the Nucleus's door table.
> The Nucleus identifies the local door and forwards the invocation.
> For remote objects, this door points to a network proxy domain rather than the actual server domain.
> The proxy marshals the request and transmits it over the network to the server machine's proxy, which unpacks it and invokes the actual object via its own local door.
> Security is maintained because the client often interacts with a front object, which performs access checks before forwarding the valid request to the underlying target object.

> [!question]- Q3. Describe the interaction between address space objects, memory objects, and pager objects in Spring's virtual memory management. What happens when a page fault occurs? (concepts: L06a-06, L06a-07)
> An address space object represents the virtual memory of a domain and maps virtual addresses to memory objects.
> A memory object represents the actual data, like a file or swap space, and is backed by a pager object, which knows how to fetch and store the data pages.
> When a page fault occurs, the address space object contacts the memory object's pager, which fetches the required page, perhaps using a cached memory object for optimization, and maps it into the domain's address space.

> [!question]- Q4. The subcontract mechanism decouples the communication details of an object invocation from its application-level interface. Explain the roles of the marshal and invoke interfaces within a subcontract, and how this mechanism enables dynamic client-server relationships. (concepts: L06a-08, L06a-09, L06a-10, P-Subcontract)
> A subcontract is a pluggable module that implements the underlying transport and object management policies, like caching or replication.
> The marshal interface defines how object references are serialized and sent across the network, while the invoke interface handles the actual transmission of the method call arguments and waits for the reply.
> Because subcontracts can be swapped out transparently, the relationship between a client and a server becomes dynamic.
> A client could start with a simple RPC subcontract and dynamically switch to a caching or fault-tolerant subcontract without changing its application logic.

## Java Distributed Object Model

> [!question]- Q5. In the Java distributed object model, how does the system distinguish between objects that can be invoked remotely and objects that are passed by value? Discuss the design choice of reuse of local implementation versus reuse of remote object class. (concepts: L06b-01, L06b-02, L06b-03, P-Java-Distributed-Object-Model)
> Remote objects and remote interfaces must implement the Remote interface and their methods must declare RemoteException.
> These remote objects are passed by reference via stubs.
> Objects that do not implement Remote are passed by value, meaning their state is copied across the network.
> Java chose not to reuse local implementation classes blindly for remote objects because remote calls have different failure semantics and latency characteristics.
> Instead, the model enforces explicit remote interfaces, forcing the developer to handle remote exceptions, but it does allow reusing the familiar Java syntax and type system.

> [!question]- Q6. Trace the layers of the Java RMI architecture when a client invokes a method on a server's remote object. Explain the specific responsibilities of the remote reference layer and the RMI transport layer, including endpoints, transports, channels, and connections. (concepts: L06b-04, L06b-06, L06b-07)
> The client side of RMI calls a method on the local stub.
> The stub forwards the call to the remote reference layer, which determines the invocation semantics, such as unicast or multicast, and provides a reference to the remote object.
> The request then descends to the RMI transport layer, which is responsible for the actual wire protocol.
> Within the transport layer, an endpoint identifies the remote address space, a transport manages the network protocol stack, a channel represents a logical link between two endpoints, and a connection is the actual physical stream for data transmission.
> The server side of RMI unmarshals the request, routes it through a skeleton or dispatcher, and invokes the real object.

> [!question]- Q7. Contrast the parameter passing semantics for remote objects with local Java parameter passing. How does distributed garbage collection interact with these semantics to prevent memory leaks across JVMs? (concepts: L06b-05, L06b-08)
> In local Java, all objects are passed by reference.
> In RMI, local objects are passed by value via deep copy, while remote objects are passed by reference using a stub.
> This semantic difference ensures that large local objects are isolated across the network.
> To manage remote references, RMI uses a distributed garbage collection system based on leasing.
> Clients request a lease for a remote reference and periodically renew it.
> If the server does not receive a renewal before the lease expires, it decrements the reference count and, if it reaches zero, allows the local garbage collector to reclaim the actual object.

## Enterprise Java Beans

> [!question]- Q8. N-tier applications separate presentation, business logic, and data management. How does the EJB architecture map to these concerns, and what specific roles do containers play in managing the EJB structure? (concepts: L06c-01, L06c-02, L06c-03)
> The EJB architecture typically sits in the middle tier for business logic, bridging the client presentation layer and the backend database tier.
> Containers manage the entire EJB structure by providing infrastructural services like lifecycle management, transactions, security, and concurrency for the beans.
> This frees the developer to focus purely on business logic.
> The container manages three main types of beans: entity beans for persistent data, session beans for client conversational state, and message-driven beans for asynchronous messaging.

> [!question]- Q9. A development team needs to build a scalable EJB application and is considering three design alternatives. The alternatives are using a coarse-grained session bean that executes direct SQL, using a data access object pattern, or using session beans with entity beans. Compare the trade-offs in concurrency, security, and persistence across these designs. (concepts: L06c-04, L06c-05, L06c-06, L06c-07, P-EJB-Performance)
> Using a coarse-grained session bean provides the highest performance because it minimizes network round trips, but it tangles business logic with database access, delegating all concurrency and persistence to the database.
> Using a data access object improves maintainability by separating business logic from database access, allowing the database dialect to change easily while keeping performance high and relying on the database for concurrency.
> Using session beans with entity beans lets the EJB container handle persistence and complex concurrency, providing the best abstraction and security hooks.
> However, the overhead of managing fine-grained entity beans can cause a significant performance bottleneck and excessive database queries if not carefully optimized.
