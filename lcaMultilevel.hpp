#ifndef LCAMULTILEVEL_H
#define LCAMULTILEVEL_H

#include <list>
#include <vector>
#include <string>
#include "lcaTree.hpp"

// Represents a node in T_2
class MultilevelTreeNode {
    public:
        static const int twoSubtreeMaxSize = 32; // Number of bits in a RAM word
        std::string data;
        int twoSubtreeSize;
        MultilevelTreeNode* twoSubtreeRoot; //ie. the root of x-hat
        ExpensiveTreeNode* summaryNode;
        
        unsigned int ancestorWord;
        std::vector<MultilevelTreeNode*> intToSubtreeNode; //Only initialized for roots of subtrees

        MultilevelTreeNode* parent;
        std::list<MultilevelTreeNode*> children;

        //TODO: constructor from existing tree - MultilevelTreeNode(...)
        MultilevelTreeNode(std::string id);
        void add_leaf(MultilevelTreeNode* leaf);
        static MultilevelTreeNode* lca(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY);
        static MultilevelTreeNode* naiveLca(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY);

        void print(int level = 0);
        void deleteTree();
    private:
        static MultilevelTreeNode* lcaWithinSubtree(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY);
};

#endif