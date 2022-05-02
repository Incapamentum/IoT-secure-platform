import pickle
import sys

import statistics as stats

if (len(sys.argv) != 3):
    print("Insufficient args")
    sys.exit(-1)

data_path = sys.argv[1]
output_path = sys.argv[2]

pickled_file = open(data_path, "rb")
pickled_data = pickle.load(pickled_file)

pickled_file.close()

# Collecting descripting stats
avg_latency = stats.fmean(pickled_data)
med_latency = stats.median(pickled_data)
min_latency = min(pickled_data)
max_latency = max(pickled_data)

quant_latency = stats.quantiles(pickled_data)
iqr_latency = quant_latency[2] - quant_latency[0]

# Finding number of outliers in the data
lower = quant_latency[0] - 1.5 * iqr_latency
upper = quant_latency[2] + 1.5 * iqr_latency

outlier_count = 0

for d in pickled_data:

    # Way below
    if (d < lower):
        outlier_count += 1
    elif (d > upper):
        outlier_count += 1

# Building up the message output
output_msg = ""
output_msg += f'Average latency: {avg_latency:.4f}\n'
output_msg += f'Median latency: {med_latency:.4f}\n'
output_msg += f'Minimum latency: {min_latency:.4f}\n'
output_msg += f'Maximum latency: {max_latency:.4f}\n'
output_msg += f'Number of outliers: {outlier_count}\n'

with open(output_path, "w") as f:
    f.write(output_msg)