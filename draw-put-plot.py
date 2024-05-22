import sys
import os
import matplotlib.pyplot as plt
import re
import subprocess as sh
import time
def main():
    compare_only = False
    if len(sys.argv) >= 2 and sys.argv[1] == 'cmp':
        compare_only = True
    files = [f"put_plot_{i+1}KB.txt" for i in range(16)]
    bf_sizes = [(i+1) * 1024  for i in range( 15)]
    prog = "./build/put_plot_test"
    put_size = 100
    max_seconds = 20
    out_data_dir = "./text/put_plot_test"
    out_img_dir = "./imgs/put_plot_test"
    if not os.path.exists(out_data_dir):
        os.makedirs(out_data_dir)
    if not os.path.exists(out_img_dir):
        os.makedirs(out_img_dir)
    # 读取数据
    all_log_lines = []
    for out_file, bf_size in zip(files, bf_sizes):
        log_path = os.path.join(out_data_dir, out_file)
        img_path = os.path.join(out_img_dir, out_file)
        if not compare_only:
            sh.run(f"{prog} {put_size} {max_seconds} {bf_size} > {log_path}", shell=True)
            time.sleep(3)
        with open(log_path, 'r') as f:
            lines = f.readlines()
        # 提取吞吐量的值 put throughput: <number>
        filtered_lines = [line for line in lines if 'put throughput' in line]
        all_log_lines.append(filtered_lines.copy())
        if not compare_only:
            throughputs = [int(line.split()[2]) for line in filtered_lines]

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
            plt.savefig(img_path[:-4] + '.png')
            plt.close()


    plt.title('Put Throughput')
    plt.xlabel('Time (s)')
    plt.ylabel('Throughput (ops/s)')
    for i in range(len(all_log_lines)):
        if (i) % 4 != 0:
            continue
        lines = all_log_lines[i]
        throughputs = [int(line.split()[2]) for line in lines]

        # 绘制数据
        plt.plot(throughputs, label = f"{i+1}KB")
        # 保存图像
        # 在每5个数据点旁边添加数值
        # for i, v in enumerate(throughputs):
        #     if i % 5 == 0:
        #         plt.text(i, v, str(v), ha='right', va='bottom')
    plt.legend()
    plt.savefig(os.path.join(out_img_dir, "comparison.png"))
    plt.close()

main()