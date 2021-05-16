#include <list>
#include <string>
#include <iostream>
#include "lcaTree.hpp"
#include "generateRandTrees.hpp"

/////////////////////
//// Example Run ////
/////////////////////

using std::cout;
using std::endl;

void smallStaticLCA() {
    TreeNode root = TreeNode("0");
    TreeNode node1L = TreeNode("1-L");
    TreeNode node1R = TreeNode("1-R");
    TreeNode node2 = TreeNode("2");
    TreeNode node3 = TreeNode("3");
    TreeNode node4L = TreeNode("4-L");
    TreeNode node4R = TreeNode("4-R");
    TreeNode node5 = TreeNode("5");
    root.addChild(&node1L);
    root.addChild(&node1R);
    node1R.addChild(&node2);
    node2.addChild(&node3);
    node3.addChild(&node4L);
    node3.addChild(&node4R);
    node4L.addChild(&node5);

    root.print();

    root.preprocess();

    std::cout << "\n--------\n\n";
    
    //root.printIntervals(0);
    //std::cout << "\n--------\n\n";
    //root.printAncestors(0);
    //std::cout << "\n--------\n\n";

    TreeNode::lca(&node1L, &node1R); //0
    TreeNode::lca(&node4R, &node4L); //3
    TreeNode::lca(&node5, &node4L); //4L
    TreeNode::lca(&node2, &node4L); //2
    TreeNode::lca(&node1L, &node4L); //0
}

void testRandTree() {
    //std::vector<int> seq = {1,1,1,1,6,5};
    //std::vector<int> seq = {0,0,0,0,5,4};

    int numNodes = 1000;
    for (int i = 0; i < 50; ++i)
    {
        std::cout << "Checking tree " << i << std::endl;
        treeAndNodes randTree = generateRandTree(numNodes);//treeFromSeq(seq);
        std::cout << "    Generated tree " << i << std::endl;
        // randTree.tree->print();
        randTree.tree->preprocess();
        // randTree.tree->printIntervals(0);
        //randTree.tree->printAncestors(0);

        for (int j = 0; j < 1000; ++j)
        {
            int nodeX = rand() % numNodes;
            int nodeY = rand() % numNodes;
            //std::cout << "Computing LCA of " << nodeX << ", " << nodeY << std::endl;
            TreeNode* lca1 = TreeNode::lca(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            TreeNode* lca2 = TreeNode::naiveLca(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            std::cout << i << "." << j << ") LCA of " << nodeX << ", " << nodeY << ": "<< lca1->nodeId << " and " << lca2->nodeId << std::endl;
    
            assert(lca1 != NULL);
            assert(lca1 == lca2);
        }

    
        deleteTree(randTree.tree);
    }
}

treeAndNodes generateIncrementalTree(int numNodes) {
    std::vector<std::vector<int>> sequences = randInsertionSeq(numNodes);
    std::vector<int> leaves = sequences[0];
    std::vector<int> parents = sequences[1];

    std::vector<TreeNode*> nodes(numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        nodes[i] = new TreeNode(std::to_string(i));
    }

    TreeNode* root = nodes[parents[parents.size() - 1]];
    root->preprocess();

    for (int i = leaves.size() - 1; i >= 0; --i) {
        nodes[parents[i]]->add_leaf(nodes[leaves[i]]);
    }
    
    treeAndNodes toReturn;
    toReturn.tree = root;
    toReturn.nodes = nodes;
    return toReturn;
}

std::string getId1(TreeNode* node) {
    if (node) {return (node->nodeId);}
    else {return "NULL";}
}

void testAddNode() {
    int numNodes = 10000;

    for (int i = 0; i < 50; ++i)
    {
        treeAndNodes randTree = generateIncrementalTree(numNodes);
        // cout << "*** UNCOMPRESSED TREE ***" << endl;
        // randTree.tree->print();
        // cout << "*** COMPRESSED DYNAMIC TREE ***" << endl;
        // randTree.tree->printIntervals(0);
        // cout << "*** ANCESTORS ***" << endl;
        // randTree.tree->printAncestors(0);
        for (int j = 0; j < 10000; ++j)
        {
            int nodeX = rand() % numNodes;
            int nodeY = rand() % numNodes;

            // cout << "Computing LCA of " << nodeX << " and " << nodeY << endl;
            TreeNode::caTuple cas1 = TreeNode::cas(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            TreeNode::caTuple cas2 = TreeNode::naiveCas(randTree.nodes[nodeX], randTree.nodes[nodeY]);

            std::cout << i << "." << j << ") LCA of " << nodeX << ", " << nodeY << ": "<< cas1.lca->nodeId << " and " << cas2.lca->nodeId << std::endl;
            std::cout << "    CA_X: "<< cas1.ca_x->nodeId << " and " << cas2.ca_x->nodeId << std::endl;
            std::cout << "    CA_Y: "<< getId1(cas1.ca_y) << " and " << getId1(cas2.ca_y) << std::endl;
    
            assert(cas1.lca  != NULL);
            assert(cas1.ca_x != NULL);
            assert(cas1.ca_y != NULL);
            assert(cas1.lca  == cas2.lca);
            assert(cas1.ca_x == cas2.ca_x);
            assert(cas1.ca_y == cas2.ca_y);
        }

        deleteTree(randTree.tree);
    }    
}

int main(){
    //smallStaticLCA();
    // testRandTree();
    testAddNode();
    return 0;
}