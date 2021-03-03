/* José Guilherme de Castro Rodrigues
 * Matrícula: 651201
 * Trabalho Prático 2: Questão 1.
 */
#include <iostream>
#include <cstdint>
#include <vector>
#include <exception>
#include <cmath>

class Graph {
private:
    std::uint32_t m_n;
    std::vector<std::uint32_t> m_matrix;
public:
    Graph(std::uint32_t nodeNumber) noexcept;
    Graph(std::vector<std::uint32_t>) noexcept;

    void ConnectNodes(std::uint32_t n1, std::uint32_t n2);

    std::uint32_t GetNodeNumber() const noexcept;
    std::vector<std::uint32_t> GetAdjacentNodes(std::uint32_t) const noexcept;
    void PrintMatrix() const noexcept;

    enum class Color : std::uint8_t {
	White,
	Gray,
	Black
    };
};

// Constructs an empty graph with nodeNumber nodes and no edges.
Graph::Graph(std::uint32_t nodeNumber) noexcept
    :
    m_n(nodeNumber),
    m_matrix(nodeNumber* nodeNumber, 0)
{
}

// Constructs a graph with the NxN node adjacency matrix given.
Graph::Graph(std::vector<std::uint32_t> matrix) noexcept
    :
    m_n(static_cast<std::uint32_t>(std::sqrt(matrix.size()))),
    m_matrix(matrix)
{
}

// Connects node n1 to node n2. Throws an exception if one of the nodes doesn't exist.
void Graph::ConnectNodes(std::uint32_t n1, std::uint32_t n2)
{
    if (n1 < m_n && n2 < m_n)
    {
	// Matrix(n1, n2) = Matrix(n1, n2) = 1.
	m_matrix.at(n1 * m_n + n2) = 1;
	m_matrix.at(n2 * m_n + n1) = 1;
    }
    else
	throw std::runtime_error("Invalid nodes received on ConnectNodes.");
}

// Returns the number of nodes the graph has.
std::uint32_t Graph::GetNodeNumber() const noexcept
{
    return m_n;
}

// Returns a vector that contains all nodes that are adjacent to the given node.
std::vector<std::uint32_t> Graph::GetAdjacentNodes(std::uint32_t node) const noexcept
{
    const std::uint32_t rowStartingIndex = node * m_n;

    std::vector<std::uint32_t> adjacentNodes;
    for (std::uint32_t i = 0; i != m_n; ++i)
    {
	if (m_matrix.at(i + rowStartingIndex) != 0)
	    adjacentNodes.push_back(i);
    }

    return adjacentNodes;
}

// Prints a matrix that represents the graph.
void Graph::PrintMatrix() const noexcept
{
    for (decltype(m_matrix.size()) i = 0; i != m_matrix.size(); ++i)
    {
	std::cout << m_matrix[i] << ' ';
	if ((i + 1) % m_n == 0)
	    std::cout << '\n';
    }
}

void DepthFirstSearchVisit(
    const Graph& graph,
    std::uint32_t node,
    std::uint32_t& time,
    std::vector<Graph::Color>& colors,
    std::vector<std::pair<std::uint32_t, std::uint32_t>>& inOutTime,
    std::vector<std::uint32_t>& parents)
{
    colors.at(node) = Graph::Color::Gray;

    std::cout << static_cast<char>(node + 'a') << ',';

    inOutTime.at(node).first = time++;

    const auto adjacentNodes = graph.GetAdjacentNodes(node);
    for (auto adjacentNode : adjacentNodes)
    {
	if (colors.at(adjacentNode) == Graph::Color::White)
	{
	    parents.at(adjacentNode) = node;
	    DepthFirstSearchVisit(graph, adjacentNode, time, colors, inOutTime, parents);
	}
    }

    colors.at(node) = Graph::Color::Black;
    inOutTime.at(node).second = time++;
}

void DepthFirstSearch(const Graph& graph)
{
    const auto nodeNumber = graph.GetNodeNumber();
    constexpr auto infinite = std::numeric_limits<std::uint32_t>::max();

    std::vector<Graph::Color> colors(nodeNumber, Graph::Color::White);
    std::vector<std::pair<std::uint32_t, std::uint32_t>> inOutTime(nodeNumber, { infinite, infinite });
    std::vector<std::uint32_t> parents(nodeNumber, infinite);

    auto time = 0u;
    auto components = 0u;

    for (auto i = 0u; i != nodeNumber; ++i)
    {
	if (colors.at(i) == Graph::Color::White)
	{
	    ++components;
	    DepthFirstSearchVisit(graph, i, time, colors, inOutTime, parents);
	    std::cout << '\n';
	}
	
    }

    std::cout << components << " connected components\n\n";
}

int main()
{
    std::uint32_t testNumber;
    std::cin >> testNumber;

    for (std::uint32_t i = 0; i != testNumber; ++i)
    {
	std::cout << "Case #" << (i + 1) << ':' << '\n';

	std::uint32_t nodes;
	std::uint32_t edges;

	std::cin >> nodes >> edges;

	Graph graph(nodes);

	for (std::uint32_t edge = 0; edge != edges; ++edge)
	{
	    char n1 = ' ';
	    char n2 = ' ';

	    std::cin >> n1 >> n2;

	    graph.ConnectNodes(static_cast<std::uint32_t>(n1 - 'a'), static_cast<std::uint32_t>(n2 - 'a'));
	}

	DepthFirstSearch(graph);	
    }

    return 0;
}