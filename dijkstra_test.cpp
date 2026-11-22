
#include <iostream>

#include "graph.h"
#include "dijkstra.h"

using namespace std;

int main()
{

    // Create Graph
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

    // Create Dijkstra object
    Dijkstra dijkstra(graph);

    cout << "===== SafeRoute Dijkstra Test ====="
         << endl;

    // Calculate shortest distances from Entrance
    cout << "\nCalculating safe routes from Entrance..."
         << endl;

    if (dijkstra.calculate(0))
    {

        dijkstra.printAllDistances();

        cout << "\nRoute to Exit B:" << endl;

        dijkstra.printPath(4);
    }

    // Block a road
    cout << "\n===== Blocking Road 1-2 ====="
         << endl;

    graph.blockRoad(1, 2);

    // Calculate again
    cout << "\nRecalculating safe routes..."
         << endl;

    dijkstra.calculate(0);

    dijkstra.printAllDistances();

    cout << "\nRoute to Exit B:" << endl;

    dijkstra.printPath(4);

    cout << "\n===== Test Completed ====="
         << endl;

    return 0;
}
