#include "lcaTree.hpp"
#include <vector>

typedef struct treeAndNodes {
    TreeNode* tree;
    std::vector<TreeNode*> nodes;
} treeAndNodes; //TODO: Figure out correct syntax (cf. caTuple)

treeAndNodes generateRandTree(int numNodes);
treeAndNodes treeFromSeq(std::vector<int> seq);
void deleteTree(TreeNode* node);
//void assignChildrenDFS(int currNode, vector<TreeNode*> nodes, vector<vector<int>> adjList, vector<bool> discovered);