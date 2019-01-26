#include "test/register_lib/tree.h"
#include "test/register_lib/tree_factory.h"
#include "test/register_lib/base.h"
#include "test/register_lib/base_factory.h"

int main(int argc, char *argv[]) {
  Tree* cpu_tree = openmi::Register<TreeFactory>::Find("cpu")->func();
  if (cpu_tree == nullptr) {
    printf("cpu_tree is nullptr\n");
  } else {
    printf("cpu_tree is not nullptr\n");
    cpu_tree->Print();
  }
  Tree *binary = openmi::Register<TreeFactory>::Find("BinaryTree")->func();
  binary->Print();
  Tree *avl = openmi::Register<TreeFactory>::Find("AVLTree")->func();
  avl->Print();
  delete binary; delete avl; 

  Base* sub1 = openmi::Register<BaseFactory>::Find("Sub1")->func();
  sub1->Print();
  Base* sub2 = openmi::Register<BaseFactory>::Find("Sub2")->func();
  sub2->Print();
  delete sub1; delete sub2;
  return 0;
}
