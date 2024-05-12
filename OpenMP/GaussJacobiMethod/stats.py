import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Function to read timing data from a log file
def read_timing_data(file_path):
    with open(file_path, 'r') as file:
        times = file.readlines()[1:]  # Skip the header line
    # Convert times to floats and handle the comma as decimal point
    return np.array([float(time.strip().replace(',', '.')) for time in times])

# Calculate statistics
def calculate_statistics(times):
    mean = np.mean(times)
    median = np.median(times)
    std_dev = np.std(times)
    return mean, median, std_dev

# Main directory for logs
log_dir = './logs'

# Find all log files
log_files = [os.path.join(log_dir, f) for f in os.listdir(log_dir) if f.endswith('.txt')]

# Data structure to hold the results
results = []
times = []

# Read each log file and calculate statistics
for file_path in log_files:
    local_times = read_timing_data(file_path)
    mean, median, std_dev = calculate_statistics(local_times)
    file_name = os.path.basename(file_path)
    mode = file_name.split('-')[0]
    N = (file_name.split('-')[-1].split('.')[0])[1:]
    T = (file_name.split('-')[1])[1:] if mode == 'PAR' else 1

    results.append({
        'Mode': mode,
        'N': int(N),
        'T': int(T),
        'Mean': mean,
        'Median': median,
        'Std Dev': std_dev
    })
    times.append({
        'Mode': mode,
        'N': int(N),
        'T': int(T),
        'Times': local_times
    })


# Convert to DataFrame
df = pd.DataFrame(results)
df.sort_values(['N', 'T'], inplace=True)

# Calculate speedup and efficiency
def calculate_speedup(row):
    seq_mean = df[(df['N'] == row['N']) & (df['Mode'] == 'SEQ')]['Mean'].values
    if len(seq_mean) > 0:  # Check if there is a sequential mean available
        return seq_mean[0] / row['Mean']
    return 1  # Return 1 if no sequential mean is available, indicating no speedup

df['Speedup'] = df.apply(lambda row: calculate_speedup(row) if row['Mode'] == 'PAR' else 1, axis=1)
df['Efficiency'] = df.apply(lambda row: row['Speedup'] / row['T'] if row['Mode'] == 'PAR' else None, axis=1)

# Save the table to CSV
df.to_csv('performance_metrics.csv', index=False)

# Plotting
fig, axs = plt.subplots(1, 3, figsize=(10, 10))

# get the unique mode-threads combinations that exist in the data
combinations = df[['Mode', 'T']].drop_duplicates().values

# Plot for Mean
for mode, thread in combinations:
    data = df[(df['Mode'] == mode) & (df['T'] == thread)]
    axs[0].plot(data['N'], data['Mean'], marker='o', linestyle='-', label=f'{mode} T={thread}')
axs[0].set_xlabel('N (matrix size)')
axs[0].set_ylabel('Mean time (s)')
axs[0].legend()
axs[0].grid(True)

# Plot for Speedup for each combination
for mode, thread in combinations:
    data = df[(df['Mode'] == mode) & (df['T'] == thread)]
    axs[1].plot(data['N'], data['Speedup'], marker='o', linestyle='-', label=f'{mode} T={thread}')
axs[1].set_xlabel('N (matrix size)')
axs[1].set_ylabel('Speedup')
axs[1].legend()
axs[1].grid(True)

# Plot for Efficiency for each combination
for mode, thread in combinations:
    data = df[(df['Mode'] == mode) & (df['T'] == thread)]
    axs[2].plot(data['N'], data['Efficiency'], marker='o', linestyle='-', label=f'{mode} T={thread}')
axs[2].set_xlabel('N (matrix size)')
axs[2].set_ylabel('Efficiency')
axs[2].legend()
axs[2].grid(True)

# histogram of execution times for each combination using N=5000, each combination in a different subplot
# add space for each subplot
fig4, axs4 = plt.subplots(1, 4, figsize=(20, 10))

i = 0
# use the "times" list to get the execution times for each combination
for mode, thread in combinations:
    data = [times[i]['Times'] for i in range(len(times)) if times[i]['N'] == 5000 and times[i]['Mode'] == mode and times[i]['T'] == thread][0]
    axs4[i].hist(data, bins=10)
    axs4[i].set_title(f'{mode} T={thread} N=5000')
    axs4[i].set_xlabel('Time (s)')
    axs4[i].set_ylabel('Frequency')
    axs4[i].grid(True)
    i += 1

# second figure
fig2, axs2 = plt.subplots(1, 3, figsize=(20, 10))

# print one boxplot for each N
unique_N = df['N'].unique()
for i, N in enumerate(unique_N):
    data = [times[i]['Times'] for i in range(len(times)) if times[i]['N'] == N]
    axs2[i].boxplot(data, labels=[f'T={times[i]["T"]}' for i in range(len(times)) if times[i]['N'] == N])
    axs2[i].set_title(f'N={N}')
    axs2[i].set_ylabel('Time (s)')
    axs2[i].grid(True)
fig2.suptitle('Execution time for different N and T', fontsize=16)

# print results in a table in a new figure, rounded to 4 decimal places
fig3, axs3 = plt.subplots(1, 1, figsize=(10, 10))
axs3.axis('off')
df = df.round(4)
print(df)
table = axs3.table(cellText=df.values, colLabels=df.columns, cellLoc='center', loc='center')
table.auto_set_font_size(False)
table.set_fontsize(14)
table.scale(1.2, 1.2)

plt.tight_layout()
plt.show()
