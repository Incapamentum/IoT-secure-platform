import pickle
import sys

import numpy as np

from matplotlib import pyplot as plt

OUTPUT_FILE = "..\\output\\Latencies.png"

if (len(sys.argv) != 3):
    print("Insufficient args")
    sys.exit(-1)

baseline_path = sys.argv[1]
transaction_path = sys.argv[2]

pickled_baseline = open(baseline_path, "rb")
pickled_transaction = open(transaction_path, "rb")

baseline_data = pickle.load(pickled_baseline)
transaction_data = pickle.load(pickled_transaction)

# Plotting figures
plt.boxplot([baseline_data, transaction_data], sym='')
plt.title("Data Collection & Publishing Latencies (1000 Samples)")
plt.ylabel("Latency (seconds)")
plt.yscale("log")
plt.xticks(ticks=[1, 2], labels=["Baseline", "Transaction"])

# fig.tight_layout()
plt.savefig(OUTPUT_FILE)

pickled_baseline.close()
pickled_transaction.close()