import json

# Define the mapping based on research data
MAPPING = {
    "CWE-89": "A05: Injection",
    "CWE-78": "A05: Injection",
    "CWE-79": "A05: Injection",
    "CWE-22": "A01: Broken Access Control",
    "CWE-918": "A01: Broken Access Control",
    "CWE-611": "A02: Security Misconfiguration",
    "CWE-327": "A04: Cryptographic Failures"
}

def analyze_scan(json_path):
    with open(json_path, 'r') as f:
        data = json.load(f)
        
    stats = {}
    # Iterate through Snyk findings
    for issue in data.get('vulnerabilities', []):
        for cwe in issue.get('identifiers', {}).get('CWE', []):
            category = MAPPING.get(cwe, "Other / Emerging Risk")
            stats[category] = stats.get(category, 0) + 1
            
    print("--- Verification Accuracy Report ---")
    for cat, count in stats.items():
        print(f"{cat}: {count} occurrences detected")

# Run it on your Snyk output
analyze_scan('results/raw_scans/owasp_benchmark_results.json')