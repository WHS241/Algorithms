#include <sequence/Huffman.h>

#include <algorithm>
#include <iterator>
#include <list>
#include <memory>
#include <unordered_map>

tree::binary_tree<char>::node* sequence::createHuffmanTree(const std::string& message)
{
    if (message.empty())
        return nullptr;

    // get all the counts
    std::unordered_map<char, uint32_t> countMap;
    for (char c : message)
        if (countMap.find(c) == countMap.end())
            countMap[c] = 1;
        else
            ++countMap[c];

    std::list<std::pair<tree::binary_tree<char>::node*, uint32_t>> values, buffer;
    try {
        std::transform(countMap.begin(), countMap.end(), std::back_inserter(values),
            [](const std::pair<char, uint32_t>& count) {
                return std::make_pair(new tree::binary_tree<char>::node(count.first), count.second);
            });

        if (values.size() == 1)
            return values.front().first;
        values.sort([](auto& x, auto& y) { return x.second < y.second; });

        // start creating the tree
        while (!values.empty()) {
            std::unique_ptr<tree::binary_tree<char>::node> node(new tree::binary_tree<char>::node);
            uint32_t count;
            if (buffer.empty() || values.front().second < buffer.front().second) {
                node->replace_left(values.front().first);
                count = values.front().second;
                values.pop_front();
            } else {
                node->replace_left(buffer.front().first);
                count = buffer.front().second;
                buffer.pop_front();
            }

            if (!values.empty()
                && (buffer.empty() || values.front().second < buffer.front().second)) {
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
        }

        // tie up the non-leaf nodes
        // once we've cleared the first list, the frequencies become irrelevant
        while (buffer.size() > 1) {
            std::unique_ptr<tree::binary_tree<char>::node> node(new tree::binary_tree<char>::node);
            node->replace_left(buffer.front().first);
            buffer.pop_front();
            node->replace_right(buffer.front().first);
            buffer.pop_front();
            buffer.push_back(std::make_pair(node.get(), 0));
            node.release();
        }

        return buffer.front().first;
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