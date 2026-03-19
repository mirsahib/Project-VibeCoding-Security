import json
import csv
import re
import os

def get_category_and_owasp(cwe_ids):
    """
    Simulated mapping for CWE to category and OWASP 2025.
    Expands categorization based on standard CWE vulnerabilities.
    """
    mapping = {
        "20":  ("Input Validation", "A03: Injection"),
        "22":  ("Path Traversal", "A01: Broken Access Control"),
        "79":  ("XSS", "A03: Injection"),
        "89":  ("SQL Injection", "A03: Injection"),
        "119": ("Memory Corruption", "A08: Software and Data Integrity Failures"),
        "120": ("Memory Corruption", "A08: Software and Data Integrity Failures"),
        "125": ("Memory Corruption", "A08: Software and Data Integrity Failures"),
        "190": ("Integer Overflow", "A08: Software and Data Integrity Failures"),
        "200": ("Data Exposure", "A02: Cryptographic Failures / Data Exposure"),
        "306": ("Auth System", "A07: Authentication Failures"),
    }
    
    # Try to find a match in the mapping
    for cwe in cwe_ids:
        if cwe in mapping:
            return mapping[cwe]
            
    # Default fallback
    return ("General / Other", "System/Other Vulnerability")

def main():
    # Input paths
    base_dir = "/home/mirsahib/Desktop/2026/Project-VibeCoding-Security"
    llmseceval_path = os.path.join(base_dir, "data/prompts/repo/LLMSecEval/Dataset/LLMSecEval-Prompts_dataset.json")
    cweid_path = os.path.join(base_dir, "data/prompts/repo/LLM-Generated-Code/prompts/cweid_prompts.csv")
    
    llmseceval_output_path = os.path.join(base_dir, "data/prompts/llmseceval_dataset.json")
    cweid_output_path = os.path.join(base_dir, "data/prompts/cweid_dataset.json")

    # 1. Process LLMSecEval
    print(f"Processing LLMSecEval dataset from {llmseceval_path}...")
    llmseceval_result = []
    try:
        with open(llmseceval_path, 'r', encoding='utf-8') as f:
            llmseceval_data = json.load(f)
            
            for index, item in enumerate(llmseceval_data):
                prompt_id = item.get("Prompt ID", f"LLMSEC-{index}")
                cwe_name = item.get("CWE Name", "")
                
                # Extract CWE ID number
                cwe_match = re.search(r'CWE-(\d+)', prompt_id)
                extracted_cwe_id = cwe_match.group(1) if cwe_match else ""
                
                if extracted_cwe_id and cwe_name:
                    target_vuln = f"CWE-{extracted_cwe_id} ({cwe_name})"
                else:
                    target_vuln = prompt_id if not cwe_name else cwe_name

                category, owasp = get_category_and_owasp([extracted_cwe_id] if extracted_cwe_id else [])
                
                # Preferred manually fixed, fallback to LLM generated
                prompt_text = item.get("Manually-fixed NL Prompt")
                if not prompt_text:
                    prompt_text = item.get("LLM-generated NL Prompt", "")
                
                entry = {
                    "prompt_id": prompt_id,
                    "category": category,
                    "prompt_text": prompt_text,
                    "target_vulnerability": target_vuln,
                    "owasp_2025": owasp
                }
                llmseceval_result.append(entry)
                
        with open(llmseceval_output_path, 'w', encoding='utf-8') as f:
            json.dump(llmseceval_result, f, indent=2)
        print(f"Successfully generated {len(llmseceval_result)} prompts and saved to {llmseceval_output_path}")

    except Exception as e:
        print(f"Error processing LLMSecEval: {e}")

    # 2. Process cweid_prompts.csv
    print(f"Processing CWE ID prompts from {cweid_path}...")
    cweid_result = []
    try:
        with open(cweid_path, 'r', encoding='utf-8') as f:
            reader = csv.DictReader(f)
            for index, row in enumerate(reader):
                cwe_id_raw = row.get("cwe-id", "").strip()
                prompt_text = row.get("prompts", "").strip()
                
                if not prompt_text:
                    continue
                
                # Extract all numbers/CWEs in case of something like "401-415-416"
                cwe_list = re.findall(r'\d+', cwe_id_raw)
                
                if cwe_list:
                    target_vuln = " / ".join([f"CWE-{c}" for c in cwe_list])
                else:
                    target_vuln = "Unknown CWE"
                
                category, owasp = get_category_and_owasp(cwe_list)
                
                entry = {
                    "prompt_id": f"CSV-CWE-{cwe_id_raw}-{index+1:03d}",
                    "category": category,
                    "prompt_text": prompt_text,
                    "target_vulnerability": target_vuln,
                    "owasp_2025": owasp
                }
                cweid_result.append(entry)
                
        with open(cweid_output_path, 'w', encoding='utf-8') as f:
            json.dump(cweid_result, f, indent=2)
        print(f"Successfully generated {len(cweid_result)} prompts and saved to {cweid_output_path}")

    except Exception as e:
        print(f"Error processing cweid_prompts: {e}")

if __name__ == "__main__":
    main()
