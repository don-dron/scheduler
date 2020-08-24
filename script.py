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

names = ['LOCAL_QUEUE_WITH_STEAL','LOCAL_QUEUE']

kinds = {'LOCAL_QUEUE_WITH_STEAL': '-DLOCAL_QUEUES_WITH_STEAL=ON','LOCAL_QUEUE': '-DLOCAL_QUEUES_WITH_STEAL=ON'}

plot_types = {'HIS_FB': '-DFIBER_STAT=ON -DTHREAD_STAT=OFF',
              'TIME': '-DFIBER_STAT=OFF -DTHREAD_STAT=OFF',
              'HIS_TH': '-DTHREAD_STAT=ON -DFIBER_STAT=OFF'}

interrupt_flags = {'INT': '-DINTERRUPT_ENABLED=ON',
                   'NO_INT': '-DINTERRUPT_ENABLED=OFF'}


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

# for kind in kinds:
#     for plot_type in plot_types:
#         for interrupt_flag in interrupt_flags:
#             run('cmake -U -D ' + kinds[kind] + ' ' +
#                 interrupt_flags[interrupt_flag] + ' ' + plot_types[plot_type])
#             run('make clean')
#             run('make')
#             run('mkdir -p ' + path)
#             run('./run.sh')
#             run('mkdir -p ' + images)

#             print(kind + '_' + interrupt_flag+'_'+plot_type + '  ' + 'DONE')

#             if(plot_type == 'HIS_FB'):
#                 for file in os.listdir(path):
#                     name = file + '_'+kind + '_' + interrupt_flag+'_'+plot_type
#                     data = np.genfromtxt(
#                         path+'/'+file, dtype=int, names=['Number', 'State', 'Start'])

#                     fig, ax1 = plt.subplots(figsize=(16, 9), dpi=80)

#                     ymin = min(data['Start'])
#                     ymax = max(data['Start'])

#                     ymax += ymax/20

#                     ax1.set_facecolor('k')
#                     for i in range(len(data['Number'])):
#                         state = data['State'][i]
#                         c = get_color(state)
#                         if(state == 0):
#                             rect = matplotlib.patches.Rectangle((data['Number'][i], ymin),
#                                                                 1, data['Start'][i], 0,
#                                                                 color=c)
#                             ax1.add_patch(rect)

#                             rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
#                                                                 1, data['Start'][i+1] -
#                                                                 data['Start'][i], 0,
#                                                                 color=c)
#                             ax1.add_patch(rect)
#                         elif(state == 4):
#                             ax1.vlines(data['Number'][i],
#                                        data['Start'][i], ymax, color=c)
#                             rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
#                                                                 1, ymax -
#                                                                 data['Start'][i], 0,
#                                                                 color=c)
#                             ax1.add_patch(rect)
#                         else:
#                             rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
#                                                                 1, data['Start'][i+1] -
#                                                                 data['Start'][i], 0,
#                                                                 color=c)
#                             ax1.add_patch(rect)

#                     ax1.set_xlabel('Number')
#                     ax1.set_ylabel('Start')

#                     l1 = matplotlib.patches.Patch(
#                         color=get_color(0), label='Before start')
#                     l2 = matplotlib.patches.Patch(
#                         color=get_color(1), label='Work')
#                     l3 = matplotlib.patches.Patch(
#                         color=get_color(2), label='Yield')
#                     l4 = matplotlib.patches.Patch(
#                         color=get_color(3), label='Sleep')
#                     l5 = matplotlib.patches.Patch(
#                         color=get_color(4), label='Terminate')

#                     plt.legend(handles=[l1, l2, l3, l4, l5])

#                     plt.savefig(images + '/' + name + '.png')
#             elif(plot_type == 'HIS_TH'):
#                 for file in os.listdir(path):
#                     name = file + '_'+kind + '_' + interrupt_flag+'_'+plot_type
#                     data = np.genfromtxt(
#                         path+'/'+file, dtype=int, names=['Number', 'State', 'Start'])

#                     fig, ax1 = plt.subplots(figsize=(16, 9), dpi=80)

#                     ymin = min(data['Start'])
#                     ymax = max(data['Start'])

#                     ymax += ymax/20

#                     ax1.set_facecolor('k')

#                     for i in range(len(data['Number'])):
#                         state = data['State'][i]
#                         c = get_color(state)
#                         if((i == 0 or data['Number'][i-1] < data['Number'][i]) and i < len(data['Number'])-1):
#                             # print(1)
#                             rect = matplotlib.patches.Rectangle((data['Number'][i], ymin),
#                                                                 1, data['Start'][i], 0,
#                                                                 color=c)
#                             ax1.add_patch(rect)

#                             rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
#                                                                 1, data['Start'][i+1] -
#                                                                 data['Start'][i], 0,
#                                                                 color=c)
#                             ax1.add_patch(rect)
#                         elif(i >= len(data['Number'])-1 or data['Number'][i] < data['Number'][i+1]):
#                             # print(2)
#                             ax1.vlines(data['Number'][i],
#                                        data['Start'][i], ymax, color=c)
#                             rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
#                                                                 1, ymax -
#                                                                 data['Start'][i], 0,
#                                                                 color=c)
#                             ax1.add_patch(rect)
#                         else:
#                             # print(3)
#                             rect = matplotlib.patches.Rectangle((data['Number'][i], data['Start'][i]),
#                                                                 1, data['Start'][i+1] -
#                                                                 data['Start'][i], 0,
#                                                                 color=c)
#                             ax1.add_patch(rect)

#                     ax1.set_xlabel('Number')
#                     ax1.set_ylabel('Start')

#                     l1 = matplotlib.patches.Patch(
#                         color=get_color(0), label='Schedule')
#                     l2 = matplotlib.patches.Patch(
#                         color=get_color(1), label='Work')
#                     l3 = matplotlib.patches.Patch(
#                         color=get_color(2), label='Sleep')

#                     plt.legend(handles=[l1, l2, l3])

#                     plt.savefig(images + '/' + name + '.png')

d = {}

for interrupt_flag in interrupt_flags:
    d[interrupt_flag] = {}
    for kind in kinds:
        run('cmake -U -D ' + kinds[kind] + ' ' +
            interrupt_flags[interrupt_flag] + ' ' + plot_types['TIME'])
        run('make clean')
        run('make')
        run('mkdir -p ' + path)
        run('./run.sh')
        run('mkdir -p ' + images)

        lst = []
        for file in os.listdir(path):
            data = np.genfromtxt(
                path+'/'+file, names=['thread', 'time', 'interrupt', 'interrupt_fail', 'switch'])
            lst.append(data)
        print(len(lst))
        d[interrupt_flag][kind] = lst
    
    print(len(d[interrupt_flag]))

for interrupt_flag in interrupt_flags:
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
            data = d[interrupt_flag][kind][i]
            name = kind + '_'+interrupt_flag+'_'+'TIME'
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
            bbox_to_anchor=(-0.1, -0.1))
        plt.savefig(images + '/' + os.listdir(path)[i]+"_" + interrupt_flag+"_TIME"+ '.png')