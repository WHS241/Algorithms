#include <cmath>
#include <fstream>
#include <random>
#include <unordered_set>

#include <gtest/gtest.h>

#include <npc/CDCL.h>
#include <npc/karp/certificate.h>
#include <npc/karp/reduction.h>

#include <structures/graph.h>

#include <graph/max_flow_min_cut.h>
#include <graph/order_dimension.h>
#include <graph/search.h>

#include <special_case/model.h>
#include <special_case/model_gen.h>

#include "CNF_reader.h"
#include "generator.h"

class AlgorithmTest : public ::testing::Test {
protected:
    std::mt19937_64 engine;

    void SetUp() override
    {
        std::random_device base;
        engine = std::mt19937_64(base());
    }
};

TEST_F(AlgorithmTest, Circle_Graph_Clique)
{
    std::uniform_real_distribution<double> point_dist;
    for (int i = 0; i < 100; ++i) {
        std::unordered_set<double> used_points;
        std::vector<std::pair<double, double>> chords(100);
        for (int j = 0; j < 100; ++j) {
            std::pair<double, double> next;
            while (next.first < 0. || next.first > 1. || used_points.count(next.first) != 0)
                next.first = point_dist(engine);
            used_points.insert(next.first);
            while (next.second < 0. || next.second > 1. || used_points.count(next.second) != 0)
                next.second = point_dist(engine);
            used_points.insert(next.second);
            if (next.second < next.first)
                std::swap(next.first, next.second);
            chords[j] = next;
        }

        std::vector<std::size_t> result
            = special_case::model_max_clique_circle_graph(chords.begin(), chords.end());
        ASSERT_LE(result.size(), chords.size());
        ASSERT_NE(result.size(), 0);

        std::vector<bool> in_set(100, false);
        for (std::size_t x : result)
            in_set.at(x) = true;

        for (std::size_t j = 0; j < 100; ++j) {
            bool intersects_all = true;
            for (std::size_t k = 0; intersects_all && k < 100; ++k)
                if (j != k && in_set[k])
                    intersects_all = ((chords[j].first < chords[k].first)
                                         && (chords[k].first < chords[j].second)
                                         && (chords[j].second < chords[k].second))
                        || ((chords[k].first < chords[j].first)
                            && (chords[j].first < chords[k].second)
                            && (chords[k].second < chords[j].second));
            ASSERT_EQ(intersects_all, in_set[j]);
        }
    }
}

TEST_F(AlgorithmTest, CNF_Test)
{
    for (int i = 1; i <= 1000; ++i) {
        std::ostringstream path_builder;
        path_builder << "CNF Tests/uf20-0" << i << ".cnf";
        std::ifstream reader(path_builder.str().c_str());
        std::list<std::list<std::pair<int, bool>>> instance = read_CNF(reader);

        std::unordered_map<int, bool> result = NP_complete::SAT_solver_CDCL(instance);

        ASSERT_FALSE(result.empty());
        ASSERT_TRUE(NP_complete::cert_CNF_SAT(instance, result)) << std::to_string(i);
    }
}

TEST_F(AlgorithmTest, Reductions_from_CNF)
{
    for (int i = 1; i < +1000; ++i) {
        std::ostringstream path_builder;
        path_builder << "CNF Tests/uf20-0" << i << ".cnf";
        std::ifstream input_file(path_builder.str().c_str());

        std::list<std::list<std::pair<int, bool>>> CNF_inst = read_CNF(input_file);

        // sanitize, so reduction works as expected
        CNF_inst.remove_if([](std::list<std::pair<int, bool>>& clause) {
            std::unordered_map<int, bool> current_clause;
            for (auto it = clause.begin(); it != clause.end();) {
                if (current_clause.find(it->first) == current_clause.end()) {
                    current_clause.insert(*it);
                    ++it;
                } else if (current_clause.at(it->first) == it->second) {
                    it = clause.erase(it);
                } else {
                    return true;
                }
            }
            return false;
        });
        std::unordered_map<int, bool> CNF_cert = NP_complete::SAT_solver_CDCL(CNF_inst);
        auto reduction_res = NP_complete::SAT_to_Clique(CNF_inst);

        std::list<std::pair<int, std::size_t>> clique_cert;
        std::size_t j = 0;
        for (const std::list<std::pair<int, bool>>& clause : CNF_inst) {
            for (const std::pair<int, bool>& literal : clause) {
                if (CNF_cert.at(literal.first) == literal.second) {
                    clique_cert.emplace_back(literal.first, j);
                    break;
                }
            }
            ++j;
        }

        ASSERT_TRUE(NP_complete::cert_clique(
            reduction_res, std::make_pair(clique_cert.begin(), clique_cert.end())))
            << path_builder.str();
        auto IS_res = NP_complete::Clique_to_Independent_Set(reduction_res);
        ASSERT_TRUE(NP_complete::cert_independent_set(
            IS_res, std::make_pair(clique_cert.begin(), clique_cert.end())));

        auto VC_res = NP_complete::Independent_Set_to_Vertex_Cover(IS_res);
        std::unordered_set<std::pair<int, std::size_t>, util::pair_hash<int, std::size_t>>
            clique_cert_set(clique_cert.begin(), clique_cert.end());
        std::vector<std::pair<int, std::size_t>> all_verts = VC_res.first.vertices();
        all_verts.resize(std::remove_if(all_verts.begin(), all_verts.end(),
                             [&clique_cert_set](const std::pair<int, std::size_t>& v) {
                                 return clique_cert_set.find(v) != clique_cert_set.end();
                             })
            - all_verts.begin());
        ASSERT_TRUE(NP_complete::cert_vertex_cover(
            VC_res, std::make_pair(all_verts.begin(), all_verts.end())));
    }
}

TEST_F(AlgorithmTest, Reductions_from_Vertex_Cover)
{
    for (int j = 0; j < 100; ++j) {
        graph::graph<int, false, false> input = random_graph<false, false>(engine);

        // Get a vertex cover; doesn't have to be minimal
        graph::graph<int, false, false> copy(input);
        std::list<int> certificate;

        for (int v : input.vertices())
            if (input.degree(v) == 0)
                copy.remove(v);

        while (copy.order() != 0) {
            std::vector<int> curr_vertices = copy.vertices();
            int next = *std::max_element(curr_vertices.begin(), curr_vertices.end(),
                [&copy](int i, int j) { return copy.degree(i) < copy.degree(j); });
            certificate.push_back(next);
            copy.remove(next);

            for (int v : copy.vertices())
                if (copy.degree(v) == 0)
                    copy.remove(v);
        }

        auto vertex_cover_inst = std::make_pair(input, certificate.size());
        ASSERT_TRUE(NP_complete::cert_vertex_cover(
            vertex_cover_inst, std::make_pair(certificate.begin(), certificate.end())));
        std::pair<std::vector<std::unordered_set<std::pair<int, int>,
                      util::pair_hash_unordered<int>, util::key_eq_unordered<int>>>,
            std::size_t>
            set_cover_inst = NP_complete::Vertex_Cover_to_Set_Cover(vertex_cover_inst);

        std::vector<std::unordered_set<std::pair<int, int>, util::pair_hash_unordered<int>,
            util::key_eq_unordered<int>>>
            set_cover_certificate(certificate.size());
        std::transform(certificate.begin(), certificate.end(), set_cover_certificate.begin(),
            [&input, &set_cover_inst](
                int v) { return set_cover_inst.first[input.get_translation().at(v)]; });
        ASSERT_TRUE(NP_complete::cert_set_cover(set_cover_inst, set_cover_certificate));

        ASSERT_TRUE(NP_complete::cert_feedback_vertex(
            NP_complete::Vertex_Cover_to_Feedback_Vertex(vertex_cover_inst),
            std::make_pair(certificate.begin(), certificate.end())));

        std::vector<std::pair<std::pair<int, bool>, std::pair<int, bool>>>
            feedback_edge_certificate;
        feedback_edge_certificate.reserve(certificate.size());
        for (int v : certificate)
            feedback_edge_certificate.emplace_back(
                std::make_pair(v, false), std::make_pair(v, true));
        ASSERT_TRUE(NP_complete::cert_feedback_edge(
            NP_complete::Vertex_Cover_to_Feedback_Edge(vertex_cover_inst),
            std::make_pair(feedback_edge_certificate.begin(), feedback_edge_certificate.end())));

        // DHC
        std::list<std::tuple<std::size_t, std::size_t, int, int>> DHC_certificate;
        std::unordered_set<int> cert_verts(certificate.begin(), certificate.end());
        std::size_t k = 0;
        for (int v : cert_verts) {
            DHC_certificate.emplace_back(k++, 0, 0, 0);
            for (int u : input.neighbors(v)) {
                DHC_certificate.emplace_back(-1, 1, v, u);
                if (cert_verts.find(u) == cert_verts.end()) {
                    DHC_certificate.emplace_back(-1, 1, u, v);
                    DHC_certificate.emplace_back(-1, 2, u, v);
                }
                DHC_certificate.emplace_back(-1, 2, v, u);
            }
        }
        ASSERT_TRUE(
            NP_complete::cert_Hamiltonian_cycle(NP_complete::Vertex_Cover_to_DHC(vertex_cover_inst),
                std::make_pair(DHC_certificate.begin(), DHC_certificate.end())));
    }
}

TEST_F(AlgorithmTest, Lex_BFS)
{
    auto lex_compare = [](const std::set<int>& x, const std::set<int>& y, int bound) -> bool {
        if (x.empty())
            return true;
        auto it1 = x.begin(), it2 = y.begin();
        for (; it1 != x.end() && it2 != y.end(); ++it1, ++it2) {
            if (*it1 < *it2)
                return true;
            if (*it1 > *it2)
                return false;
        }
        return it2 == y.end() || *it2 >= bound;
    };
    for (int j = 0; j < 100; ++j) {
        graph::graph<int, false, false> input = random_graph<false, false>(engine);
        std::list<int> output = graph_alg::generate_lex_bfs(input);
        std::vector<int> lex_bfs(output.begin(), output.end());
        std::vector<std::unordered_set<int>> bfs_levels;
        std::vector<std::size_t> level_map(input.order(), input.order());
        std::vector<int> reverse_permutation(input.order());

        // Verify BFS
        ASSERT_EQ(lex_bfs.size(), input.order());
        for (int i : lex_bfs) {
            if (level_map[i] == input.order()) {
                level_map[i] = bfs_levels.size();
                std::unordered_set<int> root_set({ i });
                bfs_levels.push_back(root_set);
            } else {
                ASSERT_NE(bfs_levels[level_map[i]].find(i), bfs_levels[level_map[i]].end());
            }
            for (int j : input.neighbors(i)) {
                if (level_map[j] != input.order()) {
                    // BFS cannot have back edges
                    if (level_map[j] < level_map[i])
                        ASSERT_EQ(level_map[j], level_map[i] - 1);
                    else if (level_map[j] > level_map[i])
                        ASSERT_EQ(level_map[j], level_map[i] + 1);
                } else {
                    level_map[j] = level_map[i] + 1;
                    if (level_map[j] == bfs_levels.size()) {
                        std::unordered_set<int> next_level({ j });
                        bfs_levels.push_back(next_level);
                    } else {
                        EXPECT_EQ(level_map[j] + 1, bfs_levels.size());
                        bfs_levels.back().insert(j);
                    }
                }
            }
        }

        // Verify lexicographic
        for (std::size_t i = 0; i < lex_bfs.size(); ++i)
            reverse_permutation[lex_bfs[i]] = i;
        std::vector<std::set<int>> lex_orders(input.order());
        for (int v : lex_bfs)
            for (int u : input.neighbors(v))
                if (reverse_permutation[u] < reverse_permutation[v])
                    lex_orders[reverse_permutation[v]].insert(reverse_permutation[u]);

        std::size_t current_root = 0;
        for (std::size_t i = 1; i < lex_orders.size(); ++i)
            if (lex_orders[i].empty()) {
                current_root = i;
            } else {
                ASSERT_GE(*lex_orders[i].begin(), current_root);
                if (level_map[lex_bfs[i - 1]] == level_map[lex_bfs[i]])
                    EXPECT_PRED3(lex_compare, lex_orders[i - 1], lex_orders[i], i - 1);
            }
    }
}

/*
 * For testing the max flow algorithms
 */
template <typename F> void test_case_one(F target_algorithm)
{
    graph::graph<char, true, true, int> input;
    input.add_vertex('A');
    input.add_vertex('B');
    input.add_vertex('C');
    input.add_vertex('D');
    input.add_vertex('E');
    input.add_vertex('F');
    input.add_vertex('G');

    input.set_edge('A', 'B', 3);
    input.set_edge('A', 'D', 3);
    input.set_edge('B', 'C', 4);
    input.set_edge('C', 'A', 3);
    input.set_edge('C', 'D', 1);
    input.set_edge('C', 'E', 2);
    input.set_edge('D', 'E', 2);
    input.set_edge('D', 'F', 6);
    input.set_edge('E', 'B', 1);
    input.set_edge('E', 'G', 1);
    input.set_edge('F', 'G', 9);

    graph::graph<char, true, true, int> result = target_algorithm(input, 'A', 'G');
    EXPECT_EQ(result.edge_cost('A', 'B'), 2);
    EXPECT_EQ(result.edge_cost('A', 'D'), 3);
    EXPECT_EQ(result.edge_cost('B', 'C'), 2);
    EXPECT_EQ(result.edge_cost('C', 'D'), 1);
    EXPECT_EQ(result.edge_cost('C', 'E'), 1);
    EXPECT_EQ(result.edge_cost('D', 'F'), 4);
    EXPECT_EQ(result.edge_cost('E', 'G'), 1);
    EXPECT_EQ(result.edge_cost('F', 'G'), 4);

    EXPECT_EQ(result.degree('A'), 2);
    EXPECT_EQ(result.degree('B'), 1);
    EXPECT_EQ(result.degree('C'), 2);
    EXPECT_EQ(result.degree('D'), 1);
    EXPECT_EQ(result.degree('E'), 1);
    EXPECT_EQ(result.degree('F'), 1);
}
template <typename F> void test_case_two(F target_algorithm)
{
    graph::graph<char, true, true, int> input;
    input.add_vertex('s');
    input.add_vertex('1');
    input.add_vertex('2');
    input.add_vertex('3');
    input.add_vertex('4');
    input.add_vertex('t');

    input.set_edge('s', '1', 10);
    input.set_edge('s', '2', 10);
    input.set_edge('1', '2', 2);
    input.set_edge('1', '3', 4);
    input.set_edge('1', '4', 8);
    input.set_edge('2', '4', 9);
    input.set_edge('3', 't', 10);
    input.set_edge('4', '3', 6);
    input.set_edge('4', 't', 10);

    graph::graph<char, true, true, int> result = target_algorithm(input, 's', 't');
    EXPECT_EQ(result.edge_cost('s', '1'), 10);
    EXPECT_EQ(result.edge_cost('s', '2'), 9);
    EXPECT_EQ(result.edge_cost('1', '3'), 4);
    EXPECT_EQ(result.edge_cost('1', '4'), 6);
    EXPECT_EQ(result.edge_cost('2', '4'), 9);
    EXPECT_EQ(result.edge_cost('3', 't'), 9);
    EXPECT_EQ(result.edge_cost('4', '3'), 5);
    EXPECT_EQ(result.edge_cost('4', 't'), 10);

    EXPECT_EQ(result.degree('s'), 2);
    EXPECT_EQ(result.degree('1'), 2);
    EXPECT_EQ(result.degree('2'), 1);
    EXPECT_EQ(result.degree('3'), 1);
    EXPECT_EQ(result.degree('4'), 2);
    EXPECT_EQ(result.degree('t'), 0);
}

template <typename F> void test_case_three(F target_algorithm)
{
    graph::graph<char, true, true, int> input;
    input.add_vertex('s');
    input.add_vertex('a');
    input.add_vertex('b');
    input.add_vertex('c');
    input.add_vertex('d');
    input.add_vertex('t');

    input.force_add('s', 'a', 15);
    input.force_add('s', 'c', 4);
    input.force_add('a', 'b', 12);
    input.force_add('b', 'c', 3);
    input.force_add('b', 't', 7);
    input.force_add('c', 'd', 10);
    input.force_add('d', 'a', 5);
    input.force_add('d', 't', 10);

    graph::graph<char, true, true, int> result = target_algorithm(input, 's', 't');
    EXPECT_EQ(result.edge_cost('s', 'a'), 10);
    EXPECT_EQ(result.edge_cost('s', 'c'), 4);
    EXPECT_EQ(result.edge_cost('a', 'b'), 10);
    EXPECT_EQ(result.edge_cost('b', 'c'), 3);
    EXPECT_EQ(result.edge_cost('b', 't'), 7);
    EXPECT_EQ(result.edge_cost('c', 'd'), 7);
    EXPECT_EQ(result.edge_cost('d', 't'), 7);

    EXPECT_EQ(result.degree('s'), 2);
    EXPECT_EQ(result.degree('a'), 1);
    EXPECT_EQ(result.degree('b'), 2);
    EXPECT_EQ(result.degree('c'), 1);
    EXPECT_EQ(result.degree('d'), 1);
    EXPECT_EQ(result.degree('t'), 0);
}

template <typename F> void verify_properties(F algorithm, std::mt19937_64& engine)
{
    for (uint32_t i = 0; i < 50; ++i) {
        graph::graph<int, true, true> input = random_graph<true, true>(engine);
        auto vertices = input.vertices();
        if (vertices.size() > 1) {
            std::uniform_int_distribution<uint32_t> vertex_picker(0, vertices.size() - 1);
            uint32_t start_index = vertex_picker(engine);
            uint32_t end_index = vertex_picker(engine);
            while (end_index == start_index)
                end_index = vertex_picker(engine);

            auto max_flow = algorithm(input, vertices[start_index], vertices[end_index]);

            // verify conservation of flow
            std::vector<double> divergence(max_flow.vertices().size(), 0);
            for (const int& v : max_flow.vertices()) {
                for (auto edge : max_flow.edges(v)) {
                    EXPECT_LE(edge.second, input.edge_cost(v, edge.first) + 1e-10);
                    divergence[v] -= edge.second;
                    divergence[edge.first] += edge.second;
                }
            }

            double out_flow = divergence[vertices[start_index]];
            double in_flow = divergence[vertices[end_index]];
            EXPECT_NEAR(out_flow, -in_flow, 1e-10);
            for (int j = 0; j < divergence.size(); ++j) {
                if (j != start_index && j != end_index) {
                    EXPECT_NEAR(divergence[j], 0., 1e-10);
                }
            }
        }
    }
}

template <typename F> void verify_directed_min_cut(F alg_to_test, std::mt19937_64& engine)
{
    for (uint32_t i = 0; i < 50; ++i) {
        graph::graph<int, true, true> input = random_graph<true, true>(engine);
        auto vertices = input.vertices();
        if (vertices.size() > 1) {
            std::uniform_int_distribution<uint32_t> vertex_picker(0, vertices.size() - 1);
            uint32_t start_index = vertex_picker(engine);
            uint32_t end_index = vertex_picker(engine);
            while (end_index == start_index)
                end_index = vertex_picker(engine);

            auto max_flow = alg_to_test(input, vertices[start_index], vertices[end_index]);
            auto min_cut = graph_alg::minimum_cut(
                input, vertices[start_index], vertices[end_index], alg_to_test);

            // verify max-flow-min-cut theorem
            auto flow_start = max_flow.edges(vertices[start_index]);
            double flow_value = std::accumulate(flow_start.begin(), flow_start.end(), 0.,
                [](double prev, const auto& curr) { return prev + curr.second; });
            double cut_cost = std::accumulate(min_cut.begin(), min_cut.end(), 0.,
                [&input](double prev, const graph_alg::cut_edge<int>& curr) {
                    return prev + input.edge_cost(curr.start, curr.end);
                });
            EXPECT_DOUBLE_EQ(flow_value, cut_cost);

            // verify cut
            for (const auto& edge : min_cut) {
                input.remove_edge(edge.start, edge.end);
            }
            EXPECT_THROW(
                graph_alg::least_edges_path(input, vertices[start_index], vertices[end_index]),
                graph_alg::no_path_exception);
        }
    }
}
template <typename F> void verify_undirected_min_cut(F alg_to_test, std::mt19937_64& engine)
{
    // Undirected
    for (uint32_t i = 0; i < 50; ++i) {
        graph::graph<int, false, true> input = random_graph<false, true>(engine);
        auto vertices = input.vertices();
        if (vertices.size() > 1) {
            std::uniform_int_distribution<uint32_t> vertex_picker(0, vertices.size() - 1);
            uint32_t start_index = vertex_picker(engine);
            uint32_t end_index = vertex_picker(engine);
            while (end_index == start_index)
                end_index = vertex_picker(engine);

            auto max_flow = alg_to_test(input, vertices[start_index], vertices[end_index]);
            auto min_cut = graph_alg::minimum_cut(
                input, vertices[start_index], vertices[end_index], alg_to_test);

            // verify max-flow-min-cut theorem
            auto flow_start = max_flow.edges(vertices[start_index]);
            EXPECT_DOUBLE_EQ(std::accumulate(flow_start.begin(), flow_start.end(), 0.,
                                 [](double prev, const auto& curr) { return prev + curr.second; }),
                std::accumulate(min_cut.begin(), min_cut.end(), 0.,
                    [&input](double prev, const graph_alg::cut_edge<int>& curr) {
                        return prev + input.edge_cost(curr.start, curr.end);
                    }));

            // verify cut
            for (const auto& edge : min_cut) {
                input.remove_edge(edge.start, edge.end);
            }
            EXPECT_THROW(
                graph_alg::least_edges_path(input, vertices[start_index], vertices[end_index]),
                graph_alg::no_path_exception);
        }
    }
}

TEST_F(AlgorithmTest, Max_Flow_Edmonds_Karp)
{
    test_case_one(graph_alg::Edmonds_Karp_max_flow<char, true, int>);
    test_case_two(graph_alg::Edmonds_Karp_max_flow<char, true, int>);
    test_case_three(graph_alg::Edmonds_Karp_max_flow<char, true, int>);
    verify_properties(graph_alg::Edmonds_Karp_max_flow<int, true, double>, engine);
    verify_directed_min_cut(graph_alg::Edmonds_Karp_max_flow<int, true, double>, engine);
    verify_undirected_min_cut(graph_alg::Edmonds_Karp_max_flow<int, false, double>, engine);
}

TEST_F(AlgorithmTest, Max_Flow_Dinic)
{
    test_case_one(graph_alg::Dinic_max_flow<char, true, int>);
    test_case_two(graph_alg::Dinic_max_flow<char, true, int>);
    test_case_three(graph_alg::Dinic_max_flow<char, true, int>);
    verify_properties(graph_alg::Dinic_max_flow<int, true, double>, engine);
    verify_directed_min_cut(graph_alg::Dinic_max_flow<int, true, double>, engine);
    verify_undirected_min_cut(graph_alg::Dinic_max_flow<int, false, double>, engine);
}
