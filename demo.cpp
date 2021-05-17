#include "lcaMultilevel.hpp"
#include <iostream>

int main(){
    MultilevelTreeNode* root = new MultilevelTreeNode("root");
    MultilevelTreeNode* node1 = new MultilevelTreeNode("node1");
    MultilevelTreeNode* node2 = new MultilevelTreeNode("node2");

    root->add_leaf(node1);
    root->add_leaf(node2);

    root->print();

    MultilevelTreeNode* lcaNode = MultilevelTreeNode::lca(node1, node2);
    std::cout << "LCA of node1 and node2: " << lcaNode->data << std::endl;

    root->deleteTree();

    return 0;
}