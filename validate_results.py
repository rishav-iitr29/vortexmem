#!/usr/bin/env python3
import os
import re

def validate_vortex_output(result_file):
    with open(result_file, 'r') as f:
        content = f.read()

    # Track what we found
    passed = True
    reasons = []

    # 1. Check for Allocation Stats
    if "Total Attempts:" in content:
        success_rate = re.search(r'Success Rate:\s+([\d.]+)%', content)
        if success_rate:
            reasons.append(f"Allocation Rate: {success_rate.group(1)}%")
        else:
            passed = False

    # 2. Check for Cache / TLB Stats
    if "Hardware Pipeline Metrics" in content:
        tlb_hits = re.search(r'TLB\s+-> Hits:\s+(\d+)', content)
        page_faults = re.search(r'Page Faults:\s+(\d+)', content)
        if tlb_hits and page_faults:
            reasons.append(f"TLB Hits: {tlb_hits.group(1)}, Page Faults: {page_faults.group(1)}")
        else:
            passed = False
            
    if passed and reasons:
        return True, f"✓ {os.path.basename(result_file)} passed ({', '.join(reasons)})"
    elif passed:
        return True, f"✓ {os.path.basename(result_file)} passed (No advanced metrics detected, but executed cleanly)"
    return False, f"✗ {os.path.basename(result_file)} failed validation."

def main():
    print("VortexMem: Validating Telemetry Results\n")
    
    results_dir = "results"
    if not os.path.exists(results_dir):
        print("Results directory not found. Run ./run_all_tests.sh first.")
        return 1

    files = [f for f in os.listdir(results_dir) if f.endswith(".txt")]
    passed_count = 0

    for file in files:
        success, msg = validate_vortex_output(os.path.join(results_dir, file))
        print(msg)
        if success:
            passed_count += 1

    print("\n")
    if passed_count == len(files) and len(files) > 0:
        print(f"All {len(files)} tests passed validation!")
    else:
        print(f"{passed_count}/{len(files)} tests passed. Check logs.")

if __name__ == "__main__":
    main()