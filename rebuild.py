import subprocess as sh
import os
import sys
if __name__ == "__main__":
    
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    sh.run("cmake -S . -B build", shell=True)
    sh.run("sudo cmake --build build", shell=True)
    os.chdir("./build")
    sh.run("ctest", shell=True)
    print(sys.argv[1])
    if len(sys.argv) >= 2 and sys.argv[1] == "run":
        with open("../log/correctness.log", "w") as cf:
            sh.run("./correctness", shell=True, stdout=cf, stderr=cf)
        with open("../log/persistence.log", "w") as pf:
            sh.run("./persistence", shell=True, stdout=pf, stderr=pf)
        sh.run("make clean", shell=True)
    os.chdir("..")