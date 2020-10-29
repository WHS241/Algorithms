#include <cmath>
#include <fstream>
#include <random>
#include <unordered_set>

#include <gtest/gtest.h>

#include <npc/CDCL.h>
#include <npc/karp/certificate.h>
#include <npc/karp/reduction.h>

#include <structures/graph.h>

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
        path_builder << "uf20-0" << i << ".cnf";
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
        path_builder << "uf20-0" << i << ".cnf";
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
    graph::graph<int, false, false> input = random_graph<false, false>(engine);
    std::list<int> lex_bfs = graph_alg::generate_lex_bfs(input);
}
