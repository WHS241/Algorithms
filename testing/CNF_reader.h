#ifndef CNF_READ_H
#define CNF_READ_H
#include <iostream>
#include <list>
#include <utility>

std::list<std::list<std::pair<int, bool>>> read_CNF(std::istream& in) {
    std::string line;
    std::getline(in, line);

    bool fail = false;
    while (line[0] != 'p' && !fail)
        fail = !std::getline(in, line);
    std::string tokens[4];
    std::size_t last = -1;
    for (int i = 0; i < 4; ++i) {
        std::size_t next = line.find(' ', last + 1);
        while (next == last + 1 && next < line.size()) {
            last = next;
            next = line.find(' ', last + 1);
        }
        tokens[i] = line.substr(last + 1, next - last - 1);
        last = next;
    }

    std::size_t num_clauses = std::stoi(tokens[3]);

    std::list<std::list<std::pair<int, bool>>> ret_val;
    std::list<std::pair<int, bool>> current;

    while (num_clauses != 0) {
        int literal;
        in >> literal;
        if (literal < 0)
            current.emplace_back(-literal, false);
        else if (literal > 0)
            current.emplace_back(literal, true);
        else {
            ret_val.emplace_back(std::move(current));
            current.clear();
            --num_clauses;
        }
    }

    return ret_val;
}

#endif // CNF_READ_H
