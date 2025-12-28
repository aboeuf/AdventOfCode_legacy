#include <solvers/2025/puzzle_2025_10.h>
#include <solvers/common.h>

#include <QDebug>
#include <QRegularExpression>
#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

namespace puzzle_2025_10 {

// --- Data Structures ---

struct MachineData {
  vector<int> target_lights;   // Part 1: 0 or 1
  vector<vector<int>> buttons; // List of affected indices per button
  vector<int> target_joltages; // Part 2: Integers
};

// --- Part 1: GF(2) Solver ---

// Returns {pivot_column_indices, free_column_indices}
pair<vector<int>, vector<int>> rref_gf2(vector<vector<int>> &A,
                                        vector<int> &b) {
  int rows = A.size();
  if (rows == 0)
    return {{}, {}};
  int cols = A[0].size();

  int pivot_row = 0;
  vector<int> pivots;
  vector<int> col_is_pivot(cols, 0);

  for (int j = 0; j < cols && pivot_row < rows; ++j) {
    // Find pivot
    int sel = -1;
    for (int i = pivot_row; i < rows; ++i) {
      if (A[i][j]) {
        sel = i;
        break;
      }
    }

    if (sel == -1)
      continue; // Column is free

    // Swap
    swap(A[pivot_row], A[sel]);
    int val_b = b[pivot_row];
    b[pivot_row] = b[sel];
    b[sel] = val_b;

    // Eliminate
    for (int i = 0; i < rows; ++i) {
      if (i != pivot_row && A[i][j]) {
        for (int k = j; k < cols; ++k) {
          A[i][k] ^= A[pivot_row][k];
        }
        b[i] ^= b[pivot_row];
      }
    }

    col_is_pivot[j] = 1;
    pivots.push_back(j);
    pivot_row++;
  }

  vector<int> frees;
  for (int j = 0; j < cols; ++j) {
    if (!col_is_pivot[j])
      frees.push_back(j);
  }

  return {pivots, frees};
}

long long solve_part1_math(const MachineData &m) {
  int R = m.target_lights.size();
  int C = m.buttons.size();

  // Build Matrix A
  vector<vector<int>> A(R, vector<int>(C, 0));
  for (int j = 0; j < C; ++j) {
    for (int row_idx : m.buttons[j]) {
      if (row_idx < R)
        A[row_idx][j] = 1;
    }
  }
  vector<int> b = m.target_lights;

  auto [pivots, frees] = rref_gf2(A, b);

  // Check for inconsistency
  for (int i = 0; i < R; ++i) {
    bool all_zero = true;
    for (int val : A[i])
      if (val)
        all_zero = false;
    if (all_zero && b[i])
      return 0; // Impossible
  }

  long long min_presses = -1;
  int num_free = frees.size();
  long long combinations = 1LL << num_free;

  // Iterate free variables
  for (long long i = 0; i < combinations; ++i) {
    vector<int> x(C, 0);

    // Assign free vars
    for (int bit = 0; bit < num_free; ++bit) {
      if ((i >> bit) & 1)
        x[frees[bit]] = 1;
    }

    // Back-substitute
    for (int r = pivots.size() - 1; r >= 0; --r) {
      int pc = pivots[r];
      int val = b[r];
      for (int k = pc + 1; k < C; ++k) {
        if (A[r][k])
          val ^= x[k];
      }
      x[pc] = val;
    }

    long long current_sum = 0;
    for (int v : x)
      current_sum += v;

    if (min_presses == -1 || current_sum < min_presses) {
      min_presses = current_sum;
    }
  }

  return (min_presses == -1) ? 0 : min_presses;
}

// --- Part 2: Integer Solver ---

struct LinSysResult {
  vector<vector<double>> A;
  vector<double> b;
  vector<int> pivots;
  vector<int> frees;
  int rank;
};

LinSysResult rref_double(vector<vector<double>> A, vector<double> b) {
  int rows = A.size();
  if (rows == 0)
    return {A, b, {}, {}, 0};
  int cols = A[0].size();

  int pivot_row = 0;
  vector<int> pivots;
  vector<int> col_is_pivot(cols, 0);
  const double EPS = 1e-9;

  for (int j = 0; j < cols && pivot_row < rows; ++j) {
    int sel = -1;
    for (int i = pivot_row; i < rows; ++i) {
      if (std::abs(A[i][j]) > EPS) {
        sel = i;
        break;
      }
    }

    if (sel == -1)
      continue;

    swap(A[pivot_row], A[sel]);
    swap(b[pivot_row], b[sel]);

    double div = A[pivot_row][j];
    for (int k = j; k < cols; ++k)
      A[pivot_row][k] /= div;
    b[pivot_row] /= div;

    for (int i = 0; i < rows; ++i) {
      if (i != pivot_row && std::abs(A[i][j]) > EPS) {
        double factor = A[i][j];
        for (int k = j; k < cols; ++k)
          A[i][k] -= factor * A[pivot_row][k];
        b[i] -= factor * b[pivot_row];
      }
    }

    col_is_pivot[j] = 1;
    pivots.push_back(j);
    pivot_row++;
  }

  vector<int> frees;
  for (int j = 0; j < cols; ++j)
    if (!col_is_pivot[j])
      frees.push_back(j);

  return {A, b, pivots, frees, pivot_row};
}

long long min_p2_presses;
bool found_p2;

void solve_p2_recursive(int f_idx, const vector<int> &frees,
                        const vector<double> &free_vals,
                        const LinSysResult &sys, const MachineData &m) {

  long long current_cost = 0;
  for (double v : free_vals)
    current_cost += (long long)v;

  if (found_p2 && current_cost >= min_p2_presses)
    return;

  if (f_idx == (int)frees.size()) {
    vector<double> x(m.buttons.size(), 0.0);
    for (size_t i = 0; i < frees.size(); ++i)
      x[frees[i]] = free_vals[i];

    bool valid = true;
    for (int r = 0; r < sys.rank; ++r) {
      int p_col = sys.pivots[r];
      double val = sys.b[r];
      for (int f_col : frees) {
        val -= sys.A[r][f_col] * x[f_col];
      }

      if (val < -1e-4) {
        valid = false;
        break;
      }
      double rounded = std::round(val);
      if (std::abs(val - rounded) > 1e-4) {
        valid = false;
        break;
      }
      x[p_col] = rounded;
    }

    if (valid) {
      long long total = 0;
      for (double v : x)
        total += (long long)v;
      if (!found_p2 || total < min_p2_presses) {
        min_p2_presses = total;
        found_p2 = true;
      }
    }
    return;
  }

  int btn_idx = frees[f_idx];
  int limit = INT_MAX;
  if (m.buttons[btn_idx].empty()) {
    limit = 0;
  } else {
    for (int aff_idx : m.buttons[btn_idx]) {
      if (aff_idx < (int)m.target_joltages.size()) {
        limit = std::min(limit, m.target_joltages[aff_idx]);
      }
    }
  }

  for (int v = 0; v <= limit; ++v) {
    vector<double> next_vals = free_vals;
    next_vals.push_back((double)v);
    solve_p2_recursive(f_idx + 1, frees, next_vals, sys, m);
    if (found_p2 && current_cost + v >= min_p2_presses)
      break;
  }
}

long long solve_part2_math(const MachineData &m) {
  int R = m.target_joltages.size();
  int C = m.buttons.size();

  vector<vector<double>> A(R, vector<double>(C, 0.0));
  for (int j = 0; j < C; ++j) {
    for (int r : m.buttons[j]) {
      if (r < R)
        A[r][j] = 1.0;
    }
  }
  vector<double> b(R);
  for (int i = 0; i < R; ++i)
    b[i] = (double)m.target_joltages[i];

  auto res = rref_double(A, b);

  // Basic consistency check
  for (int i = res.rank; i < R; ++i) {
    if (std::abs(res.b[i]) > 1e-5)
      return 0;
  }

  min_p2_presses = -1;
  found_p2 = false;

  solve_p2_recursive(0, res.frees, {}, res, m);

  return found_p2 ? min_p2_presses : 0;
}

// --- Parsing ---

MachineData parseMachine(const QString &line) {
  MachineData m;

  // Regex for: [.##.]
  QRegularExpression reLights(R"(\[([.#]+)\])");
  auto matchLights = reLights.match(line);
  if (matchLights.hasMatch()) {
    QString s = matchLights.captured(1);
    for (QChar c : s)
      m.target_lights.push_back(c == '#' ? 1 : 0);
  }

  // Regex for buttons: (0,1,2)
  // We iterate over the string to find all occurrences
  QRegularExpression reBtn(R"(\(([\d,]+)\))");
  auto it = reBtn.globalMatch(line);
  while (it.hasNext()) {
    auto match = it.next();
    QString nums = match.captured(1);
    vector<int> btn;
    for (const auto &sVal : nums.split(',')) {
      if (!sVal.isEmpty())
        btn.push_back(sVal.toInt());
    }
    m.buttons.push_back(btn);
  }

  // Regex for joltage: {3,5,4}
  QRegularExpression reJolt(R"(\{([\d,]+)\})");
  auto matchJolt = reJolt.match(line);
  if (matchJolt.hasMatch()) {
    QString nums = matchJolt.captured(1);
    for (const auto &sVal : nums.split(',')) {
      if (!sVal.isEmpty())
        m.target_joltages.push_back(sVal.toInt());
    }
  }

  return m;
}

} // namespace puzzle_2025_10

void Solver_2025_10_1::solve(const QString &input) {
  auto lines = common::splitLines(input, true);
  long long total = 0;
  for (const auto &line : lines) {
    auto m = puzzle_2025_10::parseMachine(line);
    total += puzzle_2025_10::solve_part1_math(m);
  }
  emit finished(QString::number(total));
}

void Solver_2025_10_2::solve(const QString &input) {
  auto lines = common::splitLines(input, true);
  long long total = 0;
  for (const auto &line : lines) {
    auto m = puzzle_2025_10::parseMachine(line);
    total += puzzle_2025_10::solve_part2_math(m);
  }
  emit finished(QString::number(total));
}
