# Sudoku Solver — Analisis Perbandingan Algoritma

Implementasi *Sudoku solver* dengan empat varian algoritma untuk keperluan analisis perbandingan performa pada makalah IF2211 Strategi Algoritma.

## Algoritma

### 1. Pure Backtracking
Algoritma *backtracking* standar tanpa heuristik. Mencari sel kosong pertama (baris utama, kolom utama), lalu mencoba angka 1–9 secara berurutan. Jika suatu angka valid, lanjut ke sel berikutnya secara rekursif. Jika tidak ada angka yang valid, *backtrack* ke sel sebelumnya. Sederhana namun tidak efisien untuk puzzle kompleks karena tidak mempertimbangkan informasi domain.

### 2. Backtracking + MRV (*Minimum Remaining Values*)
Menggunakan heuristik MRV dalam pemilihan variabel. Alih-alih mengambil sel kosong pertama, algoritma memilih sel dengan jumlah kandidat angka paling sedikit. Semakin sedikit pilihan, semakin tinggi probabilitas angka yang dipilih benar, sehingga memperkecil ukuran *search tree*.

### 3. Backtracking + *Constraint Propagation*
Menerapkan propagasi kendala *Naked Single* dan *Hidden Single* sebelum dan selama pencarian *backtracking*.
- **Naked Single**: Jika suatu sel hanya memiliki satu kandidat angka, angka tersebut langsung diisi.
- **Hidden Single**: Jika suatu angka hanya bisa ditempatkan di satu sel dalam suatu baris/kolom/subgrid, angka tersebut langsung diisi.

Propagasi ini mengurangi jumlah sel kosong sebelum rekursi dijalankan.

### 4. Hybrid (MRV + *Constraint Propagation*)
Menggabungkan MRV dengan *Constraint Propagation*. MRV digunakan untuk memilih variabel paling *constrained*, sementara *Constraint Propagation* (Naked Single & Hidden Single) dijalankan setelah setiap pengisian angka untuk mereduksi domain variabel lain. Pendekatan ini diharapkan paling efisien karena menggabungkan kelebihan kedua teknik.

## Kompilasi

```
g++ -std=c++20 -O2 -o sudoku_solver src/sudoku_solver.cpp
```

## Penggunaan

```
./sudoku_solver
```

Program akan men-generate 150 puzzle (50 Easy, 50 Medium, 50 Hard), menyelesaikannya dengan keempat algoritma, lalu menampilkan tabel perbandingan performa yang mencakup:
- Rata-rata waktu eksekusi (ms)
- Jumlah pemanggilan rekursif
- Jumlah *backtrack*
- Persentase puzzle yang berhasil diselesaikan

## Dataset

Puzzle dikelompokkan berdasarkan jumlah *clue*:

| Tingkat Kesulitan | Rentang Clue | Jumlah Puzzle |
|-------------------|--------------|---------------|
| Easy              | 36–40        | 50            |
| Medium            | 30–35        | 50            |
| Hard              | 24–29        | 50            |

File data disimpan dalam format CSV di direktori `data/`.

## Author

**Ega Luthfi Rais** — 13524115
