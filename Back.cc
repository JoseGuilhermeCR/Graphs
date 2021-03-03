/* José Guilherme de Castro Rodrigues
 * Matrícula: 651201
 * Trabalho Prático: Questão 4.
 */
#include <iostream>
#include <cstdint>
#include <vector>
#include <exception>
#include <cmath>
#include <functional>
#include <queue>
#include <limits>
#include <unordered_map>
#include <algorithm>

struct PlaneRoute {
    uint32_t to;
    uint32_t from;
    uint32_t cost;
};

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

std::pair<std::vector<uint32_t>, uint32_t> Dijkstra(const Graph& graph, const uint32_t start, const std::vector<PlaneRoute>& planeRoutes)
{
    // O Heap guardará dois elementos em cada posição. O primeiro indica o vértice e o segundo a distância.
    using NodeAndCost = std::pair<uint32_t, uint32_t>;

    auto heapCmp = [](const NodeAndCost& l, const NodeAndCost& r) { return l.second > r.second; };
    std::vector<NodeAndCost> minHeap;
    for (uint32_t i = 0; i != graph.GetNodeNumber(); ++i)
    {
	minHeap.push_back({ i, std::numeric_limits<uint32_t>::max() });
    }
    // Custo do inicial é 0.
    minHeap.at(start).second = 0;

    std::make_heap(minHeap.begin(), minHeap.end(), heapCmp);

    // O valor para a chave do nó é o custo que se teve para chegar nele.
    uint32_t cost = 0;
    std::vector<uint32_t> path;

    while (!minHeap.empty())
    {
	// Pega o topo.
	const NodeAndCost currentNodeAndCost = minHeap.front();
	// Coloca o fim no início.
	minHeap.at(0) = minHeap.back();
	// Retira o fim.
	minHeap.pop_back();
	// Arranja o heap novamente.
	std::make_heap(minHeap.begin(), minHeap.end(), heapCmp);

	cost += currentNodeAndCost.second;
	path.push_back(currentNodeAndCost.first);

	const std::vector<uint32_t> currentAdjacents = graph.GetAdjacentNodes(currentNodeAndCost.first);
	for (NodeAndCost& adjacent : minHeap)
	{
	    auto result = std::find(currentAdjacents.begin(), currentAdjacents.end(), adjacent.first);
	    if (result == currentAdjacents.end())
		continue;

	    // Procura a rota equivalente para pegar o custo da viagem entré os nós current e adjacente.
	    const uint32_t currentRouteCost = [&planeRoutes](uint32_t from, uint32_t to) {
		for (const auto& route : planeRoutes)
		{
		    if (route.from == from && route.to == to)
			return route.cost;
		}
	    }(currentNodeAndCost.first, adjacent.first);

	    if (adjacent.first == graph.GetNodeNumber() - 1)
	    {
		cost += currentRouteCost;
		path.push_back(graph.GetNodeNumber() - 1);
		return { path, cost };
	    }

	    const uint32_t heapAdjacentCost = adjacent.second;
	    const uint32_t heapCurrentCost = currentNodeAndCost.second;

	    if (heapAdjacentCost > heapCurrentCost + currentRouteCost)
	    {
		adjacent.second = heapCurrentCost + currentRouteCost;
		std::make_heap(minHeap.begin(), minHeap.end(), heapCmp);
	    }
	}
    }
    path.push_back(graph.GetNodeNumber() - 1);

    return { path, std::numeric_limits<uint32_t>::max() };
}

int32_t main()
{
    uint32_t cases = 3;

    for (uint32_t numCase = 0; numCase != cases; ++numCase)
    {
	std::cout << "Instancia " << numCase + 1 << '\n' << '\n';

	uint32_t cities;
	uint32_t routes;
	std::cin >> cities >> routes;

	Graph graph(cities);

	std::vector<PlaneRoute> planeRoutes;
	for (uint32_t route = 0; route != routes; ++route)
	{
	    PlaneRoute pr;
	    std::cin >> pr.from >> pr.to >> pr.cost;

	    --pr.from;
	    --pr.to;

	    planeRoutes.push_back(pr);

	    graph.ConnectNodes(pr.from, pr.to);
	}

	uint32_t friends;
	uint32_t freeSeats;
	std::cin >> friends >> freeSeats;

	uint32_t finalCost = 0;
	uint32_t neededRoutes = std::ceil(static_cast<double>(friends) / static_cast<double>(freeSeats));
	bool impossible = false;

	for (uint32_t route = 0; route != neededRoutes && !impossible; ++route)
	{
	    std::pair<std::vector<uint32_t>, uint32_t> pathAndCost = Dijkstra(graph, 0, planeRoutes);

	    if (pathAndCost.second != std::numeric_limits<uint32_t>::max())
		finalCost += pathAndCost.second * (friends / static_cast<uint32_t>(neededRoutes));
	    else
		impossible = true;
	    
	    std::vector<uint32_t>& path = pathAndCost.first;
	    for (uint32_t i = 0; i != path.size() - 1; ++i)
		graph.DisconnectNodes(path[i], path[i + 1]);
	}

	if (!impossible)
	    std::cout << finalCost << '\n';
	else
	    std::cout << "impossivel\n";

	std::cout << '\n' << '\n' << '\n';
    }

    return 0;
}