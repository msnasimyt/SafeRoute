
#include <iostream>
#include "min_heap.h"

using namespace std;

int main()
{
    MinHeap heap(10);

    cout << "Inserting nodes..." << endl;

    heap.insert(0, 20);
    heap.insert(1, 10);
    heap.insert(2, 30);
    heap.insert(3, 5);
    heap.insert(4, 15);

    cout << "\nExtracting nodes in ascending distance:"
         << endl;

    while (!heap.isEmpty())
    {
        HeapNode node = heap.extractMin();

        cout << "Location: " << node.vertexId
             << " | Distance: " << node.distance
             << endl;
    }

    cout << "\nHeap test completed!" << endl;

    return 0;
}
