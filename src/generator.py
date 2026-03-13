import os
import json
import subprocess
from openai import OpenAI
from dotenv import load_dotenv

load_dotenv()

# Configuration
client = OpenAI(
    base_url="https://openrouter.ai/api/v1",
    api_key=os.getenv("OPENROUTER_API_KEY"),
    default_headers={
        "HTTP-Referer": "http://localhost:3000", # Required by OpenRouter
        "X-Title": "Vibe Coding Security Research",
    }
)

MODELS = ["openai/gpt-4o", "google/gemini-2.0-flash-001"]

def generate_and_scan():
    # Load your VPB-20 prompts
    with open('data/prompts/vpb_20.json', 'r') as f:
        prompts = json.load(f)

    for model in MODELS:
        model_slug = model.replace("/", "-")
        
        for p in prompts[:5]: # Testing with first 5 for now
            print(f"🚀 Generating: {model} | {p['prompt_id']}")
            
            # 1. Generate Code
            response = client.chat.completions.create(
                model=model,
                messages=[{"role": "user", "content": p['prompt_text'] + "\nOnly provide the Python code, no explanation."}]
            )
            code = response.choices[0].message.content
            
            # 2. Save Code to RVD-100 Structure
            app_dir = f"data/raw_apps/{model_slug}/{p['prompt_id']}"
            os.makedirs(app_dir, exist_ok=True)
            with open(f"{app_dir}/main.py", "w") as f:
                f.write(code)

            # 3. Trigger Snyk Scan (Step 3: Automated Scan)
            print(f"🛡️ Scanning {p['prompt_id']} with Snyk...")
            result_file = f"results/raw_scans/{model_slug}_{p['prompt_id']}.json"
            
            # We use 'snyk code test' as defined in your plan
            subprocess.run([
                "snyk", "code", "test", app_dir, 
                f"--json-file-output={result_file}"
            ])

if __name__ == "__main__":
    generate_and_scan()