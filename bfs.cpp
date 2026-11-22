
#include "bfs.h"
#include "custom_queue.h"

#include <iostream>

using namespace std;

// Constructor
BFS::BFS(const Graph &g) : graph(g)
{
}

// Return BFS traversal order
int BFS::getTraversalOrder(
    int source,
    int *order,
    int maxSize) const
{
    if (!graph.hasVertex(source) ||
        order == nullptr ||
        maxSize <= 0)
    {
        return -1;
    }

    int vertexCount = graph.getVertexCount();

    bool *visited = new bool[vertexCount];

    for (int i = 0; i < vertexCount; i++)
    {
        visited[i] = false;
    }

    CustomQueue queue(vertexCount);

    int count = 0;

    visited[source] = true;
    queue.enqueue(source);

    while (!queue.isEmpty())
    {
        int current = queue.dequeue();

        if (count < maxSize)
        {
            order[count++] = current;
        }

        const Vertex *vertex = graph.getVertex(current);

        if (vertex == nullptr)
        {
            continue;
        }

        Edge *edge = vertex->head;

        while (edge != nullptr)
        {
            int neighbor = edge->destination;

            if (neighbor >= 0 &&
                neighbor < vertexCount &&
                !edge->blocked &&
                !visited[neighbor])
            {
                visited[neighbor] = true;
                queue.enqueue(neighbor);
            }

            edge = edge->next;
        }
    }

    delete[] visited;

    return count;
}

// Console traversal (existing functionality)
void BFS::traverse(int source) const
{
    int vertexCount = graph.getVertexCount();

    int *order = new int[vertexCount];

    int count = getTraversalOrder(
        source,
        order,
        vertexCount);

    if (count < 0)
    {
        cout << "Invalid source location!" << endl;
        delete[] order;
        return;
    }

    cout << "BFS Traversal: ";

    for (int i = 0; i < count; i++)
    {
        const Vertex *vertex = graph.getVertex(order[i]);

        if (vertex != nullptr)
        {
            cout << vertex->name << " ";
        }
    }

    cout << endl;

    delete[] order;
}

// Check whether destination is reachable
bool BFS::isReachable(int source, int destination) const
{
    if (!graph.hasVertex(source) ||
        !graph.hasVertex(destination))
    {
        return false;
    }

    int vertexCount = graph.getVertexCount();

    int *order = new int[vertexCount];

    int count = getTraversalOrder(
        source,
        order,
        vertexCount);

    bool found = false;

    for (int i = 0; i < count; i++)
    {
        if (order[i] == destination)
        {
            found = true;
            break;
        }
    }

    delete[] order;

    return found;
}
