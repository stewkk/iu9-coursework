#!/usr/bin/env python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import json
import re

def main():
    matplotlib.use('TkAgg')
    json_data = None
    with open('out.json', 'r') as f:
        json_data = json.load(f)
    df = pd.DataFrame(json_data['benchmarks']).set_index('name')
    df = df[df['aggregate_name'] == 'mean']
    df['message_size'] = df.index
    df['message_size'] = df['message_size'].transform(lambda s: int(re.match('BM_.*\/([0-9]+)_mean', s)[1]))
    df = df.filter(items=['message_size', 'real_time'])
    df.index = pd.DataFrame(df.index)['name'].transform(lambda s: re.match('BM_([^</]*)', s)[1])
    df = df.pivot(columns='message_size', values='real_time').T
    df = df.filter(regex='(?!(^FileMemMapping$))^.*$')

    print(df)
    plot = df.plot(title='Mean real time', xticks=[16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192])
    plt.show()


if __name__ == "__main__":
    main()
