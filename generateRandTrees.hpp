#include "lcaTree.hpp"
#include "lcaMultilevel.hpp"
#include <vector>

using std::vector;

template <typename T>
struct treeAndNodes {
    T* tree;
    std::vector<T*> nodes;
};

/*
 * Returns a representation of a sequence of "add_leaf" operations
 * that construct a random tree.
 *
 * Output: [leafIds, parentIds] where leafIds and parentIds
 *    are both vectors of ints. Starting with a tree consisting
 *    of a single node with id parentIds[0], inserting the node
 *    leafIds[i] as a child of parentIds[i] will produce a random
 *    tree.
 */
vector<vector<int>> randInsertionSeq(int numNodes);

/*
 * Returns a random ExpensiveTreeNode tree (with no preprocessing)
 * generated from a random Prüfer sequence, along with a vector of
 * all nodes in the tree
 */
treeAndNodes<ExpensiveTreeNode> generateStaticTree(int numNodes);

/* Returns a random ExpensiveTreeNode tree, built using add_leaf */
treeAndNodes<ExpensiveTreeNode> generateIncrementalTree(int numNodes);

/* Returns a random MultilevelTreeNode, built using add_leaf */
treeAndNodes<MultilevelTreeNode> generateIncrementalMultilevelTree(int numNodes);


