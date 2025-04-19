# 🚀 UltraMalloc - Advanced Memory Allocation Library

**Project Name:** UltraMalloc  
**Version:** 2.1.0  
**License:** MIT  

## 📖 Overview

UltraMalloc is a high-performance memory allocation library that combines cutting-edge allocation algorithms with comprehensive debugging and visualization tools. Designed for systems programming where memory efficiency is critical, it offers significant improvements over standard allocators while maintaining full compatibility.

## ✨ Key Features

### Memory Management Core
- Hybrid best-fit/next-fit allocation strategy
- Automatic block coalescing with O(1) merging
- Low fragmentation design (<15% in most workloads)
- Thread-safe implementation with optimized locking
- Small object optimization (specialized allocator for ≤256B)

### Advanced Tooling
- **Real-time memory visualization**
  - ASCII-based heap maps
  - Allocation heatmaps
  - Fragmentation analysis
- **Comprehensive tracking**
  - Full call stack recording
  - Allocation timestamps
  - Memory type tagging
- **Debugging Support**
  - Buffer overflow detection
  - Use-after-free protection
  - Memory leak reports

## 📊 Performance Characteristics

| Operation | UltraMalloc | System malloc |
|-----------|------------|--------------|
| malloc()  | 8.2M ops/s | 6.9M ops/s   |
| free()    | 9.1M ops/s | 7.3M ops/s   |
| realloc() | 5.7M ops/s | 4.2M ops/s   |

*Benchmarks performed on Intel i9-13900K with 32GB DDR5*

## 🛠 Installation

```bash
git clone https://github.com/iyoramu/ultramalloc.git
cd ultramalloc
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

## 📝 Basic Usage

```c
#include <ultramalloc.h>

int main() {
    // Standard API (drop-in replacement)
    int* data = malloc(1024 * sizeof(int));
    
    // Extended features
    malloc_enable_tracing();  // Start allocation tracking
    
    // Get real-time statistics
    malloc_stats();
    
    // Visualize heap
    malloc_visualize();
    
    free(data);
    return 0;
}
```

## 🔧 Advanced Usage

### Custom Allocation Strategies
```c
// Set allocation strategy
malloc_set_strategy(MALLOC_STRATEGY_LOWFRAG);

// Allocate with specific flags
void* ptr = malloc_ex(1024, MALLOC_ZERO_INIT | MALLOC_GUARD_PAGES);
```

### Memory Analysis
```c
// Generate full memory report
malloc_report("memory_profile.json");

// Check for leaks at specific point
malloc_leak_check();
```

## 📚 Documentation

Full documentation available at:  
[https://github.com/iyoramu/ultramalloc/wiki](https://github.com/iyoramu/ultramalloc/wiki)

## 📦 Integration Options

1. **Full Replacement**:
   ```c
   #define malloc ultramalloc
   #define free ultrafree
   ```

2. **Selective Usage**:
   ```c
   void* ptr = ultramalloc(1024);
   ultrafree(ptr);
   ```

3. **LD_PRELOAD**:
   ```bash
   LD_PRELOAD=/path/to/libultramalloc.so ./your_program
   ```

## 🌍 Supported Platforms

- Linux (glibc and musl)
- macOS (x86_64 and ARM)
- Windows (via WSL2)
- Embedded systems (RTOS support)

## 🧪 Testing

```bash
# Run comprehensive test suite
make test

# Benchmark against system allocator
./benchmarks/compare_with_system
```

## 📜 License

MIT License - Free for academic and commercial use

## 🤝 Contributing

We welcome contributions! Please see CONTRIBUTING.md for guidelines.

## 📧 Contact

For questions or support: yirutabyose@gmail.com
