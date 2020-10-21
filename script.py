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


names = ['LOCAL_QUEUE_WITH_STEAL',
         'LOCAL_QUEUE',
         'RB_TREE',
         'SPLAY_TREE',
         'THIN_HEAP',
         'FIBONACCI_HEAP']

kinds = {'LOCAL_QUEUE_WITH_STEAL': '-DLOCAL_QUEUES_WITH_STEAL:BOOL=ON',
         'LOCAL_QUEUE': '-DLOCAL_QUEUES_WITH_STEAL:BOOL=ON',
         'RB_TREE': '-DRB_TREE:BOOL=ON',
         'SPLAY_TREE' : '-DSPLAY_TREE:BOOL=ON',
         'THIN_HEAP' : '-DTHIN_HEAP:BOOL=ON',
         'FIBONACCI_HEAP' : '-DFIBONACCI_HEAP:BOOL=ON'}

plot_types = {#'HIS_FB': '-DFIBER_STAT:BOOL=ON -DTHREAD_STAT:BOOL=OFF',
              #'HIS_TH': '-DTHREAD_STAT:BOOL=ON -DFIBER_STAT:BOOL=OFF',
              'TIME': '-DFIBER_STAT:BOOL=OFF -DTHREAD_STAT:BOOL=OFF'}

realtime_flags = {#'REALTIME': '-DREALTIME:BOOL=ON',
                  'NO_REALTIME': '-DREALTIME:BOOL=OFF'}

interrupt_flags = {#'INT': '-DINTERRUPT_ENABLED:BOOL=ON',
                   'NO_INT': '-DINTERRUPT_ENABLED:BOOL=OFF'}


def get_color(state):
    if state == 0:
        return 'orange'
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


count = 0

for kind in kinds:
    for plot_type in plot_types:
        for realtime_flag in realtime_flags:
            for interrupt_flag in interrupt_flags:
                if(plot_type == 'HIS_FB'):
                    print(kind + '_' + interrupt_flag+'_' +
                      realtime_flag + '_'+plot_type + '  ' + 'DONE')
                    run('rm -rf build')
                    run('rm -rf CMakeCache.txt')
                    run('cmake ' + kinds[kind] + ' ' +
                    interrupt_flags[interrupt_flag] + ' ' + realtime_flags[realtime_flag] + ' ' + plot_types[plot_type])
                    run('cmake --build .')
                    run('mkdir -p ' + path)
                    run('./run.sh')
                    run('mkdir -p ' + images)
                    run('mkdir -p ' + images + "/" + plot_type)

                    count += 1
                    print(count)

                    for file in os.listdir(path):
                        name = file + '_'+kind + '_' + interrupt_flag + '_' + realtime_flag
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

                        l1 = matplotlib.patches.Patch(
                            color=get_color(0), label='Before start')
                        l2 = matplotlib.patches.Patch(
                            color=get_color(1), label='Work')
                        l3 = matplotlib.patches.Patch(
                            color=get_color(2), label='Yield')
                        l4 = matplotlib.patches.Patch(
                            color=get_color(3), label='Sleep')
                        l5 = matplotlib.patches.Patch(
                            color=get_color(4), label='Terminate')

                        plt.legend(handles=[l1, l2, l3, l4, l5])

                        plt.savefig(images + '/'+plot_type +
                                    '/' + name + '.png')
                elif(plot_type == 'HIS_TH'):
                    print(kind + '_' + interrupt_flag+'_' +
                      realtime_flag + '_'+plot_type + '  ' + 'DONE')
                    run('rm -rf build')
                    run('rm -rf CMakeCache.txt')
                    run('cmake ' + kinds[kind] + ' ' +
                    interrupt_flags[interrupt_flag] + ' ' + realtime_flags[realtime_flag] + ' ' + plot_types[plot_type])
                    run('cmake --build .')
                    run('mkdir -p ' + path)
                    run('./run.sh')
                    run('mkdir -p ' + images)
                    run('mkdir -p ' + images + "/" + plot_type)

                    count += 1
                    print(count)

                    for file in os.listdir(path):
                        name = file + '_'+kind + '_' + interrupt_flag + '_' + realtime_flag
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
                            if((i == 0 or data['Number'][i-1] < data['Number'][i]) and i < len(data['Number'])-1):
                                # print(1)
                                rect = matplotlib.patches.Rectangle((data['Number'][i], ymin),
                                                                    1, data['Start'][i], 0,
                                                                    color=c)
                                ax1.add_patch(rect)

                                rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
                                                                    1, data['Start'][i+1] -
                                                                    data['Start'][i], 0,
                                                                    color=c)
                                ax1.add_patch(rect)
                            elif(i >= len(data['Number'])-1 or data['Number'][i] < data['Number'][i+1]):
                                # print(2)
                                ax1.vlines(data['Number'][i],
                                           data['Start'][i], ymax, color=c)
                                rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
                                                                    1, ymax -
                                                                    data['Start'][i], 0,
                                                                    color=c)
                                ax1.add_patch(rect)
                            else:
                                # print(3)
                                rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
                                                                    1, data['Start'][i+1] -
                                                                    data['Start'][i], 0,
                                                                    color=c)
                                ax1.add_patch(rect)

                        ax1.set_xlabel('Number')
                        ax1.set_ylabel('Start')

                        l1 = matplotlib.patches.Patch(
                            color=get_color(0), label='Schedule')
                        l2 = matplotlib.patches.Patch(
                            color=get_color(1), label='Work')
                        l3 = matplotlib.patches.Patch(
                            color=get_color(2), label='Sleep')

                        plt.legend(handles=[l1, l2, l3])

                        plt.savefig(images + '/'+plot_type +
                                    '/' + name + '.png')

d = {}

for interrupt_flag in interrupt_flags:
    d[interrupt_flag] = {}
    for realtime_flag in realtime_flags:
        d[interrupt_flag][realtime_flag] = {}
        for kind in kinds:

            print(kind + '_' + interrupt_flag+'_' +
                realtime_flag + '_'+'TIME' + '  ' + 'DONE')
            run('rm -rf build')
            run('rm -rf CMakeCache.txt')
            run('cmake ' + kinds[kind] + ' ' +
                interrupt_flags[interrupt_flag] + ' ' + realtime_flags[realtime_flag] + ' ' + plot_types['TIME'])
            run('make clean')
            run('make')
            run('mkdir -p ' + path)
            run('./run.sh')
            run('mkdir -p ' + images)
            lst = []
            count += 1
            print(count)
            for file in os.listdir(path):
                data = np.genfromtxt(
                    path+'/'+file, names=['thread', 'time', 'interrupt', 'interrupt_fail', 'switch'])
                lst.append(data)
            d[interrupt_flag][realtime_flag][kind] = lst

for interrupt_flag in interrupt_flags:
    for realtime_flag in realtime_flags:
        for i in range(len(os.listdir(path))):
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

            h = []

            for kind in kinds:
                data = d[interrupt_flag][realtime_flag][kind][i]
                name = kind + '_'+interrupt_flag + '_' + realtime_flag+'_'+'TIME'
                c = get_color(names.index(kind))
                ax1.plot(data['thread'], data['time'],
                         color=c, label=name)
                ax2.plot(data['thread'], data['interrupt'],
                         color=c, label=name)
                ax3.plot(data['thread'], data['interrupt_fail'],
                         color=c, label=name)
                ax4.plot(data['thread'], data['switch'],
                         color=c, label=name)

                h.append(matplotlib.patches.Patch(
                    color=c, label=kind))

            plt.legend(handles=h,
                       bbox_to_anchor=(1, 0.6))
            plt.savefig(images + '/TIME/' + os.listdir(path)
                        [i]+"_" + interrupt_flag + '_' + realtime_flag+"_TIME" + '.png')
