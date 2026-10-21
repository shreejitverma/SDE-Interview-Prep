---
type: moc
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 06 - Infrastructure as Code and Delivery

Volume 06 - Infrastructure as Code and Delivery in the Cloud AI Engineering Zero to Godhood curriculum.

## Chapters

1. [Terraform and OpenTofu Internals](Chapter_01_Terraform_and_OpenTofu_Internals.md) - Directed Acyclic Graph execution, resource dependency resolution, state locking mechanics, and plan/apply lifecycles.
2. [Enterprise Module Design and Abstraction Layers](Chapter_02_Enterprise_Module_Design_and_Abstraction_Layers.md) - Reusable module architecture, variable validation rules, output contracts, and semantic versioning.
3. [Programmatic IaC: AWS CDK and Pulumi](Chapter_03_Programmatic_IaC_AWS_CDK_and_Pulumi.md) - Construct trees, synthesized CloudFormation and Terraform, imperative unit testing, and maintainability trade-offs.
4. [Policy as Code: OPA, Sentinel, and Guardrails](Chapter_04_Policy_as_Code_OPA_Sentinel_and_Guardrails.md) - Rego evaluation engines, pre-apply Terraform plan scanning, AWS Config rules, and GCP Org Policy guardrails.
5. [State Management, Drift Detection, and Compliance](Chapter_05_State_Management_Drift_Detection_and_Compliance.md) - Remote state locking on S3 and GCS, state migration safely, automated drift detection scans, and automated remediation.
6. [Progressive Delivery and Deployment Pipelines](Chapter_06_Progressive_Delivery_and_Deployment_Pipelines.md) - Canary analysis, blue/green traffic shifting, Argo Rollouts integration, and automated metric rollback triggers.
