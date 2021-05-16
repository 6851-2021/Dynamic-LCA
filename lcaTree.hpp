#ifndef LCATREE_H
#define LCATREE_H

#include <list>
#include <string>
#include <vector>

class ExpensiveTreeNode {
     public:
        struct caTuple {
            ExpensiveTreeNode* lca;
            ExpensiveTreeNode* ca_x;
            ExpensiveTreeNode* ca_y;
        };

        static constexpr float beta = 10.0/7.0;
        static const int e = 4;
        static const int c = 5;
        static constexpr float alpha = 6.0/5.0;

        //Static: beta = 2, e = 2, c = 4
        //typedef int (ExpensiveTreeNode::*sigmaType)();

        std::string nodeId;
        std::list<ExpensiveTreeNode*> children;
        ExpensiveTreeNode* parent;
        ExpensiveTreeNode* root; // Used to determine number of nodes in the tree (to determine size of ancestor tables)
        int subtreeSize; // subtreeSize used in current fat preordering
        int dynamicSubtreeSize; // dynamically updated subtreeSize

        bool isApex;
        ExpensiveTreeNode* heavyChild;
        bool isPreprocessed;

        std::list<ExpensiveTreeNode*> uncompressedChildren;
        ExpensiveTreeNode* uncompressedParent;
        int uncompressedLevel;
        

        // variables for the fat preorder numbering 
        long long start;
        long long end;
        long long startBuffered;
        long long endBuffered; //last integer in the buffered interval (inclusive)
        long long largestChildEndBuffer;

        std::vector<ExpensiveTreeNode*> ancestors; //ancestor table

        // Basic Tree Operations
        ExpensiveTreeNode(std::string id); //Because tree is static, we get to know treeSize in advance
        void print();
        void printIntervals(int level);
        void addChild(ExpensiveTreeNode* child);
        void deleteNode();

        // Methods for Static Preprocessing
        void preprocess();
        void setPreprocessedFlag();
        int assignSubtreeSizes(bool useCompressed); //Returns size of tree
        void assignApex(bool isRoot);
        void assignRoot(ExpensiveTreeNode* node);
        void compressTree(bool isRoot = false);
        bool inPath(ExpensiveTreeNode* apex);
        
        //TODO: add back sigmaType sigma= &ExpensiveTreeNode::getSubtreeSize
        void assignIntervals();
        void contAssignIntervals();

        int getSubtreeSize();

        void fillAllAncestors();
        void fillAncestorTable();
        void printAncestors(int level = 0);

        void assignLevels(int level);
        
        void recompress();
        void add_leaf(ExpensiveTreeNode* leaf);

        // Query
        static ExpensiveTreeNode* lca(ExpensiveTreeNode* nodeA, ExpensiveTreeNode* nodeB);
        static caTuple cas(ExpensiveTreeNode* nodeA, ExpensiveTreeNode* nodeB);
        static caTuple casCompressed(ExpensiveTreeNode* nodeX, ExpensiveTreeNode* nodeY);
        static ExpensiveTreeNode* naiveLca(ExpensiveTreeNode* nodeX, ExpensiveTreeNode* nodeY);
        static caTuple naiveCas(ExpensiveTreeNode* nodeA, ExpensiveTreeNode* nodeB);

        bool isAncestorOf(ExpensiveTreeNode* node);
    private:
        void print(int level);
 };

#endif