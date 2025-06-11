import pandas as pd
import matplotlib.pyplot as plt
from IPython.display import display

PLOT = False

data_path = 'data/res.csv'

n_threads = 'n_threads'
runtime = 'time'
columns = [n_threads, runtime]

df = pd.read_csv(data_path, header=None, names=columns)

if PLOT:
    fig, ax = plt.subplots()

    ax.plot(df[n_threads], df[runtime], color='blue', marker='o')

    ax.set_title('Runtime')
    ax.set_ylabel('runtime (s)')
    ax.set_xlabel('#threads')
    ax.grid(True)

    fig.savefig('res.png')
    plt.show()


# Speedup
speedup = 'speedup'
seq_runtime = 7.98

# df[speedup] = seq_runtime / df[runtime]
# display(df[[n_threads, speedup]])

# Efficiency
efficiency = 'efficiency'

time_1t = df[df[n_threads] == 1][runtime].item()

df[efficiency] = (time_1t / df[runtime]) / df[n_threads]
display(df[[n_threads, efficiency]])

  