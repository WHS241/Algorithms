#ifndef STRING_COMP_CPP
#define STRING_COMP_CPP
#include <array>

template <typename CharT>
typename std::basic_string<CharT>::const_iterator sequence::find_substring(
    const std::basic_string<CharT>& input, const std::basic_string<CharT>& target)
{
    if (target.empty())
        return input.begin();

    // preprocessing
    std::vector<typename std::basic_string<CharT>::size_type> next(target.size());
    next[0] = std::basic_string<CharT>::npos;
    if (next.size() > 1)
        next[1] = 0;
    for (auto i = 2; i < next.size(); ++i) {
        auto match_index = next[i - 1];
        while (match_index != next[0] && target[i - 1] != target[match_index])
            match_index = next[match_index];

        next[i] = match_index + 1;
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

template <typename CharT>
std::vector<sequence::instruction> sequence::edit_distance(
    const std::basic_string<CharT>& src, const std::basic_string<CharT>& target)
{
    struct helper_node {
        instruction::category step;
        uint32_t cost;
    };

    // grid[i][j] denotes the edit distance from src.substring(0, i + 1) to
    // target.substring(0, j + 1)
    std::vector<std::vector<helper_node>> grid(
        src.size() + 1, std::vector<helper_node>(target.size() + 1));
    for (uint32_t i = 0; i < grid.size(); ++i) {
        grid[i][0].step = instruction::category::delete_char;
        grid[i][0].cost = i;
    }
    for (uint32_t i = 0; i < grid[0].size(); ++i) {
        grid[0][i].step = instruction::category::insert_char;
        grid[0][i].cost = i;
    }

    // dynamic programming
    for (uint32_t i = 1; i < grid.size(); ++i) {
        for (uint32_t j = 1; j < grid[i].size(); ++j) {
            // the three possible steps to reach this node
            std::array<helper_node, 3> choice;
            choice[0].step = instruction::category::replace_char;
            choice[0].cost = grid[i - 1][j - 1].cost + (src[i - 1] == target[j - 1] ? 0 : 1);
            choice[1].step = instruction::category::delete_char;
            choice[1].cost = grid[i - 1][j].cost + 1;
            choice[2].step = instruction::category::insert_char;
            choice[2].cost = grid[i][j - 1].cost + 1;

            grid[i][j] = *std::min_element(
                choice.begin(), choice.end(), [](auto& x, auto& y) { return x.cost < y.cost; });
        }
    }

    // backtrack from final node to first
    std::vector<instruction> result;
    uint32_t current_X = grid.size() - 1, current_Y = grid[0].size() - 1;
    while (current_X != 0 || current_Y != 0) {
        auto& node = grid[current_X][current_Y];
        if (node.step != instruction::category::replace_char
            || src[current_X - 1] != target[current_Y - 1]) {
            instruction step;
            step.directive = node.step;
            step.src_index = current_X - 1;
            step.target_index = current_Y - 1;
            result.push_back(step);
        }

        // recalibrate current_X and current_Y
        if (node.step != instruction::category::delete_char)
            --current_Y;
        if (node.step != instruction::category::insert_char)
            --current_X;
    }

    return result;
}

#endif // STRING_COMP_CPP