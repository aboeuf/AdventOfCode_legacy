#!/usr/bin/env python3
import argparse
import sys
import re
import numpy as np
from scipy.optimize import linprog
import math


class CustomILPSolver:
    def __init__(self, A, y, obj_c, int_vars_indices, bounds=None):
        """
        A, y: Constraints Ax = y
        obj_c: Cost vector for the objective function (minimize c.x)
        int_vars_indices: List of indices that MUST be integers
        bounds: List of (min, max) tuples for each variable
        """
        self.A = A
        self.y = y
        self.c = obj_c
        self.int_indices = int_vars_indices
        self.n_vars = len(obj_c)
        self.initial_bounds = bounds if bounds else [(0, None)] * self.n_vars

        self.best_sol = None
        self.best_obj = float("inf")

    def solve(self):
        stack = [[]]

        while stack:
            current_branch_constraints = stack.pop()

            # ... (Bounds merging logic remains the same) ...
            current_bounds = list(self.initial_bounds)
            feasible_node = True
            for idx, lower, upper in current_branch_constraints:
                base_min, base_max = current_bounds[idx]
                new_min = lower if base_min is None else max(base_min, lower)
                new_max = upper if base_max is None else min(base_max, upper)
                if new_max is not None and new_min > new_max:
                    feasible_node = False
                    break
                current_bounds[idx] = (new_min, new_max)

            if not feasible_node:
                continue

            # Solve Relaxation
            res = linprog(
                self.c, A_eq=self.A, b_eq=self.y, bounds=current_bounds, method="highs"
            )

            if not res.success:
                continue

            # Pruning: Check strictly against best_obj (allow slight float tolerance)
            if res.fun >= self.best_obj - 1e-9:
                continue

            x_current = res.x
            first_non_int_idx = -1
            first_non_int_val = 0

            # Check Integer Feasibility
            for idx in self.int_indices:
                val = x_current[idx]
                if not np.isclose(val, round(val), atol=1e-5):
                    first_non_int_idx = idx
                    first_non_int_val = val
                    break

            if first_non_int_idx == -1:
                # FOUND INTEGER SOLUTION

                # --- FIX START ---
                # 1. Round the solution vector to nearest integers
                candidate_sol = np.round(x_current).astype(int)

                # 2. Re-calculate objective using the CLEAN integers, not the dirty float 'res.fun'
                # This fixes the 32.99999 -> 32 issue.
                candidate_obj = np.dot(self.c, candidate_sol)

                if candidate_obj < self.best_obj:
                    self.best_obj = candidate_obj
                    self.best_sol = candidate_sol
                # --- FIX END ---

            else:
                # Branching logic (remains the same)
                floor_val = math.floor(first_non_int_val)
                ceil_val = math.ceil(first_non_int_val)
                stack.append(
                    current_branch_constraints
                    + [(first_non_int_idx, -np.inf, floor_val)]
                )
                stack.append(
                    current_branch_constraints + [(first_non_int_idx, ceil_val, np.inf)]
                )

        return self.best_obj, self.best_sol


def parse_input(file_path):
    machines = []
    re_lights = re.compile(r"\[([.#]+)\]")
    re_buttons = re.compile(r"\(([\d,]+)\)")
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

        lights_match = re_lights.search(line)
        if not lights_match:
            continue
        lights_str = lights_match.group(1)
        y_lights = np.array([1 if c == "#" else 0 for c in lights_str])

        n_rows = len(y_lights)
        button_matches = re_buttons.findall(line)
        cols = []
        for btn_str in button_matches:
            col = np.zeros(n_rows, dtype=int)
            if btn_str:
                indices = [int(x) for x in btn_str.split(",")]
                for idx in indices:
                    if idx < n_rows:
                        col[idx] = 1
            cols.append(col)

        if not cols:
            continue
        A = np.column_stack(cols)

        jolts_match = re_jolts.search(line)
        if jolts_match:
            y_jolts = np.array([int(x) for x in jolts_match.group(1).split(",")])
        else:
            y_jolts = np.zeros(n_rows, dtype=int)

        machines.append({"A": A, "y_lights": y_lights, "y_jolts": y_jolts})

    return machines


def solve_machine_part1(A, y):
    """
    Part 1: Minimize sum(x) s.t. Ax = y (mod 2).
    Model: Ax - 2k = y
    We construct a larger matrix [A | -2I] and solve for [x | k].
    """
    n_rows, n_vars = A.shape

    # 1. Augment Matrix: [A  -2*Identity]
    # We introduce 'k' slack variables, one for each row.
    neg_two_identity = -2 * np.eye(n_rows, dtype=int)
    A_prime = np.hstack([A, neg_two_identity])

    # 2. Cost Vector: Minimize sum(x), ignore k
    # c = [1, 1, ... (for x), 0, 0, ... (for k)]
    c = np.concatenate([np.ones(n_vars), np.zeros(n_rows)])

    # 3. Variable Bounds
    # x must be 0 or 1 (Binary)
    # k can be 0 to infinity (Slack)
    bounds = []
    for _ in range(n_vars):
        bounds.append((0, 1))  # x is binary
    for _ in range(n_rows):
        bounds.append((0, None))  # k is non-negative integer

    # All variables (x and k) must be integers
    int_indices = list(range(n_vars + n_rows))

    solver = CustomILPSolver(A_prime, y, c, int_indices, bounds)
    obj, sol = solver.solve()

    return int(obj) if sol is not None else 0


def solve_machine_part2(A, y):
    """
    Part 2: Minimize sum(x) s.t. Ax = y.
    Standard ILP.
    """
    n_rows, n_vars = A.shape
    c = np.ones(n_vars)

    # x must be non-negative integers
    int_indices = list(range(n_vars))

    solver = CustomILPSolver(A, y, c, int_indices)
    obj, sol = solver.solve()

    return int(obj) if sol is not None else 0


def main():
    parser = argparse.ArgumentParser(
        description="Advent of Code Day 10 Solver (Custom ILP)"
    )
    parser.add_argument("file", help="Path to the input file")
    args = parser.parse_args()

    machines = parse_input(args.file)

    total_p1 = 0
    total_p2 = 0

    print(f"Processing {len(machines)} machines using Custom B&B Solver...")

    for i, m in enumerate(machines):
        # Progress indicator for slower custom solvers
        print(f"Solving Machine {i+1}...", end="\r")

        p1 = solve_machine_part1(m["A"], m["y_lights"])
        total_p1 += p1

        p2 = solve_machine_part2(m["A"], m["y_jolts"])
        total_p2 += p2

    print(f"\n{'-'*30}")
    print(f"Part 1 Total: {total_p1}")
    print(f"Part 2 Total: {total_p2}")
    print(f"{'-'*30}")


if __name__ == "__main__":
    main()
