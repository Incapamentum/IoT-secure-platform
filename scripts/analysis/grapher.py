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
fig, axes = plt.subplots(nrows=1, ncols=2)
fig.suptitle("Data Collection & Publishing Latencies (1000 Samples)")

axes[0].set_yscale("log")
axes[0].set_ylabel("Latency (seconds)")
axes[0].boxplot(baseline_data)
axes[0].title.set_text("Baseline")
axes[0].xaxis.set_ticklabels([])

axes[1].set_yscale("log")
# axes[1].set_ylabel("Latency (seconds)")
axes[1].boxplot(transaction_data)
axes[1].title.set_text("Transactions")
axes[1].xaxis.set_ticklabels([])

fig.tight_layout()
plt.savefig(OUTPUT_FILE)

pickled_baseline.close()
pickled_transaction.close()