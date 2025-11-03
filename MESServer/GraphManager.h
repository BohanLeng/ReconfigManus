//
// Created by bohanleng on 30/10/2025.
//

#ifndef RECONFIGMANUS_LABELLEDDIGRAPH_H
#define RECONFIGMANUS_LABELLEDDIGRAPH_H

#include "GraphDef.h"
#include <nlohmann/json.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

using json = nlohmann::json;

using LabelledDiGraph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::bidirectionalS,
        ST_VertexLabel,
        ST_ArcLabel
>;

class GraphManager
{
public:
    explicit GraphManager(const json& graph_model);

    ~GraphManager() = default;

    bool GetVertexProperties(uint32_t id, ST_VertexLabel& label) const;
    bool GetVertexTimeDist(uint32_t id, ST_TimeDist& dist) const;
    bool SetVertexTimeDist(uint32_t id, const ST_TimeDist& dist);

    bool GetArcProperties(uint32_t tail, uint32_t head, ST_ArcLabel& label) const;
    bool GetArcTimeDist(uint32_t tail, uint32_t head, ST_TimeDist& dist) const;
    bool SetArcTimeDist(uint32_t tail, uint32_t head, const ST_TimeDist& dist);

    bool FindShortestPath(uint32_t tail, uint32_t head, std::vector<uint32_t>& path, float & length_exp) const;

    bool WriteOutDotFile(const std::string & filename) const;


private:
    std::unique_ptr<LabelledDiGraph> graph_;

    using VertexDescriptor = boost::graph_traits<LabelledDiGraph>::vertex_descriptor;
    std::unordered_map<uint32_t, VertexDescriptor> vertex_map_;
};

#endif //RECONFIGMANUS_LABELLEDDIGRAPH_H