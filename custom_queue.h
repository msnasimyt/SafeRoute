
#ifndef CUSTOM_QUEUE_H
#define CUSTOM_QUEUE_H

class CustomQueue
{
private:
    int *queueArray;
    int capacity;
    int front;
    int rear;
    int count;

public:
    explicit CustomQueue(int maxSize);
    ~CustomQueue();

    CustomQueue(const CustomQueue &) = delete;
    CustomQueue &operator=(const CustomQueue &) = delete;

    bool enqueue(int value);
    int dequeue();

    bool isEmpty() const;
    bool isFull() const;
    int getSize() const;
};

#endif
