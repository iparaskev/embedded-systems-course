import sys
import os
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

# Function to extract the important values
def get_important(lines):
    lines = [l.strip('\n').split(':') for l in lines]

    ratio = 0
    cpu_usage = 0
    for l in lines:
        if l[0] == 'Ratio':
            ratio = float(l[1].strip(' ').split(' ')[0])
        if l[0] == 'Mean logs/cpu':
            cpu_usage = float(l[1])
    return ratio, cpu_usage

# Files path
logs_path = sys.argv[1]

# Get filenames
files = os.listdir(logs_path)

ratios = []
cpu = []
for fil in files:
    with open(logs_path + fil, 'r') as f:
        lines = f.readlines()

    name = fil.split('.')[0].split('_')[-1]
    ratio, cpu_usage = get_important(lines)
    if name == '1':
        plt.plot([ratio], [cpu_usage], 'g^')
    elif name == '7':
        plt.plot([ratio], [cpu_usage], 'b^')
    elif name == '3':
        plt.plot([ratio], [cpu_usage], 'r^')
    #ratios.append(ratio)
    #cpu.append(cpu_usage)


plt.xlabel('ratio msg/sec')
plt.ylabel('% cpu usage')
red_patch = mpatches.Patch(color='red', label='3 msgs')
blue_patch = mpatches.Patch(color='blue', label='7 msgs')
green_patch = mpatches.Patch(color='green', label='1 msgs')
plt.legend(handles=[green_patch, red_patch, blue_patch])
#plt.plot(ratios, cpu, 'g^')
plt.show()
#plt.savefig('plots/64.png', bbox_inches='tight')

