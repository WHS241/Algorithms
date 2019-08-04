#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <string>

#include <BinaryTree.h>

namespace Huffman {
    /*
    Generate the Huffman encoding
    O(n log n)
    David A. Huffman (1952)
    */
    BinaryTree<char>::BinaryNode* generateTree(const std::string& message);
}
#endif // !HUFFMAN_H
