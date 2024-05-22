import matplotlib.pyplot as plt
import numpy as np
import re
import os
import sys
if __name__=="__main__":
    # 读取日志文件
    log_file = 'report.txt'
    savepath = "./imgs/gpds"
    with open(log_file, 'r') as f:
        lines = f.readlines()
    if not os.path.exists(savepath):
        os.makedirs(savepath)
    # 初始化数据存储
    data = [] # list of single_data
    all_data = [] # list of data
    # 解析日志文件
    first_flag = True
    default_single_data ={"value_size": 0, "prebuilt_data_num": 0,"Get": [], "Put":[], "Del": [], "Scan": []} 
    single_data = default_single_data.copy()
    last_operation = "Get"
    cnt = 0
    for line in lines:
        if line.startswith("Test with different BloomFilter size"):
            continue
        if line.startswith("BloomFilter size"):
            continue

        if line.startswith("use_bf"):
            if not first_flag and single_data['Get'] != []:
                data.append(single_data.copy())
                single_data = default_single_data.copy()
                all_data.append(data.copy())
            data = []
            if cnt == 1:
                break
            cnt = 1
            continue
        if line.startswith('bf_size'):
            first_flag = False
            continue
        else:
            operation = None
            if line.startswith('value_size'):
                if len(single_data['Get']) > 0:
                    data.append(single_data.copy())
                single_data = {"value_size": 0, "prebuilt_data_num": 0,"Get": [],  "Put":[],"Del": [], "Scan": []}
                single_data['value_size'] = int(line.split(',')[0].strip().split(":")[1])
                single_data['prebuilt_data_num'] = int(line.split(',')[1].strip().split(":")[1])
                continue
            if line.startswith('Get') or line.startswith('Put') or line.startswith('Del') or line.startswith('Scan'):
                operation, time = line.split(':')
                time = int(time.strip().split()[0])
                last_operation = operation #暂时用不到time
                single_data[last_operation].append(time)
                continue
            if line.strip().startswith('throughput'):
                _, throughput = line.strip().split(':')
                throughput = format(float(throughput.strip().split("/")[0]), '.2e')
                assert len(single_data[last_operation]) < 2
                # single_data[last_operation].append(throughput)
    file_md = "./tables/gpds-time.md"
    file_tex = "./tables/gpds-time.tex"
    if not os.path.exists("./tables"):
        os.makedirs("./tables")
    for data in all_data:
        origin_stdout = sys.stdout
        # generate the markdown table
        def gen_md():
            print("| value_size | prebuilt_data_num | Get | Put | Del | Scan |")
            print("|---|---|---|---|---|---|")
            for single_data in data:
                print(f"| {single_data['value_size']} | {single_data['prebuilt_data_num']} | {single_data['Get'][0]} | {single_data['Put'][0]} | {single_data['Del'][0]} | {single_data['Scan'][0]} |")
        # generate the latex table
        def gen_tex():
            print("\\begin{table}[H]")
            print("\\centering")
            print("\\begin{tabular}{|c|c|c|c|c|c|}")
            print("\\hline")
            print("value_size & prebuilt_data_num & Get & Put & Del & Scan \\\\")
            print("\\hline")
            for single_data in data:
                print(f"{single_data['value_size']} & {single_data['prebuilt_data_num']} & {single_data['Get'][0]} & {single_data['Put'][0]} & {single_data['Del'][0]} & {single_data['Scan'][0]} \\\\")
            print("\\hline")
            print("\\end{tabular}")
            print("\\caption{gpds}")
            print("\\end{table}")

        with open(file_md, 'w') as f:
            sys.stdout = f
            gen_md()
        with open(file_tex, 'w') as f:
            sys.stdout = f
            gen_tex()
        sys.stdout = origin_stdout


