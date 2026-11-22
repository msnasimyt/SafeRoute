#include "graph.h"

#include <iostream>
#include <cstring>

using namespace std;

Graph::Graph(int maxVertices)
{
    capacity = maxVertices > 0 ? maxVertices : 1;
    vertexCount = 0;

    vertices = new Vertex[capacity];
}

Graph::~Graph()
{
    for (int i = 0; i < vertexCount; i++)
    {
        Edge *current = vertices[i].head;

        while (current != nullptr)
        {
            Edge *temp = current;
            current = current->next;
            delete temp;
        }
    }

    delete[] vertices;
}

int Graph::findVertexIndex(int id) const
{
    for (int i = 0; i < vertexCount; i++)
    {
        if (vertices[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

bool Graph::hasVertex(int id) const
{
    return findVertexIndex(id) != -1;
}

int Graph::getVertexCount() const
{
    return vertexCount;
}

bool Graph::addVertex(int id, const char *name)
{
    if (name == nullptr || name[0] == '\0')
        return false;

    if (hasVertex(id))
        return false;

    if (vertexCount >= capacity)
        return false;

    int index = vertexCount;

    vertices[index].id = id;

    strncpy(vertices[index].name, name,
            sizeof(vertices[index].name) - 1);

    vertices[index].name[sizeof(vertices[index].name) - 1] = '\0';

    vertices[index].head = nullptr;

    vertexCount++;

    return true;
}

Edge *Graph::findEdge(
    int source,
    int destination) const
{
    int index = findVertexIndex(source);

    if (index == -1)
        return nullptr;

    Edge *current = vertices[index].head;

    while (current != nullptr)
    {
        if (current->destination == destination)
            return current;

        current = current->next;
    }

    return nullptr;
}

bool Graph::addRoad(
    int source,
    int destination,
    int weight)
{
    if (source == destination || weight <= 0)
        return false;

    int sourceIndex = findVertexIndex(source);
    int destinationIndex = findVertexIndex(destination);

    if (sourceIndex == -1 || destinationIndex == -1)
        return false;

    if (findEdge(source, destination) != nullptr)
        return false;

    if (findEdge(destination, source) != nullptr)
        return false;

    Edge *forward = new Edge(destination, weight);
    Edge *backward = new Edge(source, weight);

    forward->next = vertices[sourceIndex].head;
    vertices[sourceIndex].head = forward;

    backward->next = vertices[destinationIndex].head;
    vertices[destinationIndex].head = backward;

    return true;
}

bool Graph::blockRoad(int source, int destination)
{
    Edge *forward = findEdge(source, destination);
    Edge *backward = findEdge(destination, source);

    if (forward == nullptr || backward == nullptr)
        return false;

    if (forward->blocked && backward->blocked)
        return false;

    forward->blocked = true;
    backward->blocked = true;

    return true;
}

bool Graph::unblockRoad(int source, int destination)
{
    Edge *forward = findEdge(source, destination);
    Edge *backward = findEdge(destination, source);

    if (forward == nullptr || backward == nullptr)
        return false;

    if (!forward->blocked && !backward->blocked)
        return false;

    forward->blocked = false;
    backward->blocked = false;

    return true;
}

void Graph::display() const
{
    cout << "\n===== SafeRoute Graph =====\n";

    for (int i = 0; i < vertexCount; i++)
    {
        cout << vertices[i].id
             << " (" << vertices[i].name << ")";

        Edge *current = vertices[i].head;

        while (current != nullptr)
        {
            cout << " -> "
                 << current->destination
                 << " [Distance: "
                 << current->weight;

            if (current->blocked)
                cout << ", BLOCKED";

            cout << "]";

            current = current->next;
        }

        cout << '\n';
    }
}
// Return a vertex using its ID
const Vertex *Graph::getVertex(int id) const
{
    int index = findVertexIndex(id);

    if (index == -1)
    {
        return nullptr;
    }

    return &vertices[index];
}
