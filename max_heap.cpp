
#include "max_heap.h"

#include <iostream>
#include <cstring>

using namespace std;

// RescueCase constructor
RescueCase::RescueCase(
    int id,
    const char *name,
    int level,
    int location,
    int danger,
    int signal,
    int risk,
    int selectedExit,
    int distance)
{
    caseId = id;

    strncpy(victimName, name, sizeof(victimName) - 1);
    victimName[sizeof(victimName) - 1] = '\0';

    priority = level;
    dangerScore = danger;
    signalScore = signal;
    distanceRisk = risk;

    locationId = location;
    exitId = selectedExit;
    routeDistance = distance;
}

// MaxHeap constructor
MaxHeap::MaxHeap(int maxSize)
{
    capacity = maxSize;
    heapSize = 0;

    if (capacity > 0)
        heapArray = new RescueCase[capacity];
    else
        heapArray = nullptr;
}

// Destructor
MaxHeap::~MaxHeap()
{
    delete[] heapArray;
}

// Compare rescue cases
bool MaxHeap::hasHigherPriority(
    const RescueCase &first,
    const RescueCase &second) const
{
    // Rule 1: Higher priority score first
    if (first.priority != second.priority)
        return first.priority > second.priority;

    // Rule 2: Higher danger score first
    if (first.dangerScore != second.dangerScore)
        return first.dangerScore > second.dangerScore;

    // Rule 3: Shorter route first
    if (first.routeDistance != second.routeDistance)
    {
        if (first.routeDistance == -1)
            return false;

        if (second.routeDistance == -1)
            return true;

        return first.routeDistance < second.routeDistance;
    }

    return false;
}

// Swap two cases
void MaxHeap::swapCases(int first, int second)
{
    RescueCase temp = heapArray[first];
    heapArray[first] = heapArray[second];
    heapArray[second] = temp;
}

// Move higher priority upward
void MaxHeap::heapifyUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;

        if (hasHigherPriority(
                heapArray[index],
                heapArray[parent]))
        {
            swapCases(index, parent);
            index = parent;
        }
        else
        {
            break;
        }
    }
}

// Move higher priority downward
void MaxHeap::heapifyDown(int index)
{
    while (true)
    {
        int largest = index;

        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heapSize &&
            hasHigherPriority(
                heapArray[left],
                heapArray[largest]))
        {
            largest = left;
        }

        if (right < heapSize &&
            hasHigherPriority(
                heapArray[right],
                heapArray[largest]))
        {
            largest = right;
        }

        if (largest != index)
        {
            swapCases(index, largest);
            index = largest;
        }
        else
        {
            break;
        }
    }
}

// Insert rescue case
bool MaxHeap::insert(const RescueCase &rescue)
{
    if (heapSize >= capacity)
    {
        cout << "Heap Overflow!" << endl;
        return false;
    }

    if (rescue.priority < 1 ||
        rescue.priority > 100)
    {
        cout << "Invalid priority! Use 1 to 100."
             << endl;
        return false;
    }

    int index = heapSize;

    heapArray[index] = rescue;
    heapSize++;

    heapifyUp(index);

    return true;
}

// Remove highest priority case
RescueCase MaxHeap::extractMax()
{
    if (isEmpty())
    {
        cout << "Heap Underflow!" << endl;
        return RescueCase();
    }

    RescueCase highest = heapArray[0];

    heapArray[0] = heapArray[heapSize - 1];
    heapSize--;

    if (heapSize > 0)
        heapifyDown(0);

    return highest;
}

// Check whether heap is empty
bool MaxHeap::isEmpty() const
{
    return heapSize == 0;
}

// Return current heap size
int MaxHeap::getSize() const
{
    return heapSize;
}
