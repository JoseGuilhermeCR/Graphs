/* José Guilherme de Castro Rodrigues
 * Matrícula: 651201
 * Trabalho Prático: Questão 3.
 */
#include <iostream>
#include <cstdint>
#include <vector>
#include <exception>
#include <cmath>
#include <functional>
#include <queue>
#include <limits>
#include <thread>

class Graph {
private:
    uint32_t m_n;
    std::vector<uint32_t> m_matrix;
public:
    Graph(uint32_t nodeNumber) noexcept;
    Graph(std::vector<uint32_t>) noexcept;

    void ConnectNodes(uint32_t, uint32_t, bool oneWay = false);

    uint32_t GetNodeNumber() const noexcept;
    std::vector<uint32_t> GetAdjacentNodes(uint32_t) const noexcept;
    void PrintMatrix() const noexcept;

    enum class Color : std::uint8_t {
	White,
	Gray,
	Black
    };
};

// Constructs an empty graph with nodeNumber nodes and no edges.
Graph::Graph(uint32_t nodeNumber) noexcept
    :
    m_n(nodeNumber),
    m_matrix(nodeNumber* nodeNumber, 0)
{
}

// Constructs a graph with the NxN node adjacency matrix given.
Graph::Graph(std::vector<uint32_t> matrix) noexcept
    :
    m_n(static_cast<uint32_t>(std::sqrt(matrix.size()))),
    m_matrix(matrix)
{
}

// Connects node n1 to node n2. Throws an exception if one of the nodes doesn't exist.
void Graph::ConnectNodes(uint32_t n1, uint32_t n2, bool oneWay)
{
    if (n1 < m_n && n2 < m_n)
    {
	// Matrix(n1, n2) = Matrix(n1, n2) = 1.
	m_matrix.at(n1 * m_n + n2) = 1;
	if (!oneWay)
	    m_matrix.at(n2 * m_n + n1) = 1;
    }
    else
	throw std::runtime_error("Invalid nodes received on ConnectNodes.");
}

// Returns the number of nodes the graph has.
uint32_t Graph::GetNodeNumber() const noexcept
{
    return m_n;
}

// Returns a vector that contains all nodes that are adjacent to the given node.
std::vector<uint32_t> Graph::GetAdjacentNodes(uint32_t node) const noexcept
{
    const uint32_t rowStartingIndex = node * m_n;

    std::vector<uint32_t> adjacentNodes;
    for (uint32_t i = 0; i != m_n; ++i)
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

uint32_t BreadthFirstSearch(const Graph& graph, uint32_t startingNode)
{
    uint32_t nodes = 1;

    const uint32_t nodeNumber = graph.GetNodeNumber();
    constexpr uint32_t infinite = std::numeric_limits<uint32_t>::max(); // Not actually infinite...

    std::vector<Graph::Color> colors(nodeNumber, Graph::Color::White);    
    colors.at(startingNode) = Graph::Color::Gray;

    std::queue<uint32_t> visitQueue;
    visitQueue.push(startingNode);
    
    while (!visitQueue.empty())
    {
	const uint32_t currentNode = visitQueue.front();
	visitQueue.pop();

	const std::vector<uint32_t> adjacentNodes = graph.GetAdjacentNodes(currentNode);

	for (uint32_t adjacentNode : adjacentNodes)
	{
	    if (colors.at(adjacentNode) == Graph::Color::White)
	    {
		colors.at(adjacentNode) = Graph::Color::Gray;

		++nodes;

		visitQueue.push(adjacentNode);
	    }
	}
	colors.at(currentNode) = Graph::Color::Black;
    }

    return nodes;
}

int32_t main()
{
    uint32_t species;
    uint32_t relationships;
    
    std::cin >> species >> relationships;

    Graph graph(species);
    for (uint32_t i = 0; i != relationships; ++i)
    {
	uint32_t u;
	uint32_t v;

	std::cin >> u >> v;

	graph.ConnectNodes(u - 1, v - 1, true);
    }

    uint32_t max = 0;
    for (uint32_t i = 0; i != graph.GetNodeNumber(); ++i)
    {
	uint32_t n = BreadthFirstSearch(graph, i);
	if (n > max)
	    max = n;
    }

    if (max == graph.GetNodeNumber())
	std::cout << "Bolada\n";
    else
	std::cout << "Nao Bolada\n";
   
    return 0;
}