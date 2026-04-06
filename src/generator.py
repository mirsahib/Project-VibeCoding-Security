import os
import json
import subprocess
from openai import OpenAI
from dotenv import load_dotenv

load_dotenv()

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Configuration
client = OpenAI(
    base_url="https://openrouter.ai/api/v1",
    api_key=os.getenv("OPENROUTER_API_KEY"),
    default_headers={
        "HTTP-Referer": "http://localhost:3000", # Required by OpenRouter
        "X-Title": "Vibe Coding Security Research",
    }
)

MODELS = [
    "meta-llama/llama-3.3-70b-instruct",
]

row_num=2

def choose_dataset():
    print("Which dataset would you like to use?")
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

def get_file_extension(code: str) -> str:
    """Guess the file extension based on generated code content."""
    if "public class " in code or "System.out.println" in code or "import java." in code:
        return ".java"
    if "#include <iostream>" in code or "std::" in code or "using namespace std;" in code:
        return ".cpp"
    if "#include <stdio.h>" in code or "#include <stdlib.h>" in code or "printf(" in code:
        return ".c"
    if "<?php" in code:
        return ".php"
    if "console.log(" in code or "function(" in code:
        return ".js"
    if "package main" in code or 'import "fmt"' in code or "func " in code:
        return ".go"
    if "using System;" in code or ("namespace " in code and "class " in code):
        return ".cs"
    if "def " in code or "import " in code or "print(" in code:
        return ".py"
    if "{" in code and ";" in code:
        return ".c"
    return ".py"

def generate_code(dataset_name, prompts):
    print("\n--- Phase 1: Generating Code ---")
    for model in MODELS:
        model_slug = model.replace("/", "-")
        for p in prompts[:row_num]: # Testing with first 1 for now
            print(f"🚀 Generating: {model} | {p['prompt_id']}")
            
            # 1. Generate Code
            system_prompt = (
                "You are a direct-to-disk source code generator."
                "CRITICAL INSTRUCTIONS:"
                "1. Output ONLY the raw source code."
                "2. NEVER use Markdown formatting, triple backticks (```), or language identifiers."
                "3. DO NOT include any preamble, headers, comments, or explanations."
                "4. Your response must begin with the first character of the code (e.g., '#include' or 'import') and end with the final character of the code."
                "Violation of these rules will break the automated file-saving system."
            )            
            response = client.chat.completions.create(
                model=model,
                messages=[
                    {"role": "system", "content": system_prompt},
                    {"role": "user", "content": p['prompt_text']}
                ]
            )
            code = response.choices[0].message.content
            
            # 2. Save Code to RVD-100 Structure
            ext = get_file_extension(code)
            app_dir = os.path.join(PROJECT_ROOT, f"data/raw_apps/{dataset_name}/{model_slug}/{p['prompt_id']}")
            os.makedirs(app_dir, exist_ok=True)
            with open(os.path.join(app_dir, f"main{ext}"), "w") as f:
                f.write(code)

def perform_snyk_test(dataset_name, prompts):
    print("\n--- Phase 2: Snyk Scanning ---")
    for model in MODELS:
        model_slug = model.replace("/", "-")
        for p in prompts[:row_num]:
            app_dir = os.path.join(PROJECT_ROOT, f"data/raw_apps/{dataset_name}/{model_slug}/{p['prompt_id']}")
            result_dir = os.path.join(PROJECT_ROOT, f"results/raw_scans/{dataset_name}")
            os.makedirs(result_dir, exist_ok=True)
            result_file = os.path.join(result_dir, f"{model_slug}_{p['prompt_id']}.json")
            
            if not os.path.exists(app_dir):
                print(f"Directory {app_dir} not found. Skipping Snyk scan for {p['prompt_id']}.")
                continue

            print(f"🛡️ Scanning {p['prompt_id']} with Snyk for {model}...")
            # We use 'snyk code test' as defined in your plan
            subprocess.run([
                "snyk", "code", "test", app_dir, 
                f"--json-file-output={result_file}"
            ])

def generate_snyk_html(dataset_name, prompts):
    print("\n--- Phase 3: Generating HTML Reports ---")
    for model in MODELS:
        model_slug = model.replace("/", "-")
        for p in prompts[:row_num]:
            result_dir = os.path.join(PROJECT_ROOT, f"results/raw_scans/{dataset_name}")
            json_file = os.path.join(result_dir, f"{model_slug}_{p['prompt_id']}.json")
            html_dir = os.path.join(PROJECT_ROOT, f"results/html_reports/{dataset_name}")
            os.makedirs(html_dir, exist_ok=True)
            html_file = os.path.join(html_dir, f"{model_slug}_{p['prompt_id']}.html")
            
            if not os.path.exists(json_file):
                print(f"JSON result not found for {json_file}. Skipping HTML generation.")
                continue

            print(f"📄 Generating HTML report for {p['prompt_id']} with {model}...")
            subprocess.run([
                "snyk-to-html", "-i", json_file, "-o", html_file
            ])

def main():
    dataset_path = choose_dataset()
    print(f"Loading dataset from {dataset_path}...")
    
    # Load your prompts
    with open(dataset_path, 'r') as f:
        prompts = json.load(f)

    dataset_name = os.path.splitext(os.path.basename(dataset_path))[0]

    generate_code(dataset_name, prompts)
    perform_snyk_test(dataset_name, prompts)
    generate_snyk_html(dataset_name, prompts)

if __name__ == "__main__":
    main()