#include "lcaTree.hpp"
#include <vector>

typedef struct treeAndNodes {
    ExpensiveTreeNode* tree;
    std::vector<ExpensiveTreeNode*> nodes;
    std::vector<int> seq;
} treeAndNodes; //TODO: Figure out correct syntax (cf. caTuple)

treeAndNodes generateRandTree(int numNodes);
treeAndNodes treeFromSeq(std::vector<int> seq);
ExpensiveTreeNode* getRandLeaf(ExpensiveTreeNode* root);
std::vector<std::vector<int>> randInsertionsFromTree(ExpensiveTreeNode* root);
std::vector<std::vector<int>> randInsertionSeq(int numNodes);
void deleteTree(ExpensiveTreeNode* node);
//void assignChildrenDFS(int currNode, vector<ExpensiveTreeNode*> nodes, vector<vector<int>> adjList, vector<bool> discovered);