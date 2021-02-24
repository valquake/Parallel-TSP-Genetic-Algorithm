import networkx as nx
import tsp

G = nx.read_weighted_edgelist('./tests/500_nodes.test', nodetype=int, create_using= nx.Graph())
A = nx.adjacency_matrix(G)
mat = A.todense().tolist()
print(mat)
r = range(len(mat))

# Dictionary of distance
dist = {(i, j): mat[i][j] for i in r for j in r}

print(tsp.tsp(r, dist))

# ./tests/50_nodes.test: (64.0, [0, 4, 47, 20, 45, 34, 46, 12, 26, 28, 27, 16, 10, 31, 7, 43, 29, 19, 39, 23, 17, 21, 15, 36, 42, 32, 44, 2, 11, 9, 30, 5, 14, 49, 41, 8, 22, 25, 37, 13, 33, 24, 40, 1, 6, 18, 35, 48, 3, 38])
# 