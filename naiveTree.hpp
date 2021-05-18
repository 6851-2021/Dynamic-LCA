#ifndef NAIVE_H
#define NAIVE_H

#include <list>
#include <string>

/*
 * MultilevelTreeNode
 * Represents the full tree partitioned into "2-subtrees" (for indirection)
 *   Each 2-subtree has log n nodes (which we can exaggerate to the number of
 *   bits in a RAM word)
 */
class NaiveTreeNode {
    public:
        /* Standard Tree Variables */
        std::string data;
        NaiveTreeNode* parent;
        std::list<NaiveTreeNode*> children;

        /* Standard Tree Operations */
        NaiveTreeNode(std::string id);
        void add_leaf(NaiveTreeNode* leaf);
        void deleteNode();

        /* LCA Operations */
        static NaiveTreeNode* lca(NaiveTreeNode* nodeX, NaiveTreeNode* nodeY);
};

#endif