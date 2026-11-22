
#ifndef BFS_H
#define BFS_H

#include "graph.h"

class BFS
{
private:
    const Graph &graph;

public:
    explicit BFS(const Graph &g);

    void traverse(int source) const;

    bool isReachable(int source, int destination) const;

    // Return BFS traversal order for GUI
    int getTraversalOrder(
        int source,
        int *order,
        int maxSize) const;
};

#endif
