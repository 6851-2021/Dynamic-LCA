#include "lcaTree.hpp"
#include <iostream>

//////////////////////////
// LCA for Static Trees //
//////////////////////////

TreeNode TreeNode::lca(TreeNode nodeA, TreeNode nodeB) {
    // TODO: Answer queries
    return NULL;
}

void TreeNode::assignSubtreeSizes() {
    // TODO: Calculate subtree sizes
}

void TreeNode::assignIntervals() {
    // TODO: Calculate fat preorder numbering
}


///////////////////////////
// Basic Tree Operations //
///////////////////////////

TreeNode::TreeNode(int id) {
    nodeId = id;
}

void TreeNode::print() {
    this->print(0);
}

void TreeNode::print(int level) {
    for (int i = 0; i < level; i++){
        std::cout << "    ";
    }

    std::cout << "Node " << nodeId << std::endl;
    for (TreeNode child : children) {
        child.print(level + 1);
    }
}

/////////////////////
//// Example Run ////
/////////////////////

int main(){
    TreeNode root = TreeNode(0);
    TreeNode nodeA = TreeNode(1);
    TreeNode nodeB = TreeNode(2);
    root.children.push_back(nodeA);
    root.children.push_back(nodeB);

    root.print();
    return 0;
}