import tsplib95
import networkx as nx
import os

for subdir, dirs, files in os.walk(r'./../problems'):
	for filename in files:
		filepath = subdir + os.sep + filename

		problem = tsplib95.load(filepath)
		from_lib = problem.get_graph()

		G = nx.Graph()
		G.add_weighted_edges_from((u-1,v-1,w) for u,v,w in from_lib.edges.data('weight', default=1))

		writepath = './tests_tsplib95' + os.sep + filename

		nx.write_weighted_edgelist(G, writepath)


