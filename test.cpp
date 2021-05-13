#include <list>
#include <string>
#include <iostream>
#include "lcaTree.hpp"

/////////////////////
//// Example Run ////
/////////////////////

int main(){
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

    std::cout << "\n--------\n\n";
    root.assignSubtreeSizes();
    root.assignApex();

    root.print();
    std::cout << "\n--------\n\n";

    root.compressTree();
    root.print();
    std::cout << "\n--------\n\n";

    root.assignIntervals();
    root.printIntervals(0);
    std::cout << "\n--------\n\n";

    root.fillAllAncestors();
    root.printAncestors(0);
    return 0;
}