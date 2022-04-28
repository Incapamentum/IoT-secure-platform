import pickle
import sys

OUTPUT_PATH = "..\\output\\"

if (len(sys.argv) != 3):
    sys.exit(-1)

sample = 0
times = []

log_path = sys.argv[1]
output_file = OUTPUT_PATH + sys.argv[2]

pickled_output = open(output_file, "wb")

try:
    with open(log_path, "r", encoding="cp850") as f:
        for line in f:
            if (sample == 1000):
                break
            
            if ("Elapsed time" in line):
                sample += 1
                line_split = line.split(' ')
                times.append(float(line_split[2]))
except IOError:
    print("Error type: " + str(IOError))
    print("Please check the filepath of the log file")
    sys.exit(-1)

print(f'Total samples: {sample}')
print("Pickling...")
pickle.dump(times, pickled_output)
print("Successfully pickled!")

pickled_output.close()