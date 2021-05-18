# Dynamic LCA

This repo contains a partial implementation of [Gabow's data structure](https://arxiv.org/abs/1611.07055) for the dynamic lowest common ancestor (LCA) problem. Specifically, it contains code for a data structure that supports
- O(1) worse-case LCA queries
- O(log n) amortized insertion of leaves

See `writeup.pdf` for more details (including performance analysis).

## File Structure
- `lcaTree.hpp/cpp`: Defines the class `ExpensiveTreeNode`, which supports O(1) LCA queries and O(log^2 n) amortized insertion of leaves
- `lcaMultilevel.hpp/cpp`: Defines the class `MultilevelTreeNode`, which uses indirection to support O(1) LCA queries and O(log n) amortized insertion of leaves
- `demo.cpp`: A minimal example demonstrating how to construct a tree and run LCA queries on it
- `test.cpp`: Tests correctness of the LCA implementation
- `timingTest.cpp`: Tests efficiency of the LCA implementation
- `generateRandTree.hpp/cpp`: Defines a suite of functions used to generate random trees for testing
