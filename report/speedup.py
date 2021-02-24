import matplotlib.pyplot as plt
import numpy as np


plt.style.use('grayscale')

plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = 'Ubuntu'
plt.rcParams['font.monospace'] = 'Ubuntu Mono'
plt.rcParams['font.size'] = 10
plt.rcParams['axes.labelsize'] = 10
plt.rcParams['axes.labelweight'] = 'bold'
plt.rcParams['axes.titlesize'] = 10
plt.rcParams['xtick.labelsize'] = 8
plt.rcParams['ytick.labelsize'] = 8
plt.rcParams['legend.fontsize'] = 10
plt.rcParams['figure.titlesize'] = 12




fig, ax = plt.subplots()

a = np.arange(8)
x = 2**a

ax.plot(a,x,label='ideal',linewidth=1.2, c='0.35')
ax.plot(a,[1.0093, 1.9895, 3.4591, 6.2181, 9.7933, 14.1459, 17.3475, 17.6119],label='threadpool',linewidth=1.2, c='0.25', linestyle='--',marker= 'v')
ax.plot(a,[1.0655, 1.9258, 3.8638, 6.7469, 11.8027, 18.3702, 14.6787, 12.3037],label='fastflow',linewidth=1.2, c='0.15', linestyle='--',marker= 's')
#ax.xaxis.set_ticks(a)
#ax.xaxis.set_ticklabels(x)
plt.yscale('log', basey=2)
plt.xticks(a,x)
plt.xlabel('n (# Workers)')
plt.ylabel('Speedup(n)')
plt.legend()

fig.suptitle("Speedup")

# Don't allow the axis to be on top of your data
ax.set_axisbelow(True)

# Customize the grid
ax.grid(linestyle='--', linewidth='0.5', c='0.82')
plt.show()