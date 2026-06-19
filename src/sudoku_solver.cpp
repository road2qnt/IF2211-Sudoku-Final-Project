/**
 * sudoku_solver.cpp
 * Comprehensive Sudoku Solver with 4 algorithm variants
 * For academic paper: Performance Analysis of Sudoku Solvers
 * Using Backtracking, MRV, and Human-Inspired Constraint Propagation
 *
 * Author: Ega Luthfi Rais - 13524115
 * Compile: g++ -std=c++20 -O2 -o sudoku_solver sudoku_solver.cpp
 * Run:     ./sudoku_solver
 */

#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <algorithm>
#include <random>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <cassert>
#include <functional>

// ============================================================
// Constants & Types
// ============================================================
constexpr int N = 9;
constexpr int SUB = 3;
constexpr int EMPTY = 0;

using Board = std::array<std::array<int, N>, N>;

struct PuzzleResult {
    std::string puzzle_id;
    int clue_count;
    std::string difficulty;
    
    // Pure Backtracking
    long long bt_time_us;
    long long bt_recursive_calls;
    long long bt_backtracks;
    bool bt_solved;
    
    // Backtracking + MRV
    long long mrv_time_us;
    long long mrv_recursive_calls;
    long long mrv_backtracks;
    bool mrv_solved;
    
    // Backtracking + Constraint Propagation
    long long cp_time_us;
    long long cp_recursive_calls;
    long long cp_backtracks;
    bool cp_solved;
    
    // Hybrid (MRV + Constraint Propagation)
    long long hybrid_time_us;
    long long hybrid_recursive_calls;
    long long hybrid_backtracks;
    bool hybrid_solved;
};

// ============================================================
// Utility Functions
// ============================================================

void print_board(const Board& board) {
    for (int i = 0; i < N; i++) {
        if (i % SUB == 0) std::cout << "+-------+-------+-------+\n";
        for (int j = 0; j < N; j++) {
            if (j % SUB == 0) std::cout << "| ";
            if (board[i][j] == EMPTY)
                std::cout << ". ";
            else
                std::cout << board[i][j] << " ";
        }
        std::cout << "|\n";
    }
    std::cout << "+-------+-------+-------+\n";
}

Board copy_board(const Board& src) {
    Board dst;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            dst[i][j] = src[i][j];
    return dst;
}

std::string board_to_string(const Board& board) {
    std::string s;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            s += (board[i][j] == EMPTY ? '0' : static_cast<char>('0' + board[i][j]));
    return s;
}

Board string_to_board(const std::string& s) {
    Board board{};
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            board[i][j] = (s[i * N + j] == '0') ? EMPTY : (s[i * N + j] - '0');
    return board;
}

// ============================================================
// Validation
// ============================================================

bool is_valid_move(const Board& board, int row, int col, int num) {
    // Check row
    for (int j = 0; j < N; j++)
        if (board[row][j] == num) return false;
    
    // Check column
    for (int i = 0; i < N; i++)
        if (board[i][col] == num) return false;
    
    // Check subgrid
    int sr = (row / SUB) * SUB;
    int sc = (col / SUB) * SUB;
    for (int i = sr; i < sr + SUB; i++)
        for (int j = sc; j < sc + SUB; j++)
            if (board[i][j] == num) return false;
    
    return true;
}

bool is_board_valid(const Board& board) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] != EMPTY) {
                int val = board[i][j];
                // Temporarily clear and re-check
                Board temp = copy_board(board);
                temp[i][j] = EMPTY;
                if (!is_valid_move(temp, i, j, val)) return false;
                temp[i][j] = val;
            }
        }
    }
    return true;
}

bool is_board_solved(const Board& board) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == EMPTY) return false;
    return is_board_valid(board);
}

// ============================================================
// Candidate Counting
// ============================================================

std::vector<int> get_candidates(const Board& board, int row, int col) {
    if (board[row][col] != EMPTY)
        return {};
    
    bool used[N + 1] = {false};
    
    // Row
    for (int j = 0; j < N; j++)
        if (board[row][j] != EMPTY)
            used[board[row][j]] = true;
    
    // Column
    for (int i = 0; i < N; i++)
        if (board[i][col] != EMPTY)
            used[board[i][col]] = true;
    
    // Subgrid
    int sr = (row / SUB) * SUB;
    int sc = (col / SUB) * SUB;
    for (int i = sr; i < sr + SUB; i++)
        for (int j = sc; j < sc + SUB; j++)
            if (board[i][j] != EMPTY)
                used[board[i][j]] = true;
    
    std::vector<int> candidates;
    for (int num = 1; num <= N; num++)
        if (!used[num])
            candidates.push_back(num);
    
    return candidates;
}

int count_candidates(const Board& board, int row, int col) {
    if (board[row][col] != EMPTY) return 0;
    
    bool used[N + 1] = {false};
    
    for (int j = 0; j < N; j++)
        if (board[row][j] != EMPTY)
            used[board[row][j]] = true;
    
    for (int i = 0; i < N; i++)
        if (board[i][col] != EMPTY)
            used[board[i][col]] = true;
    
    int sr = (row / SUB) * SUB;
    int sc = (col / SUB) * SUB;
    for (int i = sr; i < sr + SUB; i++)
        for (int j = sc; j < sc + SUB; j++)
            if (board[i][j] != EMPTY)
                used[board[i][j]] = true;
    
    int cnt = 0;
    for (int num = 1; num <= N; num++)
        if (!used[num]) cnt++;
    
    return cnt;
}

// ============================================================
// SOLVER A: Pure Backtracking
// ============================================================

struct SolverStats {
    long long recursive_calls = 0;
    long long backtracks = 0;
};

bool find_empty_cell(const Board& board, int& row, int& col) {
    for (row = 0; row < N; row++)
        for (col = 0; col < N; col++)
            if (board[row][col] == EMPTY)
                return true;
    return false;
}

bool solve_backtracking(Board& board, SolverStats& stats) {
    int row, col;
    if (!find_empty_cell(board, row, col))
        return true;
    
    for (int num = 1; num <= N; num++) {
        if (is_valid_move(board, row, col, num)) {
            board[row][col] = num;
            stats.recursive_calls++;
            
            if (solve_backtracking(board, stats))
                return true;
            
            board[row][col] = EMPTY;
            stats.backtracks++;
        }
    }
    return false;
}

// ============================================================
// SOLVER B: Backtracking + MRV
// ============================================================

bool find_best_cell_mrv(const Board& board, int& best_row, int& best_col) {
    int min_candidates = N + 1;
    bool found = false;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == EMPTY) {
                int cnt = count_candidates(board, i, j);
                if (cnt == 0) {
                    // Dead end — this cell has no possible values
                    return false;
                }
                if (cnt < min_candidates) {
                    min_candidates = cnt;
                    best_row = i;
                    best_col = j;
                    found = true;
                    if (min_candidates == 1) return true;
                }
            }
        }
    }
    return found;
}

bool solve_mrv(Board& board, SolverStats& stats) {
    int row, col;
    if (!find_best_cell_mrv(board, row, col)) {
        // Check if truly solved (no empty cells) or dead end (cell with 0 candidates)
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (board[i][j] == EMPTY)
                    return false;  // Dead end
        return true;  // Solved
    }
    
    std::vector<int> candidates = get_candidates(board, row, col);
    
    for (int num : candidates) {
        board[row][col] = num;
        stats.recursive_calls++;
        
        if (solve_mrv(board, stats))
            return true;
        
        board[row][col] = EMPTY;
        stats.backtracks++;
    }
    return false;
}

// ============================================================
// Constraint Propagation: Naked Single & Hidden Single
// ============================================================

bool apply_naked_single(Board& board) {
    bool changed = false;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == EMPTY) {
                auto cands = get_candidates(board, i, j);
                if (cands.size() == 1) {
                    board[i][j] = cands[0];
                    changed = true;
                }
            }
        }
    }
    return changed;
}

bool apply_hidden_single(Board& board) {
    bool changed = false;
    
    // Check rows
    for (int i = 0; i < N; i++) {
        for (int num = 1; num <= N; num++) {
            int count = 0;
            int last_col = -1;
            for (int j = 0; j < N; j++) {
                if (board[i][j] == EMPTY) {
                    auto cands = get_candidates(board, i, j);
                    if (std::find(cands.begin(), cands.end(), num) != cands.end()) {
                        count++;
                        last_col = j;
                    }
                }
            }
            if (count == 1) {
                board[i][last_col] = num;
                changed = true;
            }
        }
    }
    
    // Check columns
    for (int j = 0; j < N; j++) {
        for (int num = 1; num <= N; num++) {
            int count = 0;
            int last_row = -1;
            for (int i = 0; i < N; i++) {
                if (board[i][j] == EMPTY) {
                    auto cands = get_candidates(board, i, j);
                    if (std::find(cands.begin(), cands.end(), num) != cands.end()) {
                        count++;
                        last_row = i;
                    }
                }
            }
            if (count == 1) {
                board[last_row][j] = num;
                changed = true;
            }
        }
    }
    
    // Check subgrids
    for (int sr = 0; sr < N; sr += SUB) {
        for (int sc = 0; sc < N; sc += SUB) {
            for (int num = 1; num <= N; num++) {
                int count = 0;
                int last_row = -1, last_col = -1;
                for (int i = sr; i < sr + SUB; i++) {
                    for (int j = sc; j < sc + SUB; j++) {
                        if (board[i][j] == EMPTY) {
                            auto cands = get_candidates(board, i, j);
                            if (std::find(cands.begin(), cands.end(), num) != cands.end()) {
                                count++;
                                last_row = i;
                                last_col = j;
                            }
                        }
                    }
                }
                if (count == 1) {
                    board[last_row][last_col] = num;
                    changed = true;
                }
            }
        }
    }
    
    return changed;
}

void apply_constraint_propagation(Board& board) {
    bool progress = true;
    while (progress) {
        progress = false;
        if (apply_naked_single(board)) progress = true;
        if (apply_hidden_single(board)) progress = true;
    }
}

// ============================================================
// SOLVER C: Backtracking + Constraint Propagation
// Uses board-copy approach to safely handle constraint propagation
// during recursive search without corrupting state on backtrack.
// ============================================================

bool solve_cp(Board& board, SolverStats& stats) {
    apply_constraint_propagation(board);
    
    int row, col;
    if (!find_empty_cell(board, row, col))
        return true;
    
    std::vector<int> candidates = get_candidates(board, row, col);
    
    for (int num : candidates) {
        stats.recursive_calls++;
        
        // Work on a copy to avoid state corruption from constraint propagation
        Board next = copy_board(board);
        next[row][col] = num;
        
        if (solve_cp(next, stats)) {
            board = next;
            return true;
        }
        
        stats.backtracks++;
    }
    return false;
}

// ============================================================
// SOLVER D: Hybrid (MRV + Constraint Propagation)
// Combines MRV variable selection with constraint propagation
// Uses board-copy approach for safe propagation during search.
// ============================================================

bool solve_hybrid(Board& board, SolverStats& stats) {
    apply_constraint_propagation(board);
    
    int row, col;
    if (!find_best_cell_mrv(board, row, col)) {
        // Check if truly solved or dead end
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (board[i][j] == EMPTY)
                    return false;  // Dead end
        return true;  // Solved
    }
    
    std::vector<int> candidates = get_candidates(board, row, col);
    
    for (int num : candidates) {
        stats.recursive_calls++;
        
        // Work on a copy to avoid state corruption from constraint propagation
        Board next = copy_board(board);
        next[row][col] = num;
        
        if (solve_hybrid(next, stats)) {
            board = next;
            return true;
        }
        
        stats.backtracks++;
    }
    return false;
}

// ============================================================
// Puzzle Generator
// ============================================================

Board generate_solved_board(std::mt19937& rng) {
    Board board{};
    
    // Fill using backtracking with randomization
    std::function<bool(Board&)> fill = [&](Board& b) -> bool {
        int row, col;
        if (!find_empty_cell(b, row, col))
            return true;
        
        std::vector<int> nums(N);
        for (int i = 0; i < N; i++) nums[i] = i + 1;
        std::shuffle(nums.begin(), nums.end(), rng);
        
        for (int num : nums) {
            if (is_valid_move(b, row, col, num)) {
                b[row][col] = num;
                if (fill(b)) return true;
                b[row][col] = EMPTY;
            }
        }
        return false;
    };
    
    fill(board);
    return board;
}

PuzzleResult create_puzzle(const Board& solved, int target_clues, 
                           const std::string& difficulty, const std::string& id,
                           std::mt19937& rng) {
    Board puzzle = copy_board(solved);
    
    // Determine which cells to remove
    std::vector<int> indices(N * N);
    for (int i = 0; i < N * N; i++) indices[i] = i;
    std::shuffle(indices.begin(), indices.end(), rng);
    
    int to_remove = N * N - target_clues;
    int removed = 0;
    
    for (int idx : indices) {
        if (removed >= to_remove) break;
        int row = idx / N;
        int col = idx % N;
        puzzle[row][col] = EMPTY;
        removed++;
    }
    
    PuzzleResult result;
    result.puzzle_id = id;
    result.clue_count = target_clues;
    result.difficulty = difficulty;
    
    return result;
}

// ============================================================
// Run Single Solver & Measure
// ============================================================

template<typename SolverFunc>
void run_solver(const Board& puzzle, SolverFunc solver_func,
                long long& time_us, long long& recursive_calls,
                long long& backtracks, bool& solved) {
    Board board = copy_board(puzzle);
    SolverStats stats;
    
    auto start = std::chrono::high_resolution_clock::now();
    solved = solver_func(board, stats);
    auto end = std::chrono::high_resolution_clock::now();
    
    time_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    recursive_calls = stats.recursive_calls;
    backtracks = stats.backtracks;
    
    // Verify solution
    if (solved && !is_board_solved(board)) {
        solved = false;
    }
}

void run_all_solvers(const Board& puzzle, PuzzleResult& result) {
    // A: Pure Backtracking
    run_solver(puzzle, [](Board& b, SolverStats& s) { return solve_backtracking(b, s); },
               result.bt_time_us, result.bt_recursive_calls, result.bt_backtracks, result.bt_solved);
    
    // B: MRV
    run_solver(puzzle, [](Board& b, SolverStats& s) { return solve_mrv(b, s); },
               result.mrv_time_us, result.mrv_recursive_calls, result.mrv_backtracks, result.mrv_solved);
    
    // C: Constraint Propagation
    run_solver(puzzle, [](Board& b, SolverStats& s) { return solve_cp(b, s); },
               result.cp_time_us, result.cp_recursive_calls, result.cp_backtracks, result.cp_solved);
    
    // D: Hybrid
    run_solver(puzzle, [](Board& b, SolverStats& s) { return solve_hybrid(b, s); },
               result.hybrid_time_us, result.hybrid_recursive_calls, result.hybrid_backtracks, result.hybrid_solved);
}

// ============================================================
// CSV Output
// ============================================================

void write_csv_header(std::ofstream& out) {
    out << "puzzle_id,clue_count,difficulty,"
        << "bt_time_us,bt_recursive_calls,bt_backtracks,bt_solved,"
        << "mrv_time_us,mrv_recursive_calls,mrv_backtracks,mrv_solved,"
        << "cp_time_us,cp_recursive_calls,cp_backtracks,cp_solved,"
        << "hybrid_time_us,hybrid_recursive_calls,hybrid_backtracks,hybrid_solved\n";
}

void write_csv_row(std::ofstream& out, const PuzzleResult& r) {
    out << r.puzzle_id << "," << r.clue_count << "," << r.difficulty << ","
        << r.bt_time_us << "," << r.bt_recursive_calls << "," << r.bt_backtracks << "," << r.bt_solved << ","
        << r.mrv_time_us << "," << r.mrv_recursive_calls << "," << r.mrv_backtracks << "," << r.mrv_solved << ","
        << r.cp_time_us << "," << r.cp_recursive_calls << "," << r.cp_backtracks << "," << r.cp_solved << ","
        << r.hybrid_time_us << "," << r.hybrid_recursive_calls << "," << r.hybrid_backtracks << "," << r.hybrid_solved << "\n";
}

// ============================================================
// Summary Statistics
// ============================================================

struct SummaryStats {
    double avg_time_us;
    double avg_recursive_calls;
    double avg_backtracks;
    double solved_pct;
};

SummaryStats compute_summary(const std::vector<PuzzleResult>& results,
                              std::function<long long(const PuzzleResult&)> time_fn,
                              std::function<long long(const PuzzleResult&)> calls_fn,
                              std::function<long long(const PuzzleResult&)> bt_fn,
                              std::function<bool(const PuzzleResult&)> solved_fn) {
    SummaryStats s{};
    int count = 0;
    int solved_count = 0;
    
    for (const auto& r : results) {
        if (!solved_fn(r)) continue;
        s.avg_time_us += time_fn(r);
        s.avg_recursive_calls += calls_fn(r);
        s.avg_backtracks += bt_fn(r);
        solved_count++;
        count++;
    }
    
    if (count > 0) {
        s.avg_time_us /= count;
        s.avg_recursive_calls /= count;
        s.avg_backtracks /= count;
    }
    s.solved_pct = results.empty() ? 0.0 : (100.0 * solved_count / results.size());
    
    return s;
}

void print_summary_table(const std::vector<PuzzleResult>& easy,
                          const std::vector<PuzzleResult>& medium,
                          const std::vector<PuzzleResult>& hard) {
    auto get_bt_time = [](const PuzzleResult& r) { return r.bt_time_us; };
    auto get_mrv_time = [](const PuzzleResult& r) { return r.mrv_time_us; };
    auto get_cp_time = [](const PuzzleResult& r) { return r.cp_time_us; };
    auto get_hybrid_time = [](const PuzzleResult& r) { return r.hybrid_time_us; };
    
    auto get_bt_calls = [](const PuzzleResult& r) { return r.bt_recursive_calls; };
    auto get_mrv_calls = [](const PuzzleResult& r) { return r.mrv_recursive_calls; };
    auto get_cp_calls = [](const PuzzleResult& r) { return r.cp_recursive_calls; };
    auto get_hybrid_calls = [](const PuzzleResult& r) { return r.hybrid_recursive_calls; };
    
    auto get_bt_bt = [](const PuzzleResult& r) { return r.bt_backtracks; };
    auto get_mrv_bt = [](const PuzzleResult& r) { return r.mrv_backtracks; };
    auto get_cp_bt = [](const PuzzleResult& r) { return r.cp_backtracks; };
    auto get_hybrid_bt = [](const PuzzleResult& r) { return r.hybrid_backtracks; };
    
    auto always_true = [](const PuzzleResult&) { return true; };
    
    auto bt_solved_fn = [](const PuzzleResult& r) { return r.bt_solved; };
    auto mrv_solved_fn = [](const PuzzleResult& r) { return r.mrv_solved; };
    auto cp_solved_fn = [](const PuzzleResult& r) { return r.cp_solved; };
    auto hybrid_solved_fn = [](const PuzzleResult& r) { return r.hybrid_solved; };
    
    auto compute_all = [&](const std::vector<PuzzleResult>& data, const std::string& label) {
        SummaryStats bt_s = compute_summary(data, get_bt_time, get_bt_calls, get_bt_bt, bt_solved_fn);
        SummaryStats mrv_s = compute_summary(data, get_mrv_time, get_mrv_calls, get_mrv_bt, mrv_solved_fn);
        SummaryStats cp_s = compute_summary(data, get_cp_time, get_cp_calls, get_cp_bt, cp_solved_fn);
        SummaryStats hybrid_s = compute_summary(data, get_hybrid_time, get_hybrid_calls, get_hybrid_bt, hybrid_solved_fn);
        
        std::cout << "\n" << label << "\n";
        std::cout << "Algorithm              Avg Time(ms)   Avg Calls    Avg Backtracks   Solved(%)\n";
        std::cout << "--------------------------------------------------------------------------------\n";
        
        auto print_row = [&](const std::string& name, const SummaryStats& s) {
            std::cout << std::left << std::setw(22) << name << " "
                      << std::right << std::setw(10) << std::fixed << std::setprecision(2) << s.avg_time_us / 1000.0 << "   "
                      << std::setw(10) << std::fixed << std::setprecision(0) << s.avg_recursive_calls << "   "
                      << std::setw(12) << std::fixed << std::setprecision(0) << s.avg_backtracks << "   "
                      << std::setw(6) << std::fixed << std::setprecision(1) << s.solved_pct << "\n";
        };
        
        print_row("Pure Backtracking", bt_s);
        print_row("MRV", mrv_s);
        print_row("Constraint Prop.", cp_s);
        print_row("Hybrid Solver", hybrid_s);
    };
    
    compute_all(easy, "=== EASY PUZZLES ===");
    compute_all(medium, "=== MEDIUM PUZZLES ===");
    compute_all(hard, "=== HARD PUZZLES ===");
    
    // Overall
    std::vector<PuzzleResult> all;
    all.insert(all.end(), easy.begin(), easy.end());
    all.insert(all.end(), medium.begin(), medium.end());
    all.insert(all.end(), hard.begin(), hard.end());
    compute_all(all, "=== ALL PUZZLES ===");
}

// ============================================================
// Write puzzles to files
// ============================================================

void write_puzzles_to_file(const std::vector<PuzzleResult>& puzzles,
                            const std::vector<Board>& boards,
                            const std::string& filename,
                            const std::string& difficulty) {
    std::ofstream out(filename);
    out << "puzzle_id,clue_count,difficulty,board_string\n";
    
    for (size_t i = 0; i < puzzles.size(); i++) {
        out << puzzles[i].puzzle_id << ","
            << puzzles[i].clue_count << ","
            << difficulty << ","
            << board_to_string(boards[i]) << "\n";
    }
    out.close();
}

// ============================================================
// Main Function
// ============================================================

int main() {
    std::cout << "======================================\n";
    std::cout << "Sudoku Solver Performance Analysis\n";
    std::cout << "Author: Ega Luthfi Rais - 13524115\n";
    std::cout << "======================================\n\n";
    
    // Fixed seed for reproducibility
    std::mt19937 rng(42);
    
    const int NUM_EASY = 50;
    const int NUM_MEDIUM = 50;
    const int NUM_HARD = 50;
    
    const int EASY_CLUES = 38;    // 36-40
    const int MEDIUM_CLUES = 32;  // 30-35
    const int HARD_CLUES = 27;    // 24-29
    
    std::vector<Board> easy_boards, medium_boards, hard_boards;
    std::vector<PuzzleResult> easy_results, medium_results, hard_results;
    
    std::cout << "Generating " << (NUM_EASY + NUM_MEDIUM + NUM_HARD) << " puzzles...\n";
    
    // Generate Easy puzzles
    for (int i = 0; i < NUM_EASY; i++) {
        Board solved = generate_solved_board(rng);
        int clues = EASY_CLUES + (rng() % 5);  // 36-40
        std::string id = "E" + std::to_string(i + 1);
        PuzzleResult pr = create_puzzle(solved, clues, "Easy", id, rng);
        Board puzzle = string_to_board(board_to_string(solved)); // Rebuild to get the puzzle with removed cells
        
        // Actually create the puzzle by removing cells from solved
        Board p = copy_board(solved);
        std::vector<int> indices(N * N);
        for (int j = 0; j < N * N; j++) indices[j] = j;
        std::shuffle(indices.begin(), indices.end(), rng);
        int to_remove = N * N - clues;
        for (int j = 0; j < to_remove; j++) {
            p[indices[j] / N][indices[j] % N] = EMPTY;
        }
        
        easy_boards.push_back(p);
        easy_results.push_back(pr);
        
        if ((i + 1) % 10 == 0)
            std::cout << "  Generated " << (i + 1) << " easy puzzles\n";
    }
    
    // Generate Medium puzzles
    for (int i = 0; i < NUM_MEDIUM; i++) {
        Board solved = generate_solved_board(rng);
        int clues = MEDIUM_CLUES + (rng() % 6);  // 30-35
        std::string id = "M" + std::to_string(i + 1);
        
        Board p = copy_board(solved);
        std::vector<int> indices(N * N);
        for (int j = 0; j < N * N; j++) indices[j] = j;
        std::shuffle(indices.begin(), indices.end(), rng);
        int to_remove = N * N - clues;
        for (int j = 0; j < to_remove; j++) {
            p[indices[j] / N][indices[j] % N] = EMPTY;
        }
        
        PuzzleResult pr;
        pr.puzzle_id = id;
        pr.clue_count = clues;
        pr.difficulty = "Medium";
        
        medium_boards.push_back(p);
        medium_results.push_back(pr);
        
        if ((i + 1) % 10 == 0)
            std::cout << "  Generated " << (i + 1) << " medium puzzles\n";
    }
    
    // Generate Hard puzzles
    for (int i = 0; i < NUM_HARD; i++) {
        Board solved = generate_solved_board(rng);
        int clues = HARD_CLUES + (rng() % 6);  // 24-29
        std::string id = "H" + std::to_string(i + 1);
        
        Board p = copy_board(solved);
        std::vector<int> indices(N * N);
        for (int j = 0; j < N * N; j++) indices[j] = j;
        std::shuffle(indices.begin(), indices.end(), rng);
        int to_remove = N * N - clues;
        for (int j = 0; j < to_remove; j++) {
            p[indices[j] / N][indices[j] % N] = EMPTY;
        }
        
        PuzzleResult pr;
        pr.puzzle_id = id;
        pr.clue_count = clues;
        pr.difficulty = "Hard";
        
        hard_boards.push_back(p);
        hard_results.push_back(pr);
        
        if ((i + 1) % 10 == 0)
            std::cout << "  Generated " << (i + 1) << " hard puzzles\n";
    }
    
    std::cout << "\nPuzzle generation complete!\n\n";
    
    std::cout << "======================================\n";
    std::cout << "Running Solvers...\n";
    std::cout << "======================================\n";
    
    // Run solvers on Easy puzzles
    std::cout << "\nSolving Easy puzzles...\n";
    for (size_t i = 0; i < easy_boards.size(); i++) {
        run_all_solvers(easy_boards[i], easy_results[i]);
        if ((i + 1) % 10 == 0)
            std::cout << "  Solved " << (i + 1) << "/" << NUM_EASY << " easy puzzles\n";
    }
    
    // Run solvers on Medium puzzles
    std::cout << "\nSolving Medium puzzles...\n";
    for (size_t i = 0; i < medium_boards.size(); i++) {
        run_all_solvers(medium_boards[i], medium_results[i]);
        if ((i + 1) % 10 == 0)
            std::cout << "  Solved " << (i + 1) << "/" << NUM_MEDIUM << " medium puzzles\n";
    }
    
    // Run solvers on Hard puzzles
    std::cout << "\nSolving Hard puzzles...\n";
    for (size_t i = 0; i < hard_boards.size(); i++) {
        run_all_solvers(hard_boards[i], hard_results[i]);
        if ((i + 1) % 10 == 0)
            std::cout << "  Solved " << (i + 1) << "/" << NUM_HARD << " hard puzzles\n";
    }
    
    std::cout << "\nAll solvers complete!\n";
    
    // Print summary tables
    print_summary_table(easy_results, medium_results, hard_results);
    
    // Write results to CSV
    std::ofstream csv_out("../output/results.csv");
    write_csv_header(csv_out);
    for (const auto& r : easy_results) write_csv_row(csv_out, r);
    for (const auto& r : medium_results) write_csv_row(csv_out, r);
    for (const auto& r : hard_results) write_csv_row(csv_out, r);
    csv_out.close();
    
    // Write puzzles to files
    write_puzzles_to_file(easy_results, easy_boards, "../data/easy_puzzles.csv", "Easy");
    write_puzzles_to_file(medium_results, medium_boards, "../data/medium_puzzles.csv", "Medium");
    write_puzzles_to_file(hard_results, hard_boards, "../data/hard_puzzles.csv", "Hard");
    
    std::cout << "\nResults saved to:\n";
    std::cout << "  - output/results.csv\n";
    std::cout << "  - data/easy_puzzles.csv\n";
    std::cout << "  - data/medium_puzzles.csv\n";
    std::cout << "  - data/hard_puzzles.csv\n";
    
    return 0;
}
