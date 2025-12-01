#!/usr/bin/python3

import sympy as sym


def main():

    P = sym.Symbol("P")

    P_i = sym.Symbol("P_i")
    P_j = sym.Symbol("P_j")
    P_k = sym.Symbol("P_k")

    v = sym.Symbol("v")

    v_i = sym.Symbol("v_i")
    v_j = sym.Symbol("v_j")
    v_k = sym.Symbol("v_k")

    t_i = sym.Symbol("t_i")
    t_j = sym.Symbol("t_j")
    t_k = sym.Symbol("t_k")

    v_xi = sym.Symbol("v_{xi}")
    v_xj = sym.Symbol("v_{xj}")
    v_xk = sym.Symbol("v_{xk}")

    v_yi = sym.Symbol("v_{yi}")
    v_yj = sym.Symbol("v_{yj}")
    v_yk = sym.Symbol("v_{yk}")

    v_zi = sym.Symbol("v_{zi}")
    v_zj = sym.Symbol("v_{zj}")
    v_zk = sym.Symbol("v_{zk}")

    x_i = sym.Symbol("x_i")
    x_j = sym.Symbol("x_j")
    x_k = sym.Symbol("x_k")

    y_i = sym.Symbol("y_i")
    y_j = sym.Symbol("y_j")
    y_k = sym.Symbol("y_k")

    z_i = sym.Symbol("z_i")
    z_j = sym.Symbol("z_j")
    z_k = sym.Symbol("z_k")

    equations = [
        t_i * (v_i - v) + P_i - P,
        t_j * (v_j - v) + P_j - P,
        t_k * (v_k - v) + P_k - P,
    ]

    equations.append(equations[1] - equations[0])
    equations.append(equations[2] - equations[0])

    equations.append((t_i - t_k) * equations[3] - (t_i - t_j) * equations[4])

    equations.append(
        t_i * t_j * (v_xj - v_xi)
        + t_j * t_k * (v_xk - v_xj)
        + t_i * t_k * (v_xi - v_xk)
        + t_i * (x_j - x_k)
        + t_j * (x_k - x_i)
        + t_k * (x_i - x_j)
    )

    equations.append(
        t_i * t_j * (v_yj - v_yi)
        + t_j * t_k * (v_yk - v_yj)
        + t_i * t_k * (v_yi - v_yk)
        + t_i * (y_j - y_k)
        + t_j * (y_k - y_i)
        + t_k * (y_i - v_j)
    )

    equations.append(
        t_i * t_j * (v_zj - v_zi)
        + t_j * t_k * (v_zk - v_zj)
        + t_i * t_k * (v_zi - v_zk)
        + t_i * (z_j - z_k)
        + t_j * (z_k - z_i)
        + t_k * (z_i - v_j)
    )

    equations.append((v_xj - v_xi) * equations[-2] + (v_yj - v_yi) * equations[-3])
    equations.append((v_xj - v_xi) * equations[-1] + (v_zj - v_zi) * equations[-3])

    new_lines = set((3, 5, 6, 9))
    for i in range(len(equations)):
        if i in new_lines:
            print("")
        print(equations[i])


if __name__ == "__main__":
    main()
