#include "lcaTree.hpp"
#include "generateRandTrees.hpp"
#include <iostream>

using namespace std;
// Pseudocode taken from https://en.wikipedia.org/wiki/Pr%C3%BCfer_sequence

vector<vector<int>> randInsertionSeq(int numNodes) {
    treeAndNodes randTree = generateRandTree(numNodes);
    vector<vector<int>> insertions = randInsertionsFromTree(randTree.tree);
    return insertions;
}

// Destructively transforms a tree into a sequence of leaves to add
vector<vector<int>> randInsertionsFromTree(ExpensiveTreeNode* root) {
    vector<int> leafIds;
    vector<int> parentIds;

    while(root->uncompressedChildren.size() > 0) {
        ExpensiveTreeNode* leaf = getRandLeaf(root);
        leafIds.push_back(stoi(leaf->nodeId));
        if (leaf->uncompressedParent){
            parentIds.push_back(stoi(leaf->uncompressedParent->nodeId));
        }
        leaf->deleteNode();
    }

    // The root has no parent - just assume it has been initialized already

    return {leafIds, parentIds};
}

ExpensiveTreeNode* getRandLeaf(ExpensiveTreeNode* root) {
    ExpensiveTreeNode* currNode = root;
    std::list<ExpensiveTreeNode*> children = currNode->uncompressedChildren;
    while (children.size() > 0) {
        size_t childNum = rand() % children.size();
        std::list<ExpensiveTreeNode*>::iterator it = std::next(children.begin(), childNum);
        currNode = *it;
        children = currNode->uncompressedChildren;
    }
    return currNode;
}

void assignChildrenDFS(int currNode, vector<ExpensiveTreeNode*> nodes, vector<vector<int>> adjList, vector<bool> discovered) {
    discovered[currNode] = true;
    for (int childNode : adjList[currNode]) {
        if (!discovered[childNode]) {
            nodes[currNode]->addLeafNoPreprocessing(nodes[childNode]);
            assignChildrenDFS(childNode, nodes, adjList, discovered);
        }
    }
}

treeAndNodes treeFromAdj(vector<vector<int>> adjList, vector<int> seq) {
    int treeSize = adjList.size();

    vector<ExpensiveTreeNode*> nodes(treeSize);
    vector<bool> discovered(treeSize);
    for (int i = 0; i < treeSize; ++i)
    {
        nodes[i] = new ExpensiveTreeNode(to_string(i));
        discovered[i] = false;
    }

    assignChildrenDFS(0, nodes, adjList, discovered);

    treeAndNodes toReturn;
    toReturn.tree = nodes[0];
    toReturn.nodes = nodes;
    toReturn.seq = seq;
    return toReturn;
}

treeAndNodes treeFromSeq(vector<int> seq) {
    int treeSize = seq.size() + 2;
    vector<int> degree(treeSize);
    vector<vector<int>> adjList;
    adjList.resize(treeSize, vector<int>());

    for (int i = 0; i < treeSize; ++i)
    {
        degree[i] = 1;
    }

    for (int i : seq) {
        degree[i] += 1;
    }

    for (int i : seq) {
        for (int j = 0; j < treeSize; ++j)
        {
            if (degree[j] == 1) {
                //Insert edge [i,j]
                adjList[i].push_back(j);
                adjList[j].push_back(i);
                degree[i] += -1;
                degree[j] += -1;
                break;
            }
        }
    }

    int u = -1, v = -1;
    for (int i = 0; i < treeSize; ++i)
    {
        if (degree[i] == 1) {
            if (u == -1) {
                u = i;
            } else {
                v = i;
                break;
            }
        }
    }

    adjList[u].push_back(v);
    adjList[v].push_back(u);

    return (treeFromAdj(adjList, seq));
}

void deleteTree(ExpensiveTreeNode* node) {
    for (ExpensiveTreeNode* child : node->uncompressedChildren) {
        deleteTree(child);
    }
    delete node;
}


treeAndNodes generateRandTree(int numNodes) {
    vector<int> seq;
    for (int i = 0; i < numNodes - 2; ++i)
    {
        seq.push_back(rand() % numNodes);
    }
    
    return treeFromSeq(seq);
}