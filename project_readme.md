# CPU-Optimized Parallel Image Processing: Case Study 1 (Image Blurring)

A high-performance, multi-threaded C application implementing **Case Study 1 (Image Blurring)** from parallel computing workflows. Engineered specifically for standard consumer laptops (Intel i7 CPU with integrated Iris Xe graphics) **without requiring an NVIDIA GPU or CUDA**.

---

## 👥 Team 4: Project Role Distribution

| Team Member | Engineering Role | Key Responsibilities |
| :--- | :--- | :--- |
| **Member 1** | **Core Algorithm & Memory Architect** | Designed mathematical convolution and box-blur sliding window logic; managed row-column boundary checks and memory buffers in pure C. |
| **Member 2** | **Parallel Computing & Hardware Engineer** | Adapted parallelism for the Intel i7 architecture using **OpenMP** directives (`#pragma omp parallel for`) to saturate multi-core CPU threads. |
| **Member 3** | **I/O & Graphics Window Specialist** | Implemented raw binary BMP file stream parsing and configured native OS process spawning for instant local window visualization. |
| **Member 4** | **Benchmarking & Documentation Lead** | Profiled execution time metrics across resolutions, verified multi-core scaling, and compiled technical documentation & presentation guides. |

---

## 🚀 Key Technical Features
* **No GPU Required:** Bypasses NVIDIA CUDA hardware requirements by leveraging CPU multi-threading via OpenMP.
* **Low-Level C Implementation:** Manages raw pixel arrays and BMP headers directly without relying on heavy external graphics frameworks.
* **Native OS Visualization:** Automatically triggers a native operating system image viewer window upon completion.

---

## 🛠️ Prerequisites & Setup (Windows / MSYS2 / GCC)

1. **Install GCC with OpenMP Support:**
   Ensure you have MSYS2 installed with the toolchain (`mingw-w64-ucrt-x86_64-toolchain`).
2. **Configure System Path:**
   Add your compiler's binary path (e.g., `C:\msys64\ucrt64\bin`) to your Windows Environment Variables (`Path`).
3. **Verify Installation:**
   Open a fresh terminal and check your compiler version:
   ```bash
   gcc --version
   ```

---

## 📦 How to Compile and Run

1. Place a 24-bit uncompressed `.bmp` image in the project root directory and name it **`input.bmp`**.
2. Compile the program with OpenMP enabled:
   ```bash
   gcc -fopenmp image_blur.c -o image_blur
   ```
3. Run the executable:
   ```bash
   ./image_blur.exe
   ```

---

## 📊 Performance Workflow & Pipeline
1. **Memory Allocation:** Host memory is allocated for input and output image buffers.
2. **File Stream Ingestion:** Binary BMP header and pixel bytes are read sequentially.
3. **Parallel Kernel Execution:** OpenMP distributes pixel neighborhood averaging loops across available Intel i7 CPU cores dynamically.
4. **Result Persistence:** Processed pixels are written back to an output BMP file (`output_blurred.bmp`).
5. **Native Display:** The OS launches a native preview window to display the filtered output instantly.