# Hybrid MPI + OpenMP Parallel System

## 📌 Project Overview
This project demonstrates a **Hybrid Parallel Programming Model** using:

- **MPI (Message Passing Interface)** → Process-level parallelism
- **OpenMP** → Thread-level parallelism

The system runs **10 parallel processes** where a **master process** distributes tasks to different worker processes. Each worker performs a specific computation using OpenMP, then returns results to the master.

---

## 🧠 Parallel Architecture

| Rank | Role | Operation |
|------|------|-----------|
| 0 | Master Process | Sends tasks & collects results |
| 1 | Integer Process | Computes factorial using OpenMP |
| 2 | String Process | Reverse string + count vowels |
| 3 | File Process | Split file into even/odd lines |
| 4 | Matrix Manager | Distributes matrix work |
| 5–9 | Matrix Workers | Parallel matrix addition |

---

## ⚙️ Features

- Hybrid MPI + OpenMP programming
- Parallel factorial computation
- Parallel string processing
- Parallel file handling
- Distributed matrix addition (50×50 matrices)
- Master-worker communication model

---

## 🛠️ Requirements

- GCC Compiler
- OpenMPI
- OpenMP support
- Linux / WSL / Ubuntu environment

Install dependencies (Ubuntu/WSL):

```bash
sudo apt update
sudo apt install openmpi-bin libopenmpi-dev

---

## 🔨 Compilation

```bash
mpicc -fopenmp Building_a_Parallel_System.c -o program

---

## ▶️ Run Program

The program requires 10 MPI processes.

```bash
mpirun --oversubscribe -np 10 ./program