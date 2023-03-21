"""
Plot results from the experiments.
"""
import matplotlib.ticker
import pandas as pd
import matplotlib.pyplot as plt


def plot_sbm_results():
    filename = "sbm/aggregate_results.csv"
    results_df = pd.read_csv(filename)

    fig = plt.figure(figsize=(3.25, 2.75))
    plt.rcParams.update({
        "text.usetex": True,
        "font.family": "Times"
    })
    ax = plt.axes([0.2, 0.16, 0.75, 0.82])
    plt.plot(0.001 * results_df['k'],
             0.001 * results_df['disk_time'],
             label="On disk",
             linewidth=3,
             linestyle='solid',
             color='red')
    plt.plot(0.001 * results_df['k'],
             0.001 * results_df['mem_time'],
             label="In memory",
             linewidth=3,
             linestyle='dashed',
             color='blue')
    plt.legend(loc='best', fontsize=10)
    plt.xlabel('Number of nodes (millions)', fontsize=10)
    plt.ylabel('Running time (s)', fontsize=10)
    plt.show()


def main():
    plot_sbm_results()


if __name__ == "__main__":
    main()
