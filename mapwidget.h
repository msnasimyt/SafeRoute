#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QGraphicsView>

class QGraphicsScene;
class Graph;
class QResizeEvent;

class MapWidget : public QGraphicsView
{
public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

    void setGraph(const Graph *graph);
    void setShortestPath(const int *path, int pathLength);

    void zoomIn();
    void zoomOut();
    void fitMap();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsScene *scene;
    const Graph *graph;
    int *shortestPath;
    int pathLength;

    void drawMap();
    bool isPathEdge(int source, int destination) const;
};

#endif
