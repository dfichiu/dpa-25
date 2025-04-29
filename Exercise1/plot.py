import pandas as pd
import matplotlib.pyplot as plt

# Set problem number.
PROBLEM = 3
PLOT = False

#################
### Problem 1 ###
#################
if PROBLEM == 1:
    data_path = 'data/pb1_out.csv'

    n = 'n'
    error = 'error'
    time = 'time'
    columns = [n, error, time]

    df = pd.read_csv(data_path, header=None, names=columns)

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6), sharex=True)

    # error
    ax1.plot(df[n], df[error], color='blue', marker='o')
    ax1.set_title('Error')
    ax1.set_ylabel('error')
    ax1.grid(True)

    # runtime
    ax2.plot(df[n], df[time], color='green', marker='s')
    ax2.set_title('Runtime')
    ax2.set_xlabel('n')
    ax2.set_ylabel('runtime (s)')
    ax2.grid(True)

    plt.tight_layout()
    plt.show()

#################
### Problem 2 ###
#################
if PROBLEM == 2:
    data_path = 'data/pb2_out.csv'

    n = 'n'
    size = 'size'
    error = 'error'
    time = 'time'
    columns = [n, size, error, time]

    df = pd.read_csv(data_path, header=None, names=columns)

    if PLOT:
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6), sharex=True)

        for n_process, ls in zip(df[size].unique(), ['-', '--', ':']):
            df_tmp = df[df[size] == n_process]

            # error
            ax1.plot(
                df_tmp[n],
                df_tmp[error],
                color='blue',
                marker='o',
                ls=ls,
                label=f'{n_process} processes',
            )

            # runtime
            ax2.plot(
                df_tmp[n],
                df_tmp[time],
                color='green',
                marker='s',
                ls=ls,
                label=f'{n_process} processes',
            )

        ax1.set_title('Error')
        ax1.set_ylabel('error')
        ax1.grid(True)

        ax2.set_title('Runtime')
        ax2.set_xlabel('n')
        ax2.set_ylabel('runtime (s)')
        ax2.grid(True)

        plt.tight_layout()
        ax1.legend()
        ax2.legend()
        plt.show()

    data_path1 = 'data/pb1_out.csv'
    df1 = pd.read_csv(data_path1, header=None, names=[n, error, time])
    df1 = df1[df1[n] >= 1000].reset_index(drop=True)

    if False:
        for n_process in [2, 4]:
            df_tmp = df[df[size] == n_process].reset_index(drop=True)
            print(df1[time] / df_tmp[time] / n_process)
    
    df_1p = df[df[size] == 1]
    df_1p = df_1p[df_1p[size] < 1e7].reset_index(drop=True)

    if True:
        for n_process in [2, 4]:
            df_tmp = df[df[size] == n_process].reset_index(drop=True)
            print(df_1p[time] / df_tmp[time] / n_process)


#################
### Problem 3 ###
#################
if PROBLEM == 3:
    data_path = 'data/pb3_out.csv'

    n = 'n'
    size = 'size'
    error = 'error'
    time = 'time'
    columns = [n, size, error, time]

    df = pd.read_csv(data_path, header=None, names=columns)

    if PLOT:
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6), sharex=True)

        for n_process, ls in zip(df[size].unique(), ['-', '--', ':']):
            df_tmp = df[df[size] == n_process]

            # error
            ax1.plot(
                df_tmp[n],
                df_tmp[error],
                color='blue',
                marker='o',
                ls=ls,
                label=f'{n_process} processes',
            )

            # runtime
            ax2.plot(
                df_tmp[n],
                df_tmp[time],
                color='green',
                marker='s',
                ls=ls,
                label=f'{n_process} processes',
            )

        ax1.set_title('Error')
        ax1.set_ylabel('error')
        ax1.grid(True)

        ax2.set_title('Runtime')
        ax2.set_xlabel('n')
        ax2.set_ylabel('runtime (s)')
        ax2.grid(True)

        plt.tight_layout()
        ax1.legend()
        ax2.legend()
        plt.show()

    data_path1 = 'data/pb1_out.csv'
    df1 = pd.read_csv(data_path1, header=None, names=[n, error, time])
    df1 = df1[df1[n] >= 1000].reset_index(drop=True)

    if False:
        for n_process in [2, 4]:
            df_tmp = df[df[size] == n_process].reset_index(drop=True)
            print(df1[time] / df_tmp[time] / n_process)
    
    df_1p = df[df[size] == 1]
    df_1p = df_1p[df_1p[size] < 1e7].reset_index(drop=True)

    if True:
        for n_process in [2, 4]:
            df_tmp = df[df[size] == n_process].reset_index(drop=True)
            print(df_1p[time] / df_tmp[time] / n_process)
