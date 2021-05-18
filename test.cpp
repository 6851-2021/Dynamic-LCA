#include <list>
#include <string>
#include <iostream>
#include "lcaTree.hpp"
#include "generateRandTrees.hpp"
#include "lcaMultilevel.hpp"

/*---------------------------*/
/*   Tests for Correctness   */
/*---------------------------*/

/* Each test generates a random sequence of "add_leaf" operations
 * using generateRandTrees.hpp. A tree is constructed based on this sequence
 * in one of three ways:
 * 1. Static: The tree is constructed and then preprocessed after
 * 2. Expensive Dynamic: The tree is built using ExpensiveTreeNode::add_leaf
 *    operations so that the data structure is maintained during construction
 * 3. Multilevel Dynamic: The tree is built using MultilevelTreeNode::add_leaf
 *    operations (similar to #2, but with indirection)
 *
 * A random sequence of LCA queries is generated, and the result of
 * of Gabow's O(1) LCA algorithm is compared to the result of a naive
 * linear-time LCA algorithm.
 */

using std::cout;
using std::endl;
using std::vector;


void testStaticTree() {
    int numNodes = 1000;
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

void testExpensiveIncremental() {
    int numNodes = 1000;

    for (int i = 0; i < 100; ++i)
    {
        treeAndNodes<ExpensiveTreeNode> randTree = generateIncrementalTree(numNodes);
        for (int j = 0; j < 100; ++j)
        {
            int nodeX = rand() % numNodes;
            int nodeY = rand() % numNodes;

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

        randTree.tree->deleteNode();
    }
    cout << "Passed 'multilevel' tests" << endl;
}
int main(){
    testStaticTree();
    testExpensiveIncremental();
    testMultilevel();
    return 0;
}