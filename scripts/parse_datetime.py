import sys
import os.path
from datetime import datetime, timedelta

if len(sys.argv) < 2:
    print("Please provide a path to a datetime file as a command-line argument.")
    sys.exit(1)

file_path = sys.argv[1]

if not os.path.isfile(file_path):
    print(f"'{file_path}' does not exist.")
    sys.exit(1)

with open(file_path, 'r') as f:
    start_time = datetime.strptime(f.readline().strip(), '%a %b %d %H:%M:%S %Y')
    elapsed_time = timedelta(0)
    print (elapsed_time)
    for line in f:
        if not line.strip():
            continue
        timestamp = datetime.strptime(line.strip(), '%a %b %d %H:%M:%S %Y')
        time_since_start = timestamp - start_time
        print(time_since_start)
