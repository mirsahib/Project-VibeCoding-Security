# Progress Report: Infrastructure & Calibration

**Project:** Security AI Agent of "Vibe Coding"  
**Date:** March 8, 2026  
**Phase:** Week 1 (System Setup & Automation)

---

## 📋 Overview

The primary goal of Day 1 was to establish the research environment, define the data architecture, and calibrate the primary security sensor (**Snyk SAST**) against an industry-standard benchmark. This ensures that the **"Measurement Layer"** of the research is accurate before testing AI-generated code.

---

## ⚙️ System Installation & Setup

To replicate the research measurement environment, the following security tools must be installed:

### 1. Snyk CLI Installation
The Snyk CLI is the core engine used for the **Measurement Layer**. It performs Static Analysis (SAST) on the generated code.
*   **Prerequisite:** Node.js and npm installed.
*   **Command:** 
    ```bash
    npm install -g snyk
    ```
*   **Authentication:**
    ```bash
    snyk auth
    ```
*   **Reference:** [Snyk CLI Documentation](https://docs.snyk.io/developer-tools/snyk-cli)

### 2. Snyk-to-HTML Installation
This utility transforms raw JSON scan data into professional, human-readable HTML reports.
*   **Command:**
    ```bash
    npm install -g snyk-to-html
    ```
*   **Usage:**
    ```bash
    snyk code test --json | snyk-to-html -o results.html
    ```
*   **Reference:** [snyk-to-html Documentation](https://docs.snyk.io/developer-tools/snyk-cli/scan-and-maintain-projects-using-the-cli/cli-tools/snyk-to-html)

---

## 🛠️ Key Achievements

### 1. Research Infrastructure Deployment

We successfully implemented the automated folder structure to support the collection and analysis of the **RVD-100 (Raw Vibe Dataset)** and **VVC-100 (Vibe Vulnerability Corpus)**.

- **Action:**  Executed a baseline bash script to generate the research hierarchy.

- **Result:**  Created dedicated directories for prompts, model-specific raw apps, external benchmarks, and automated analysis results.

---

### 2. Security Sensor Integration (Snyk CLI)

To automate the **"Measurement Layer,"** the **Snyk CLI** was integrated into the local environment.

- **Action:** Installed Snyk via    `npm`,Successfully authenticated the CLI and enabled **Snyk Code (SAST)** in the organization settings

- **Impact:**  The system is now capable of performing deep static analysis on custom source code and mapping findings to official security taxonomies.

---

### 3. Baseline Calibration (OWASP Benchmark)

The system was tested against the **OWASP Benchmark Project for Java** to verify the scanner’s reliability.

- **Action:**  
Cloned the **OWASP Benchmark (2,000+ ground-truth labeled test cases)** into `data/benchmarks/`.

- **Scan Execution:**  
Performed an automated scan of the benchmark source code using: `snyk code test
`
- Data Export: Generated a raw JSON result file `(owasp_benchmark_results.json)` for data-flow analysis.

### 4. Automated Reporting Setup
To streamline the "Mapping" phase , we integrated the snyk-to-html reporting engine.

- Action: Installed a professional report generator to transform raw JSON data into human-readable dashboards.

- Result: The project can now generate visual reports in results/analysis/ showing CWE distributions and severity rankings without manual data entry.

### 📂 Current Project State

The project root is now organized as follows:

- /data: Contains the external OWASP Benchmark and the VPB-20 (Vibe Prompt Benchmark) placeholder.

- /src: Ready for the Generator and Repair Agent scripts.

- /results: Contains the first raw scan data from the calibration phase.

- README.md: Updated with the theoretical framework and research architecture.


## Result
- [Synk Code Report](https://htmlpreview.github.io/?https://raw.githubusercontent.com/mirsahib/Project-VibeCoding-Security/refs/heads/master/results/analysis/owasp_calibration_report.html)