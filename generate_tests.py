import networkx as nx
import random

G = nx.Graph()

# Test with 5 nodes and max weight 10
G.add_weighted_edges_from((u, v, random.randint(1, 10)) for u,v in nx.complete_graph(5).edges())
nx.write_weighted_edgelist(G, './tests/5_nodes.test')

# Test with 50 nodes and max weight 20
G.add_weighted_edges_from((u, v, random.randint(1, 20)) for u,v in nx.complete_graph(50).edges())
nx.write_weighted_edgelist(G, './tests/50_nodes.test')

# Test with 150 nodes and max weight 25
G.add_weighted_edges_from((u, v, random.randint(1, 25)) for u,v in nx.complete_graph(150).edges())
nx.write_weighted_edgelist(G, './tests/150_nodes.test')

# Test with 350 nodes and max weight 25
G.add_weighted_edges_from((u, v, random.randint(1, 25)) for u,v in nx.complete_graph(350).edges())
nx.write_weighted_edgelist(G, './tests/350_nodes.test')

# Test with 500 nodes and max weight 25
G.add_weighted_edges_from((u, v, random.randint(1, 25)) for u,v in nx.complete_graph(500).edges())
nx.write_weighted_edgelist(G, './tests/500_nodes.test')

# Test with 850 nodes and max weight 25
G.add_weighted_edges_from((u, v, random.randint(1, 25)) for u,v in nx.complete_graph(850).edges())
nx.write_weighted_edgelist(G, './tests/850_nodes.test')

# Test with 1000 nodes and max weight 25
G.add_weighted_edges_from((u, v, random.randint(1, 25)) for u,v in nx.complete_graph(1000).edges())
nx.write_weighted_edgelist(G, './tests/1000_nodes.test')
