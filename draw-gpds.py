import matplotlib.pyplot as plt
import numpy as np
import re

if __name__=="__main__":
    # 读取日志文件
    log_file = 'report.txt'
    with open(log_file, 'r') as f:
        lines = f.readlines()

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
    single_data = default_single_data.copy()
    last_operation = "Get"
    # FIXME: 135->513->513 error
    for line in lines:
        if line.startswith("Test with different BloomFilter size"):
            if len(single_data['Get']) > 0:
                data.append(single_data.copy())
                single_data = default_single_data.copy()
            if len(data) > 0:
                all_data.append(data.copy())
                data = []
        
            test_mode = "change-bf"
            continue
        if line.startswith("BloomFilter size"):
            parttern = "\d+ Bytes"
            bf_size = re.findall(parttern, line)[0]
            single_config['bf_size'] = int(bf_size.split()[0])
            if len(single_data['Get']) > 0:
                data.append(single_data.copy())
            if len(data) > 0:
                change_bf_data.append(data.copy())
            data = []
            continue

        if line.startswith("use_bf"):
            if not first_flag:
                if test_mode == "nochange-bf":
                    if single_data['Get'] != []:
                        data.append(single_data.copy())
                    single_data = default_single_data.copy()
                    all_data.append(data.copy())
                else:
                    change_bf_data.append(data.copy())
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
                config.append(single_config.copy())
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
                continue
            if line.strip().startswith('throughput'):
                _, throughput = line.strip().split(':')
                throughput = float(throughput.strip().split("/")[0])
                assert len(single_data[last_operation]) < 2
                single_data[last_operation].append(throughput)

    data.append(single_data.copy())
    change_bf_data.append(data.copy())
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
            plt.title(f'Thourghput with prebuilt_data_num={data[it]["prebuilt_data_num"]}')
            plt.legend()
            plt.show()

    # 1024 * 1~15
    print(len(change_bf_data))
    print(len(change_bf_data[0]))
    for i in range(12):
        get_throughput = []
        put_throughput = []
        del_throughput = []
        scan_throughput = []
        for j in range(15):
            get_throughput.append(change_bf_data[j][i]['Get'][0])
            put_throughput.append(change_bf_data[j][i]['Put'][0])
            del_throughput.append(change_bf_data[j][i]['Del'][0])
            scan_throughput.append(change_bf_data[j][i]['Scan'][0])
        plt.xlabel("BloomFilter Size (KB)")
        plt.xticks(range(len(get_throughput)), [str(i+1) for i in range(len(get_throughput))])
        plt.ylabel('Throughput (ops/s)')
        plt.plot(get_throughput, marker='o', label='Get')
        value_size = pow(10, i%4+1)
        pn = (i//4 * 2 + 1) * 1000
        plt.title(f'GET Thourghput with value_size={value_size} bytes, prebuilt num={pn} ')
        plt.legend()
        plt.show()

        plt.plot(put_throughput, marker='o', label='Put')
        plt.title(f'PUT Thourghput with value_size={value_size} bytes, prebuilt num={pn} ')
        plt.legend()
        plt.show()

        plt.plot(del_throughput, marker='o', label='Del')
        plt.title(f'DEL Thourghput with value_size={value_size} bytes, prebuilt num={pn} ')
        plt.legend()
        plt.show()


        plt.plot(scan_throughput, marker='o', label='Scan')
        plt.title(f'SCAN Thourghput with value_size={value_size} bytes, prebuilt num={pn} ')
        plt.legend()
        plt.show()

