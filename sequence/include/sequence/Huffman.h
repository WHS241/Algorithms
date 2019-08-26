#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <string>

#include <structures/BinaryTree.h>

namespace Sequence {
/*
Generate the Huffman encoding
O(n log n)
David A. Huffman (1952)
*/
BinaryTree<char>::BinaryNode* createHuffmanTree(const std::string& message);
} // namespace Sequence
#endif // !HUFFMAN_H
