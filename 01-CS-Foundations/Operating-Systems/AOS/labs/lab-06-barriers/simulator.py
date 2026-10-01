import sys
import math
import argparse
import csv

def calc_counting(N):
    return {
        'rounds': 2, 
        'messages': 2 * N, 
        'critical_path': N
    }

def calc_tree(N, K=2):
    rounds = 2 * math.ceil(math.log(N, K))
    return {
        'rounds': rounds,
        'messages': 4 * (N - 1), 
        'critical_path': rounds
    }

def calc_mcs(N):
    arrival_rounds = math.ceil(math.log(N, 4)) if N > 1 else 0
    wakeup_rounds = math.ceil(math.log(N, 2)) if N > 1 else 0
    return {
        'rounds': arrival_rounds + wakeup_rounds,
        'messages': 2 * (N - 1),
        'critical_path': arrival_rounds + wakeup_rounds
    }

def calc_tournament(N):
    rounds = math.ceil(math.log(N, 2)) if N > 1 else 0
    return {
        'rounds': 2 * rounds,
        'messages': 2 * (N - 1),
        'critical_path': 2 * rounds
    }

def calc_dissemination(N):
    rounds = math.ceil(math.log(N, 2)) if N > 1 else 0
    return {
        'rounds': rounds,
        'messages': N * rounds,
        'critical_path': rounds
    }

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--plot', action='store_true', help='Generate plot')
    args = parser.parse_args()

    results = []
    for n in range(2, 65):
        c = calc_counting(n)
        t = calc_tree(n, K=2)
        m = calc_mcs(n)
        to = calc_tournament(n)
        d = calc_dissemination(n)

        results.append({
            'N': n,
            'Counting_msg': c['messages'], 'Counting_cp': c['critical_path'],
            'Tree_msg': t['messages'], 'Tree_cp': t['critical_path'],
            'MCS_msg': m['messages'], 'MCS_cp': m['critical_path'],
            'Tournament_msg': to['messages'], 'Tournament_cp': to['critical_path'],
            'Dissemination_msg': d['messages'], 'Dissemination_cp': d['critical_path']
        })

    writer = csv.DictWriter(sys.stdout, fieldnames=results[0].keys())
    writer.writeheader()
    for r in results:
        writer.writerow(r)

    if args.plot:
        try:
            import matplotlib.pyplot as plt
            import pandas as pd
            df = pd.DataFrame(results)
            
            fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
            
            ax1.plot(df['N'], df['Counting_msg'], label='Counting')
            ax1.plot(df['N'], df['Tree_msg'], label='Tree (K=2)')
            ax1.plot(df['N'], df['MCS_msg'], label='MCS')
            ax1.plot(df['N'], df['Tournament_msg'], label='Tournament')
            ax1.plot(df['N'], df['Dissemination_msg'], label='Dissemination')
            ax1.set_title('Network Messages vs N')
            ax1.set_xlabel('Threads (N)')
            ax1.set_ylabel('Total Messages')
            ax1.legend()
            
            ax2.plot(df['N'], df['Counting_cp'], label='Counting')
            ax2.plot(df['N'], df['Tree_cp'], label='Tree (K=2)')
            ax2.plot(df['N'], df['MCS_cp'], label='MCS')
            ax2.plot(df['N'], df['Tournament_cp'], label='Tournament')
            ax2.plot(df['N'], df['Dissemination_cp'], label='Dissemination')
            ax2.set_title('Critical Path vs N')
            ax2.set_xlabel('Threads (N)')
            ax2.set_ylabel('Critical Path Length (Rounds)')
            ax2.legend()
            
            plt.tight_layout()
            plt.savefig('barrier_plot.png')
            print("Plot saved to barrier_plot.png", file=sys.stderr)
        except ImportError:
            print("matplotlib/pandas not found. Plot not generated.", file=sys.stderr)

if __name__ == '__main__':
    main()
