
#  SearchEngine


[Chinese](README.zh.md) | **中文说明**

A **high-performance multithreaded search engine server framework** built with **C++11** on **Linux**.  
The project consists of **offline index building** and **online search services**, supporting Chinese word segmentation, pagination, and keyword search.  
It adopts a **Reactor pattern + thread pool** architecture to achieve efficient concurrency and scalable performance.

---

##  Features

- **Offline Index Builder**
  - Generates and maintains inverted index files  
  - Performs Chinese text segmentation and preprocessing  


- **Online Search Service**
  - Event-driven **Reactor** architecture for concurrent I/O  
  - Multithreaded query handling with task scheduling  
  - Pagination and keyword highlighting support  
  - Optimized for Chinese language search  

- **System Highlights**
  - Lightweight, modular, and highly efficient  
  - Clear separation between online/offline modules  


---

##  Tech Stack

| Category | Technology |
|-----------|-------------|
| Language | C++11 |
| Platform | Linux |
| Concurrency | Reactor Pattern + Thread Pool |
| Tokenizer | CppJieba |
| Database | MySQL |
| Build Tool | CMake |
| Network | epoll / socket API |

---

##  Module Overview

###  Offline Module
- Reads raw data from MySQL  
- Performs word segmentation and data cleaning  
- Builds an inverted index and mapping tables  
- Supports incremental or scheduled index updates  

###  Online Module
- Handles client connections using the Reactor model  
- Uses a thread pool for concurrent query processing  
- Supports keyword and Chinese-language search  


---

##  Core Design Concepts

- **Reactor Model**: separates I/O multiplexing from request handling  
- **Thread Pool**: improves performance by reusing worker threads   
- **Pagination System**: retrieves results efficiently by offset and page size  
- **Chinese Segmentation**: integrates CppJieba for higher accuracy in tokenization  

---

##  Quick Start

```bash
# Build the project
mkdir build && cd build
cmake ..
make

# Build offline index
./SearchEngine --build-index

# Run online search service
./SearchEngine --serve

---

