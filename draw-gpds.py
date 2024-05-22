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

    default_single_config = {"use_bf": True, "use_cache": True,
"bf_size": 65536, "bf_func_num": 3}
    single_config = deepcopy(default_single_config)
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
            parttern = "\d+ Bytes"
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
            true_re = r'true'
            false_re = r'false'
            single_config['use_bf'] = len(re.findall(true_re, substr[0])) > 0
            single_config['use_cache'] = len(re.findall(true_re, substr[1])) > 0
            print(single_config)
            continue
        if line.startswith('bf_size'):
            substr = line.strip().split(',')
            single_config['bf_size'] = int(substr[0].split(":")[1])
            single_config['bf_func_num'] = int(substr[1].split(":")[1])
            first_flag = False
            config.append(deepcopy(single_config))
            single_config = deepcopy(default_single_config)
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
    get_cache_bf = []
    get_cache_nobf = []
    get_nocache_nobf = []
    # 生成三种config下get操作的对比图
    def gen_title(config: dict):
        if config["use_bf"] and config["use_cache"]:
            return "use_bf, use_cache"
        elif config["use_cache"]:
            return "no_bf, use_cache"
        else:
            return "no_bf, no_cache"
    for single_config, data in zip(config, all_data):
        if single_config["use_cache"] and single_config["use_bf"]:
            get_cache_bf.append(deepcopy(data))
        elif single_config["use_cache"]:
            get_cache_nobf.append(deepcopy(data))
        else:
            get_nocache_nobf.append(deepcopy(data))
    
    for get1, get2, get3 in zip(get_cache_bf, get_cache_nobf, get_nocache_nobf):
        # print(single_config)
        # print(len(config))
        # print(data)
        # len(get1) == 12
        for i in range(3):
            it = 4 * i
            get1_throughput = [d['Get'][0] for d in get1[it: it+4]]
            get2_throughput = [d['Get'][0] for d in get2[it: it+4]]
            get3_throughput = [d['Get'][0] for d in get3[it: it+4]]
            plt.plot(get1_throughput, marker='o', label="use_bf, use_cache")
            plt.plot(get2_throughput, marker='o', label="no_bf, use_cache")
            plt.plot(get3_throughput, marker='o', label="no_bf, no_cache")
            title = f"GET Throughput with prebuilt_data_size={i*2000 + 1000}"
            plt.title(title)
            plt.ylabel('Throughput (ops/s)')
            plt.xlabel("value size (bytes)")
            plt.xticks(range(4), ["10", "100", "1000", "10000"])
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

