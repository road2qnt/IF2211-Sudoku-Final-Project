# Sudoku Solver

Implementasi Sudoku solver dengan 4 varian algoritma buat makalah IF2211.

## Algoritma

- Pure Backtracking
- Backtracking + MRV
- Backtracking + Constraint Propagation (Naked Single & Hidden Single)
- Hybrid (MRV + Constraint Propagation)

## Kompilasi

```
g++ -std=c++20 -O2 -o sudoku_solver src/sudoku_solver.cpp
```

## Run

```
./sudoku_solver
```

Bakal nge-generate 150 puzzle, solve pake 4 algoritma, terus nampilin tabel perbandingan performa.

## Dataset

Ada 3 tingkat kesulitan:
- Easy (36-40 clue)
- Medium (30-35 clue)
- Hard (24-29 clue)

Masing-masing 50 puzzle, disimpan di `data/`.

## Author

Ega Luthfi Rais — 13524115
