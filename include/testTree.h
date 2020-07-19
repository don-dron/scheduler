
#pragma once
#include <stdlib.h>

#ifndef TEST_TREE
#define TEST_TREE

struct TreeNode;
typedef struct TreeNode TreeNode;

struct TreeNode
{
    TreeNode *left_;
    TreeNode *right_;
};

TreeNode *Create(TreeNode *left, TreeNode *right)
{
    TreeNode *treeNode = (TreeNode *)malloc(sizeof(TreeNode));
    treeNode->left_ = left;
    treeNode->right_ = right;
    return treeNode;
}

TreeNode *CreateLeaf()
{
    return (TreeNode *)malloc(sizeof(TreeNode));
}

#endif