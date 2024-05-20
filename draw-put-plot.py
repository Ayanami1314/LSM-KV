import sys
import os
import matplotlib.pyplot as plt
import re

def main():

    # 读取数据
    with open('put_plot.txt', 'r') as f:
        lines = f.readlines()

    # 提取吞吐量的值 put throughput: <number>
    throughputs = [int(line.split()[2]) for line in lines]

    # 绘制数据
    plt.plot(throughputs, marker='o')
    plt.title('Put Throughput')
    plt.xlabel('Time (s)')
    plt.ylabel('Throughput (ops/s)')
    # 保存图像
    # 在每5个数据点旁边添加数值
    # for i, v in enumerate(throughputs):
    #     if i % 5 == 0:
    #         plt.text(i, v, str(v), ha='right', va='bottom')
    plt.savefig('./imgs/put_throughput.png')

main()