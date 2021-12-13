#include <sequence/Huffman.h>

#include <algorithm>
#include <iterator>
#include <list>
#include <memory>
#include <unordered_map>

sequence::Huffman_tree sequence::create_Huffman_tree(const std::string& message) {
    if (message.empty())
        return Huffman_tree();

    // get all the counts
    std::unordered_map<char, uint32_t> countMap;
    for (char c : message)
        if (countMap.find(c) == countMap.end())
            countMap[c] = 1;
        else
            ++countMap[c];

    std::list<std::pair<Huffman_tree::node*, uint32_t>> values, buffer;
    try {
        std::transform(countMap.begin(), countMap.end(), std::back_inserter(values),
                       [](const std::pair<char, uint32_t>& count) {
                           return std::make_pair(new Huffman_tree::node(count.first), count.second);
                       });

        if (values.size() == 1) {
            Huffman_tree result;
            result._root.reset(values.front().first);
            result._size = 1;
            return result;
        }
        values.sort([](auto& x, auto& y) { return x.second < y.second; });

        uint32_t final_size = values.size();

        // start creating the tree
        // Take two lowest-frequence nodes and make a new node with the two as children
        while (!values.empty()) {
            std::unique_ptr<Huffman_tree::node> node(new Huffman_tree::node);
            uint32_t count;

            // Find the lowest-frequency node
            if (buffer.empty() || values.front().second < buffer.front().second) {
                node->replace_left(values.front().first);
                count = values.front().second;
                values.pop_front();
            } else {
                node->replace_left(buffer.front().first);
                count = buffer.front().second;
                buffer.pop_front();
            }
            // Second-lowest frequency
            if (!values.empty() &&
                (buffer.empty() || values.front().second < buffer.front().second)) {
                node->replace_right(values.front().first);
                count += values.front().second;
                values.pop_front();
            } else {
                node->replace_right(buffer.front().first);
                count += buffer.front().second;
                buffer.pop_front();
            }

            buffer.push_back(std::make_pair(node.get(), count));
            node.release();

            ++final_size;
        }

        // tie up the non-leaf nodes
        // once we've cleared the first list, the frequencies become irrelevant
        while (buffer.size() > 1) {
            std::unique_ptr<Huffman_tree::node> node(new Huffman_tree::node);
            node->replace_left(buffer.front().first);
            buffer.pop_front();
            node->replace_right(buffer.front().first);
            buffer.pop_front();
            buffer.push_back(std::make_pair(node.get(), 0));
            node.release();
            ++final_size;
        }

        Huffman_tree result;
        result._root.reset(buffer.front().first);
        result._size = final_size;
        return result;
    } catch (...) {
        for (auto& tracker : values) {
            delete tracker.first;
        }
        for (auto& tracker : buffer) {
            delete tracker.first;
        }
        throw;
    }
}
