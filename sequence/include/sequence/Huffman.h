#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <string>

#include <structures/binary_tree_base.h>

namespace sequence {
/*
Generate the Huffman encoding
Θ(n log n)
David A. Huffman (1952)
*/
tree::binary_tree<char>::node* createHuffmanTree(const std::string& message);
} // namespace Sequence
#endif // !HUFFMAN_H
