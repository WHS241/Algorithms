#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <string>

#include <structures/binary_tree_base.h>

namespace sequence {

class Huffman_tree : public tree::binary_tree<char> {
    public:
    using tree::binary_tree<char>::iterator, tree::binary_tree<char>::const_iterator;
    using tree::binary_tree<char>::binary_tree;
    std::pair<iterator, bool> insert(const char&) { throw; }
    iterator erase(iterator) { throw; }

    protected:
    using tree::binary_tree<char>::node;
    node* _find(const char&) const { return nullptr; }
    using tree::binary_tree<char>::_root;
    using tree::binary_tree<char>::_size;

    friend Huffman_tree create_Huffman_tree(const std::string&);
};
/*
Generate the Huffman encoding

David A. Huffman
A method for the construction of minimum-redundancy codes
(1952) doi:10.1109/JRPROC.1952.273898

Θ(n log n)
*/
Huffman_tree create_Huffman_tree(const std::string& message);
} // namespace sequence
#endif // !HUFFMAN_H
