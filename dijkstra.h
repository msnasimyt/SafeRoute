
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

class Dijkstra
{
private:
    const Graph &graph;

    int vertexCount;
    int *distances;
    int *previous;

    bool calculated;

public:
    explicit Dijkstra(const Graph &g);
    ~Dijkstra();

    Dijkstra(const Dijkstra &) = delete;
    Dijkstra &operator=(const Dijkstra &) = delete;

    bool calculate(int source);

    int getDistance(int destination) const;

    // Returns the shortest path as an array of vertex IDs.
    // The caller must delete[] the returned array.
    int *getPath(int destination, int &pathLength) const;

    void printPath(int destination) const;
    void printAllDistances() const;
};

#endif
