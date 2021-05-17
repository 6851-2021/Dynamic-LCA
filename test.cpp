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

struct multilevelTreeAndNodes {
    MultilevelTreeNode* tree;
    std::vector<MultilevelTreeNode*> nodes;
};


void testStaticTree() {
    int numNodes = 100;
    for (int i = 0; i < 50; ++i)
    {
        treeAndNodes randTree = generateRandTree(numNodes);//treeFromSeq(seq);
        randTree.tree->preprocess();
     
        for (int j = 0; j < 100; ++j)
        {
            int nodeX = rand() % numNodes;
            int nodeY = rand() % numNodes;
            //std::cout << "Computing LCA of " << nodeX << ", " << nodeY << std::endl;
            ExpensiveTreeNode* lca1 = ExpensiveTreeNode::lca(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            ExpensiveTreeNode* lca2 = ExpensiveTreeNode::naiveLca(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            // std::cout << i << "." << j << ") LCA of " << nodeX << ", " << nodeY << ": "<< lca1->nodeId << " and " << lca2->nodeId << std::endl;
    
            assert(lca1 != NULL);
            assert(lca1 == lca2);
        }

    
        deleteTree(randTree.tree);
    }
    cout << "Passed 'static' tests" << endl;
}

treeAndNodes generateIncrementalTree(int numNodes) {
    std::vector<std::vector<int>> sequences = randInsertionSeq(numNodes);
    std::vector<int> leaves = sequences[0];
    std::vector<int> parents = sequences[1];

    std::vector<ExpensiveTreeNode*> nodes(numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        nodes[i] = new ExpensiveTreeNode(std::to_string(i));
    }

    ExpensiveTreeNode* root = nodes[parents[parents.size() - 1]];
    root->preprocess();

    for (int i = leaves.size() - 1; i >= 0; --i) {
        nodes[parents[i]]->add_leaf(nodes[leaves[i]]);
    }
    
    treeAndNodes toReturn;
    toReturn.tree = root;
    toReturn.nodes = nodes;
    return toReturn;
}

multilevelTreeAndNodes generateIncrementalMultilevelTree(int numNodes) {
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
    
    multilevelTreeAndNodes toReturn;
    toReturn.tree = root;
    toReturn.nodes = nodes;
    return toReturn;
}

std::string getId1(ExpensiveTreeNode* node) {
    if (node) {return (node->nodeId);}
    else {return "NULL";}
}

void testAddNode() {
    int numNodes = 100;

    for (int i = 0; i < 50; ++i)
    {
        treeAndNodes randTree = generateIncrementalTree(numNodes);
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
            // std::cout << "    CA_X: "<< cas1.ca_x->nodeId << " and " << cas2.ca_x->nodeId << std::endl;
            // std::cout << "    CA_Y: "<< getId1(cas1.ca_y) << " and " << getId1(cas2.ca_y) << std::endl;
    
            assert(cas1.lca  != NULL);
            assert(cas1.ca_x != NULL);
            assert(cas1.ca_y != NULL);
            assert(cas1.lca  == cas2.lca);
            assert(cas1.ca_x == cas2.ca_x);
            assert(cas1.ca_y == cas2.ca_y);
        }

        deleteTree(randTree.tree);
    }
    cout << "Passed 'expensive' tests" << endl;
}

void testMultilevelSmall() {
    MultilevelTreeNode node1 = MultilevelTreeNode("1");
    MultilevelTreeNode node2 = MultilevelTreeNode("2");
    MultilevelTreeNode node3 = MultilevelTreeNode("3");
    MultilevelTreeNode node4 = MultilevelTreeNode("4");
    node1.add_leaf(&node2);
    node1.add_leaf(&node3);
    node1.add_leaf(&node4);

    node1.print();

    cout << "Passed 'small multilevel' tests" << endl;
}

void testMultilevel() {
    int numNodes = 100;

    for (int i = 0; i < 100; ++i)
    {
        multilevelTreeAndNodes randTree = generateIncrementalMultilevelTree(numNodes);
        randTree.tree->print();
        for (int j = 0; j < 1000; ++j)
        {
            int nodeX = rand() % numNodes;
            int nodeY = rand() % numNodes;

            MultilevelTreeNode* lca1 = MultilevelTreeNode::lca(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            MultilevelTreeNode* lca2 = MultilevelTreeNode::naiveLca(randTree.nodes[nodeX], randTree.nodes[nodeY]);

            std::cout << i << "." << j << ") LCA of " << nodeX << ", " << nodeY << ": "<< lca1->data << " and " << lca2->data << std::endl;
            // std::cout << "    CA_X: "<< cas1.ca_x->nodeId << " and " << cas2.ca_x->nodeId << std::endl;
            // std::cout << "    CA_Y: "<< getId1(cas1.ca_y) << " and " << getId1(cas2.ca_y) << std::endl;
    
            assert(lca1  != NULL);
            assert(lca1  == lca2);
        }

        randTree.tree->deleteTree();
    }
    cout << "Passed 'multilevel' tests" << endl;
}
int main(){
    //smallStaticLCA();
    // testRandTree();
    testStaticTree();
    testAddNode();
    testMultilevelSmall();
    testMultilevel();
    return 0;
}