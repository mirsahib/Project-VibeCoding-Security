# Weekly Progress Report: Week 2

**Project:** Security AI Agent of "Vibe Coding"  
**Period:** March 9, 2026 – March 19, 2026  
**Status:** Complete / On Track 

---

## 📝 Executive Summary
The focus of Week 2 shifted from infrastructure setup to the **Data Blitz** phase. This week centered on standardizing the input layer (prompts) and developing the automation scripts required to generate the **RVD-100** (Raw Vibe Dataset). We expanded our datasets, refactored the master orchestrator, executed the generation pipeline, and fully automated HTML report dashboards via Snyk. The foundational framework for our Measurement and Transformation layers is now complete and actively logging data.

---

## 🛠️ Key Activities & Accomplishments

### 1. Finalization & Expansion of Datasets
We completed the design of our input layers designed to test for "Insecure Defaults" by using vague natural language mapping to OWASP/CWE definitions.
* **Expanded Integration:** Added comprehensive testing coverage by integrating `cweid_dataset` and `llmseceval_dataset` alongside standard prompts.
* **Target Vulnerabilities:** Each prompt is systematically mapped to specific CWEs (e.g., SQL Injection, Path Traversal, SSRF) to ensure measurable research outcomes.

### 2. Execution of the Automation Pipeline (`generator.py`)
A central Python orchestration script was developed and fully executed to handle the "Transformation" and "Measurement" layers sequentially.
* **OpenRouter Integration:** Configured the script to communicate with multiple models (`openai`, `qwen`, `gemini`, `llama`, `glm-4.7`, `deepseek`, etc.) through a single gateway.
* **Smart Language Detection:** Implemented an autonomous `get_file_extension` heuristic to recognize LLM code outputs and save structurally sound syntax files instead of blindly forcing `.py`.
* **Full Generation Run:** Executed the `generator.py` suite across target LLMs, caching responses safely in dynamically labeled dataset directories.

### 3. "Measurement Layer" Pipeline Completion
The framework for analyzing and visualizing the results is fully functional and populated with the first data batch.
* **Snyk Subprocess Integration:** Integrated logic to trigger Snyk CLI scans matching each model's generation loop to pull vulnerability metrics as raw JSON.
* **HTML Dashboards Generated:** Built an automated loop utilizing `snyk-to-html` to instantly compile all sequential `results/raw_scans/*.json` outputs into unified, visual reports located in `results/html_reports`.

---

## 📂 Current Project State

| Component | Status | Location |
| :--- | :--- | :--- |
| **Input Layer (Datasets)** | **Complete** | `data/prompts/` |
| **Transformation Layer** | **Complete** | `src/generator.py` (Executed & Modular) |
| **Measurement Layer** | **Complete** | Snyk CLI -> HTML Dashboards Generated |
| **Infrastructure** | **Complete** | Folder structure and Python .venv setup |

---

## 🚧 Challenges & Blockers
* **LLM Output Formatting:** Anticipating rogue LLM Markdown injection required system prompt tuning to restrict LLMs to pure source code streams.
* **Path Variables:** Dynamic execution path errors required replacing relative paths iteratively with hardcoded `os.path` contextual project roots.

---

## 🚀 Roadmap for Week 3
1.  **Vulnerability Density Analysis:** Calculate the initial security "Risk Score" comparing the generated reports for each LLM model.
2.  **Prototype Repair Agent:** Begin development of the `repair_agent.py` logic to facilitate the "Self-Correction Loop" using Snyk vulnerability metrics to prompt LLMs for healing iterations.
