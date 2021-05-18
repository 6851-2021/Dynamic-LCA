#include "lcaTree.hpp"
#include "generateRandTrees.hpp"
#include <iostream>

using namespace std;

/*----------------------------------*/
/*   Private Function Declarations  */
/*----------------------------------*/

/*
 * Returns the tree corresponding to the input Prufer sequence
 * Pseudocode taken from:
 * https://en.wikipedia.org/wiki/Pr%C3%BCfer_sequence
 */
treeAndNodes<ExpensiveTreeNode> treeFromSeq(vector<int> seq);

/* Converts an adjacency list representation into an ExpensiveTreeNode*/
treeAndNodes<ExpensiveTreeNode> treeFromAdj(vector<vector<int>> adjList);

/* Given an adjacency list, runs DFS to add children to an ExpensiveTreeNode
   representation*/
void assignChildrenDFS(int currNode, vector<ExpensiveTreeNode*> nodes,
                        vector<vector<int>> adjList, vector<bool> discovered);
/*
 * Returns a representation of a sequence of "add_leaf" operations
 * that can be used to construct the input tree.
 * Note: This destroys the input and frees all memory associated with it.
 */
vector<vector<int>> randInsertionsFromTree(ExpensiveTreeNode* root);

/* Returns a random leaf in the input tree */
ExpensiveTreeNode* getRandLeaf(ExpensiveTreeNode* root);

/*----------------------------------*/
/*      Function Implementations    */
/*----------------------------------*/


///// Methods to implement getStaticTree //////

vector<vector<int>> randInsertionSeq(int numNodes) {
    treeAndNodes<ExpensiveTreeNode> randTree = generateStaticTree(numNodes);
    vector<vector<int>> insertions = randInsertionsFromTree(randTree.tree);
    return insertions;
}

treeAndNodes<ExpensiveTreeNode> generateStaticTree(int numNodes) {
    vector<int> seq;
    for (int i = 0; i < numNodes - 2; ++i)
    {
        seq.push_back(rand() % numNodes);
    }
    
    return treeFromSeq(seq);
}


treeAndNodes<ExpensiveTreeNode> treeFromSeq(vector<int> seq) {
    // Construct an adjacency list representation of the tree,
    // then convert that into an ExpensiveTreeNode tree.
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

    return (treeFromAdj(adjList));
}

treeAndNodes<ExpensiveTreeNode> treeFromAdj(vector<vector<int>> adjList) {
    int treeSize = adjList.size();

    vector<ExpensiveTreeNode*> nodes(treeSize);
    vector<bool> discovered(treeSize);
    for (int i = 0; i < treeSize; ++i)
    {
        nodes[i] = new ExpensiveTreeNode(to_string(i));
        discovered[i] = false;
    }

    assignChildrenDFS(0, nodes, adjList, discovered);

    treeAndNodes<ExpensiveTreeNode> toReturn;
    toReturn.tree = nodes[0];
    toReturn.nodes = nodes;
    return toReturn;
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

    root->deleteNode();

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

///// Generalizations of getStaticTree for other types of trees //////

treeAndNodes<ExpensiveTreeNode> generateIncrementalTree(int numNodes) {
    vector<vector<int>> sequences = randInsertionSeq(numNodes);
    vector<int> leaves = sequences[0];
    vector<int> parents = sequences[1];

    vector<ExpensiveTreeNode*> nodes(numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        nodes[i] = new ExpensiveTreeNode(std::to_string(i));
    }

    ExpensiveTreeNode* root = nodes[parents[parents.size() - 1]];
    root->preprocess();

    for (int i = leaves.size() - 1; i >= 0; --i) {
        nodes[parents[i]]->add_leaf(nodes[leaves[i]]);
    }
    
    treeAndNodes<ExpensiveTreeNode> toReturn;
    toReturn.tree = root;
    toReturn.nodes = nodes;
    return toReturn;
}

treeAndNodes<MultilevelTreeNode> generateIncrementalMultilevelTree(int numNodes) {
    std::vector<std::vector<int>> sequences = randInsertionSeq(numNodes);
    std::vector<int> leaves = sequences[0];
    std::vector<int> parents = sequences[1];

    std::vector<MultilevelTreeNode*> nodes(numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        nodes[i] = new MultilevelTreeNode(std::to_string(i));
    }

    MultilevelTreeNode* root = nodes[parents[parents.size() - 1]];

    for (int i = leaves.size() - 1; i >= 0; --i) {
        nodes[parents[i]]->add_leaf(nodes[leaves[i]]);
    }
    
    treeAndNodes<MultilevelTreeNode> toReturn;
    toReturn.tree = root;
    toReturn.nodes = nodes;
    return toReturn;
}
