import pandas as pd
import matplotlib.pyplot as plt
from IPython.display import display

PLOT = True

data_path = 'res/res.csv'

matrix = 'matrix'
n_threads = 'n_threads'
runtime = 'time'
columns = [matrix, n_threads, runtime]

df = pd.read_csv(data_path, header=None, names=columns)

if PLOT:
    for pb in df[matrix].unique():
        df_mat = df[df[matrix] == pb]
        fig, ax = plt.subplots()

        ax.plot(df_mat[n_threads], df_mat[runtime], color='blue', marker='o')

        ax.set_title('Runtime')
        ax.set_ylabel('runtime (s)')
        ax.set_xlabel('#threads')
        ax.grid(True)

        fig.savefig(f'res_{pb}.png')
        plt.show()


# Speedup
speedup = 'speedup'
cag_seq_runtime = 0.002306
road_seq_runtime = 0.002306

df_cag = df[df[matrix] == 'CAG']
df_cag[speedup] = cag_seq_runtime / df[runtime]
display(df_cag[[n_threads, speedup]])

df_road = df[df[matrix] == 'road']
df_road[speedup] = road_seq_runtime / df[runtime]
display(df_road[[n_threads, speedup]])

# Efficiency
efficiency = 'efficiency'

time_1t_cag = df_cag[df_cag[n_threads] == 1][runtime].item()
df_cag[efficiency] = (time_1t_cag / df_cag[runtime]) / df_cag[n_threads]
display(df_cag[[n_threads, efficiency]])

time_1t_road = df_road[df_road[n_threads] == 1][runtime].item()
df_road[efficiency] = (time_1t_road / df_road[runtime]) / df_road[n_threads]
display(df_road[[n_threads, efficiency]])

  