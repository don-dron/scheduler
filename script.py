import numpy as np
import os
import subprocess
import matplotlib.pyplot as plt
import matplotlib
from matplotlib import colors
path = 'log'
images = 'images'


def run(command):
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
    output, error = process.communicate()


params1 = {   'HIS_FB': '-DFIBER_STAT=ON -DTHREAD_STAT=OFF',
            'HIS_TH': '-DTHREAD_STAT=ON -DFIBER_STAT=OFF' ,
            'TIME': '-DFIBER_STAT=OFF -DTHREAD_STAT=OFF'}

params = {'INT': '-DINTERRUPT_ENABLED=ON',
          'NO_INT': '-DINTERRUPT_ENABLED=OFF'}


def get_color(state):
    if state == 0:
        return 'yellow'
    elif state == 1:
        return 'green'
    elif state == 2:
        return 'blue'
    elif state == 3:
        return 'm'
    elif state == 4:
        return 'red'
    else:
        return 'black'


for param1 in params1:
    for param in params:
        run('cmake -U -D ' + params[param] + ' ' + params1[param1])
        run('make clean')
        run('make')
        run('mkdir -p ' + path)
        run('./run.sh')
        run('mkdir -p ' + images)

        print(param + ' ' + param1 + ' ' + 'done')

        if(param1 == 'TIME'):
            for file in os.listdir(path):
                data = np.genfromtxt(
                    path+'/'+file, names=['thread', 'time', 'interrupt', 'interrupt_fail', 'switch'])

                fig, ax = plt.subplots(
                    nrows=2, ncols=2, figsize=(16, 9), dpi=80)
                ax1, ax2, ax3, ax4 = ax.flatten()

                ax1.set_xlabel('Threads')
                ax1.set_ylabel('Time')

                ax2.set_xlabel('Threads')
                ax2.set_ylabel('Interrupt')

                ax3.set_xlabel('Threads')
                ax3.set_ylabel('Interrupt fail')

                ax4.set_xlabel('Threads')
                ax4.set_ylabel('Switch')

                name = file + '_'+param+'_'+param1

                ax1.plot(data['thread'], data['time'], color='r', label=name)
                ax2.plot(data['thread'], data['interrupt'],
                         color='r', label=name)
                ax3.plot(data['thread'], data['interrupt_fail'],
                         color='r', label=name)
                ax4.plot(data['thread'], data['switch'], color='r', label=name)

                # leg = ax.legend()

                plt.savefig(images + '/' + name + '.png')
        elif(param1 == 'HIS_FB'):
            for file in os.listdir(path):
                data = np.genfromtxt(
                    path+'/'+file, dtype=int, names=['Number', 'State', 'Start'])

                fig, ax1 = plt.subplots(figsize=(16, 9), dpi=80)

                ymin = min(data['Start'])
                ymax = max(data['Start'])

                ymax += ymax/20

                ax1.set_facecolor('k')
                for i in range(len(data['Number'])):
                    state = data['State'][i]
                    c = get_color(state)
                    if(state == 0):
                        rect = matplotlib.patches.Rectangle((data['Number'][i], ymin),
                                                            1, data['Start'][i], 0,
                                                            color=c)
                        ax1.add_patch(rect)

                        rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
                                                            1, data['Start'][i+1] -
                                                            data['Start'][i], 0,
                                                            color=c)
                        ax1.add_patch(rect)
                    elif(state == 4):
                        ax1.vlines(data['Number'][i],
                                   data['Start'][i], ymax, color=c)
                        rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
                                                            1, ymax -
                                                            data['Start'][i], 0,
                                                            color=c)
                        ax1.add_patch(rect)
                    else:
                        rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
                                                            1, data['Start'][i+1] -
                                                            data['Start'][i], 0,
                                                            color=c)
                        ax1.add_patch(rect)

                ax1.set_xlabel('Number')
                ax1.set_ylabel('Start')

                name = file + '_'+param+'_'+param1

                l1 = matplotlib.patches.Patch(
                    color=get_color(0), label='Before start')
                l2 = matplotlib.patches.Patch(color=get_color(1), label='Work')
                l3 = matplotlib.patches.Patch(
                    color=get_color(2), label='Yield')
                l4 = matplotlib.patches.Patch(
                    color=get_color(3), label='Sleep')
                l5 = matplotlib.patches.Patch(
                    color=get_color(4), label='Terminate')

                plt.legend(handles=[l1, l2, l3, l4, l5])

                plt.savefig(images + '/' + name + '.png')
        elif(param1 == 'HIS_TH'):
            for file in os.listdir(path):
                data = np.genfromtxt(
                    path+'/'+file, dtype=int, names=['Number', 'State', 'Start'])

                fig, ax1 = plt.subplots(figsize=(16, 9), dpi=80)

                ymin = min(data['Start'])
                ymax = max(data['Start'])

                ymax += ymax/20

                ax1.set_facecolor('k')
                for i in range(len(data['Number']-1)):
                    state = data['State'][i]
                    c = get_color(state)
                    if(i==0 or data['Number'][i-1] < data['Number'][i]):
                        rect = matplotlib.patches.Rectangle((data['Number'][i], ymin),
                                                            1, data['Start'][i], 0,
                                                            color=c)
                        ax1.add_patch(rect)

                        rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
                                                            1, data['Start'][i+1] -
                                                            data['Start'][i], 0,
                                                            color=c)
                        ax1.add_patch(rect)
                    elif(i == len(data['Number'])-1 or data['Number'][i] > data['Number'][i+1]):
                        ax1.vlines(data['Number'][i],
                                   data['Start'][i], ymax, color=c)
                        rect=matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
                                                            1, ymax -
                                                            data['Start'][i], 0,
                                                            color=c)
                        ax1.add_patch(rect)
                    else:
                        rect=matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
                                                            1, data['Start'][i+1] -
                                                            data['Start'][i], 0,
                                                            color=c)
                        ax1.add_patch(rect)

                ax1.set_xlabel('Number')
                ax1.set_ylabel('Start')

                name=file + '_'+param+'_'+param1

                l1=matplotlib.patches.Patch(
                    color=get_color(0), label='Schedule')
                l2=matplotlib.patches.Patch(color=get_color(1), label='Work')
                l3=matplotlib.patches.Patch(
                    color=get_color(2), label='Sleep')

                plt.legend(handles=[l1, l2, l3])

                plt.savefig(images + '/' + name + '.png')
