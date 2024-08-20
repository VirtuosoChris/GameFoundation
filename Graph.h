#include <concepts>
#include <iterator>

namespace Virtuoso
{
    namespace GameFoundations
    {
        /// Generic Graph Interface
        template<typename G>
        concept Graph = requires(G graph, typename G::NodeHandle nh) 
        {
            // Type requirements
            typename G::NodeType;
            typename G::NodeHandle;
            typename G::NeighborIterator;

            // Node access via handle
            { graph[nh] } -> std::convertible_to<const typename G::NodeType&>;

            // Neighbor iteration
            { graph.neighbor_begin(nh) } -> std::same_as<typename G::NeighborIterator>;
            { graph.neighbor_end(nh) } -> std::same_as<typename G::NeighborIterator>;

            // Graph size
            { graph.size() } -> std::same_as<size_t>;

            // Node validity check
            { graph.is_valid_handle(nh) } -> std::same_as<bool>;
        };

    }
}