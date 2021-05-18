#ifndef LCAMULTILEVEL_H
#define LCAMULTILEVEL_H

#include <list>
#include <vector>
#include <string>
#include "lcaTree.hpp"

/*
 * MultilevelTreeNode
 * Represents the full tree partitioned into "2-subtrees" (for indirection)
 *   Each 2-subtree has log n nodes (which we can exaggerate to the number of
 *   bits in a RAM word)
 */
class MultilevelTreeNode {
    public:
        static const int twoSubtreeMaxSize = 64;
        
        /* Standard Tree Variables */
        std::string data;
        MultilevelTreeNode* parent;
        std::list<MultilevelTreeNode*> children;

        /* Standard Tree Operations */
        MultilevelTreeNode(std::string id);
        void print(int level = 0, bool details = false);
        void deleteTree();

        /* Dynamic LCA */
        void add_leaf(MultilevelTreeNode* leaf);
        static MultilevelTreeNode* lca(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY);
        static MultilevelTreeNode* naiveLca(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY);

    private:        
        /* Variables for 2-subtrees */
        MultilevelTreeNode* twoSubtreeRoot; // Root of this node's 2-subtree
        int twoSubtreeSize; // Only set for the root of a 2-subtree
        ExpensiveTreeNode* summaryNode; // Only set for the root of a 2-subtree
        
        /*-------------------------*/
        /*  LCA within a 2-subtree */
        /*-------------------------*/

        /* Each node in a 2-subtree is assigned an integer. 
         * This vector gives the correspondence between integers and nodes.
         * It is only set for the root of a 2-subtree. */
        std::vector<MultilevelTreeNode*> intToSubtreeNode;

        /* The ith bit is 1 iff the node with ID i is an ancestor */
        unsigned long long ancestorWord;

        /* Given two nodes in the same 2-subtree, return their LCA */
        static MultilevelTreeNode* lcaWithinSubtree(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY);


};

#endif