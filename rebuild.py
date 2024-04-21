import subprocess as sh
import threading
import os
import sys
import time
MAX_LOG_SIZE = 1024 * 1024  # 1MB, adjust as needed

def check_file_size(proc, filename):
    while proc.poll() is None:  # while the process is running
        if os.stat(filename).st_size > MAX_LOG_SIZE:
            proc.terminate()
            print("Log file is too large, exiting.")
            sys.exit(1)
        time.sleep(1)  # check every second
# def check_file_size(proc, filename):
#     while proc.poll() is None:  # while the process is running
#         if os.stat(filename).st_size > MAX_LOG_SIZE:
#             os.killpg(os.getpgid(proc.pid), signal.SIGKILL)  # Kill the entire process group
#             print("Log file is too large, exiting.")
#             sys.exit(1)
#         time.sleep(1)  # check every second

if __name__ == "__main__":
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    if(not os.path.exists("./log")):
        os.makedirs("./log")
    if(os.path.exists("./build")):
        sh.run("sudo rm -rf build", shell=True)
    sh.run("cmake -S . -B build", shell=True)
    os.chdir("./build")
    sh.run("make -j8", shell=True)
    sh.run("ctest", shell=True)

    if len(sys.argv) >= 2 and sys.argv[1] == "run":
        print(sys.argv[1])
        with open("../log/correctness.log", "w") as cf:
            proc = sh.Popen(["./correctness", "-v"], stdout=cf, stderr=cf, preexec_fn=os.setpgrp)
            threading.Thread(target=check_file_size, args=(proc, cf.name)).start()
            proc.wait()
        # with open("../log/persistence.log", "w") as pf:
        #     proc = sh.Popen(["./persistence", "-v"])
        #     threading.Thread(target=check_file_size, args=(proc, pf.name)).start()
        #     proc.wait()
    os.chdir("..")