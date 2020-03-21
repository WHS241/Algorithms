#ifndef HW_2_6310
#define HW_2_6310

#include <list>
#include <map>
#include <utility>
#include <vector>

namespace sequence {
    std::list<std::pair<uint32_t, uint32_t>> limit_pairs(const std::vector<uint32_t>& input_1, const std::vector<uint32_t>& input_2, const std::vector<uint32_t>& input_3) {
        if(input_1.size() != input_2.size() || input_2.size() != input_3.size())
            throw std::invalid_argument("Incompatible inputs");
        std::list<std::pair<uint32_t, uint32_t>> result;
        uint32_t size = input_1.size();
        std::vector<uint32_t> iso_1(size), iso_2(size), translate(size);
        for(uint32_t i = 0; i < size; ++i)
            translate[input_1[i]] = i;
        for(uint32_t i = 0; i < size; ++i)
            iso_1[i] = translate[input_3[i]];
        for(uint32_t i = 0; i < size; ++i)
            translate[input_2[i]] = i;
        for(uint32_t i = 0; i < size; ++i)
            iso_2[i] = translate[input_3[i]];

        for(uint32_t i = 0; i < size; ++i) {
            std::map<uint32_t, uint32_t> pair_BST;
            for(uint32_t j = i + 1; j < size; ++j) {
                if (iso_1[i] < iso_1[j] && iso_2[i] < iso_2[j]) {
                    const auto [it, success] = pair_BST.insert({iso_1[j], iso_2[j]});
                    if (!success) throw std::runtime_error("Unknown exception");
                    bool remove = false;
                    if(it != pair_BST.begin()) {
                        auto pred = it;
                        --pred;
                        if (pred->second < it-> second)
                            remove = true;
                    }
                    if(remove) {
                        pair_BST.erase(it);
                    } else {
                        result.push_back({input_3[i], input_3[j]});
                        auto successor = it;
                        ++successor;
                        while (successor != pair_BST.end() && successor->second > it->second) {
                            auto temp = successor;
                            ++successor;
                            pair_BST.erase(temp);
                        }
                    }
                }
            }
        }
        
        return result;
    }
}


#endif // 6310_HW_2