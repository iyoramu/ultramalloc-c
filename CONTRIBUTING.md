# Contributing to ultramalloc-c

Thank you for considering contributing to ultramalloc-c! This document outlines how to contribute effectively to my high-performance memory allocator.

## 📚 Before You Start

1. **Read the [Wiki](https://github.com/iyoramu/ultramalloc-c/wiki)** - Contains essential documentation:
   - [Architecture Overview](https://github.com/iyoramu/ultramalloc-c/wiki/Architecture)
   - [Design Principles](https://github.com/iyoramu/ultramalloc-c/wiki/Design-Principles)
   - [Performance Metrics](https://github.com/iyoramu/ultramalloc-c/wiki/Performance-Metrics)

2. **Review Open Issues** - Check for existing reports of your idea/bug

## 🛠 Development Setup

### Quickstart
```bash
git clone https://github.com/iyoramu/ultramalloc-c.git
cd ultramalloc-c
./configure --enable-debug
make && make test
```

### Advanced Setup
See detailed instructions in the [Development Environment Wiki](https://github.com/iyoramu/ultramalloc-c/wiki/Development-Environment)

## 🧩 Contribution Workflow

1. **Create a Branch** from `main`:
   ```bash
   git checkout -b type/description
   ```
   Branch types:
   - `feat/`: New features
   - `fix/`: Bug fixes
   - `perf/`: Performance improvements
   - `doc/`: Documentation updates

2. **Implement Changes** following my [Coding Standards](https://github.com/iyoramu/ultramalloc-c/wiki/Coding-Standards)

3. **Test Thoroughly**:
   ```bash
   make check-all  # Runs unit, integration, and memory tests
   ```

4. **Update Documentation**:
   - Modify relevant Wiki pages
   - Update inline Doxygen comments

5. **Submit PR** with:
   - Description of changes
   - Benchmark results if applicable
   - Reference to related issues

## 🏆 Good First Issues

Label | Description | Wiki Reference
----- | ----------- | -------------
`good-first-issue` | Small, well-defined tasks | [Starter Projects](https://github.com/iyoramu/ultramalloc-c/wiki/Starter-Projects)
`documentation` | Docs improvements | [Documentation Guide](https://github.com/iyoramu/ultramalloc-c/wiki/Documentation-Guide)

## 🚨 Critical Contribution Areas

1. **Memory Allocators**:
   - [Small Block Allocator](https://github.com/iyoramu/ultramalloc-c/wiki/Small-Block-Allocator)
   - [Large Object Heap](https://github.com/iyoramu/ultramalloc-c/wiki/Large-Object-Heap)

2. **Platform Support**:
   - [Windows Porting](https://github.com/iyoramu/ultramalloc-c/wiki/Windows-Porting)
   - [ARM Optimization](https://github.com/iyoramu/ultramalloc-c/wiki/ARM-Optimization)

3. **Debug Tools**:
   - [Heap Visualizer](https://github.com/iyoramu/ultramalloc-c/wiki/Heap-Visualizer)
   - [Leak Detector](https://github.com/iyoramu/ultramalloc-c/wiki/Leak-Detector)

## 🔍 Review Process

1. Automated CI checks (builds, tests, benchmarks)
2. Maintainer review within 3 business days
3. Possible request for:
   - Additional tests
   - Performance measurements
   - Documentation updates

See [Review Criteria](https://github.com/iyoramu/ultramalloc-c/wiki/Review-Criteria) for details

## 🏅 Recognition

All significant contributions will be:
1. Listed in release notes
2. Acknowledged in [Contributors Hall of Fame](https://github.com/iyoramu/ultramalloc-c/wiki/Contributors)
3. Eligible for maintainer nomination after 3 accepted PRs

---

For any questions, please consult the [Contributor FAQ](https://github.com/iyoramu/ultramalloc-c/wiki/Contributor-FAQ) in my Wiki.
