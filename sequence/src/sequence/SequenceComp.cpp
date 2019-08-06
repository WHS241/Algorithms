#ifndef STRING_COMP_CPP
#define STRING_COMP_CPP
#include <array>

template<typename CharT>
typename std::basic_string<CharT>::const_iterator Sequence::findSubstring(const std::basic_string<CharT>& input, const std::basic_string<CharT>& target) {
    if (target.empty())
        return input.begin();

    // preprocessing
    std::vector<typename std::basic_string<CharT>::size_type> next(target.size());
    next[0] = std::basic_string<CharT>::npos;
    if(next.size() > 1)
        next[1] = 0;
    for (auto i = 2; i < next.size(); ++i) {
        auto matchIndex = next[i - 1];
        while (matchIndex != next[0] && target[i - 1] != target[matchIndex])
            matchIndex = next[matchIndex];

        next[i] = matchIndex + 1;
    }

    // find string
    typename std::basic_string<CharT>::size_type current = 0;
    for (uint32_t i = 0; i < input.size(); ++i, ++current) {
        if (current == target.size())
            return input.begin() + i - target.size();

        while (current != std::basic_string<CharT>::npos && target[current] != input[i])
            current = next[current];
    }

    if (current == target.size())
        return input.end() - target.size();
    return input.end();
}

template<typename CharT>
std::vector<Sequence::Instruction> Sequence::editDistance(const std::basic_string<CharT>& src, const std::basic_string<CharT>& target) {
    struct CompNode {
        Instruction::Category step;
        uint32_t cost;
    };

    // grid[i][j] denotes the edit distance from src.substring(0, i + 1) to target.substring(0, j + 1)
    std::vector<std::vector<CompNode>> grid(src.size() + 1, std::vector<CompNode>(target.size() + 1));
    for (uint32_t i = 0; i < grid.size(); ++i) {
        grid[i][0].step = Instruction::Category::Delete;
        grid[i][0].cost = i;
    }
    for (uint32_t i = 0; i < grid[0].size(); ++i) {
        grid[0][i].step = Instruction::Category::Insert;
        grid[0][i].cost = i;
    }

    // dynamic programming
    for (uint32_t i = 1; i < grid.size(); ++i) {
        for (uint32_t j = 1; j < grid[i].size(); ++j) {
            // the three possible steps to reach this node
            std::array<CompNode, 3> choice;
            choice[0].step = Instruction::Category::Replace;
            choice[0].cost = grid[i - 1][j - 1].cost + (src[i - 1] == target[j - 1] ? 0 : 1);
            choice[1].step = Instruction::Category::Delete;
            choice[1].cost = grid[i - 1][j].cost + 1;
            choice[2].step = Instruction::Category::Insert;
            choice[2].cost = grid[i][j - 1].cost + 1;

            grid[i][j] = *std::min_element(choice.begin(), choice.end(), [](auto& x, auto& y) {
                return x.cost < y.cost;
                });
        }
    }

    // backtrack from final node to first
    std::vector<Instruction> result;
    uint32_t currentX = grid.size() - 1, currentY = grid[0].size() - 1;
    while (currentX != 0 || currentY != 0) {
        auto& node = grid[currentX][currentY];
        if (node.step != Instruction::Category::Replace || src[currentX - 1] != target[currentY - 1]) {
            Instruction instruction;
            instruction.directive = node.step;
            instruction.srcIndex = currentX - 1;
            instruction.targetIndex = currentY - 1;
            result.push_back(instruction);
        }

        // recalibrate currentX and currentY
        if (node.step != Instruction::Category::Delete)
            --currentY;
        if (node.step != Instruction::Category::Insert)
            --currentX;
    }

    return result;
}

#endif // STRING_COMP_CPP