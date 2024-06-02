import matplotlib.pyplot as plt
import numpy as np
import re
import os
from copy import deepcopy

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
    test_mode = "nochange-bf"
    change_bf_data = [] # list of data

    single_config = {"use_bf": True, "use_cache": True,
"bf_size": 65536, "bf_func_num": 3}
    config = []
    # 解析日志文件
    first_flag = True
    default_single_data ={"value_size": 0, "prebuilt_data_num": 0,"Get": [], "Put":[], "Del": [], "Scan": []} 
    single_data = deepcopy(default_single_data)
    last_operation = "Get"
    # FIXME bug in change_bf
    for line in lines:
        if line.startswith("Test with different BloomFilter size"):
            if len(single_data['Get']) > 0:
                data.append(deepcopy(single_data))
                single_data = deepcopy(default_single_data)
            if len(data) > 0:
                all_data.append(deepcopy(data))
                data = []
        
            test_mode = "change-bf"
            continue
        if line.startswith("BloomFilter size"):
            parttern = r"\d+ Bytes"
            bf_size = re.findall(parttern, line)[0]
            single_config = {"use_bf": True, "use_cache": True,
"bf_size": 65536, "bf_func_num": 3}
            single_config['bf_size'] = int(bf_size.split()[0])
            if len(single_data['Get']) > 0:
                data.append(deepcopy(single_data))
                single_data = deepcopy(default_single_data)
            if len(data) > 0:
                change_bf_data.append(deepcopy(data))
            data = []
            continue

        if line.startswith("use_bf"):
            if not first_flag:
                if test_mode == "nochange-bf":
                    if single_data['Get'] != []:
                        data.append(deepcopy(single_data))
                    single_data = deepcopy(default_single_data)
                    all_data.append(deepcopy(data))
                else:
                    assert(0)
                    # change_bf_data.append(deepcopy(data))
                data = []
            test_mode = "nochange-bf"
            substr = line.strip().split(',')
            single_config['use_bf'] = substr[0].split(":")[1] == 'true'
            single_config['use_cache'] = substr[1].split(":")[1] == 'true'
            continue
        if line.startswith('bf_size'):
            substr = line.strip().split(',')
            single_config['bf_size'] = int(substr[0].split(":")[1])
            single_config['bf_func_num'] = int(substr[1].split(":")[1])
            first_flag = False
            if not first_flag:
                config.append(deepcopy(single_config))
            continue
        else:
            operation = None
            if line.startswith('value_size'):
                if len(single_data['Get']) > 0:
                    data.append(deepcopy(single_data))
                single_data = deepcopy(default_single_data)
                single_data['value_size'] = int(line.split(',')[0].strip().split(":")[1])
                single_data['prebuilt_data_num'] = int(line.split(',')[1].strip().split(":")[1])
                continue
            if line.startswith('Get') or line.startswith('Put') or line.startswith('Del') or line.startswith('Scan'):
                operation, time = line.split(':')
                # time = int(time.strip().split()[0])
                last_operation = operation #暂时用不到time
                continue
            if line.strip().startswith('throughput'):
                _, throughput = line.strip().split(':')
                throughput = float(throughput.strip().split("/")[0])
                assert len(single_data[last_operation]) < 2
                single_data[last_operation].append(throughput)

    data.append(deepcopy(single_data))
    change_bf_data.append(deepcopy(data))
    # 绘制数据
    # 在相同配置下的数据
    for single_config, data in zip(config, all_data):
        # print(single_config)
        print(len(config))
        # print(data)
        for i in range(3):
            it = 4 * i
            get_throughput = [d['Get'][0] for d in data[it: it+4]]
            put_throughput = [d['Put'][0] for d in data[it: it+4]]
            del_throughput = [d['Del'][0] for d in data[it: it+4]]
            scan_throughput = [d['Scan'][0] for d in data[it: it+4]]
            plt.plot(get_throughput, marker='o', label='Get')
            plt.plot(put_throughput, marker='o', label='Put')
            plt.plot(del_throughput, marker='o', label='Del')
            plt.plot(scan_throughput, marker='o', label='Scan')
            plt.ylabel('Throughput (ops/s)')
            plt.xlabel("value size (bytes)")
            plt.xticks(range(4), ["10", "100", "1000", "10000"])
            title = f"Thourghput with prebuilt_data_num={data[it]['prebuilt_data_num']}"
            plt.title(title)
            plt.legend()
            plt.savefig(os.path.join(savepath, title))
            plt.close()

    # 1024 * 1~15
    
    print(len(change_bf_data))
    print(len(change_bf_data[0]))
    print(change_bf_data[0][0]['Get'][0])
    print(change_bf_data[0][1]['Get'][0])
    print(change_bf_data[1][0]['Get'][0])
    for i in range(len(change_bf_data[0])): # 12
        print(len(change_bf_data[i]))
        x_data_num = len(change_bf_data) # 15
        get_throughput = [change_bf_data[j][i]['Get'][0] for j in range(x_data_num)]
        put_throughput = [change_bf_data[j][i]['Put'][0] for j in range(x_data_num)]
        del_throughput = [change_bf_data[j][i]['Del'][0] for j in range(x_data_num)]
        scan_throughput = [change_bf_data[j][i]['Scan'][0] for j in range(x_data_num)]
        plt.xlabel("BloomFilter Size (KB)")
        plt.xticks(range(len(get_throughput)), [str(i+1) for i in range(len(get_throughput))])
        plt.ylabel('Throughput (ops/s)')
        plt.plot(get_throughput, marker='o', label='Get')
        value_size = pow(10, i%4+1)
        pn = (i//4 * 2 + 1) * 1000
        title = f'GET Thourghput with value_size={value_size} bytes, prebuilt num={pn} '
        plt.title(title)
        plt.legend()
        plt.savefig(os.path.join(savepath, title))
        plt.close()

        plt.xlabel("BloomFilter Size (KB)")
        plt.xticks(range(len(get_throughput)), [str(i+1) for i in range(len(get_throughput))])
        plt.ylabel('Throughput (ops/s)')
        plt.plot(put_throughput, marker='o', label='Put')
        title = f'PUT Thourghput with value_size={value_size} bytes, prebuilt num={pn} '
        plt.title(title)
        plt.legend()
        plt.savefig(os.path.join(savepath, title))
        plt.close()

        plt.xlabel("BloomFilter Size (KB)")
        plt.xticks(range(len(get_throughput)), [str(i+1) for i in range(len(get_throughput))])
        plt.ylabel('Throughput (ops/s)')
        plt.plot(del_throughput, marker='o', label='Del')
        title = f'DEL Thourghput with value_size={value_size} bytes, prebuilt num={pn} '
        plt.title(title)
        plt.legend()
        plt.savefig(os.path.join(savepath, title))
        plt.close()

        plt.xlabel("BloomFilter Size (KB)")
        plt.xticks(range(len(get_throughput)), [str(i+1) for i in range(len(get_throughput))])
        plt.ylabel('Throughput (ops/s)')
        plt.plot(scan_throughput, marker='o', label='Scan')
        title = f'SCAN Thourghput with value_size={value_size} bytes, prebuilt num={pn} '
        plt.title(title)
        plt.legend()
        plt.savefig(os.path.join(savepath, title))
        plt.close()

