#!/usr/bin/env python3
import argparse
import sys
import re
import pulp
import numpy as np


def parse_input(file_path):
    """
    Parses the input file and returns a list of machines.
    Each machine is a dict with: 'A' (matrix), 'y_lights', 'y_jolts'.
    """
    machines = []

    # Regex patterns
    # Matches [.##.] -> Groups the dots/hashes
    re_lights = re.compile(r"\[([.#]+)\]")
    # Matches (0,3,4) -> Groups the numbers inside parens
    re_buttons = re.compile(r"\(([\d,]+)\)")
    # Matches {3,5,4,7} -> Groups the numbers inside braces
    re_jolts = re.compile(r"\{([\d,]+)\}")

    try:
        with open(file_path, "r") as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
        sys.exit(1)

    for line in lines:
        line = line.strip()
        if not line:
            continue

        # 1. Parse Lights Target (Part 1 Target)
        lights_match = re_lights.search(line)
        if not lights_match:
            continue
        lights_str = lights_match.group(1)
        # Convert ".##." -> [0, 1, 1, 0]
        y_lights = np.array([1 if c == "#" else 0 for c in lights_str])

        n_rows = len(y_lights)

        # 2. Parse Buttons (Matrix A columns)
        # Find all occurrences of (...)
        button_matches = re_buttons.findall(line)
        cols = []
        for btn_str in button_matches:
            # Create a column of zeros
            col = np.zeros(n_rows, dtype=int)
            # Parse indices "0,3,4" -> [0, 3, 4]
            if btn_str:  # handle empty parens if any
                indices = [int(x) for x in btn_str.split(",")]
                for idx in indices:
                    if idx < n_rows:
                        col[idx] = 1
            cols.append(col)

        # Stack columns to form Matrix A (Rows = Lights/Counters, Cols = Buttons)
        # Check if we actually found buttons
        if cols:
            A = np.column_stack(cols)
        else:
            # Fallback for malformed lines
            continue

        # 3. Parse Joltage Target (Part 2 Target)
        jolts_match = re_jolts.search(line)
        if jolts_match:
            y_jolts = np.array([int(x) for x in jolts_match.group(1).split(",")])
        else:
            y_jolts = np.zeros(n_rows, dtype=int)

        machines.append({"A": A, "y_lights": y_lights, "y_jolts": y_jolts})

    return machines


def solve_ilp(A, y, mode="part1"):
    """
    Solves Ax = y for minimal sum(x).

    mode='part1': Solves Ax = y + 2k (Modulo 2 logic via slack vars)
                  Variables x are Binary (0 or 1).
    mode='part2': Solves Ax = y (Standard Integer logic)
                  Variables x are Integer >= 0.
    """
    n_rows, n_vars = A.shape

    # 1. Initialize Model
    # Suppress output logs by default in newer PuLP versions if needed,
    # but here we rely on the solver command options.
    prob = pulp.LpProblem(f"Minimize_Buttons_{mode}", pulp.LpMinimize)

    # 2. Define Variables
    if mode == "part1":
        # Part 1: Binary (0 or 1) because pressing twice toggles back to original state.
        x = [pulp.LpVariable(f"x_{i}", cat=pulp.LpBinary) for i in range(n_vars)]

        # Slack variables 'k' for the modulo math: Ax = y + 2k
        # Bounds: k can range from 0 to roughly n_vars (max presses)
        k = [
            pulp.LpVariable(f"k_{i}", lowBound=0, upBound=n_vars, cat=pulp.LpInteger)
            for i in range(n_rows)
        ]
    else:
        # Part 2: x can be any non-negative integer.
        x = [
            pulp.LpVariable(f"x_{i}", lowBound=0, cat=pulp.LpInteger)
            for i in range(n_vars)
        ]

    # 3. Objective: Minimize total button presses
    prob += pulp.lpSum(x)

    # 4. Constraints
    for i in range(n_rows):
        # Calculate dot product for this row: A[i] . x
        row_sum = pulp.lpSum([A[i][j] * x[j] for j in range(n_vars)])

        if mode == "part1":
            # Constraint: row_sum == target + 2 * k
            prob += row_sum == y[i] + 2 * k[i]
        else:
            # Constraint: row_sum == target
            prob += row_sum == y[i]

    # 5. Solve
    # pulp.PULP_CBC_CMD(msg=False) silences the solver output (logs)
    status = prob.solve(pulp.PULP_CBC_CMD(msg=False))

    if pulp.LpStatus[status] == "Optimal":
        return int(pulp.value(prob.objective))
    else:
        return 0


def main():
    parser = argparse.ArgumentParser(description="Advent of Code Day 10 Solver")
    parser.add_argument("file", help="Path to the input file")

    args = parser.parse_args()

    machines = parse_input(args.file)

    total_presses_p1 = 0
    total_presses_p2 = 0

    print(f"Processing {len(machines)} machines from '{args.file}'...")

    for i, m in enumerate(machines):
        # Solve Part 1
        res1 = solve_ilp(m["A"], m["y_lights"], mode="part1")
        total_presses_p1 += res1

        # Solve Part 2
        res2 = solve_ilp(m["A"], m["y_jolts"], mode="part2")
        total_presses_p2 += res2

    print("-" * 30)
    print(f"Part 1 Total Presses: {total_presses_p1}")
    print(f"Part 2 Total Presses: {total_presses_p2}")
    print("-" * 30)


if __name__ == "__main__":
    main()
