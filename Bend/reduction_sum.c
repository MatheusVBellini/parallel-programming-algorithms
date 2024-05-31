#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct TreeNode {
  bool is_leaf;
  int val;
  struct TreeNode* left;
  struct TreeNode* right;
} TreeNode;

TreeNode* create_node(bool is_leaf, int val) {
  TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
  node->is_leaf = is_leaf;
  node->val = val;
  node->left = NULL;
  node->right = NULL;
  return node;
}

void free_tree(TreeNode* node) {
  if (node) {
    free_tree(node->left);
    free_tree(node->right);
    free(node);
  }
}

int reduction_sum(TreeNode* tree) {
  if (tree->is_leaf)
    return tree->val;
  return reduction_sum(tree->left) + reduction_sum(tree->right);
}

TreeNode* treefy(int first, int last) {
  TreeNode* node = NULL;
  if (last > first) {
    int mid = (last + first) / 2;
    node = create_node(false, 0);
    node->left = treefy(first, mid);
    node->right = treefy(mid + 1, last);
  } else {
    node = create_node(true, first);
  }
  return node;
}

int main() {
  TreeNode* tree;
  for (int i = 2; i <= 2000; i++) {
    tree = treefy(1,i);
    printf("%d ", reduction_sum(tree));
    free_tree(tree); 
  }
  printf("\n");
  
  return 0;
}
