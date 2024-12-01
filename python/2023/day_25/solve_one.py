#!/usr/bin/python3

import argparse
import networkx as nx


def parse_input(filepath):
    with open(filepath, "r") as file:
        G = nx.Graph()
        for line in file.readlines():
            stripped_line = line.strip()
            if len(stripped_line):
                splitted = stripped_line.split(":")
                if len(splitted) != 2 or not len(splitted[0]) or not len(splitted[1]):
                    raise RuntimeError('parse_input: bad ":" split')
                parent = splitted[0]
                if len(parent):
                    if parent not in G:
                        G.add_node(parent)
                    for child in splitted[1].split(" "):
                        if len(child):
                            if child not in G:
                                G.add_node(child)
                            G.add_edge(parent, child)
        return G
    raise FileNotFoundError('parse_input: cannot open file "{}"'.format(filepath))


def solve(G):
    edges = nx.minimum_edge_cut(G)
    for edge in edges:
        G.remove_edge(edge[0], edge[1])
    product = 1
    for c in nx.connected_components(G):
        product *= len(c)
    return product


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("filepath")
    args = parser.parse_args()
    G = parse_input(args.filepath)
    print(solve(G))


if __name__ == "__main__":
    main()
