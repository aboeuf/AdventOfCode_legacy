#!/usr/bin/env python3

import argparse
from functools import lru_cache
import networkx as nx
import sys


def parse_input(filepath):
    try:
        with open(filepath, "r") as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Error: File '{filepath}' not found.")
        sys.exit(1)

    G = nx.DiGraph()

    for line in lines:
        line = line.strip()
        if not line:
            continue
        labels = line.split(" ")
        labels[0] = labels[0][:-1]
        for label in labels:
            G.add_node(label)
        for i in range(1, len(labels)):
            G.add_edge(labels[0], labels[i])

    return G


def get_nb_paths(G, source, target):

    @lru_cache(maxsize=None)
    def count_paths(u):
        if u == target:
            return 1
        return sum(count_paths(v) for v in G.successors(u))

    return count_paths(source)


def main():
    parser = argparse.ArgumentParser(description="Advent of Code Day 10 Solver")
    parser.add_argument("filepath", help="Path to the input file")
    args = parser.parse_args()

    G = parse_input(args.filepath)

    print(G)
    print("Acyclic:", "true" if nx.is_directed_acyclic_graph(G) else "False")

    print("\nPart one:", get_nb_paths(G, "you", "out"))

    svr_fft = get_nb_paths(G, "svr", "fft")
    fft_dac = get_nb_paths(G, "fft", "dac")
    dac_out = get_nb_paths(G, "dac", "out")

    svr_dac = get_nb_paths(G, "svr", "dac")
    dac_fft = get_nb_paths(G, "dac", "fft")
    fft_out = get_nb_paths(G, "fft", "out")

    print("\nPart two:", svr_fft * fft_dac * dac_out + svr_dac * dac_fft * fft_out)


if __name__ == "__main__":
    main()
