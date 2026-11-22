
#include <iostream>
#include "max_heap.h"

using namespace std;

int main()
{

    MaxHeap rescueHeap(10);

    cout << "===== SafeRoute Rescue Priority Test ====="
         << endl;

    // Add rescue cases
    cout << "\nAdding emergency cases..."
         << endl;

    rescueHeap.insert(
        RescueCase(101, "Victim_A", 3, 1));

    rescueHeap.insert(
        RescueCase(102, "Victim_B", 9, 2));

    rescueHeap.insert(
        RescueCase(103, "Victim_C", 5, 3));

    rescueHeap.insert(
        RescueCase(104, "Victim_D", 7, 4));

    cout << "Total Cases: "
         << rescueHeap.getSize()
         << endl;

    // Process cases by priority
    cout << "\nRescue Order:"
         << endl;

    while (!rescueHeap.isEmpty())
    {

        RescueCase current = rescueHeap.extractMax();

        cout << "Case ID: " << current.caseId
             << " | Victim: " << current.victimName
             << " | Priority: " << current.priority
             << " | Location: " << current.locationId
             << endl;
    }

    cout << "\nRescue Priority Test Completed!"
         << endl;

    return 0;
}
