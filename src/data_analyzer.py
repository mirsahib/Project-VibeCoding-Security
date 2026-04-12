import os
import sys
import json
import csv
from dotenv import load_dotenv

load_dotenv()

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

MODELS = [
    "meta-llama/llama-3.3-70b-instruct",
    "z-ai/glm-4.7-flash",
    "qwen/qwen3.5-9b"
]

def choose_dataset():
    print("Which dataset do you want to analyze?")
    print("1. cweid_dataset")
    print("2. llmseceval_dataset")
    
    while True:
        choice = input("Enter your choice (1 or 2): ")
        if choice == '1':
            return os.path.join(PROJECT_ROOT, 'data/prompts/cweid_dataset.json')
        elif choice == '2':
            return os.path.join(PROJECT_ROOT, 'data/prompts/llmseceval_dataset.json')
        else:
            print("Invalid choice. Please enter 1 or 2.")

def choose_prompt_count(total):
    while True:
        try:
            choice = input(f"\nFound {total} prompts in the dataset. How many prompts would you like to analyze? (Enter a number or 'all'): ").strip().lower()
            if choice == 'all':
                return total
            num = int(choice)
            if num <= 0:
                print("Please enter a number greater than 0.")
                continue
            if num > total:
                print(f"❌ Error: Requested {num} prompts, but only {total} are available in the dataset. Stopping process.")
                sys.exit(1)
            return num
        except ValueError:
            print("Invalid input. Please enter a valid number or 'all'.")

def get_snyk_vuln_count(json_path):
    """Parses a Snyk JSON report and returns the total number of vulnerabilities."""
    if not os.path.exists(json_path):
        return None
    try:
        with open(json_path, 'r') as f:
            data = json.load(f)
            runs = data.get("runs", [])
            if runs:
                return len(runs[0].get("results", []))
            return 0
    except Exception:
        return None

def get_test_verdict(json_path):
    """Parses the test_verdict.json and returns a boolean indicating total success."""
    if not os.path.exists(json_path):
        return None
    try:
        with open(json_path, 'r') as f:
            data = json.load(f)
            return data.get("compilation_success", False) and data.get("test_success", False)
    except Exception:
        return None

def get_latest_pass(dataset_name, model_slug, prompt_id):
    """Finds the highest pass directory for a given prompt (e.g., pass2, pass3)."""
    base_app_dir = os.path.join(PROJECT_ROOT, f"data/raw_apps/{dataset_name}/{model_slug}/{prompt_id}")
    latest_pass = 1
    
    if os.path.exists(base_app_dir):
        for item in os.listdir(base_app_dir):
            if item.startswith("pass") and os.path.isdir(os.path.join(base_app_dir, item)):
                try:
                    pass_num = int(item.replace("pass", ""))
                    if pass_num > latest_pass:
                        latest_pass = pass_num
                except ValueError:
                    pass
    return latest_pass

def analyze_data():
    dataset_path = choose_dataset()
    dataset_name = os.path.splitext(os.path.basename(dataset_path))[0]
    
    with open(dataset_path, 'r') as f:
        all_prompts = json.load(f)

    total_prompts = len(all_prompts)
    num_prompts = choose_prompt_count(total_prompts)
    prompts = all_prompts[:num_prompts]

    # Initialize stats tracking per model
    stats = {model.replace("/", "-"): {
        "total_processed": 0,
        "pass1_vulnerable": 0,
        "pass1_functional": 0,
        "attempted_heals": 0,
        "successful_heals": 0,
        "functional_regressions": 0,
        "ultimate_success": 0
    } for model in MODELS}

    print(f"\n🔍 Analyzing data for {dataset_name} (First {num_prompts} prompts)...\n")

    for model in MODELS:
        model_slug = model.replace("/", "-")
        model_stats = stats[model_slug]
        
        for p in prompts:
            prompt_id = p.get('prompt_id')
            
            # --- PASS 1 DATA ---
            p1_snyk_path = os.path.join(PROJECT_ROOT, f"results/raw_scans/{dataset_name}/{model_slug}_{prompt_id}.json")
            p1_test_path = os.path.join(PROJECT_ROOT, f"data/raw_apps/{dataset_name}/{model_slug}/{prompt_id}/test_verdict.json")
            
            p1_vulns = get_snyk_vuln_count(p1_snyk_path)
            p1_func_success = get_test_verdict(p1_test_path)
            
            if p1_vulns is None:
                continue # Skip if no generation/scan data exists for this prompt/model
                
            model_stats["total_processed"] += 1
            
            if p1_vulns > 0:
                model_stats["pass1_vulnerable"] += 1
                if p1_func_success:
                    model_stats["pass1_functional"] += 1
            else:
                # If 0 vulns on Pass 1, generator skipped test generation. 
                # We count this as an ultimate success out-of-the-box.
                model_stats["ultimate_success"] += 1
                continue # No healing loop data to check
                
            # --- FINAL PASS DATA (HEALING LOOP) ---
            latest_pass = get_latest_pass(dataset_name, model_slug, prompt_id)
            
            if latest_pass > 1:
                model_stats["attempted_heals"] += 1
                final_pass_name = f"pass{latest_pass}"
                
                final_snyk_path = os.path.join(PROJECT_ROOT, f"results/raw_scans/{dataset_name}/{final_pass_name}/{model_slug}_{prompt_id}.json")
                final_test_path = os.path.join(PROJECT_ROOT, f"data/raw_apps/{dataset_name}/{model_slug}/{prompt_id}/{final_pass_name}/test_verdict.json")
                
                final_vulns = get_snyk_vuln_count(final_snyk_path)
                final_func_success = get_test_verdict(final_test_path)
                
                # Metric 1: Did it fix the security issue?
                if final_vulns == 0:
                    model_stats["successful_heals"] += 1
                    
                # Metric 2: Did it break functionality that was previously working?
                if p1_func_success and final_func_success is False:
                    model_stats["functional_regressions"] += 1
                    
                # Metric 3: The ultimate goal (0 vulns + working code in final pass)
                if final_vulns == 0 and final_func_success:
                    model_stats["ultimate_success"] += 1

    # --- PRINT TERMINAL REPORT ---
    print("="*80)
    print(f"🏆 METRICS REPORT: {dataset_name.upper()}")
    print("="*80)
    
    for model_slug, data in stats.items():
        total = data['total_processed']
        if total == 0:
            continue
            
        insecure_rate = (data['pass1_vulnerable'] / total) * 100
        
        heal_rate = 0
        if data['attempted_heals'] > 0:
            heal_rate = (data['successful_heals'] / data['attempted_heals']) * 100
            
        regression_rate = 0
        if data['attempted_heals'] > 0:
            regression_rate = (data['functional_regressions'] / data['attempted_heals']) * 100
            
        ultimate_rate = (data['ultimate_success'] / total) * 100

        print(f"\n🤖 MODEL: {model_slug}")
        print("-" * 40)
        print(f"Total Prompts Analyzed:    {total}")
        print(f"Insecure Default Rate:     {insecure_rate:.1f}% ({data['pass1_vulnerable']}/{total} generated insecure code initially)")
        print(f"Repair Success Rate:       {heal_rate:.1f}% ({data['successful_heals']}/{data['attempted_heals']} fixed in loop)")
        print(f"Functional Regressions:    {regression_rate:.1f}% ({data['functional_regressions']} times the fix broke the app)")
        print(f"Ultimate Success Rate:     {ultimate_rate:.1f}% ({data['ultimate_success']}/{total} are secure AND working)")

    # --- EXPORT TO CSV ---
    csv_dir = os.path.join(PROJECT_ROOT, "results")
    os.makedirs(csv_dir, exist_ok=True)
    csv_file = os.path.join(csv_dir, f"{dataset_name}_analysis_report.csv")
    
    with open(csv_file, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["Model", "Total Processed", "Insecure Default Rate (%)", "Repair Success Rate (%)", "Functional Regressions", "Ultimate Success Rate (%)"])
        
        for model_slug, data in stats.items():
            if data['total_processed'] == 0: continue
            writer.writerow([
                model_slug,
                data['total_processed'],
                round((data['pass1_vulnerable'] / data['total_processed']) * 100, 2),
                round((data['successful_heals'] / data['attempted_heals']) * 100, 2) if data['attempted_heals'] > 0 else 0,
                data['functional_regressions'],
                round((data['ultimate_success'] / data['total_processed']) * 100, 2)
            ])
            
    print(f"\n📁 CSV Report saved to: {csv_file}")

if __name__ == "__main__":
    analyze_data()