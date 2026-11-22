#include "mapwidget.h"
#include "graph.h"

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsPathItem>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QFrame>
#include <QColor>
#include <QSizePolicy>
#include <QLineF>
#include <QPointF>
#include <QPolygonF>
#include <QtMath>

MapWidget::MapWidget(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)),
      graph(nullptr), shortestPath(nullptr), pathLength(0)
{
    setScene(scene);
    setFrameShape(QFrame::NoFrame);
    setBackgroundBrush(QColor("#C6CCD1"));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignCenter);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::TextAntialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(0, 0);
    scene->setSceneRect(0, 0, 1040, 700);
}

MapWidget::~MapWidget() { delete[] shortestPath; }

void MapWidget::setGraph(const Graph *newGraph)
{
    graph = newGraph;
    drawMap();
}

void MapWidget::setShortestPath(const int *path, int length)
{
    delete[] shortestPath;
    shortestPath = nullptr;
    pathLength = 0;
    if (path && length > 0)
    {
        shortestPath = new int[length];
        for (int i = 0; i < length; ++i)
            shortestPath[i] = path[i];
        pathLength = length;
    }
    drawMap();
}

bool MapWidget::isPathEdge(int source, int destination) const
{
    for (int i = 0; i < pathLength - 1; ++i)
        if ((shortestPath[i] == source && shortestPath[i + 1] == destination) ||
            (shortestPath[i] == destination && shortestPath[i + 1] == source))
            return true;
    return false;
}

void MapWidget::drawMap()
{
    scene->clear();
    scene->setSceneRect(0, 0, 1040, 700);
    if (!graph || graph->getVertexCount() <= 0)
    {
        fitMap();
        return;
    }

    // Hand-sketched campus arrangement: organic, open layout (NOT a rigid rectangle).
    // The overall campus footprint remains broad and rectangular, while each
    // landmark follows the relative position in the user's reference sketch.
    // Coordinates are top-down screen positions; graph edges are drawn separately.
    const QPointF positions[] = {
        {105, 385}, // 0 Main Entrance - far west, vertically centered
        {315, 145}, // 1 Reception - upper-left
        {525, 125}, // 2 Main Hall - upper-middle
        {735, 220}, // 3 Computer Lab - upper-right-middle
        {900, 285}, // 4 Library - right side, above Admin
        {350, 590}, // 5 Cafeteria - lower-left
        {525, 355}, // 6 Central Hub - central junction
        {745, 475}, // 7 Science Block - lower-right-middle
        {915, 505}, // 8 Admin Block - right-lower
        {565, 625}, // 9 Parking Area - bottom-middle
        {900, 85},  // 10 North Exit - top-right
        {900, 650}  // 11 South Exit - bottom-right
    };
    const QString icons[] = {"⇧", "i", "▤", "⌘", "▣", "☕", "✥", "⚗", "▤", "P", "N", "S"};
    const QString iconNames[] = {"GATE", "INFO", "HALL", "LAB", "LIBRARY", "CAFE", "HUB", "SCIENCE", "ADMIN", "PARKING", "NORTH EXIT", "SOUTH EXIT"};
    const QColor colors[] = {QColor("#38A8FF"), QColor("#27C6D7"), QColor("#E5B85C"), QColor("#A78BFA"), QColor("#60A5FA"), QColor("#F59E0B"), QColor("#20C997"), QColor("#B58AF5"), QColor("#D69A45"), QColor("#93A4B8"), QColor("#16C784"), QColor("#16C784")};
    const int count = qMin(graph->getVertexCount(), 12);

    // QGraphicsScene has no addRoundedRect() method. Build rounded rectangles
    // with QPainterPath and add them as normal scene path items instead.
    auto addRoundedRect = [&](const QRectF &rect, qreal xRadius, qreal yRadius,
                              const QPen &pen, const QBrush &brush) -> QGraphicsPathItem *
    {
        QPainterPath path;
        path.addRoundedRect(rect, xRadius, yRadius);
        return scene->addPath(path, pen, brush);
    };

    // Clean, high-contrast map background. Keep decoration minimal so the
    // campus layout and every connection remain easy to understand.
    scene->addRect(0, 0, 1040, 700, QPen(Qt::NoPen),
                   QBrush(QColor("#C6CCD1")))
        ->setZValue(-25);

    // A very subtle campus boundary gives the map a clear visual frame.
    QPen boundaryPen(QColor("#8E999F"), 2, Qt::DashLine);
    auto *boundary = scene->addRect(QRectF(18, 18, 1004, 664),
                                    boundaryPen, QBrush(Qt::NoBrush));
    boundary->setZValue(-20);

    // Roads are simple, clean lines. Each road displays its weight/distance
    // in a small high-contrast label. Blocked roads are red dashed; safe route
    // is highlighted in emerald.
    for (int i = 0; i < count; ++i)
    {
        const Vertex *v = graph->getVertex(i);
        if (!v)
            continue;
        for (const Edge *e = v->head; e; e = e->next)
        {
            const int d = e->destination;
            if (d <= i || d >= count)
                continue;

            const QPointF a = positions[i];
            const QPointF b = positions[d];
            const bool route = isPathEdge(i, d);

            // Draw every road with a dark outer casing so it stays visible
            // against the grey ground. The selected safe route gets three
            // layers: dark border, vivid green body and a fine mint highlight.
            if (route && !e->blocked)
            {
                auto *routeBorder = scene->addLine(
                    QLineF(a, b),
                    QPen(QColor("#07543D"), 13.0, Qt::SolidLine,
                         Qt::RoundCap, Qt::RoundJoin));
                routeBorder->setZValue(0);

                auto *routeBody = scene->addLine(
                    QLineF(a, b),
                    QPen(QColor("#00D878"), 8.5, Qt::SolidLine,
                         Qt::RoundCap, Qt::RoundJoin));
                routeBody->setZValue(1);

                auto *routeHighlight = scene->addLine(
                    QLineF(a, b),
                    QPen(QColor("#A7FFD8"), 2.0, Qt::SolidLine,
                         Qt::RoundCap, Qt::RoundJoin));
                routeHighlight->setZValue(2);
            }
            else
            {
                QColor roadColor = e->blocked ? QColor("#E53945")
                                              : QColor("#59666E");
                QColor casingColor = e->blocked ? QColor("#762B32")
                                                : QColor("#34434B");
                Qt::PenStyle roadStyle = e->blocked ? Qt::DashLine
                                                    : Qt::SolidLine;

                auto *roadCasing = scene->addLine(
                    QLineF(a, b),
                    QPen(casingColor, 7.0, Qt::SolidLine,
                         Qt::RoundCap, Qt::RoundJoin));
                roadCasing->setZValue(-4);

                auto *roadSurface = scene->addLine(
                    QLineF(a, b),
                    QPen(roadColor, 3.5, roadStyle,
                         Qt::RoundCap, Qt::RoundJoin));
                roadSurface->setZValue(-3);
            }

            // Restore road-distance labels. Place each label slightly away
            // from the road midpoint so it remains readable on the clean map.
            const QPointF delta = b - a;
            const qreal length = qMax<qreal>(1.0, QLineF(a, b).length());
            const QPointF normal(-delta.y() / length, delta.x() / length);
            const QPointF mid = (a + b) / 2.0 + normal * 13.0;

            QGraphicsTextItem *distanceText = scene->addText(
                QString::number(e->weight) + QStringLiteral(" m"),
                QFont("Segoe UI", 9, QFont::Bold));
            distanceText->setDefaultTextColor(QColor("#23343D"));
            const QRectF textRect = distanceText->boundingRect();
            const QRectF chipRect(mid.x() - textRect.width() / 2.0 - 5.0,
                                  mid.y() - textRect.height() / 2.0 - 2.0,
                                  textRect.width() + 10.0,
                                  textRect.height() + 4.0);
            auto *chip = addRoundedRect(chipRect, 5, 5,
                                        QPen(QColor("#AAB8BD"), 0.8),
                                        QBrush(QColor("#FFFFFF")));
            chip->setZValue(4);
            distanceText->setPos(mid.x() - textRect.width() / 2.0,
                                 mid.y() - textRect.height() / 2.0);
            distanceText->setZValue(5);
        }
    }

    // Building / landmark icons: draw category-specific vector pictograms.
    // Node positions, road connections, route highlighting and graph logic stay unchanged.
    auto addIconLine = [&](const QPointF &a, const QPointF &b, const QColor &color,
                           qreal width = 2.2)
    {
        QPen pen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        auto *item = scene->addLine(QLineF(a, b), pen);
        item->setZValue(7);
        return item;
    };
    auto addIconRect = [&](const QRectF &r, const QColor &color, qreal width = 2.0,
                           bool filled = false)
    {
        QPen pen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        QBrush brush = filled ? QBrush(color) : QBrush(Qt::NoBrush);
        auto *item = scene->addRect(r, pen, brush);
        item->setZValue(7);
        return item;
    };
    auto addIconEllipse = [&](const QRectF &r, const QColor &color, qreal width = 2.0,
                              bool filled = false)
    {
        QPen pen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        QBrush brush = filled ? QBrush(color) : QBrush(Qt::NoBrush);
        auto *item = scene->addEllipse(r, pen, brush);
        item->setZValue(7);
        return item;
    };

    // Orthographic top-down campus buildings and outdoor facilities.
    // Decorative top-down scene items only; graph vertices and road logic are unchanged.
    auto addPoly = [&](const QPolygonF &poly, const QColor &fill,
                       const QColor &outline, qreal outlineWidth = 1.0, qreal z = 0)
    {
        auto *item = scene->addPolygon(poly, QPen(outline, outlineWidth), QBrush(fill));
        item->setZValue(z);
        return item;
    };

    auto addBuilding = [&](const QPointF &c, int type, const QColor &accent)
    {
        // True top-down footprint: no perspective walls or angled camera projection.
        // Layered pavement, shadow, roof, parapet, skylights and rooftop equipment.
        const QRectF footprint(c.x() - 52, c.y() - 32, 104, 64);
        addRoundedRect(QRectF(c.x() - 59, c.y() - 39, 118, 78), 9, 9,
                       QPen(QColor("#A9B7A0"), 1.0), QBrush(QColor("#B9B9A2")))
            ->setZValue(-1);
        addRoundedRect(QRectF(c.x() - 55, c.y() - 35, 110, 70), 7, 7,
                       QPen(QColor(0, 0, 0, 75), 2), QBrush(QColor(0, 0, 0, 65)))
            ->setZValue(0);

        QColor roof = type == 3   ? QColor("#657F91")
                      : type == 4 ? QColor("#827D70")
                      : type == 5 ? QColor("#B18A5E")
                      : type == 7 ? QColor("#718A83")
                      : type == 8 ? QColor("#A58A68")
                                  : QColor("#9AA9A3");
        addRoundedRect(footprint, 5, 5, QPen(QColor("#E1E7DB"), 2), QBrush(roof))->setZValue(2);

        // Roof border and subtle roof-panel seams.
        addRoundedRect(QRectF(c.x() - 47, c.y() - 27, 94, 54), 3, 3,
                       QPen(QColor("#536B68"), 1.2), QBrush(Qt::NoBrush))
            ->setZValue(3);
        QPen seam(QColor(235, 243, 232, 125), 1.0);
        scene->addLine(c.x() - 40, c.y() - 8, c.x() + 40, c.y() - 8, seam)->setZValue(3);
        scene->addLine(c.x() - 40, c.y() + 9, c.x() + 40, c.y() + 9, seam)->setZValue(3);

        // Rooftop skylights, HVAC units and solar/utility panels.
        for (int k = 0; k < 3; ++k)
        {
            QRectF panel(c.x() - 34 + k * 25, c.y() - 22, 17, 10);
            auto *glass = addRoundedRect(panel, 2, 2,
                                         QPen(QColor("#D1E7E2"), 0.8), QBrush(QColor("#477D88")));
            glass->setZValue(4);
            scene->addLine(panel.left() + 3, panel.center().y(), panel.right() - 3,
                           panel.center().y(), QPen(QColor("#9AC8C4"), 0.7))
                ->setZValue(5);
        }
        addRoundedRect(QRectF(c.x() - 12, c.y() + 3, 24, 17), 2, 2,
                       QPen(QColor("#52696A"), 1), QBrush(QColor("#D1D8C9")))
            ->setZValue(4);
        scene->addLine(c.x() - 7, c.y() + 8, c.x() + 7, c.y() + 8, QPen(QColor("#849894"), 1))->setZValue(5);
        scene->addLine(c.x() - 7, c.y() + 13, c.x() + 7, c.y() + 13, QPen(QColor("#849894"), 1))->setZValue(5);
        Q_UNUSED(accent);
    };

    for (int i = 0; i < count; ++i)
    {
        const Vertex *v = graph->getVertex(i);
        if (!v)
            continue;

        const QPointF c = positions[i];
        // Campus buildings: reception, academic blocks, library, cafeteria and science/admin.
        if (i == 1 || i == 2 || i == 3 || i == 4 || i == 5 || i == 7 || i == 8)
            addBuilding(c, i, colors[i]);
        else if (i == 6) // central landscaped plaza
        {
            scene->addEllipse(c.x() - 34, c.y() - 20, 68, 40, QPen(QColor("#B6D9C3"), 1.2), QBrush(QColor("#356E58")))->setZValue(0);
            scene->addEllipse(c.x() - 16, c.y() - 10, 32, 20, QPen(QColor("#B5E9E5"), 1.2), QBrush(QColor("#278C91")))->setZValue(1);
        }
        else if (i == 9) // parking bays and cars
        {
            addRoundedRect(QRectF(c.x() - 43, c.y() - 24, 86, 48), 5, 5, QPen(QColor("#A7B4B2"), 1), QBrush(QColor("#46545A")))->setZValue(0);
            for (int bay = 0; bay < 3; ++bay)
            {
                qreal bx = c.x() - 31 + bay * 25;
                scene->addRect(QRectF(bx, c.y() - 16, 18, 32), QPen(QColor("#D5DED8"), 0.8), QBrush(Qt::NoBrush))->setZValue(1);
                addRoundedRect(QRectF(bx + 3, c.y() - 10, 12, 20), 3, 3, QPen(QColor("#263A42"), 0.8), QBrush(bay == 1 ? QColor("#D5A14A") : QColor("#4C91A5")))->setZValue(2);
            }
        }

        bool onRoute = false;
        for (int j = 0; j < pathLength; ++j)
            if (shortestPath[j] == i)
            {
                onRoute = true;
                break;
            }

        QColor col = onRoute ? QColor("#20E3A2") : colors[i];

        // Soft halo and compact building marker.
        scene->addEllipse(c.x() - 30, c.y() - 30, 60, 60,
                          QPen(Qt::NoPen),
                          QBrush(QColor(col.red(), col.green(), col.blue(), 32)))
            ->setZValue(4);

        auto *body = addRoundedRect(QRectF(c.x() - 21, c.y() - 21, 42, 42),
                                    14, 14, QPen(QColor("#E7F5EF"), 2),
                                    QBrush(QColor("#263943")));
        body->setZValue(5);
        body->setToolTip(QString::fromUtf8(v->name));

        auto *tile = addRoundedRect(QRectF(c.x() - 21, c.y() - 21, 42, 42),
                                    10, 10, QPen(col.lighter(135), 1.2),
                                    QBrush(col.darker(145)));
        tile->setZValue(6);
        tile->setToolTip(QString::fromUtf8(v->name));

        // White vector pictograms are intentionally simple and readable at map scale.
        const QColor ink("#F7FFFB");
        switch (i)
        {
        case 0: // Main Entrance: gate
            addIconLine({c.x() - 12, c.y() + 10}, {c.x() - 12, c.y() - 10}, ink, 3);
            addIconLine({c.x() + 12, c.y() + 10}, {c.x() + 12, c.y() - 10}, ink, 3);
            addIconLine({c.x() - 12, c.y() - 9}, {c.x() + 12, c.y() - 9}, ink, 3);
            addIconLine({c.x() - 7, c.y() - 5}, {c.x() - 7, c.y() + 10}, ink, 1.8);
            addIconLine({c.x() + 7, c.y() - 5}, {c.x() + 7, c.y() + 10}, ink, 1.8);
            break;
        case 1: // Reception: information desk and person
            addIconEllipse(QRectF(c.x() - 4, c.y() - 13, 8, 8), ink, 1.8, true);
            addIconLine({c.x(), c.y() - 4}, {c.x(), c.y() + 1}, ink, 2);
            addIconRect(QRectF(c.x() - 13, c.y() + 2, 26, 10), ink, 2, false);
            addIconLine({c.x() - 10, c.y() + 12}, {c.x() + 10, c.y() + 12}, ink, 2);
            break;
        case 2: // Main Hall: building with windows
        case 8: // Admin Block
            addIconRect(QRectF(c.x() - 13, c.y() - 8, 26, 20), ink, 2, false);
            addIconLine({c.x() - 16, c.y() - 8}, {c.x(), c.y() - 16}, ink, 2);
            addIconLine({c.x(), c.y() - 16}, {c.x() + 16, c.y() - 8}, ink, 2);
            for (int wx = -8; wx <= 8; wx += 8)
                addIconRect(QRectF(c.x() + wx - 2, c.y() - 3, 4, 5), ink, 1.3, false);
            addIconRect(QRectF(c.x() - 3, c.y() + 4, 6, 8), ink, 1.5, false);
            break;
        case 3: // Computer Lab: monitor
            addIconRect(QRectF(c.x() - 14, c.y() - 11, 28, 19), ink, 2, false);
            addIconLine({c.x(), c.y() + 8}, {c.x(), c.y() + 13}, ink, 2);
            addIconLine({c.x() - 8, c.y() + 13}, {c.x() + 8, c.y() + 13}, ink, 2);
            addIconLine({c.x() - 9, c.y() - 5}, {c.x() + 9, c.y() - 5}, ink, 1.2);
            break;
        case 4: // Library: books
            addIconRect(QRectF(c.x() - 13, c.y() - 12, 7, 24), ink, 1.5, false);
            addIconRect(QRectF(c.x() - 4, c.y() - 12, 7, 24), ink, 1.5, false);
            addIconRect(QRectF(c.x() + 5, c.y() - 12, 8, 24), ink, 1.5, false);
            addIconLine({c.x() - 13, c.y() + 5}, {c.x() + 13, c.y() + 5}, ink, 1.2);
            break;
        case 5: // Cafeteria: cup and steam
            addIconLine({c.x() - 8, c.y() - 13}, {c.x() - 5, c.y() - 7}, ink, 1.8);
            addIconLine({c.x(), c.y() - 13}, {c.x() + 2, c.y() - 7}, ink, 1.8);
            addIconLine({c.x() + 8, c.y() - 13}, {c.x() + 6, c.y() - 7}, ink, 1.8);
            addIconLine({c.x() - 11, c.y() - 4}, {c.x() + 9, c.y() - 4}, ink, 2);
            addIconLine({c.x() - 9, c.y() - 3}, {c.x() - 6, c.y() + 10}, ink, 2);
            addIconLine({c.x() - 6, c.y() + 10}, {c.x() + 7, c.y() + 10}, ink, 2);
            addIconLine({c.x() + 7, c.y() + 10}, {c.x() + 9, c.y() - 3}, ink, 2);
            addIconEllipse(QRectF(c.x() + 7, c.y() - 2, 8, 8), ink, 1.8, false);
            break;
        case 6: // Central Hub: connected junction
            addIconEllipse(QRectF(c.x() - 6, c.y() - 6, 12, 12), ink, 2, false);
            addIconEllipse(QRectF(c.x() - 2, c.y() - 2, 4, 4), ink, 1, true);
            addIconLine({c.x(), c.y() - 7}, {c.x(), c.y() - 14}, ink, 2);
            addIconLine({c.x(), c.y() + 7}, {c.x(), c.y() + 14}, ink, 2);
            addIconLine({c.x() - 7, c.y()}, {c.x() - 14, c.y()}, ink, 2);
            addIconLine({c.x() + 7, c.y()}, {c.x() + 14, c.y()}, ink, 2);
            break;
        case 7: // Science Block: laboratory flask
            addIconLine({c.x() - 5, c.y() - 13}, {c.x() + 5, c.y() - 13}, ink, 2);
            addIconLine({c.x() - 3, c.y() - 13}, {c.x() - 3, c.y() - 3}, ink, 2);
            addIconLine({c.x() + 3, c.y() - 13}, {c.x() + 3, c.y() - 3}, ink, 2);
            addIconLine({c.x() - 3, c.y() - 3}, {c.x() - 12, c.y() + 10}, ink, 2);
            addIconLine({c.x() + 3, c.y() - 3}, {c.x() + 12, c.y() + 10}, ink, 2);
            addIconLine({c.x() - 12, c.y() + 10}, {c.x() + 12, c.y() + 10}, ink, 2);
            addIconLine({c.x() - 8, c.y() + 4}, {c.x() + 8, c.y() + 4}, ink, 1.5);
            break;
        case 9: // Parking: letter P inside parking sign
            addIconRect(QRectF(c.x() - 13, c.y() - 13, 26, 26), ink, 2, false);
            {
                auto *pText = scene->addText("P", QFont("Segoe UI", 19, QFont::Bold));
                pText->setDefaultTextColor(ink);
                QRectF pr = pText->boundingRect();
                pText->setPos(c.x() - pr.width() / 2, c.y() - pr.height() / 2 - 1);
                pText->setZValue(7);
            }
            break;
        case 10: // North Exit: outward arrow
        case 11: // South Exit
            addIconLine({c.x() - 12, c.y()}, {c.x() + 10, c.y()}, ink, 2.8);
            addIconLine({c.x() + 3, c.y() - 8}, {c.x() + 11, c.y()}, ink, 2.8);
            addIconLine({c.x() + 3, c.y() + 8}, {c.x() + 11, c.y()}, ink, 2.8);
            addIconLine({c.x() - 12, c.y() - 10}, {c.x() - 12, c.y() + 10}, ink, 2);
            break;
        }

        // Keep labels below the marker, preserving the current node layout.
        QString name = QString::fromUtf8(v->name);
        QFont labelFont("Segoe UI", 9, QFont::DemiBold);
        QFontMetricsF fm(labelFont);
        qreal w = qMax<qreal>(fm.horizontalAdvance(name),
                              fm.horizontalAdvance(iconNames[i])) +
                  18;
        QRectF labelRect(c.x() - w / 2, c.y() + 34, w, 34);
        auto *labelBg = addRoundedRect(labelRect, 8, 8,
                                       QPen(QColor("#AAB9BD"), 0.8),
                                       QBrush(QColor("#263943")));
        labelBg->setZValue(8);

        auto *nameText = scene->addText(name, labelFont);
        nameText->setDefaultTextColor(QColor("#FFFFFF"));
        QRectF nr = nameText->boundingRect();
        nameText->setPos(c.x() - nr.width() / 2, c.y() + 35);
        nameText->setZValue(9);

        QFont smallFont("Segoe UI", 7, QFont::Bold);
        auto *kind = scene->addText(iconNames[i], smallFont);
        kind->setDefaultTextColor(QColor("#B9DCD0"));
        QRectF kr = kind->boundingRect();
        kind->setPos(c.x() - kr.width() / 2, c.y() + 49);
        kind->setZValue(9);
    }

    // Small north indicator.
    auto *north = scene->addText("N  ↑", QFont("Segoe UI", 10, QFont::Bold));
    north->setDefaultTextColor(QColor("#334B55"));
    north->setPos(985, 18);
    north->setZValue(20);
    scene->setSceneRect(0, 0, 1040, 700);
    fitMap();
}

void MapWidget::zoomIn() { scale(1.15, 1.15); }
void MapWidget::zoomOut() { scale(1.0 / 1.15, 1.0 / 1.15); }
void MapWidget::fitMap()
{
    if (!scene || scene->sceneRect().isEmpty() || viewport()->size().isEmpty())
        return;
    resetTransform();
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}
void MapWidget::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitMap();
}
