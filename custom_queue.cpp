
#include "custom_queue.h"
#include <iostream>

using namespace std;

// Constructor
CustomQueue::CustomQueue(int maxSize)
{
    capacity = maxSize;

    front = 0;
    rear = -1;
    count = 0;

    if (capacity > 0)
    {
        queueArray = new int[capacity];
    }
    else
    {
        queueArray = nullptr;
    }
}

// Destructor
CustomQueue::~CustomQueue()
{
    delete[] queueArray;
}

// Insert an element
bool CustomQueue::enqueue(int value)
{

    if (isFull())
    {
        cout << "Queue Overflow!" << endl;
        return false;
    }

    rear = (rear + 1) % capacity;

    queueArray[rear] = value;

    count++;

    return true;
}

// Remove an element
int CustomQueue::dequeue()
{

    if (isEmpty())
    {
        cout << "Queue Underflow!" << endl;
        return -1;
    }

    int value = queueArray[front];

    front = (front + 1) % capacity;

    count--;

    return value;
}

// Check whether queue is empty
bool CustomQueue::isEmpty() const
{
    return count == 0;
}

// Check whether queue is full
bool CustomQueue::isFull() const
{
    return count == capacity;
}

// Return current size
int CustomQueue::getSize() const
{
    return count;
}
