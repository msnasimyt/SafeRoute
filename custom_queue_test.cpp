
#include <iostream>
#include "custom_queue.h"

using namespace std;

int main()
{

    CustomQueue queue(5);

    cout << "===== Custom Queue Test ====="
         << endl;

    // Insert elements
    cout << "\nInserting elements..." << endl;

    queue.enqueue(10);
    queue.enqueue(20);
    queue.enqueue(30);
    queue.enqueue(40);

    cout << "Queue Size: " << queue.getSize()
         << endl;

    // Remove elements
    cout << "\nRemoving elements:" << endl;

    while (!queue.isEmpty())
    {
        cout << "Removed: " << queue.dequeue()
             << endl;
    }

    cout << "\nQueue Empty: "
         << (queue.isEmpty() ? "Yes" : "No")
         << endl;

    cout << "\nQueue test completed!" << endl;

    return 0;
}
