import pickle
import sys

import numpy as np

from matplotlib import pyplot as plt

if (len(sys.argv) != 3):
    print("Insufficient args")
    sys.exit(-1)

labels = ("Baseline", "Transactions")

baseline_path = sys.argv[1]
transaction_path = sys.argv[2]

pickled_baseline = open(baseline_path, "rb")
pickled_transaction = open(transaction_path, "rb")

baseline_data = pickle.load(pickled_baseline)
transaction_data = pickle.load(pickled_transaction)

# baseline_data = log(baseline_data)
# transaction_data = log(transaction_data)

data = [baseline_data, transaction_data]

# Plotting figures
fig = plt.figure()
fig.suptitle("Data Collection & Publishing Latencies (1000 Samples)")

ax = fig.add_subplot(111)

ax.set_yscale("log")
ax.set_ylabel("Latency (seconds)")
ax.set_xticklabels(labels)

ax.boxplot(data)

plt.show()

# plt.yscale("log")
# plt.figure(figsize=(5, 5))
# plt.yscale("log")
# plt.ylabel("Latency (seconds)")
# plt.xticks(np.arange(len(labels)) + 1, labels)

# plt.boxplot(data)
# plt.title("Data Collection & Publishing Latencies")
# plt.show()