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
        cout << "-------ADDING leaf " << leaves[i] << endl;
        nodes[parents[i]]->add_leaf(nodes[leaves[i]]);
    }
    
    cout << "000000 FINISHED CREATING TREE 0000000" << endl;
    // root->print();
    treeAndNodes toReturn;
    toReturn.tree = root;
    toReturn.nodes = nodes;
    return toReturn;
}

void testAddNode() {
    int numNodes = 1000;

    for (int i = 0; i < 50; ++i)
    {
        treeAndNodes randTree = generateIncrementalTree(numNodes);
        // cout << "*** UNCOMPRESSED TREE ***" << endl;
        // randTree.tree->print();
        // cout << "*** COMPRESSED DYNAMIC TREE ***" << endl;
        // randTree.tree->printIntervals(0);
        // cout << "*** ANCESTORS ***" << endl;
        // randTree.tree->printAncestors(0);
        for (int j = 0; j < 1000; ++j)
        {
            int nodeX = rand() % numNodes;
            int nodeY = rand() % numNodes;

            // cout << "***********" << endl;
            // cout << "*** UNCOMPRESSED TREE ***" << endl;
            // randTree.tree->print();
            // cout << "*** COMPRESSED DYNAMIC TREE ***" << endl;
            // randTree.tree->printIntervals(0);
            // cout << "*** ANCESTORS ***" << endl;
            // randTree.tree->printAncestors(0);
            // cout << "***********" << endl;

            cout << "Computing LCA of " << nodeX << " and " << nodeY << endl;
            TreeNode* lca1 = TreeNode::lca(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            TreeNode* lca2 = TreeNode::naiveLca(randTree.nodes[nodeX], randTree.nodes[nodeY]);
            std::cout << i << "." << j << ") LCA of " << nodeX << ", " << nodeY << ": "<< lca1->nodeId << " and " << lca2->nodeId << std::endl;
    
            assert(lca1 != NULL);
            assert(lca1 == lca2);
        }

        deleteTree(randTree.tree);
    }

    // cout << "Leaves: ";
    // for (int leaf : leaves) {
    //     cout << leaf << ", ";
    // }
    // cout << endl;

    // cout << "Parents: ";
    // for (int parent : parents) {
    //     cout << parent << ", ";
    // }
    // cout << endl;
    // assert(leaves.size() == parents.size());
    
}

void smallTestAddNode() {
    //std::vector<int> seq = {1,1,1,1,6,5};
    //std::vector<int> seq = {0,0,0,0,5,4};

    int numNodes = 6;
    for (int i = 0; i < 1; ++i)
    {
        std::cout << "Checking tree " << i << std::endl;
        treeAndNodes randTree = generateRandTree(numNodes);//treeFromSeq(seq);
        std::cout << "    Generated tree " << i << std::endl;
        randTree.tree->print();
        randTree.tree->preprocess();
        randTree.tree->printIntervals(0);
        //randTree.tree->printAncestors(0);

        TreeNode* newnode = new TreeNode("NEWNODE");
        randTree.nodes[0]->add_leaf(newnode);

        for (int j = 0; j < 1; ++j)
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


        TreeNode* leaf = getRandLeaf(randTree.tree);
        cout << "Randomly chose leaf: " << leaf->nodeId << endl;
        leaf = getRandLeaf(randTree.tree);
        cout << "Randomly chose leaf: " << leaf->nodeId << endl;
        leaf = getRandLeaf(randTree.tree);
        cout << "Randomly chose leaf: " << leaf->nodeId << endl;

    
        deleteTree(randTree.tree);
    }
}
int main(){
    //smallStaticLCA();
    // testRandTree();
    testAddNode();
    return 0;
}