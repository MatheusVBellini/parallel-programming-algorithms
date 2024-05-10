import pandas as pd
import numpy as np
import glob
import re

# Define the lists
Ns = [10, 20, 30]  # Add more values as needed
Ts = ["A", "B", "C"]  # Add more values as needed

# Create a dictionary to store the data
data = {}

# Loop through the files
for file in glob.glob("PAR-N*-T*.txt"):
    # Extract the values from the filename
    match = re.search(r"PAR-N(\d+)-T(.+).txt", file)
    N = int(match.group(1))
    T = match.group(2)

    # Read the file
    with open(file, "r") as f:
        next(f)  # Skip the first line
        numbers = [float(line.strip()) for line in f]

    # Calculate statistics
    mean = np.mean(numbers)
    std_dev = np.std(numbers)
    variance = np.var(numbers)

    # Store the data
    if N not in data:
        data[N] = {}
    data[N][T] = {"Mean": mean, "Standard Deviation": std_dev, "Variance": variance}

# Create a DataFrame
df = pd.DataFrame()
for N, values in data.items():
    for T, stats in values.items():
        row = pd.DataFrame(
            {
                "Statistic": list(stats.keys()),
                "File": f"PAR-N{N}-T{T}",
                "Value": list(stats.values()),
            }
        )
        df = pd.concat([df, row])

# Write to Excel file
df.to_excel("statistics.xlsx", index=False)
