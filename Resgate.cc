/* José Guilherme de Castro Rodrigues
 * Matrícula: 651201
 * Trabalho Prático 2: Questão 2.
 * Cada pessoa é um vértice. Toda pessoa é ligada a outra por uma possível aresta cujo peso é a distância.
 */
#include <iostream>
#include <cstdint>
#include <vector>
#include <exception>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <limits>
#include <iomanip>

struct Coord {
    uint32_t x;
    uint32_t y;

    double Distance(const Coord& coords) const {
	return std::sqrt((coords.x - x) * (coords.x - x) + (coords.y - y) * (coords.y - y));
    }
};  

class Graph {
private:
    uint32_t m_n;
    std::vector<uint32_t> m_matrix;
public:
    Graph(uint32_t nodeNumber) noexcept;
    Graph(std::vector<uint32_t>) noexcept;

    void ConnectNodes(uint32_t n1, uint32_t n2);

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
void Graph::ConnectNodes(uint32_t n1, uint32_t n2)
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

double Prim(const Graph& graph, const std::vector<Coord>& coords)
{
    // O Heap guardará dois elementos em cada posição. O primeiro indica o vértice e o segundo a distância.
    using NodeAndDist = std::pair<uint32_t, double>;

    constexpr uint32_t STARTING_NODE = 0;
    double distance = 0;

    // No heap, vamos considerar que as distâncias menores devem ficar no topo. Por isso, fazemos uma comparação especial.
    // O número do vértice nem entra na comparação.
    auto heapCmp = [](const NodeAndDist& l, const NodeAndDist& r) { return l.second > r.second;  };

    // Inicializa os heaps.
    std::vector<NodeAndDist> minHeap;
    for (uint32_t i = 1; i != graph.GetNodeNumber(); ++i)
    {
	minHeap.push_back({ i, std::numeric_limits<double>::infinity() });
    }
    minHeap.push_back({ STARTING_NODE, 0 });
    std::make_heap(minHeap.begin(), minHeap.end(), heapCmp);

    while (!minHeap.empty())
    {
	// Pega o topo.
	const NodeAndDist currentNodeAndDist = minHeap.front();
	// Coloca o fim no início.
	minHeap.at(0) = minHeap.back();
	// Retira o fim.
	minHeap.pop_back();
	// Arranja o heap novamente.
	std::make_heap(minHeap.begin(), minHeap.end(), heapCmp);

	distance += currentNodeAndDist.second;

	// Para cada outro vértice adjacente, checa se a distância
	// salva no heap é maior do que a nova distância calculada.
	for (NodeAndDist& adjacent : minHeap)
	{
	    const Coord& currentCoord = coords.at(currentNodeAndDist.first);
	    const Coord& adjacentCoord = coords.at(adjacent.first);
	    double dis = currentCoord.Distance(adjacentCoord);

	    if (adjacent.second > dis)
	    {
		adjacent.second = dis;
		std::make_heap(minHeap.begin(), minHeap.end(), heapCmp);
	    }
	}
    }

    return distance;
}

int main()
{
    uint32_t testNumber;
    std::cin >> testNumber;

    for (uint32_t i = 0; i != testNumber; ++i)
    {
	uint32_t peopleNumber;
	std::cin >> peopleNumber;

	Graph graph(peopleNumber);
	
	// Ler as coordenadas de todos.
	std::vector<Coord> peopleCoords;
	for (uint32_t person = 0; person != peopleNumber; ++person)
	{
	    uint32_t x, y;
	    std::cin >> x >> y;
	    peopleCoords.push_back(Coord{ x, y });
	}

	std::cout << std::fixed << std::setprecision(2) << Prim(graph, peopleCoords) / 100.0 << '\n' << '\n';
    }

    return 0;
}