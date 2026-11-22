
#include "graph.h"

#include <iostream>

using namespace std;

int main()
{
    Graph graph(5);

    graph.addVertex(0, "Entrance");
    graph.addVertex(1, "Hall");
    graph.addVertex(2, "Hall B");
    graph.addVertex(3, "Exit A");
    graph.addVertex(4, "Exit B");

    graph.addRoad(0, 1, 4);
    graph.addRoad(1, 2, 2);
    graph.addRoad(1, 3, 6);
    graph.addRoad(2, 4, 3);

    cout << "Initial graph:";
    graph.display();

    cout << "\nBlocking road 1-3...\n";

    if (graph.blockRoad(1, 3))
        cout << "Road blocked successfully.\n";
    else
        cout << "Could not block road.\n";

    graph.display();

    cout << "\nUnblocking road 1-3...\n";

    if (graph.unblockRoad(1, 3))
        cout << "Road reopened successfully.\n";
    else
        cout << "Could not reopen road.\n";

    graph.display();

    cout << "\nTotal locations: "
         << graph.getVertexCount() << '\n';

    return 0;
}
