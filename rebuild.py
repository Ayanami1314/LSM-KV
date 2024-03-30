import subprocess as sh
import os

if __name__ == "__main__":

    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    sh.run("cmake -S . -B build", shell=True)
    sh.run("sudo cmake --build build", shell=True)
    os.chdir("./build")
    sh.run("ctest", shell=True)
    os.chdir("..")