# Cache Simulator in C

This project is a **cache memory simulator** written in C.  
It simulates cache behavior by processing memory traces and reporting cache performance.

---

## 🚀 Features
- Simulates cache accesses from trace files
- Tracks hits, misses, and miss rates
- Modular C implementation for clarity

---

## 🛠️ Technologies
- **Language:** C  
- **Tools:** GCC, VS Code  

---

## 📂 Files
- **Main.c** — source code for the cache simulator  
- **trace.txt** — sample memory access trace  
- *(Executable `Main.exe` is excluded, compile manually as shown below)*  

---

## ▶️ How to Compile & Run
1. Clone the repository:  
   ```bash
   git clone https://github.com/PedroBriz-22/A-Cache-Project-in-C.git
   cd A-Cache-Project-in-C

## Compile the code
2.gcc Main.c -o cache_sim
## Run the simulator with the provided trace file
3./cache_sim trace.txt
