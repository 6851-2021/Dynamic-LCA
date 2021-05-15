#include <list>
#include <string>
#include <iostream>
#include "lcaTree.hpp"
#include "generateRandTrees.hpp"

/////////////////////
//// Example Run ////
/////////////////////

void smallStaticLCA() {
    int n = 8;
    TreeNode root = TreeNode("0", n);
    TreeNode node1L = TreeNode("1-L", n);
    TreeNode node1R = TreeNode("1-R", n);
    TreeNode node2 = TreeNode("2", n);
    TreeNode node3 = TreeNode("3", n);
    TreeNode node4L = TreeNode("4-L", n);
    TreeNode node4R = TreeNode("4-R", n);
    TreeNode node5 = TreeNode("5", n);
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
    // TODO: check 1000, 50, 1000

    int numNodes = 1000;
    for (int i = 0; i < 50; ++i)
    {
        std::cout << "Checking tree " << i << std::endl;
        treeAndNodes randTree = generateRandTree(numNodes);//treeFromSeq(seq);
        std::cout << "    Generated tree " << i << std::endl;
        //randTree.tree->print();
        randTree.tree->preprocess();
        //randTree.tree->printIntervals(0);
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
int main(){
    //smallStaticLCA();
    testRandTree();
    return 0;
}