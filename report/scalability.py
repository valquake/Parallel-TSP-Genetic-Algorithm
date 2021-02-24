import matplotlib.pyplot as plt
import numpy as np

'''
[v.montagna@C6320p-2 ~]$ ./execute.sh 
Running sequential code
1
2
3
4
5
6
7
8
results SEQ for:  ../tests_tsplib95/u2152.tsp 2048
150.23

End Seq.
Running parallel ST code
1
2
3
4
5
6
7
8
results PAR for:  1 ../tests_tsplib95/u2152.tsp 2048
148.84

8
1
2
3
4
5
6
7
8
results PAR for:  2 ../tests_tsplib95/u2152.tsp 2048
75.51

8
1
2
3
4
5
6
7
8
results PAR for:  4 ../tests_tsplib95/u2152.tsp 2048
43.43

8
1
2
3
4
5
6
7
8
results PAR for:  8 ../tests_tsplib95/u2152.tsp 2048
24.16

8
1
2
3
4
5
6
7
8
results PAR for:  16 ../tests_tsplib95/u2152.tsp 2048
15.34

8
1
2
3
4
5
6
7
8
results PAR for:  32 ../tests_tsplib95/u2152.tsp 2048
10.62

8
1
2
3
4
5
6
7
8
results PAR for:  64 ../tests_tsplib95/u2152.tsp 2048
8.66

8
1
2
3
4
5
6
7
8
results PAR for:  128 ../tests_tsplib95/u2152.tsp 2048
8.53

End parallel ST.

speedup=[1.0093 1.9895 3.4591 6.2181 9.7933 14.1459 17.3475 17.6119]
scalability=[1.0000 1.9711 3.4271 6.1605 9.7027 14.0150 17.1870 17.4490]

Running parallel FastFlow code
1
2
3
4
5
6
7
8
results PAR for:  1 ../tests_tsplib95/u2152.tsp 2048
140.16

1
2
3
4
5
6
7
8
results PAR for:  2 ../tests_tsplib95/u2152.tsp 2048
74.24

1
2
3
4
5
6
7
8
results PAR for:  4 ../tests_tsplib95/u2152.tsp 2048
39.01

1
2
3
4
5
6
7
8
results PAR for:  8 ../tests_tsplib95/u2152.tsp 2048
21.49

1
2
3
4
5
6
7
8
results PAR for:  16 ../tests_tsplib95/u2152.tsp 2048
12.70

1
2
3
4
5
6
7
8
results PAR for:  32 ../tests_tsplib95/u2152.tsp 2048
8.23

1
2
3
4
5
6
7
8
results PAR for:  64 ../tests_tsplib95/u2152.tsp 2048
9.68

1
2
3
4
5
6
7
8
results PAR for:  128 ../tests_tsplib95/u2152.tsp 2048
11.52

End parallel FF.
'''

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
ax.plot(a,[1.0, 1.97, 3.43, 6.16, 9.7, 14.02, 17.19, 17.45],label='threadpool',linewidth=1.2, c='0.25', linestyle='--',marker= 'v')
ax.plot(a,[1.0, 1.89, 3.59, 6.52, 11.04, 17.03, 14.48, 12.17],label='fastflow',linewidth=1.2, c='0.15', linestyle='--',marker= 's')
#ax.xaxis.set_ticks(a)
#ax.xaxis.set_ticklabels(x)
plt.yscale('log', basey=2)
plt.xticks(a,x)
plt.xlabel('n (# Workers)')
plt.ylabel('Scalability(n)')
plt.legend()

fig.suptitle("Scalability")

# Don't allow the axis to be on top of your data
ax.set_axisbelow(True)

# Customize the grid
ax.grid(linestyle='--', linewidth='0.5', c='0.82')
plt.show()