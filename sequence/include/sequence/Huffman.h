#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <string>

#include <structures/binary_tree_base.h>

namespace sequence {
/*
Generate the Huffman encoding

David A. Huffman
A method for the construction of minimum-redundancy codes
(1952) doi:10.1109/JRPROC.1952.273898

Θ(n log n)
*/
tree::binary_tree<char>::node* createHuffmanTree(const std::string& message);
} // namespace Sequence
#endif // !HUFFMAN_H
