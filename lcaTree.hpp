#ifndef LCATREE_H
#define LCATREE_H

#include <list>
#include <string>
#include <vector>

class MultilevelTreeNode;

class ExpensiveTreeNode {
     public:
        /*
         * Tuple to store "Characteristic Ancestors":
         * "LCA"  = the lowest common ancestor of X and Y
         * "ca_x" = the child of the LCA that is an ancestor of X
         * "ca_y" = the child of the LCA that is an ancestor of Y
         */
        struct caTuple {
            ExpensiveTreeNode* lca;
            ExpensiveTreeNode* ca_x;
            ExpensiveTreeNode* ca_y;
        };

        /* Parameters for the fat preordering proposed by Gabow */
        static constexpr float beta = 10.0/7.0;
        static const int e = 4;
        static const int c = 5;
        static constexpr float alpha = 6.0/5.0;

        /* Maintain uncompressed tree */
        std::string nodeId;
        std::list<ExpensiveTreeNode*> uncompressedChildren;
        ExpensiveTreeNode* uncompressedParent;
        int uncompressedLevel;

        /* Indirection */
        MultilevelTreeNode* associatedTwoSubtree;

        /*-------------------------------------*/
        /*        Basic Tree Operations        */
        /*-------------------------------------*/

        /* Creates a new node with the given ID */
        ExpensiveTreeNode(std::string id);

        /*
         * Creates a new node with the given ID,
         * associated with the given node (indirection)
         */
        ExpensiveTreeNode(std::string id, MultilevelTreeNode* twoSubtree);

        /* Prints the uncompressed tree */
        void print();

        /* Prints the compressed tree and the fat preordering */
        void printIntervals(int level = 0);

        /* Prints the ancestor tables of each node in the tree */
        void printAncestors(int level = 0);

        /*
         * Adds a given node as a child without preprocessing it.
         * This method can be used in conjunction with `preprocess`
         *   to construct static trees in O(n \log n) time and space.
         */
        void addLeafNoPreprocessing(ExpensiveTreeNode* child);

        /*
         * Removes the node from its tree and frees any memory associated
         * with either it or its children in the uncompressed tree.
         */
        void deleteNode();

        /*-------------------------------------*/
        /*            LCA Operations           */
        /*-------------------------------------*/        
        
        /* Preprocesses a tree to be ready for LCA queries */
        void preprocess();

        /*
         * Adds a given node as a child, maintaining the fat preordering
         * This operation has an amortized O(\log^2 n) runtime.
         */
        void add_leaf(ExpensiveTreeNode* leaf);

        /* Computes the LCA of two nodes in O(1) time */
        static ExpensiveTreeNode* lca(ExpensiveTreeNode* nodeA, ExpensiveTreeNode* nodeB);

        /* Computes the characteristic ancestors of two nodes in O(1) time */
        static caTuple cas(ExpensiveTreeNode* nodeA, ExpensiveTreeNode* nodeB);
                
        /* Computes LCA in O(n) time */
        static ExpensiveTreeNode* naiveLca(ExpensiveTreeNode* nodeX, ExpensiveTreeNode* nodeY);

        /* Computes characteristic ancestors in O(n) time */
        static caTuple naiveCas(ExpensiveTreeNode* nodeA, ExpensiveTreeNode* nodeB);

                
    private:
        void print(int level);
        void init(std::string id);

        // Maintain compressed tree
        std::list<ExpensiveTreeNode*> children;
        ExpensiveTreeNode* parent;
        ExpensiveTreeNode* root; // Mantain the root to determine number of nodes in the tree (to determine size of ancestor tables)

        bool isApex;
        ExpensiveTreeNode* heavyChild;
        
        // Maintain fat preordering
        long long int start;
        long long int end; // `end` is the last integer in the interval (inclusive)
        long long int startBuffered;
        long long int endBuffered;

        int subtreeSize; // (old) subtreeSize used by current fat preordering
        int dynamicSubtreeSize; // (updated) subtreeSize updated dynamically
        long long int largestChildEndBuffer;

        // Support LCA queries
        std::vector<ExpensiveTreeNode*> ancestors; //ancestor table
        bool isPreprocessed;

        /*-------------------------------------------*/
        /*   Methods for Generating Compressed Tree  */
        /*-------------------------------------------*/

        /* Sets `uncompressedLevel` for each node in the subtree */
        void assignLevels(int level);

        /*
         * Sets `subtreeSize` to the subtree size in either the
         * compressed or uncompressed tree
         */
        int assignSubtreeSizes(bool useCompressed); //Returns size of tree

        /*
         * Sets `isApex` field of all nodes (based on subtreeSize),
         * indicating whether the node is a heavy or light child.
         *
         * Specifically, a node is "heavy" if its subtree size is more
         * than half the size of its parent's subtree size. A node is
         * "apex" if it is either heavy or the root.
         */
        void assignApex(bool isRoot);

        /*
         * Compresses the tree based on a heavy-light path decomposition
         * (using `isApex` flags). The parent of a node in a compressed
         * tree is the first proper ancestor that is an apex.
         */
        void compressTree(bool isRoot = false);

        /* Checks if the node is the heavy path with that starting at `apex`*/
        bool inPath(ExpensiveTreeNode* apex);

        /* Sets the `root` field of all nodes to equal to the input */
        void assignRoot(ExpensiveTreeNode* node);

        /* Sets a flag for all nodes indicating preprocessing is complete */
        void setPreprocessedFlag();


        /*-------------------------------------------*/
        /*   Methods for Generating Fat Preordering  */
        /*-------------------------------------------*/

        /* Assigns fat preordering intervals, treating current node as root*/
        void assignIntervals();

        /*
         * Assigns fat preordering intervals, assuming startBuffered
         * and endBuffered have already been set.
         */
        void contAssignIntervals();

        /* Fills all ancestor tables */
        void fillAllAncestors();

        /* Updates the ancestor table of the current node */
        void fillAncestorTable();

        /*-------------------------------------------*/
        /*       Helper Methods for LCA Queries      */
        /*-------------------------------------------*/

        /* Computes characteristic ancestors in compressed tree in O(1) time */
        static caTuple casCompressed(ExpensiveTreeNode* nodeX, ExpensiveTreeNode* nodeY);

        bool isAncestorOf(ExpensiveTreeNode* node);

        /*-------------------------------------------*/
        /*   Helper Methods for Dynamic Operations   */
        /*-------------------------------------------*/

        /*
         * Replaces the subtree headed by the current node
         * in the current compressed tree with the
         * compressed subtree generated by the heavy-light
         * decomposition of the original tree.
         * 
         * Note that add_leaf operations may make the compressed
         * tree fall out of sync with the heavy-light decomposition:
         * this method will be called periodically when subtrees
         * grow too large.
         */
        void recompress();

 };

#endif