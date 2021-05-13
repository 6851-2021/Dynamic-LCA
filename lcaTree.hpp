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

        const int beta = 2;
        const int e = 2;
        const int c = 4;
        //typedef int (TreeNode::*sigmaType)();

        std::string nodeId;
        std::list<TreeNode*> children;
        TreeNode* parent;
        TreeNode* uncompressedParent;
        int subtreeSize;
        bool isApex;
        TreeNode* closestApex;

        // variables for the fat preorder numbering 
        int start;
        int end;
        int startBuffered;
        int endBuffered; //last integer in the buffered interval (inclusive)
        
        std::vector<TreeNode*> ancestors;

        // Basic Tree Operations
        TreeNode(std::string id, int treeSize); //Because tree is static, we get to know treeSize in advance
        void print();
        void printIntervals(int level);
        void addChild(TreeNode* child);

        // Methods for Static Preprocessing
        int assignSubtreeSizes(); //Returns size of tree
        void assignApex();
        void compressTree();
        
        //TODO: add back sigmaType sigma= &TreeNode::getSubtreeSize
        void assignIntervals();
        void contAssignIntervals();

        int getSubtreeSize();

        void fillAllAncestors();
        void fillAncestorTable();
        void printAncestors(int level = 0);

        // Query
        void lca(TreeNode nodeA, TreeNode nodeB);
        caTuple lcaCompressed(TreeNode* nodeX, TreeNode* nodeY);
    private:
        void print(int level);
 };