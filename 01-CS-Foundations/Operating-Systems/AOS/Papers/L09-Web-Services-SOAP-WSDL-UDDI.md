---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1109/4236.991449"]
course: cs6210
lesson: L09
reading: self-study
venue: "IEEE Internet Computing 2002"
authors: ["Francisco Curbera", "Matthew Duftler", "Rania Khalaf", "William Nagy", "Nirmal Mukhi", "Sanjiva Weerawarana"]
tags: [cs6210, cs6210/paper]
aliases: ["Unraveling the Web Services Web: An Introduction to SOAP, WSDL, and UDDI"]
---

# Unraveling the Web Services Web: An Introduction to SOAP, WSDL, and UDDI

IEEE Internet Computing 2002. Reading status: self-study. [Link](https://doi.org/10.1109/4236.991449).

> [!abstract] One-line summary
> This paper provides a conceptual overview of the foundational Web services framework, detailing how SOAP, WSDL, and UDDI combine to enable standardized, XML-based application-to-application integration.

## Problem

Businesses traditionally interacted over the internet using ad hoc, proprietary, or incompatible applications and protocols, making systematic application-to-application integration difficult and complex.

## Key idea

The paper introduces the emerging Web services framework, a systematic and extensible architecture for application-to-application interaction built on open XML standards.
The framework relies on three core technologies: SOAP for communication, WSDL for service description, and UDDI for service discovery and registration, which together enable dynamic, platform-independent integration.

## Design

The architecture is divided into three functional areas.
First, SOAP provides an XML-based messaging protocol for remote procedure calls over transports like HTTP.
Second, WSDL provides formal, computer-readable XML descriptions of Web services, separating the abstract application-level interface from the concrete protocol-dependent bindings and network endpoints.
Third, UDDI offers a centralized registry and discovery mechanism, organizing information into white, yellow, and green pages to categorize and locate businesses and their technical service descriptions.

## Evaluation

As this is an introductory tutorial and conceptual overview, the paper does not include a formal empirical evaluation, experimental setup, or performance metrics.
It instead provides illustrative scenarios, such as an online travel agency booking system, to demonstrate the practical application and interoperability of the SOAP, WSDL, and UDDI stack.

## Limitations and critiques

The paper acknowledges that the basic Web services stack described is incomplete for full-scale enterprise usage.
It notes the absence of built-in mechanisms for robust security, reliable messaging, quality of service descriptions, and orchestration of complex, long-running business processes.

## What it led to

This overview documented the foundational technologies that catalyzed the widespread adoption of Service-Oriented Architectures.
While UDDI saw limited long-term adoption, SOAP and WSDL became industry standards for enterprise integration before the industry largely shifted toward RESTful architectures and JSON for web APIs.

## Exam angles

<details>
<summary>How does WSDL separate the abstract description of a service from its concrete implementation details?</summary>
WSDL separates the description into two parts to allow similar application-level functionality to be deployed across different endpoints.
The abstract description defines the service's vocabulary using XML Schema, messages, and operations grouped into portTypes.
The concrete description uses binding elements to specify the communication protocol and data formats, and port elements to map these bindings to specific network addresses.
</details>

<details>
<summary>Explain the purpose of tModels in the UDDI registry and how they enable technical service discovery.</summary>
In UDDI, tModels represent registered technical specifications or interfaces, such as a standard WSDL document.
Rather than embedding arbitrary technical descriptions directly in the directory, a service provider registers a specification and receives a unique tModel identifier key.
Service endpoints that implement the specification then include a reference to that tModel key in their binding templates, allowing clients to query the registry for services that support a specific technical interface.
</details>

<details>
<summary>Why is SOAP required for Web services security and reliability instead of relying entirely on underlying transport protocols like HTTPS?</summary>
Transport-level protocols like HTTPS provide security and reliability only for single-hop, point-to-point connections.
Web services often involve multi-hop interactions where business messages pass through several intermediaries.
Therefore, end-to-end security, confidentiality, and reliable delivery must be defined and enforced at the message level using SOAP extensions, ensuring the message is protected regardless of the underlying transport mechanism.
</details>

## Related

- Lessons: [L09a](../Part-5-Internet-Scale-Real-Time-and-Security/L09a-Giant-Scale-Services.md), [L09b](../Part-5-Internet-Scale-Real-Time-and-Security/L09b-MapReduce.md), [L09c](../Part-5-Internet-Scale-Real-Time-and-Security/L09c-Content-Delivery-Networks.md)
