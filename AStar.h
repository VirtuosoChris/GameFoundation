#include <concepts>
#include <type_traits>

#include "Graph.h"

namespace Virtuoso
{
    namespace GameFoundations
    {
        template<typename CostFn, typename GraphType>
        concept CostFunction = requires(CostFn costFn, const GraphType& graph, 
                                            typename GraphType::NodeHandle n1, 
                                            typename GraphType::NodeHandle n2) 
        {
            { costFn(graph, n1, n2) } -> std::convertible_to<double>;
        };

        template <typename NodeIndex>
        struct TraversalInfoRecord
        {
            NodeIndex parent;
            double cost = std::numeric_limits<double>::max();
            double estimatedTotalCost = std::numeric_limits<double>::max();
            //bool visited = false;
        };

        template <typename T>
        using AStarResult = std::stack<T>;

        template <typename NodeHandleType>
        struct AStarFrontierEntry
        {
            NodeHandleType node;
            double cost;

            inline bool operator<(const AStarFrontierEntry& other) const
            {
                return cost < other.cost;
            }

            inline bool operator>(const AStarFrontierEntry& other) const
            {
                return cost > other.cost;
            }
        };

        template<Graph GraphType, CostFunction<GraphType> CostFn>
        AStarResult<typename GraphType::NodeHandle> AStar(
            const GraphType& graph,
            typename const GraphType::NodeHandle& start,
            typename const GraphType::NodeHandle& target,
            CostFn costFunction,
            CostFn heuristic
            )
        {
            // A* algorithm implementation
            using NodeHandle = typename GraphType::NodeHandle;
            using NodeType = typename GraphType::NodeType;
            using NeighborIterator = typename GraphType::NeighborIterator;
            using Solution = AStarResult<NodeHandle>;
            using FrontierEntry = AStarFrontierEntry<NodeHandle>;
            using OpenList = std::priority_queue<FrontierEntry, std::vector<FrontierEntry>, std::greater<FrontierEntry> >;
            using TraversalInfo = TraversalInfoRecord<NodeHandle>;
            using TraversalRecords = std::unordered_map<NodeHandle, TraversalInfo>;

            Solution solution;

            if (!graph.is_valid_handle(start) || !graph.is_valid_handle(target))
            {
                return solution;
            }
            
            if (start != target)
            {
                TraversalRecords graphTraversal;

                TraversalInfo startRec = {start, 0.0, heuristic(graph, start, target)};

                graphTraversal[start] = startRec;

                OpenList frontier;
                frontier.push({start, startRec.estimatedTotalCost});

                while (!frontier.empty())
                {
                    NodeHandle tNode = frontier.top().node;
                    frontier.pop();
                    
                    //graphTraversal[tNode].visited = true;

                    if (tNode == target)
                    {
                        solution.push(target);

                        // work backwards and build the path
                        NodeHandle q = target;
                        while(q != graphTraversal[q].parent)
                        {
                            NodeHandle p = graphTraversal[q].parent;
                            solution.push(p);
                            q = p;
                        }

                        break;
                    }

                    const double& costSoFar = graphTraversal[tNode].cost;

                    for (NeighborIterator it = graph.neighbor_begin(tNode); it != graph.neighbor_end(tNode); ++it)
                    {
                        NodeHandle neighbor = *it;

                        const TraversalInfo& traversalInfo = graphTraversal[neighbor];

                        double proposedCost = costSoFar + costFunction(graph, tNode, neighbor);

                        TraversalInfo info;
                        info.parent = tNode;
                        info.cost = proposedCost;
                        info.estimatedTotalCost = info.cost + heuristic(graph, neighbor, target);   ///\todo cache heuristic cost instead of recalculating
                        //info.visited = false;

                        bool newPathBetter = proposedCost < traversalInfo.cost;

                        if (newPathBetter)
                        {
                            graphTraversal[neighbor] = info;
                            frontier.push({neighbor, info.estimatedTotalCost});
                        }
                    }       
                }
            }
            
            return solution;
        }
    }
}