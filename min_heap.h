
#ifndef MIN_HEAP_H
#define MIN_HEAP_H

struct HeapNode
{
    int vertexId;
    int distance;

    HeapNode()
    {
        vertexId = -1;
        distance = 0;
    }

    HeapNode(int id, int dist)
    {
        vertexId = id;
        distance = dist;
    }
};

class MinHeap
{
private:
    HeapNode *heapArray;
    int capacity;
    int heapSize;

    void swapNodes(int first, int second);
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    explicit MinHeap(int maxSize);
    ~MinHeap();

    MinHeap(const MinHeap &) = delete;
    MinHeap &operator=(const MinHeap &) = delete;

    bool insert(int vertexId, int distance);
    HeapNode extractMin();

    bool isEmpty() const;
    int getSize() const;
};

#endif
