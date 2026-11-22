
#ifndef MAX_HEAP_H
#define MAX_HEAP_H

struct RescueCase
{
    int caseId;
    char victimName[50];

    int priority;
    int dangerScore;
    int signalScore;
    int distanceRisk;

    int locationId;
    int exitId;
    int routeDistance;

    RescueCase()
    {
        caseId = -1;
        victimName[0] = '\0';

        priority = 0;
        dangerScore = 0;
        signalScore = 0;
        distanceRisk = 0;

        locationId = -1;
        exitId = -1;
        routeDistance = -1;
    }

    RescueCase(int id, const char *name,
               int level, int location,
               int danger = 0,
               int signal = 0,
               int risk = 0,
               int selectedExit = -1,
               int distance = -1);
};

class MaxHeap
{
private:
    RescueCase *heapArray;
    int capacity;
    int heapSize;

    void swapCases(int first, int second);
    void heapifyUp(int index);
    void heapifyDown(int index);

    // Compare two rescue cases
    bool hasHigherPriority(
        const RescueCase &first,
        const RescueCase &second) const;

public:
    explicit MaxHeap(int maxSize);
    ~MaxHeap();

    MaxHeap(const MaxHeap &) = delete;
    MaxHeap &operator=(const MaxHeap &) = delete;

    bool insert(const RescueCase &rescue);
    RescueCase extractMax();

    bool isEmpty() const;
    int getSize() const;
};

#endif
