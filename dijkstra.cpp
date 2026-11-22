
#include "dijkstra.h"
#include "min_heap.h"

#include <iostream>
#include <climits>

using namespace std;

// Constructor
Dijkstra::Dijkstra(const Graph &g)
    : graph(g)
{
    vertexCount = graph.getVertexCount();

    distances = new int[vertexCount];
    previous = new int[vertexCount];

    calculated = false;
}

// Destructor
Dijkstra::~Dijkstra()
{
    delete[] distances;
    delete[] previous;
}

// Run Dijkstra Algorithm
bool Dijkstra::calculate(int source)
{
    if (!graph.hasVertex(source))
    {
        cout << "Invalid source location!" << endl;
        calculated = false;
        return false;
    }

    // Initialize arrays
    for (int i = 0; i < vertexCount; i++)
    {
        distances[i] = INT_MAX;
        previous[i] = -1;
    }

    // Create custom Min-Heap
    MinHeap heap(vertexCount * vertexCount + 1);

    // Source distance is zero
    distances[source] = 0;
    heap.insert(source, 0);

    while (!heap.isEmpty())
    {
        // Get minimum distance node
        HeapNode current = heap.extractMin();

        int u = current.vertexId;

        // Skip outdated heap entries
        if (current.distance != distances[u])
        {
            continue;
        }

        // Get current vertex
        const Vertex *vertex = graph.getVertex(u);

        if (vertex == nullptr)
        {
            continue;
        }

        // Check all connected roads
        const Edge *edge = vertex->head;

        while (edge != nullptr)
        {
            // Ignore blocked roads
            if (!edge->blocked)
            {
                int v = edge->destination;
                int weight = edge->weight;

                // Check whether a shorter path exists
                if (distances[u] != INT_MAX &&
                    distances[u] + weight < distances[v])
                {
                    distances[v] = distances[u] + weight;
                    previous[v] = u;

                    // Insert updated distance
                    heap.insert(v, distances[v]);
                }
            }

            edge = edge->next;
        }
    }

    calculated = true;

    return true;
}

// Get shortest distance
int Dijkstra::getDistance(int destination) const
{
    if (!calculated || !graph.hasVertex(destination))
    {
        return -1;
    }

    if (distances[destination] == INT_MAX)
    {
        return -1;
    }

    return distances[destination];
}

// Get shortest path as an array of vertex IDs
int *Dijkstra::getPath(int destination, int &pathLength) const
{
    // Initially, no path is available
    pathLength = 0;

    // Check whether Dijkstra was calculated
    if (!calculated || !graph.hasVertex(destination))
    {
        return nullptr;
    }

    // Check whether destination is reachable
    if (distances[destination] == INT_MAX)
    {
        return nullptr;
    }

    // Allocate dynamic array
    int *path = new int[vertexCount];

    int current = destination;

    // Reconstruct path using previous array
    while (current != -1 && pathLength < vertexCount)
    {
        path[pathLength] = current;
        pathLength++;

        current = previous[current];
    }

    // Reverse array to get Source -> Destination order
    for (int i = 0; i < pathLength / 2; i++)
    {
        int temp = path[i];

        path[i] = path[pathLength - 1 - i];

        path[pathLength - 1 - i] = temp;
    }

    return path;
}

// Print shortest path
void Dijkstra::printPath(int destination) const
{
    if (!calculated || !graph.hasVertex(destination))
    {
        cout << "Calculate routes first or check destination!"
             << endl;
        return;
    }

    if (distances[destination] == INT_MAX)
    {
        cout << "No safe route available!" << endl;
        return;
    }

    int *path = new int[vertexCount];

    int pathSize = 0;

    int current = destination;

    // Reconstruct path
    while (current != -1 && pathSize < vertexCount)
    {
        path[pathSize] = current;
        pathSize++;

        current = previous[current];
    }

    cout << "Shortest Path: ";

    for (int i = pathSize - 1; i >= 0; i--)
    {
        const Vertex *vertex = graph.getVertex(path[i]);

        if (vertex != nullptr)
        {
            cout << vertex->name;
        }
        else
        {
            cout << path[i];
        }

        if (i > 0)
        {
            cout << " -> ";
        }
    }

    cout << endl;

    cout << "Total Distance: "
         << distances[destination]
         << endl;

    delete[] path;
}

// Print distances to all locations
void Dijkstra::printAllDistances() const
{
    if (!calculated)
    {
        cout << "Calculate routes first!" << endl;
        return;
    }

    cout << "\n===== Shortest Distances ====="
         << endl;

    for (int i = 0; i < vertexCount; i++)
    {
        const Vertex *vertex = graph.getVertex(i);

        cout << "Location " << i;

        if (vertex != nullptr)
        {
            cout << " (" << vertex->name << ")";
        }

        cout << " : ";

        if (distances[i] == INT_MAX)
        {
            cout << "Unreachable";
        }
        else
        {
            cout << distances[i];
        }

        cout << endl;
    }
}
