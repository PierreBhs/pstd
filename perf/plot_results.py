import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import json
import argparse
import sys
import os

def parse_benchmark_json(json_file):
    """Parses Google Benchmark JSON output."""
    try:
        with open(json_file, 'r') as f:
            data = json.load(f)
    except FileNotFoundError:
        print(f"Error: JSON file not found at {json_file}", file=sys.stderr)
        sys.exit(1)
    except json.JSONDecodeError:
        print(f"Error: Could not decode JSON from {json_file}", file=sys.stderr)
        sys.exit(1)

    benchmarks = []
    if 'benchmarks' not in data:
        print(f"Error: 'benchmarks' key not found in {json_file}", file=sys.stderr)
        sys.exit(1)

    for item in data['benchmarks']:
        name = item.get('name', 'unknown')
        # Extract N (input size) from name like 'BM_PstdVectorPushBack/1024' or 'BM_PstdVectorPushBack/1024/...'
        parts = name.split('/')
        input_n = None
        if len(parts) > 1:
            try:
                input_n = int(parts[1])
            except (ValueError, IndexError):
                print(f"Warning: Could not parse N from benchmark name: {name}", file=sys.stderr)
                # Try to get from run_name if available and matches pattern
                run_name = item.get('run_name')
                if run_name:
                     rn_parts = run_name.split('/')
                     if len(rn_parts) > 1:
                          try:
                               input_n = int(rn_parts[1])
                          except ValueError:
                              pass # Ignore if run_name doesn't have N


        # Determine library (pstd or std) and operation
        library = "unknown"
        operation = "unknown"
        base_name = parts[0] # e.g., BM_PstdVectorPushBack
        if base_name.startswith("BM_Pstd"):
            library = "pstd"
            operation = base_name.replace("BM_Pstd", "")
        elif base_name.startswith("BM_Std"):
            library = "std"
            operation = base_name.replace("BM_Std", "")
        else:
             print(f"Warning: Could not determine library/operation from: {base_name}", file=sys.stderr)

        # Prefer real_time over cpu_time for wall-clock performance
        time_ns = item.get('real_time')
        if time_ns is None:
             time_ns = item.get('cpu_time') # Fallback to CPU time

        if time_ns is not None and input_n is not None:
             # Handle time units (Google Benchmark default is nanoseconds)
             time_unit = item.get('time_unit', 'ns')
             if time_unit == 'ms':
                 time_ns *= 1e6
             elif time_unit == 'us':
                 time_ns *= 1e3

             benchmarks.append({
                 'name': name,
                 'base_name': base_name,
                 'library': library,
                 'operation': operation,
                 'N': input_n,
                 'time_ns': time_ns
             })
        else:
             print(f"Warning: Skipping benchmark due to missing N or time: {name}", file=sys.stderr)


    if not benchmarks:
         print(f"Error: No valid benchmark data parsed from {json_file}", file=sys.stderr)
         sys.exit(1)

    return pd.DataFrame(benchmarks)

def plot_benchmarks(df, output_dir="."):
    """Generates plots comparing pstd and std for each operation."""
    sns.set_theme(style="whitegrid")
    operations = df['operation'].unique()

    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        print(f"Created output directory: {output_dir}")

    for op in operations:
        op_df = df[df['operation'] == op].sort_values('N')

        if op_df.empty:
            print(f"Skipping plot for operation '{op}' - no data.")
            continue
        if len(op_df['library'].unique()) < 2:
             print(f"Skipping plot for operation '{op}' - only one library found.")
             continue


        plt.figure(figsize=(10, 6))
        plot = sns.lineplot(data=op_df, x='N', y='time_ns', hue='library', marker='o')
        plt.title(f'Performance Comparison: {op}')
        plt.xlabel('Input Size (N)')
        plt.ylabel('Time (nanoseconds)')
        plt.xscale('log', base=2) # Use log scale for N if it spans orders of magnitude
        plt.yscale('log') # Use log scale for time if it spans orders of magnitude
        plt.legend(title='Library')
        plt.grid(True, which="both", ls="--")

        plot_filename = os.path.join(output_dir, f'perf_{op}.png')
        plt.savefig(plot_filename)
        print(f"Saved plot: {plot_filename}")
        plt.close() # Close the plot to free memory

def main():
    parser = argparse.ArgumentParser(description='Plot Google Benchmark results.')
    parser.add_argument('json_file', help='Path to the Google Benchmark JSON output file.')
    parser.add_argument('-o', '--output-dir', default='perf_plots',
                        help='Directory to save the generated plots (default: perf_plots)')
    args = parser.parse_args()

    df = parse_benchmark_json(args.json_file)
    # print("Parsed DataFrame head:")
    # print(df.head()) # Debug print
    plot_benchmarks(df, args.output_dir)

if __name__ == "__main__":
    main()
