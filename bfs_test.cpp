
#include <iostream>

#include "graph.h"
#include "bfs.h"

using namespace std;

int main()
{

    Graph graph(5);

    // Add locations
    graph.addVertex(0, "Entrance");
    graph.addVertex(1, "Hall");
    graph.addVertex(2, "Hall_B");
    graph.addVertex(3, "Exit_A");
    graph.addVertex(4, "Exit_B");

    // Add roads
    graph.addRoad(0, 1, 4);
    graph.addRoad(1, 2, 2);
    graph.addRoad(1, 3, 6);
    graph.addRoad(2, 4, 3);

    BFS bfs(graph);

    cout << "===== SafeRoute BFS Test ====="
         << endl;

    // BFS Traversal
    cout << "\nStarting from Entrance:"
         << endl;

    bfs.traverse(0);

    // Check reachability
    cout << "\nChecking Exit B:" << endl;

    if (bfs.isReachable(0, 4))
    {
        cout << "Exit B is reachable!" << endl;
    }
    else
    {
        cout << "Exit B is NOT reachable!" << endl;
    }

    // Block road
    cout << "\nBlocking road 1-2..."
         << endl;

    graph.blockRoad(1, 2);

    // Run BFS again
    cout << "\nAfter blocking road:"
         << endl;

    bfs.traverse(0);

    if (bfs.isReachable(0, 4))
    {
        cout << "Exit B is reachable!" << endl;
    }
    else
    {
        cout << "Exit B is NOT reachable!" << endl;
    }

    cout << "\n===== BFS Test Completed ====="
         << endl;

    return 0;
}
