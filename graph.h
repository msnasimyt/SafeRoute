
#ifndef GRAPH_H
#define GRAPH_H

struct Edge
{
    int destination;
    int weight;
    bool blocked;
    Edge *next;

    Edge(int dest, int cost)
    {
        destination = dest;
        weight = cost;
        blocked = false;
        next = nullptr;
    }
};

struct Vertex
{
    int id;
    char name[50];
    Edge *head;

    Vertex()
    {
        id = -1;
        name[0] = '\0';
        head = nullptr;
    }
};

class Graph
{
private:
    Vertex *vertices;
    int vertexCount;
    int capacity;

public:
    const Vertex *getVertex(int id) const;
    explicit Graph(int maxVertices);
    ~Graph();

    Graph(const Graph &) = delete;
    Graph &operator=(const Graph &) = delete;

    bool addVertex(int id, const char *name);

    bool addRoad(int source, int destination, int weight);

    bool blockRoad(int source, int destination);

    bool unblockRoad(int source, int destination);

    void display() const;

    int getVertexCount() const;

    bool hasVertex(int id) const;

private:
    Edge *findEdge(int source, int destination) const;

    int findVertexIndex(int id) const;
};

#endif
