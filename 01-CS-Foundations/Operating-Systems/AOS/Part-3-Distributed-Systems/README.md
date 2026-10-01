---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
course: cs6210
tags: [cs6210]
---

# Part 3 - Distributed Systems, Objects, and Middleware

Lessons 5-6: ordering events with logical clocks, low-latency communication, active networks, systems from components, and distributed objects (Spring, Java RMI, EJB).

| Note | Concepts | Lab |
| --- | ---: | --- |
| [L05a Distributed Systems Definitions](L05a-Distributed-Systems-Definitions.md) | 5 | [lab-10-clocks-and-mutex](../labs/lab-10-clocks-and-mutex/README.md) |
| [L05b Lamport Clocks](L05b-Lamport-Clocks.md) | 9 | [lab-10-clocks-and-mutex](../labs/lab-10-clocks-and-mutex/README.md) |
| [L05c Latency Limits](L05c-Latency-Limits.md) | 7 | [lab-07-rpc-costs](../labs/lab-07-rpc-costs/README.md) |
| [L05d Active Networks](L05d-Active-Networks.md) | 10 | [lab-11-network-latency](../labs/lab-11-network-latency/README.md) |
| [L05e Systems from Components](L05e-Systems-from-Components.md) | 7 | [lab-12-components](../labs/lab-12-components/README.md) |
| [L06a Spring Operating System](L06a-Spring-Operating-System.md) | 10 | [lab-13-distributed-objects](../labs/lab-13-distributed-objects/README.md) |
| [L06b Java RMI](L06b-Java-RMI.md) | 8 | [lab-13-distributed-objects](../labs/lab-13-distributed-objects/README.md) |
| [L06c Enterprise Java Beans](L06c-Enterprise-Java-Beans.md) | 7 | [lab-13-distributed-objects](../labs/lab-13-distributed-objects/README.md) |

## Papers

- [Time, Clocks, and the Ordering of Events in a Distributed System](../Papers/L05-Time-Clocks-Ordering.md) - CACM 1978, required
- [Limits to Low-Latency Communication on High-Speed Networks](../Papers/L05-Limits-Low-Latency.md) - TOCS 1993, required
- [The x-Kernel: An Architecture for Implementing Network Protocols](../Papers/L05-x-Kernel.md) - IEEE TSE 1991, required
- [Active Networks: Vision and Reality: Lessons from a Capsule-based System](../Papers/L05-Active-Networks-ANTS.md) - SOSP 1999, required
- [Building Reliable, High-Performance Communication Systems from Components](../Papers/L05-Ensemble-Systems-from-Components.md) - SOSP 1999, required
- [Performance of the Firefly RPC](../Papers/L05-Firefly-RPC.md) - SOSP 1989, partial
- [An Overview of the Spring System](../Papers/L06-Spring-Overview.md) - Compcon 1994, required
- [Subcontract: A Flexible Base for Distributed Programming](../Papers/L06-Subcontract.md) - SOSP 1993, required
- [A Distributed Object Model for the Java System](../Papers/L06-Java-Distributed-Object-Model.md) - USENIX COOTS 1996, required
- [Performance and Scalability of EJB Applications](../Papers/L06-EJB-Performance.md) - OOPSLA 2002, required

## Practice

- [Practice L05](../Practice/Practice-L05.md)
- [Practice L06](../Practice/Practice-L06.md)
