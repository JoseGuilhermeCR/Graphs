/* José Guilherme de Castro Rodrigues
 * Matrícula: 651201
 * Trabalho Prático: Questão 5.
 */
#include <iostream>
#include <cstdint>
#include <vector>
#include <exception>
#include <cmath>
#include <limits>
#include <algorithm>

class Graph {
private:
    uint32_t m_n;
    std::vector<uint32_t> m_matrix;

    void SetMatrix(uint32_t, uint32_t, uint32_t);
public:
    Graph(uint32_t nodeNumber) noexcept;
    Graph(std::vector<uint32_t>) noexcept;

    void ConnectNodes(uint32_t, uint32_t, bool oneWay = false);
    void DisconnectNodes(uint32_t, uint32_t, bool oneWay = false);

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

void Graph::SetMatrix(uint32_t x, uint32_t y, uint32_t val)
{
    if (x < m_n && y < m_n)
    {
	m_matrix.at(x * m_n + y) = val;
    }
    else
	throw std::runtime_error("Invalid values received on SetMatrix");
}

// Connects node n1 to node n2. Throws an exception if one of the nodes doesn't exist.
void Graph::ConnectNodes(uint32_t n1, uint32_t n2, bool oneWay)
{
    SetMatrix(n1, n2, 1);
    if (!oneWay)
	SetMatrix(n2, n1, 1);
}

void Graph::DisconnectNodes(uint32_t n1, uint32_t n2, bool oneWay)
{
    SetMatrix(n1, n2, 0);
    if (!oneWay)
	SetMatrix(n2, n1, 0);
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

void DepthFirstSearchVisit(const Graph& graph, std::uint32_t node, std::vector<Graph::Color>& colors)
{
    colors.at(node) = Graph::Color::Gray;

    const std::vector<uint32_t> adjacentNodes = graph.GetAdjacentNodes(node);
    for (uint32_t adjacentNode : adjacentNodes)
    {
	if (colors.at(adjacentNode) == Graph::Color::White)
	    DepthFirstSearchVisit(graph, adjacentNode, colors);
    }

    colors.at(node) = Graph::Color::Black;
}

uint32_t DepthFirstSearch(const Graph& graph)
{
    const uint32_t nodeNumber = graph.GetNodeNumber();
    constexpr uint32_t infinite = std::numeric_limits<std::uint32_t>::max();

    std::vector<Graph::Color> colors(nodeNumber, Graph::Color::White);

    uint32_t components = 0;

    for (uint32_t i = 0; i != nodeNumber; ++i)
    {
	if (colors.at(i) == Graph::Color::White)
	{
	    ++components;
	    DepthFirstSearchVisit(graph, i, colors);
	}
    }

    return components;
}

int32_t main()
{
    uint32_t tests;
    std::cin >> tests;

    for (uint32_t test = 0; test != tests; ++test)
    {
	uint32_t node, edges, edgesToAdd, colors;
	std::cin >> node >> edges >> edgesToAdd >> colors;
	
	Graph graph(node);
	std::vector<uint32_t> nodeColors(node, 0);

	for (uint32_t& color : nodeColors)
	    std::cin >> color;

	for (uint32_t edge = 0; edge != edges; ++edge)
	{
	    uint32_t u, v;
	    std::cin >> u >> v;
	    graph.ConnectNodes(u - 1, v - 1);
	}

	// Adição das arestas.
	bool connected = false;
	for (uint32_t edge = 0; edge != edgesToAdd; ++edge)
	{
	    connected = false;
	    // Para cada vértice do grafo.	    
	    for (uint32_t u = 0; u != graph.GetNodeNumber() && !connected; ++u)
	    {
		std::vector<uint32_t> adjacentNodes = graph.GetAdjacentNodes(u);

		for (uint32_t v = 0; v != graph.GetNodeNumber() && !connected; ++v)
		{
		    if (u != v && std::find(adjacentNodes.cbegin(), adjacentNodes.cend(), v) == adjacentNodes.end() && nodeColors.at(u) != nodeColors.at(v))
		    {
			graph.ConnectNodes(u, v);
			connected = true;
			std::cout << "Adicionei" << '\n';
		    }
		}
	    }
	}

	if (connected)
	{
	    if (DepthFirstSearch(graph) != 1)
		std::cout << 'N';
	    else
		std::cout << 'Y';
	}
	else
	    std::cout << 'N';

	std::cout << '\n' << '\n';
    }

    return 0;
}