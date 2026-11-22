
#include "min_heap.h"
#include <iostream>

using namespace std;

// Constructor
MinHeap::MinHeap(int maxSize)
{
    capacity = maxSize;
    heapSize = 0;

    if (capacity > 0)
    {
        heapArray = new HeapNode[capacity];
    }
    else
    {
        heapArray = nullptr;
    }
}

// Destructor
MinHeap::~MinHeap()
{
    delete[] heapArray;
}

// Swap two nodes
void MinHeap::swapNodes(int first, int second)
{
    HeapNode temp = heapArray[first];
    heapArray[first] = heapArray[second];
    heapArray[second] = temp;
}

// Move a node upward
void MinHeap::heapifyUp(int index)
{
    while (index > 0)
    {
        int parent = (index - 1) / 2;

        if (heapArray[index].distance <
            heapArray[parent].distance)
        {

            swapNodes(index, parent);
            index = parent;
        }
        else
        {
            break;
        }
    }
}

// Move a node downward
void MinHeap::heapifyDown(int index)
{
    while (true)
    {
        int smallest = index;

        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heapSize &&
            heapArray[left].distance <
                heapArray[smallest].distance)
        {

            smallest = left;
        }

        if (right < heapSize &&
            heapArray[right].distance <
                heapArray[smallest].distance)
        {

            smallest = right;
        }

        if (smallest != index)
        {
            swapNodes(index, smallest);
            index = smallest;
        }
        else
        {
            break;
        }
    }
}

// Insert a new node
bool MinHeap::insert(int vertexId, int distance)
{
    if (heapSize >= capacity)
    {
        cout << "Heap Overflow!" << endl;
        return false;
    }

    if (distance < 0)
    {
        cout << "Invalid distance!" << endl;
        return false;
    }

    int index = heapSize;

    heapArray[index] = HeapNode(vertexId, distance);
    heapSize++;

    heapifyUp(index);

    return true;
}

// Remove and return the minimum node
HeapNode MinHeap::extractMin()
{
    if (isEmpty())
    {
        cout << "Heap Underflow!" << endl;
        return HeapNode(-1, -1);
    }

    HeapNode minimum = heapArray[0];

    heapArray[0] = heapArray[heapSize - 1];
    heapSize--;

    if (heapSize > 0)
    {
        heapifyDown(0);
    }

    return minimum;
}

// Check whether heap is empty
bool MinHeap::isEmpty() const
{
    return heapSize == 0;
}

// Return current heap size
int MinHeap::getSize() const
{
    return heapSize;
}
