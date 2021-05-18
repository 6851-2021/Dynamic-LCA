#include <list>
#include <string>
#include <iostream>
#include "lcaTree.hpp"
#include "generateRandTrees.hpp"
#include "lcaMultilevel.hpp"

/////////////////////
//// Example Run ////
/////////////////////

using std::cout;
using std::endl;
using std::vector;


void testStaticTree() {
    int numNodes = 100;
    for (int i = 0; i < 100; ++i)
    {
        treeAndNodes<ExpensiveTreeNode> randTree = generateStaticTree(numNodes);
        randTree.tree->preprocess();
     
        for (int j = 0; j < 100; ++j)
        {
            int nodeX = rand() % numNodes;
            int nodeY = rand() % numNodes;
            ExpensiveTreeNode* lca1 = ExpensiveTreeNode::lca(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            ExpensiveTreeNode* lca2 = ExpensiveTreeNode::naiveLca(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            cout << i << "." << j << ") LCA of " << nodeX << ", " << nodeY << ": "<< lca1->nodeId << " and " << lca2->nodeId << endl;
    
            assert(lca1 != NULL);
            assert(lca1 == lca2);
        }
        randTree.tree->deleteNode();
    }
    cout << "Passed 'static' tests" << endl;
}

void testAddNode() {
    int numNodes = 10;

    for (int i = 0; i < 10; ++i)
    {
        treeAndNodes<ExpensiveTreeNode> randTree = generateIncrementalTree(numNodes);
        // cout << "*** UNCOMPRESSED TREE ***" << endl;
        // randTree.tree->print();
        // cout << "*** COMPRESSED DYNAMIC TREE ***" << endl;
        // randTree.tree->printIntervals(0);
        // cout << "*** ANCESTORS ***" << endl;
        // randTree.tree->printAncestors(0);
        for (int j = 0; j < 100; ++j)
        {
            int nodeX = rand() % numNodes;
            int nodeY = rand() % numNodes;

            // cout << "Computing LCA of " << nodeX << " and " << nodeY << endl;
            ExpensiveTreeNode::caTuple cas1 = ExpensiveTreeNode::cas(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            ExpensiveTreeNode::caTuple cas2 = ExpensiveTreeNode::naiveCas(randTree.nodes[nodeX], randTree.nodes[nodeY]);

            // std::cout << i << "." << j << ") LCA of " << nodeX << ", " << nodeY << ": "<< cas1.lca->nodeId << " and " << cas2.lca->nodeId << std::endl;

            assert(cas1.lca  != NULL);
            assert(cas1.ca_x != NULL);
            assert(cas1.ca_y != NULL);
            assert(cas1.lca  == cas2.lca);
            assert(cas1.ca_x == cas2.ca_x);
            assert(cas1.ca_y == cas2.ca_y);
        }

        randTree.tree->deleteNode();
    }
    cout << "Passed 'expensive' tests" << endl;
}

void testMultilevel() {
    int numNodes = 1000;

    for (int i = 0; i < 100; ++i)
    {
        treeAndNodes<MultilevelTreeNode> randTree = generateIncrementalMultilevelTree(numNodes);
        // randTree.tree->print();
        for (int j = 0; j < 1000; ++j)
        {
            int nodeX = rand() % numNodes;
            int nodeY = rand() % numNodes;

            MultilevelTreeNode* lca1 = MultilevelTreeNode::lca(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            MultilevelTreeNode* lca2 = MultilevelTreeNode::naiveLca(randTree.nodes[nodeX], randTree.nodes[nodeY]);

            // std::cout << i << "." << j << ") LCA of " << nodeX << ", " << nodeY << ": "<< lca1->data << " and " << lca2->data << std::endl;

            assert(lca1  != NULL);
            assert(lca1  == lca2);
        }

        randTree.tree->deleteTree();
    }
    cout << "Passed 'multilevel' tests" << endl;
}
int main(){
    // testStaticTree();
    // testAddNode();
    testMultilevel();
    return 0;
}