#!/usr/bin/python3

import argparse
import networkx as nx
import matplotlib.pyplot as plt


class Graph(nx.DiGraph):

    def __init__(self, filepath):
        nx.DiGraph.__init__(self)
        self.i_max = 0
        with open(filepath, "r") as file:
            for line in file.readlines():
                stripped_line = line.strip()
                if not len(stripped_line):
                    continue
                splitted = stripped_line.split("|")
                if len(splitted) != 3:
                    raise RuntimeError("Graph::constructor: bad line split")
                head = splitted[0].split(",")
                if len(head) != 2:
                    raise RuntimeError("Graph::constructor: bad head split")
                head = (int(head[0]), int(head[1]))
                tail = splitted[1].split(",")
                if len(tail) != 2:
                    raise RuntimeError("Graph::constructor: bad tail split")
                tail = (int(tail[0]), int(tail[1]))
                weight = int(splitted[2])
                if head not in self:
                    self.i_max = max(self.i_max, head[0])
                    self.add_node(head)
                if tail not in self:
                    self.i_max = max(self.i_max, tail[0])
                    self.add_node(tail)
                self.add_edge(head, tail, weight=weight)
            file.close()
            return
        raise FileNotFoundError(
            'Graph::constructor: cannot open file "{}"'.format(filepath)
        )

    def display(self):
        pos = dict()
        for node in self.nodes:
            pos[node] = (node[1], self.i_max - node[0])
        edge_labels = dict()
        for head, tail, data in self.edges(data=True):
            edge_labels[(head, tail)] = data["weight"]
        print(edge_labels)
        nx.draw(self, pos=pos)
        nx.draw_networkx_edge_labels(self, pos=pos, edge_labels=edge_labels)
        # node_size=10, arrowsize=1)
        plt.show()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("filepath")
    args = parser.parse_args()
    graph = Graph(args.filepath)
    print("Is DAG:", nx.is_directed_acyclic_graph(graph))
    graph.display()


if __name__ == "__main__":
    main()
