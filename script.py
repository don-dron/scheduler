import numpy as np
import os
import subprocess
import matplotlib.pyplot as plt

path = "log"
images = 'images'

def run(command):
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
    output, error = process.communicate()

run("cmake -U CMakeLists.txt")
run("make clean")
run("make")
run("mkdir -p " + path)
run("./run.sh")
run("mkdir -p " + images)

for file in os.listdir(path): 
    data = np.genfromtxt(path+"/"+file, names=['thread', 'time'])
    fig = plt.figure()
    ax1 = fig.add_subplot(111)
    ax1.plot(data['thread'], data['time'], color='r', label=file)
    leg = ax1.legend()
    plt.savefig(images + '/' + file+'.png')