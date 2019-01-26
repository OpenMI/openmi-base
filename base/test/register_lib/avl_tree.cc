#include "avl_tree.h"
#include "tree_factory.h"


OPENMI_REGISTER_FILE_TAG(avl_tree);
REGISTER_TREE(AVLTree);
REGISTER_TREE_WITH_NAME(AVLTree, cpu);

void AVLTree::Print() {
  printf("I am avl tree\n");
}
