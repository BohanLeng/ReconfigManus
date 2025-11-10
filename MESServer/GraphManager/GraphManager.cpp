//
// Created by bohanleng on 30/10/2025.
//

#include "GraphManager.h"
#include <fstream>
#include <limits>
#include <algorithm>
#include <unordered_set>
#include <boost/property_map/function_property_map.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>


GraphManager::GraphManager(const json & graph_model)
{
    graph_ = std::make_unique<LabelledDiGraph>();
    // TODO check validity of the file
    for (const auto & v : graph_model["vertices"])
    {
        ST_VertexLabel vertex_label{};
        v["id"].get_to(vertex_label.id);
        v["name"].get_to(vertex_label.name);
        v["buffer_capacity"].get_to(vertex_label.buffer_capacity);
        {
            std::string tstr;
            v["service_time_distribution"]["type"].get_to(tstr);
            vertex_label.service_time_dist.type = TimeDistTypeFromString(tstr);
        }
        v["service_time_distribution"]["parameters"].get_to(vertex_label.service_time_dist.parameters);

        auto vertex_descriptor = boost::add_vertex(vertex_label, *graph_);
        vertex_map_[vertex_label.id] = vertex_descriptor;
    }
    for (const auto & a : graph_model["arcs"])
    {
        ST_ArcLabel arc_label{};
        a["tail"].get_to(arc_label.tail);
        a["head"].get_to(arc_label.head);
        {
            std::string tstr;
            a["transfer_time_distribution"]["type"].get_to(tstr);
            arc_label.transfer_time_dist.type = TimeDistTypeFromString(tstr);
        }
        a["transfer_time_distribution"]["parameters"].get_to(arc_label.transfer_time_dist.parameters);

        auto [edge_descriptor, success] = boost::add_edge(vertex_map_[arc_label.tail],
            vertex_map_[arc_label.head], arc_label, *graph_);
        if (!success)
        {
            std::cout << "Failed to add arc" << std::endl;
            return;
        }
    }
}

bool GraphManager::GetVertexProperties(uint32_t id, ST_VertexLabel& label) const
{
    auto it = vertex_map_.find(id);
    if (it == vertex_map_.end())
        return false;

    label = (*graph_)[it->second];
    return true;
}

bool GraphManager::GetVertexTimeDist(uint32_t id, ST_TimeDist& dist) const
{
    auto it = vertex_map_.find(id);
    if (it == vertex_map_.end())
        return false;

    dist = (*graph_)[it->second].service_time_dist;
    return true;
}

bool GraphManager::SetVertexTimeDist(uint32_t id, const ST_TimeDist& dist)
{
    auto it = vertex_map_.find(id);
    if (it == vertex_map_.end())
        return false;

    (*graph_)[it->second].service_time_dist = dist;
    return true;
}

bool GraphManager::GetArcProperties(uint32_t tail, uint32_t head, ST_ArcLabel& label) const
{
    auto itTail = vertex_map_.find(tail);
    auto itHead = vertex_map_.find(head);
    if (itTail == vertex_map_.end() || itHead == vertex_map_.end())
        return false;

    auto [e, found] = boost::edge(itTail->second, itHead->second, *graph_);
    if (!found)
        return false;

    label = (*graph_)[e];
    return true;
}

bool GraphManager::GetArcTimeDist(uint32_t tail, uint32_t head, ST_TimeDist& dist) const
{
    auto itTail = vertex_map_.find(tail);
    auto itHead = vertex_map_.find(head);
    if (itTail == vertex_map_.end() || itHead == vertex_map_.end())
        return false;

    auto [e, found] = boost::edge(itTail->second, itHead->second, *graph_);
    if (!found)
        return false;

    dist = (*graph_)[e].transfer_time_dist;
    return true;
}

bool GraphManager::SetArcTimeDist(uint32_t tail, uint32_t head, const ST_TimeDist& dist)
{
    auto itTail = vertex_map_.find(tail);
    auto itHead = vertex_map_.find(head);
    if (itTail == vertex_map_.end() || itHead == vertex_map_.end())
        return false;

    auto [e, found] = boost::edge(itTail->second, itHead->second, *graph_);
    if (!found)
        return false;

    (*graph_)[e].transfer_time_dist = dist;
    return true;
}

bool GraphManager::GetOutgoingNeighborVertices(uint32_t vertex_id, std::list<uint32_t>& out_vertices) const
{
    out_vertices.clear();
    auto it = vertex_map_.find(vertex_id);
    if (it == vertex_map_.end() || !graph_)
        return false;

    using Graph = LabelledDiGraph;
    using vertex_descriptor = boost::graph_traits<Graph>::vertex_descriptor;

    const vertex_descriptor v = it->second;

    // Collect only outgoing neighbors
    auto adj_pair = boost::adjacent_vertices(v, *graph_);
    for (auto ai = adj_pair.first; ai != adj_pair.second; ++ai)
    {
        out_vertices.push_back((*graph_)[*ai].id);
    }

    return !out_vertices.empty();
}

bool GraphManager::FindShortestPath(uint32_t tail, uint32_t head, std::vector<uint32_t>& out_path, float& out_length) const
{
    // If head == tail, out_path contains only the same vertex, out_length is 0.0f
    auto itTail = vertex_map_.find(tail);
    auto itHead = vertex_map_.find(head);
    if (itTail == vertex_map_.end() || itHead == vertex_map_.end())
        return false;

    using Graph = LabelledDiGraph;
    using vertex_descriptor = boost::graph_traits<Graph>::vertex_descriptor;
    using edge_descriptor = boost::graph_traits<Graph>::edge_descriptor;

    const auto n = boost::num_vertices(*graph_);
    if (n == 0)
        return false;

    const auto weight_map = boost::make_function_property_map<edge_descriptor, double>(
        [this](const edge_descriptor& e) -> double {
            const auto & transfer_dist = (*graph_)[e].transfer_time_dist;
            return transfer_dist.expected_value();
        }
    );

    std::vector<vertex_descriptor> pred(n);
    std::vector<double> dist(n, std::numeric_limits<double>::infinity());

    auto index_map = get(boost::vertex_index, *graph_);

    // Run Dijkstra from source
    boost::dijkstra_shortest_paths(
        *graph_,
        itTail->second,
        boost::weight_map(weight_map)
            .distance_map(boost::make_iterator_property_map(dist.begin(), index_map))
            .predecessor_map(boost::make_iterator_property_map(pred.begin(), index_map))
    );

    // Return false if unreachable
    if (!std::isfinite(dist[index_map[itHead->second]]))
        return false;

    // Reconstruct the path from head back to tail
    std::vector<uint32_t> rev_path;
    for (auto v = itHead->second; ; )
    {
        rev_path.push_back((*graph_)[v].id);
        if (v == itTail->second) break;
        auto pv = pred[index_map[v]];
        if (pv == v) break; // safety
        v = pv;
    }
    std::ranges::reverse(rev_path);

    out_path = std::move(rev_path);
    out_length = static_cast<float>(dist[index_map[itHead->second]]);
    return true;
}

bool GraphManager::WriteOutDotFile(const std::string& filename) const
{
    std::ofstream out(filename);
    if (!out.is_open())
        return false;

    using Graph = LabelledDiGraph;
    using VD = boost::graph_traits<Graph>::vertex_descriptor;
    using ED = boost::graph_traits<Graph>::edge_descriptor;

    struct VertexWriter {
        const Graph* g;
        void operator()(std::ostream& os, const VD& v) const {
            const auto& lbl = (*g)[v];
            const double mean = lbl.service_time_dist.expected_value();
            os << " [shape=box, style=filled, fillcolor=lightyellow, color=black, penwidth=1, label=\"" << lbl.id;
            if (!lbl.name.empty()) os << ": " << lbl.name;
            os << "\\ncap=" << static_cast<int>(lbl.buffer_capacity)
               << "\\nsvc=" << mean << " (" << TimeDistTypeToString(lbl.service_time_dist.type) << ")\"]";
        }
    };

    struct EdgeWriter {
        const Graph* g;
        void operator()(std::ostream& os, const ED& e) const {
            const auto& el = (*g)[e];
            const auto& td = el.transfer_time_dist;
            os << " [color=black, penwidth=1, arrowsize=1.0, label=\"" << TimeDistTypeToString(td.type) << ": ";
            for (size_t i = 0; i < td.parameters.size(); ++i) {
                if (i) os << ",";
                os << td.parameters[i];
            }
            os << " | mean=" << td.expected_value() << "\"]";
        }
    };

    // Can be converted to png using command:
    // `dot -Tpng xxx.dot -o xxx.png`
    boost::write_graphviz(out, *graph_, VertexWriter{graph_.get()}, EdgeWriter{graph_.get()});

    return static_cast<bool>(out);
}


