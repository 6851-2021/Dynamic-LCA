#ifndef LCATREE_H
#define LCATREE_H

#include <list>
#include <string>
#include <vector>

class TreeNode {
     public:
        struct caTuple {
            TreeNode* lca;
            TreeNode* ca_x;
            TreeNode* ca_y;
        };

        static constexpr float beta = 10.0/7.0;
        static const int e = 4;
        static const int c = 5;
        static constexpr float alpha = 6.0/5.0;

        //Static: beta = 2, e = 2, c = 4
        //typedef int (TreeNode::*sigmaType)();

        std::string nodeId;
        std::list<TreeNode*> children;
        TreeNode* parent;
        int subtreeSize; // subtreeSize used in current fat preordering
        int dynamicSubtreeSize; // dynamically updated subtreeSize

        bool isApex;
        bool isPreprocessed;

        std::list<TreeNode*> uncompressedChildren;
        TreeNode* uncompressedParent;
        int uncompressedLevel;
        

        // variables for the fat preorder numbering 
        long long start;
        long long end;
        long long startBuffered;
        long long endBuffered; //last integer in the buffered interval (inclusive)
        
        std::vector<TreeNode*> ancestors; //ancestor table

        // Basic Tree Operations
        TreeNode(std::string id, int treeSize); //Because tree is static, we get to know treeSize in advance
        void print();
        void printIntervals(int level);
        void addChild(TreeNode* child);

        // Methods for Static Preprocessing
        void preprocess();
        void setPreprocessedFlag();
        int assignSubtreeSizes(bool useCompressed); //Returns size of tree
        void assignApex();
        void compressTree();
        bool inPath(TreeNode* apex);
        
        //TODO: add back sigmaType sigma= &TreeNode::getSubtreeSize
        void assignIntervals();
        void contAssignIntervals();

        int getSubtreeSize();

        void fillAllAncestors();
        void fillAncestorTable();
        void printAncestors(int level = 0);

        void assignLevels(int level = 0);
        
        void recompress();
        void add_leaf(TreeNode* leaf);

        // Query
        static TreeNode* lca(TreeNode* nodeA, TreeNode* nodeB);
        static caTuple lcaCompressed(TreeNode* nodeX, TreeNode* nodeY);
        static TreeNode* naiveLca(TreeNode* nodeA, TreeNode* nodeB);

        bool isAncestorOf(TreeNode* node);
    private:
        void print(int level);
 };

#endif