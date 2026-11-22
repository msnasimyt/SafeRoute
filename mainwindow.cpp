#include "mainwindow.h"
#include "mapwidget.h"
#include "dijkstra.h"

#include "max_heap.h"
#include <QTabWidget>

#include <QLineEdit>
#include <QSpinBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QScrollArea>
#include <QMessageBox>

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QStringList>
#include <QSizePolicy>
#include "bfs.h"
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      graph(12),
      mapWidget(nullptr),
      pageStack(nullptr),
      statusLabel(nullptr),
      routeInfoLabel(nullptr),
      sourceCombo(nullptr),
      roadSourceCombo(nullptr),
      roadDestinationCombo(nullptr),
      rescueHeap(new MaxHeap(100)),
      victimNameInput(nullptr),
      priorityInput(nullptr),
      rescueLocationCombo(nullptr),
      dangerModeCombo(nullptr),
      dangerLevelCombo(nullptr),
      signalModeCombo(nullptr),
      signalLevelCombo(nullptr),
      rescueList(nullptr),
      rescueCountLabel(nullptr),
      rescuedList(nullptr),
      rescuedCountLabel(nullptr),
      nextCaseId(1)
{
    setupSampleGraph();
    setupUI();
}

MainWindow::~MainWindow()
{
    delete rescueHeap;
}

void MainWindow::setupSampleGraph()
{
    // 12 locations in a small campus / emergency evacuation map.
    graph.addVertex(0, "Main Entrance");
    graph.addVertex(1, "Reception");
    graph.addVertex(2, "Main Hall");
    graph.addVertex(3, "Computer Lab");
    graph.addVertex(4, "Library");
    graph.addVertex(5, "Cafeteria");
    graph.addVertex(6, "Central Hub");
    graph.addVertex(7, "Science Block");
    graph.addVertex(8, "Admin Block");
    graph.addVertex(9, "Parking Area");
    graph.addVertex(10, "North Exit");
    graph.addVertex(11, "South Exit");

    // Main corridors and connecting passages (distance in meters).
    graph.addRoad(0, 1, 4);
    graph.addRoad(0, 5, 6);
    graph.addRoad(1, 2, 3);
    graph.addRoad(1, 6, 5);
    graph.addRoad(2, 3, 4);
    graph.addRoad(2, 6, 2);
    graph.addRoad(2, 10, 8);
    graph.addRoad(3, 4, 3);
    graph.addRoad(3, 7, 5);
    graph.addRoad(3, 10, 4);
    graph.addRoad(4, 8, 5);
    graph.addRoad(4, 10, 4);
    graph.addRoad(5, 6, 3);
    graph.addRoad(5, 9, 5);
    graph.addRoad(6, 7, 4);
    graph.addRoad(6, 9, 3);
    graph.addRoad(7, 8, 3);
    graph.addRoad(7, 11, 5);
    graph.addRoad(8, 11, 4);
    graph.addRoad(9, 11, 6);
}

QPushButton *MainWindow::createButton(
    const QString &text,
    const QString &accent)
{
    QPushButton *button = new QPushButton(text);
    button->setCursor(Qt::PointingHandCursor);
    button->setMinimumHeight(36);
    button->setStyleSheet(
        "QPushButton {"
        " background-color: " +
        accent + ";"
                 " color: #F8FAFC;"
                 " border: 1px solid rgba(255,255,255,22);"
                 " border-radius: 8px;"
                 " padding: 7px 12px;"
                 " font-size: 12px;"
                 " font-weight: 600;"
                 "}"
                 "QPushButton:hover {"
                 " background-color: #0F9F78;"
                 "}"
                 "QPushButton:pressed {"
                 " background-color: #047857;"
                 "}");
    return button;
}

void MainWindow::setupUI()
{
    setWindowTitle("SafeRoute | Emergency Escape & Rescue Planner");
    resize(1440, 900);
    setMinimumSize(1050, 680);

    setStyleSheet(
        "QWidget {"
        " background-color: #0A1020;"
        " color: #E5EDF7;"
        " font-family: 'Segoe UI';"
        " font-size: 12px;"
        "}"
        "QLabel { background: transparent; }"
        "QComboBox {"
        " background-color: #0C1729;"
        " color: #E5EDF7;"
        " border: 1px solid #2A3A52;"
        " border-radius: 7px;"
        " padding: 7px 9px;"
        " min-height: 22px;"
        "}"
        "QComboBox:hover { border: 1px solid #10B981; }"
        "QComboBox QAbstractItemView {"
        " background-color: #142238;"
        " color: #E5EDF7;"
        " selection-background-color: #087F5B;"
        " border: 1px solid #2A3A52;"
        "}");

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(18, 14, 18, 14);
    root->setSpacing(12);

    // Compact top header
    QFrame *header = new QFrame;
    header->setObjectName("topHeader");
    header->setStyleSheet(
        "QFrame#topHeader {"
        " background-color: #111D30;"
        " border: 1px solid #263750;"
        " border-radius: 12px;"
        "}");
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(16, 10, 16, 10);
    headerLayout->setSpacing(14);

    QLabel *brandMark = new QLabel("SR");
    brandMark->setAlignment(Qt::AlignCenter);
    brandMark->setFixedSize(42, 42);
    brandMark->setStyleSheet(
        "background-color: #087F5B;"
        "color: white;"
        "font-size: 17px;"
        "font-weight: 800;"
        "border-radius: 10px;");

    QVBoxLayout *brandTextLayout = new QVBoxLayout;
    brandTextLayout->setSpacing(1);
    QLabel *brand = new QLabel("SafeRoute");
    brand->setStyleSheet("font-size: 19px; font-weight: 800; color: #F8FAFC;");
    QLabel *tagline = new QLabel("EMERGENCY ESCAPE & RESCUE PLANNER");
    tagline->setStyleSheet("font-size: 9px; letter-spacing: 1px; color: #8293AA;");
    brandTextLayout->addWidget(brand);
    brandTextLayout->addWidget(tagline);

    headerLayout->addWidget(brandMark);
    headerLayout->addLayout(brandTextLayout);
    headerLayout->addStretch();

    auto addMetric = [headerLayout](const QString &number, const QString &caption)
    {
        QFrame *metric = new QFrame;
        metric->setStyleSheet(
            "QFrame { background-color: #0B1728; border: 1px solid #263750; border-radius: 8px; }");
        QVBoxLayout *metricLayout = new QVBoxLayout(metric);
        metricLayout->setContentsMargins(12, 5, 12, 5);
        metricLayout->setSpacing(0);
        QLabel *value = new QLabel(number);
        value->setAlignment(Qt::AlignCenter);
        value->setStyleSheet("color: #F8FAFC; font-size: 16px; font-weight: 800; border: none;");
        QLabel *label = new QLabel(caption);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: #8293AA; font-size: 9px; border: none;");
        metricLayout->addWidget(value);
        metricLayout->addWidget(label);
        headerLayout->addWidget(metric);
    };

    addMetric("12", "LOCATIONS");
    addMetric("20", "ROADS");
    addMetric("02", "EXITS");

    QLabel *online = new QLabel("●  SYSTEM ONLINE");
    online->setAlignment(Qt::AlignCenter);
    online->setStyleSheet(
        "background-color: #073D34;"
        "color: #6EE7B7;"
        "border: 1px solid #12634F;"
        "border-radius: 8px;"
        "padding: 9px 12px;"
        "font-size: 10px;"
        "font-weight: 800;");
    headerLayout->addWidget(online);
    root->addWidget(header);

    // Main workspace: large map + compact control rail
    QHBoxLayout *workspace = new QHBoxLayout;
    workspace->setSpacing(12);

    QFrame *mapPanel = new QFrame;
    mapPanel->setObjectName("mapPanel");
    mapPanel->setStyleSheet(
        "QFrame#mapPanel {"
        " background-color: #101B2D;"
        " border: 1px solid #263750;"
        " border-radius: 13px;"
        "}");
    QVBoxLayout *mapLayout = new QVBoxLayout(mapPanel);
    mapLayout->setContentsMargins(12, 10, 12, 12);
    mapLayout->setSpacing(8);

    QHBoxLayout *mapToolbar = new QHBoxLayout;
    QVBoxLayout *mapHeadingLayout = new QVBoxLayout;
    mapHeadingLayout->setSpacing(1);
    QLabel *mapTitle = new QLabel("Live Emergency Map");
    mapTitle->setStyleSheet("font-size: 16px; font-weight: 800; color: #F8FAFC;");
    QLabel *mapSubtitle = new QLabel("Graph topology  /  Safe passage monitoring");
    mapSubtitle->setStyleSheet("font-size: 10px; color: #8293AA;");
    mapHeadingLayout->addWidget(mapTitle);
    mapHeadingLayout->addWidget(mapSubtitle);
    mapToolbar->addLayout(mapHeadingLayout);
    mapToolbar->addStretch();

    QLabel *legend = new QLabel(
        "<span style='color:#3B82F6'>●</span> Entrance&nbsp;&nbsp;"
        "<span style='color:#F59E0B'>●</span> Location&nbsp;&nbsp;"
        "<span style='color:#10B981'>●</span> Exit&nbsp;&nbsp;"
        "<span style='color:#EF4444'>━</span> Blocked");
    legend->setStyleSheet("font-size: 10px; color: #A9B8CA;");
    mapToolbar->addWidget(legend);

    QPushButton *zoomOutButton = createButton("−", "#1B2B42");
    QPushButton *zoomInButton = createButton("+", "#1B2B42");
    QPushButton *fitButton = createButton("FIT MAP", "#145B4B");
    zoomOutButton->setFixedWidth(36);
    zoomInButton->setFixedWidth(36);
    mapToolbar->addWidget(zoomOutButton);
    mapToolbar->addWidget(zoomInButton);
    mapToolbar->addWidget(fitButton);
    mapLayout->addLayout(mapToolbar);

    mapWidget = new MapWidget;
    mapWidget->setGraph(&graph);
    mapWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mapLayout->addWidget(mapWidget, 1);

    connect(zoomInButton, &QPushButton::clicked, mapWidget, &MapWidget::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, mapWidget, &MapWidget::zoomOut);
    connect(fitButton, &QPushButton::clicked, this, &MainWindow::resetMapView);

    workspace->addWidget(mapPanel, 1);

    // Right-side control panel
    QFrame *controlPanel = new QFrame;
    controlPanel->setObjectName("controlPanel");
    controlPanel->setFixedWidth(292);
    controlPanel->setStyleSheet(
        "QFrame#controlPanel {"
        " background-color: #111D30;"
        " border: 1px solid #263750;"
        " border-radius: 13px;"
        "}");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
    controlLayout->setContentsMargins(13, 13, 13, 13);
    controlLayout->setSpacing(9);

    QLabel *controlTitle = new QLabel("CONTROL CENTER");
    controlTitle->setStyleSheet("font-size: 10px; font-weight: 800; letter-spacing: 1px; color: #8293AA;");
    controlLayout->addWidget(controlTitle);

    // Route planner card
    QFrame *routeCard = new QFrame;
    routeCard->setStyleSheet("QFrame { background-color: #0C1729; border: 1px solid #263750; border-radius: 10px; }");
    QVBoxLayout *routeLayout = new QVBoxLayout(routeCard);
    routeLayout->setContentsMargins(11, 11, 11, 11);
    routeLayout->setSpacing(7);

    QLabel *routeHeading = new QLabel("01  ROUTE PLANNER");
    routeHeading->setStyleSheet("font-size: 11px; font-weight: 800; color: #6EE7B7; border: none;");
    routeLayout->addWidget(routeHeading);

    routeLayout->addWidget(new QLabel("Starting Location"));

    sourceCombo = new QComboBox;
    routeLayout->addWidget(sourceCombo);

    const QStringList locations = {
        "Main Entrance",
        "Reception",
        "Main Hall",
        "Computer Lab",
        "Library",
        "Cafeteria",
        "Central Hub",
        "Science Block",
        "Admin Block",
        "Parking Area",
        "North Exit",
        "South Exit"};

    for (int i = 0; i < locations.size(); ++i)
    {
        sourceCombo->addItem(locations[i], i);
    }

    sourceCombo->setCurrentIndex(0);

    QPushButton *routeButton =
        createButton("Find Nearest Safe Exit", "#087F5B");
    routeLayout->addWidget(routeButton);
    controlLayout->addWidget(routeCard);

    // Road controls card
    QFrame *roadCard = new QFrame;
    roadCard->setStyleSheet("QFrame { background-color: #0C1729; border: 1px solid #263750; border-radius: 10px; }");
    QVBoxLayout *roadLayout = new QVBoxLayout(roadCard);
    roadLayout->setContentsMargins(11, 11, 11, 11);
    roadLayout->setSpacing(7);

    QLabel *roadHeading = new QLabel("02  ROAD CONTROL");
    roadHeading->setStyleSheet("font-size: 11px; font-weight: 800; color: #93C5FD; border: none;");
    roadLayout->addWidget(roadHeading);
    roadLayout->addWidget(new QLabel("Road start"));
    roadSourceCombo = new QComboBox;
    roadLayout->addWidget(roadSourceCombo);
    roadLayout->addWidget(new QLabel("Road end"));
    roadDestinationCombo = new QComboBox;
    roadLayout->addWidget(roadDestinationCombo);

    for (int i = 0; i < locations.size(); ++i)
    {
        roadSourceCombo->addItem(locations[i], i);
        roadDestinationCombo->addItem(locations[i], i);
    }
    roadSourceCombo->setCurrentIndex(0);
    roadDestinationCombo->setCurrentIndex(1);

    QHBoxLayout *roadButtons = new QHBoxLayout;
    QPushButton *blockButton = createButton("Block Road", "#9F343D");
    QPushButton *unblockButton = createButton("Restore Road", "#12634F");
    roadButtons->addWidget(blockButton);
    roadButtons->addWidget(unblockButton);
    roadLayout->addLayout(roadButtons);
    controlLayout->addWidget(roadCard);

    // Route result / system feedback
    QFrame *resultCard = new QFrame;
    resultCard->setStyleSheet("QFrame { background-color: #0C1729; border: 1px solid #263750; border-radius: 10px; }");
    QVBoxLayout *resultLayout = new QVBoxLayout(resultCard);
    resultLayout->setContentsMargins(11, 10, 11, 10);
    resultLayout->setSpacing(6);

    QLabel *resultHeading = new QLabel("ROUTE INFORMATION");
    resultHeading->setStyleSheet("font-size: 10px; font-weight: 800; color: #8293AA; border: none;");
    resultLayout->addWidget(resultHeading);
    routeInfoLabel = new QLabel("Choose a start and destination, then calculate a safe route.");
    routeInfoLabel->setWordWrap(true);
    routeInfoLabel->setMinimumHeight(48);
    routeInfoLabel->setStyleSheet("color: #A7F3D0; font-size: 11px; border: none;");
    resultLayout->addWidget(routeInfoLabel);
    controlLayout->addWidget(resultCard);

    controlLayout->addStretch();

    QLabel *helpText = new QLabel("Blocked roads are excluded from route calculations.");
    helpText->setWordWrap(true);
    helpText->setStyleSheet("color: #71839B; font-size: 10px; border: none;");
    controlLayout->addWidget(helpText);

    workspace->addWidget(controlPanel);

    // Main content pages: Dashboard and Rescue Management
    QWidget *dashboardPage = new QWidget;
    dashboardPage->setStyleSheet("background: transparent;");
    dashboardPage->setLayout(workspace);

    pageStack = new QStackedWidget;
    pageStack->addWidget(dashboardPage);

    QWidget *rescuePage = new QWidget;
    rescuePage->setStyleSheet("background: transparent;");
    QVBoxLayout *rescuePageLayout = new QVBoxLayout(rescuePage);
    rescuePageLayout->setContentsMargins(18, 12, 18, 12);
    rescuePageLayout->setSpacing(12);

    QLabel *rescueTitle = new QLabel("RESCUE PRIORITY MANAGEMENT");
    rescueTitle->setStyleSheet("font-size: 20px; font-weight: 800; color: #C4B5FD;");
    QLabel *rescueSubtitle = new QLabel("Manage emergency cases using the Custom Max Heap. Higher priority cases are processed first.");
    rescueSubtitle->setStyleSheet("color: #8293AA; font-size: 11px;");
    rescueSubtitle->setWordWrap(true);
    rescuePageLayout->addWidget(rescueTitle);
    rescuePageLayout->addWidget(rescueSubtitle);

    QFrame *rescueFormCard = new QFrame;
    rescueFormCard->setStyleSheet("QFrame { background-color: #111D30; border: 1px solid #263750; border-radius: 12px; }");
    QGridLayout *rescueFormLayout = new QGridLayout(rescueFormCard);
    rescueFormLayout->setContentsMargins(16, 14, 16, 14);
    rescueFormLayout->setHorizontalSpacing(12);
    rescueFormLayout->setVerticalSpacing(8);

    QLabel *victimLabel = new QLabel("Victim Name");
    victimNameInput = new QLineEdit;
    victimNameInput->setPlaceholderText("Enter victim name");
    QLabel *locationLabel = new QLabel("Rescue Location");
    rescueLocationCombo = new QComboBox;
    for (int i = 0; i < locations.size(); ++i)
        rescueLocationCombo->addItem(locations[i], i);
    // Danger Level Mode
    QLabel *dangerModeLabel = new QLabel("Danger Input Mode");
    dangerModeCombo = new QComboBox;
    dangerModeCombo->addItem("Manual");
    dangerModeCombo->addItem("Automatic Simulation");

    // Danger Level
    QLabel *dangerLevelLabel = new QLabel("Danger Level");
    dangerLevelCombo = new QComboBox;
    dangerLevelCombo->addItem("Low", 25);
    dangerLevelCombo->addItem("Medium", 50);
    dangerLevelCombo->addItem("High", 75);
    dangerLevelCombo->addItem("Critical", 100);

    // Emergency Signal Mode
    QLabel *signalModeLabel = new QLabel("Signal Input Mode");
    signalModeCombo = new QComboBox;
    signalModeCombo->addItem("Manual");
    signalModeCombo->addItem("Automatic Simulation");

    // Emergency Signal Level
    QLabel *signalLevelLabel = new QLabel("Emergency Signal");
    signalLevelCombo = new QComboBox;
    signalLevelCombo->addItem("Low", 25);
    signalLevelCombo->addItem("Medium", 50);
    signalLevelCombo->addItem("High", 75);
    signalLevelCombo->addItem("Critical", 100);

    QPushButton *addRescueButton =
        createButton("＋  Add Rescue Case", "#6D4BC3");

    rescueFormLayout->addWidget(victimLabel, 0, 0);
    rescueFormLayout->addWidget(locationLabel, 0, 1);
    rescueFormLayout->addWidget(dangerModeLabel, 0, 2);
    rescueFormLayout->addWidget(dangerLevelLabel, 0, 3);

    rescueFormLayout->addWidget(victimNameInput, 1, 0);
    rescueFormLayout->addWidget(rescueLocationCombo, 1, 1);
    rescueFormLayout->addWidget(dangerModeCombo, 1, 2);
    rescueFormLayout->addWidget(dangerLevelCombo, 1, 3);

    rescueFormLayout->addWidget(signalModeLabel, 2, 0);
    rescueFormLayout->addWidget(signalLevelLabel, 2, 1);

    rescueFormLayout->addWidget(signalModeCombo, 3, 0);
    rescueFormLayout->addWidget(signalLevelCombo, 3, 1);
    rescueFormLayout->addWidget(addRescueButton, 3, 2, 1, 2);
    rescuePageLayout->addWidget(rescueFormCard);
    //
    QFrame *rescueListCard = new QFrame;
    rescueListCard->setStyleSheet("QFrame { background-color: #111D30; border: 1px solid #263750; border-radius: 12px; }");
    QVBoxLayout *rescueListLayout = new QVBoxLayout(rescueListCard);
    rescueListLayout->setContentsMargins(14, 12, 14, 14);
    QHBoxLayout *rescueListHeading = new QHBoxLayout;
    QLabel *pendingTitle = new QLabel("PENDING RESCUE CASES");
    pendingTitle->setStyleSheet("font-size: 11px; font-weight: 800; color: #A5B4FC;");
    rescueCountLabel = new QLabel("Total: 0");
    rescueCountLabel->setStyleSheet("color: #6EE7B7; font-weight: 800;");
    rescueListHeading->addWidget(pendingTitle);
    rescueListHeading->addStretch();
    rescueListHeading->addWidget(rescueCountLabel);
    rescueListLayout->addLayout(rescueListHeading);

    rescueList = new QListWidget;
    rescueList->setMinimumHeight(260);
    rescueList->setStyleSheet(
        "QListWidget {"
        " background-color: #091426;"
        " border: 1px solid #263750;"
        " border-radius: 8px;"
        " padding: 6px;"
        " color: #E5EDF7;"
        "}"
        "QListWidget::item {"
        " padding: 12px 8px;"
        " border-bottom: 1px solid #263750;"
        " border-radius: 5px;"
        "}"
        "QListWidget::item:selected {"
        " background-color: #123C3A;"
        " border: 1px solid #00F5A0;"
        " color: #FFFFFF;"
        "}"
        "QListWidget::item:hover {"
        " background-color: #142A40;"
        "}");

    rescueListLayout->addWidget(rescueList, 1);

    QPushButton *processButton =
        createButton("Process Highest Priority Case", "#2456A6");

    rescueListLayout->addWidget(processButton);

    // ================= RESCUED CASES LIST =================

    QFrame *rescuedListCard = new QFrame;
    rescuedListCard->setStyleSheet(
        "QFrame {"
        " background-color: #111D30;"
        " border: 1px solid #263750;"
        " border-radius: 12px;"
        "}");

    QVBoxLayout *rescuedListLayout = new QVBoxLayout(rescuedListCard);
    rescuedListLayout->setContentsMargins(14, 12, 14, 14);

    QHBoxLayout *rescuedHeading = new QHBoxLayout;

    QLabel *rescuedTitle = new QLabel("SUCCESSFULLY RESCUED");
    rescuedTitle->setStyleSheet(
        "font-size: 11px;"
        "font-weight: 800;"
        "color: #6EE7B7;"
        "border: none;");

    rescuedCountLabel = new QLabel("Total: 0");
    rescuedCountLabel->setStyleSheet(
        "color: #00F5A0;"
        "font-weight: 800;"
        "border: none;");

    rescuedHeading->addWidget(rescuedTitle);
    rescuedHeading->addStretch();
    rescuedHeading->addWidget(rescuedCountLabel);

    rescuedListLayout->addLayout(rescuedHeading);

    rescuedList = new QListWidget;
    rescuedList->setMinimumHeight(260);
    rescuedList->setStyleSheet(
        "QListWidget {"
        " background-color: #091426;"
        " border: 1px solid #263750;"
        " border-radius: 8px;"
        " padding: 6px;"
        " color: #E5EDF7;"
        "}"
        "QListWidget::item {"
        " padding: 12px 8px;"
        " border-bottom: 1px solid #263750;"
        " border-radius: 5px;"
        "}"
        "QListWidget::item:hover {"
        " background-color: #142A40;"
        "}");

    rescuedListLayout->addWidget(rescuedList, 1);

    // ================= RESCUE LIST TABS =================

    QTabWidget *rescueTabs = new QTabWidget;

    rescueTabs->addTab(rescueListCard, "PENDING");
    rescueTabs->addTab(rescuedListCard, "RESCUED");

    rescueTabs->setStyleSheet(
        "QTabWidget::pane {"
        " border: 1px solid #263750;"
        " border-radius: 8px;"
        " background-color: #111D30;"
        "}"
        "QTabBar::tab {"
        " background-color: #091426;"
        " color: #8293AA;"
        " padding: 10px 20px;"
        " margin-right: 4px;"
        " border: 1px solid #263750;"
        " border-bottom: none;"
        " border-top-left-radius: 6px;"
        " border-top-right-radius: 6px;"
        " font-weight: 800;"
        "}"
        "QTabBar::tab:selected {"
        " background-color: #123C3A;"
        " color: #00F5A0;"
        " border-color: #00F5A0;"
        "}");

    // ================= RESCUE INTELLIGENCE PANEL =================

    QFrame *detailsCard = new QFrame;
    detailsCard->setObjectName("rescueDetailsCard");
    detailsCard->setStyleSheet(
        "QFrame#rescueDetailsCard {"
        " background-color: #0C1729;"
        " border: 1px solid #1E3A50;"
        " border-radius: 12px;"
        "}");

    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsCard);
    detailsLayout->setContentsMargins(14, 14, 14, 14);
    detailsLayout->setSpacing(8);

    // Panel heading
    QHBoxLayout *detailsHeading = new QHBoxLayout;

    QLabel *detailsTitle =
        new QLabel("LIVE RESCUE INTELLIGENCE");

    detailsTitle->setStyleSheet(
        "font-size: 12px;"
        "font-weight: 800;"
        "letter-spacing: 1px;"
        "color: #00D9FF;"
        "border: none;");

    QLabel *liveIndicator = new QLabel("● LIVE");

    liveIndicator->setStyleSheet(
        "color: #00F5A0;"
        "font-size: 10px;"
        "font-weight: 800;"
        "border: none;");

    detailsHeading->addWidget(detailsTitle);
    detailsHeading->addStretch();
    detailsHeading->addWidget(liveIndicator);

    detailsLayout->addLayout(detailsHeading);

    // Divider
    QFrame *detailsDivider = new QFrame;
    detailsDivider->setFrameShape(QFrame::HLine);
    detailsDivider->setStyleSheet(
        "background-color: #263750;"
        "border: none;"
        "max-height: 1px;");

    detailsLayout->addWidget(detailsDivider);

    // Selected case information
    caseDetailsLabel = new QLabel(
        "NO CASE SELECTED\n\n"
        "Select a rescue case from the pending list "
        "to view its emergency intelligence.");

    caseDetailsLabel->setWordWrap(true);
    caseDetailsLabel->setMinimumHeight(75);
    caseDetailsLabel->setStyleSheet(
        "color: #8293AA;"
        "font-size: 12px;"
        "padding: 8px;"
        "border: none;");

    detailsLayout->addWidget(caseDetailsLabel);

    // Priority category
    priorityCategoryLabel = new QLabel("PRIORITY: --");
    priorityCategoryLabel->setAlignment(Qt::AlignCenter);
    priorityCategoryLabel->setStyleSheet(
        "background-color: #17243A;"
        "color: #00F5A0;"
        "border: 1px solid #245B50;"
        "border-radius: 8px;"
        "padding: 10px;"
        "font-size: 14px;"
        "font-weight: 800;");

    detailsLayout->addWidget(priorityCategoryLabel);

    // Score information
    auto addScoreRow = [detailsLayout](
                           const QString &title,
                           QLabel *&valueLabel,
                           const QString &color)
    {
        QHBoxLayout *row = new QHBoxLayout;

        QLabel *nameLabel = new QLabel(title);
        nameLabel->setStyleSheet(
            "color: #A9B8CA;"
            "font-size: 11px;"
            "border: none;");

        valueLabel = new QLabel("-- / 100");
        valueLabel->setAlignment(Qt::AlignRight);
        valueLabel->setStyleSheet(
            QString("color: %1; font-size: 12px; font-weight: 800; border: none;")
                .arg(color));

        row->addWidget(nameLabel);
        row->addStretch();
        row->addWidget(valueLabel);

        detailsLayout->addLayout(row);
    };

    addScoreRow("Overall Priority", priorityScoreLabel, "#00F5A0");
    addScoreRow("Danger Level", dangerScoreLabel, "#FF4560");
    addScoreRow("Emergency Signal", signalScoreLabel, "#00D9FF");
    addScoreRow("Distance Risk", distanceRiskLabel, "#FFB020");

    // Recommended exit information
    QFrame *exitCard = new QFrame;
    exitCard->setStyleSheet(
        "QFrame {"
        " background-color: #102C2A;"
        " border: 1px solid #17634F;"
        " border-radius: 8px;"
        "}");

    QVBoxLayout *exitLayout = new QVBoxLayout(exitCard);
    exitLayout->setContentsMargins(12, 10, 12, 10);
    exitLayout->setSpacing(5);

    QLabel *exitHeading = new QLabel("RECOMMENDED EMERGENCY EXIT");
    exitHeading->setStyleSheet(
        "color: #6EE7B7;"
        "font-size: 9px;"
        "font-weight: 800;"
        "border: none;");

    exitDetailsLabel = new QLabel("No exit information available");
    exitDetailsLabel->setWordWrap(true);
    exitDetailsLabel->setStyleSheet(
        "color: #F8FAFC;"
        "font-size: 12px;"
        "font-weight: 700;"
        "border: none;");

    exitLayout->addWidget(exitHeading);
    exitLayout->addWidget(exitDetailsLabel);

    detailsLayout->addWidget(exitCard);
    detailsLayout->addStretch();

    // Put list and intelligence panel side by side
    // Put list and intelligence panel side by side
    QHBoxLayout *rescueContentLayout = new QHBoxLayout;
    rescueContentLayout->setSpacing(12);

    // Rescue list
    // Pending and Rescued Lists
    rescueContentLayout->addWidget(rescueTabs, 3);

    // Scrollable Rescue Details Panel
    QScrollArea *detailsScroll = new QScrollArea;

    detailsScroll->setWidgetResizable(true);
    detailsScroll->setFrameShape(QFrame::NoFrame);
    detailsScroll->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff);

    detailsScroll->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded);

    detailsCard->setMinimumHeight(430);
    detailsCard->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred);

    detailsScroll->setWidget(detailsCard);

    detailsScroll->setStyleSheet(
        "QScrollArea {"
        " background: transparent;"
        " border: none;"
        "}");

    rescueContentLayout->addWidget(detailsScroll, 2);

    rescuePageLayout->addLayout(rescueContentLayout, 1);

    pageStack->addWidget(rescuePage);

    // ================= BFS CONNECTIVITY PAGE =================

    QWidget *bfsPage = new QWidget();
    QVBoxLayout *bfsLayout = new QVBoxLayout(bfsPage);
    bfsLayout->setContentsMargins(30, 30, 30, 30);
    bfsLayout->setSpacing(18);

    QLabel *bfsTitle = new QLabel("BFS Connectivity Analysis");
    bfsTitle->setStyleSheet(
        "font-size: 26px; font-weight: bold; color: white;");

    QLabel *bfsSubtitle = new QLabel(
        "Analyze reachable locations using Breadth First Search");
    bfsSubtitle->setStyleSheet("color: #9CA3AF; font-size: 13px;");

    QLabel *sourceLabel = new QLabel("Select Starting Location");
    sourceLabel->setStyleSheet("color: white; font-size: 14px;");

    bfsSourceCombo = new QComboBox();
    bfsSourceCombo->addItems(locations);

    QPushButton *analyzeBFSButton = createButton(
        "Run BFS Analysis", "#059669");

    bfsSummaryLabel = new QLabel("Analysis result will appear here.");
    bfsSummaryLabel->setStyleSheet(
        "color: #34D399; font-size: 15px; padding: 10px;");

    QLabel *resultTitle = new QLabel("BFS Traversal Order");
    resultTitle->setStyleSheet(
        "color: white; font-size: 17px; font-weight: bold;");

    bfsResultList = new QListWidget();

    bfsLayout->addWidget(bfsTitle);
    bfsLayout->addWidget(bfsSubtitle);
    bfsLayout->addSpacing(15);
    bfsLayout->addWidget(sourceLabel);
    bfsLayout->addWidget(bfsSourceCombo);
    bfsLayout->addWidget(analyzeBFSButton);
    bfsLayout->addWidget(bfsSummaryLabel);
    bfsLayout->addWidget(resultTitle);
    bfsLayout->addWidget(bfsResultList);

    pageStack->addWidget(bfsPage);

    connect(analyzeBFSButton, &QPushButton::clicked,
            this, &MainWindow::runBFSAnalysis);

    // Persistent left navigation sidebar
    QHBoxLayout *mainArea = new QHBoxLayout;
    mainArea->setSpacing(12);
    QFrame *sidebar = new QFrame;
    sidebar->setObjectName("sideBar");
    sidebar->setFixedWidth(205);
    sidebar->setStyleSheet("QFrame#sideBar { background-color: #111D30; border: 1px solid #263750; border-radius: 13px; }");
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(12, 16, 12, 16);
    sidebarLayout->setSpacing(9);

    QLabel *menuTitle = new QLabel("NAVIGATION");
    menuTitle->setStyleSheet("font-size: 10px; font-weight: 800; letter-spacing: 1px; color: #8293AA; border: none;");
    sidebarLayout->addWidget(menuTitle);

    QPushButton *dashboardNavButton = createButton("⌂  Dashboard", "#087F5B");
    QPushButton *rescueNavButton = createButton("✚  Rescue Management", "#49366D");
    QPushButton *bfsNavButton = createButton(
        "⇄  BFS Connectivity", "#2456A6");

    sidebarLayout->addWidget(bfsNavButton);

    connect(bfsNavButton, &QPushButton::clicked, this, [this]()
            { pageStack->setCurrentIndex(2); });
    dashboardNavButton->setMinimumHeight(43);
    rescueNavButton->setMinimumHeight(43);
    sidebarLayout->addWidget(dashboardNavButton);
    sidebarLayout->addWidget(rescueNavButton);
    sidebarLayout->addStretch();

    QLabel *sidebarNote = new QLabel("SAFEROUTE\nEmergency response system");
    sidebarNote->setStyleSheet("color: #71839B; font-size: 10px; border: none;");
    sidebarNote->setWordWrap(true);
    sidebarLayout->addWidget(sidebarNote);

    mainArea->addWidget(sidebar);
    mainArea->addWidget(pageStack, 1);
    root->addLayout(mainArea, 1);

    connect(dashboardNavButton, &QPushButton::clicked,
            this, [this]()
            { pageStack->setCurrentIndex(0); });
    connect(rescueNavButton, &QPushButton::clicked,
            this, &MainWindow::setupRescueManagement);
    connect(addRescueButton, &QPushButton::clicked,
            this, &MainWindow::addRescueCase);
    connect(processButton, &QPushButton::clicked,
            this, &MainWindow::processHighestPriority);

    // Bottom status strip
    QFrame *footer = new QFrame;
    footer->setStyleSheet("QFrame { background-color: #111D30; border: 1px solid #263750; border-radius: 8px; }");
    QHBoxLayout *footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(12, 6, 12, 6);
    statusLabel = new QLabel("READY  |  Sample emergency graph loaded");
    statusLabel->setStyleSheet("color: #8293AA; font-size: 10px; border: none;");
    footerLayout->addWidget(statusLabel);
    footerLayout->addStretch();
    QLabel *footerRight = new QLabel("SAFEROUTE  •  DATA STRUCTURES LAB PROJECT");
    footerRight->setStyleSheet("color: #5F7189; font-size: 9px; border: none;");
    footerLayout->addWidget(footerRight);
    root->addWidget(footer);

    connect(routeButton, &QPushButton::clicked, this, &MainWindow::findSafeRoute);
    connect(blockButton, &QPushButton::clicked, this, &MainWindow::blockSelectedRoad);
    connect(unblockButton, &QPushButton::clicked, this, &MainWindow::unblockSelectedRoad);

    connect(
        rescueList,
        &QListWidget::currentItemChanged,
        this,
        [this](QListWidgetItem *current, QListWidgetItem *)
        {
            showRescueCaseDetails(current);
        });
}

// Find the nearest available emergency exit automatically
void MainWindow::findSafeRoute()
{
    if (!sourceCombo || !mapWidget)
        return;

    const int NORTH_EXIT = 10;
    const int SOUTH_EXIT = 11;

    // Get selected starting location
    const int source = sourceCombo->currentData().toInt();

    // Calculate shortest paths from starting location
    Dijkstra dijkstra(graph);

    if (!dijkstra.calculate(source))
    {
        mapWidget->setShortestPath(nullptr, 0);

        routeInfoLabel->setText(
            "Route calculation failed.");

        statusLabel->setText(
            "ERROR | Dijkstra calculation failed.");

        return;
    }

    // Get distances to both emergency exits
    const int northDistance =
        dijkstra.getDistance(NORTH_EXIT);

    const int southDistance =
        dijkstra.getDistance(SOUTH_EXIT);

    int selectedExit = -1;
    int selectedDistance = -1;

    // Select the nearest reachable emergency exit
    if (northDistance == -1 && southDistance == -1)
    {
        mapWidget->setShortestPath(nullptr, 0);

        routeInfoLabel->setText(
            "NO SAFE EXIT AVAILABLE\n"
            "Both emergency exits are unreachable using open roads.");

        statusLabel->setText(
            "WARNING | No safe evacuation route is available.");

        return;
    }
    else if (northDistance == -1)
    {
        selectedExit = SOUTH_EXIT;
        selectedDistance = southDistance;
    }
    else if (southDistance == -1)
    {
        selectedExit = NORTH_EXIT;
        selectedDistance = northDistance;
    }
    else if (northDistance <= southDistance)
    {
        selectedExit = NORTH_EXIT;
        selectedDistance = northDistance;
    }
    else
    {
        selectedExit = SOUTH_EXIT;
        selectedDistance = southDistance;
    }

    // Retrieve the selected exit's path
    int pathLength = 0;

    int *path =
        dijkstra.getPath(selectedExit, pathLength);

    if (path == nullptr || pathLength == 0)
    {
        delete[] path;

        mapWidget->setShortestPath(nullptr, 0);

        routeInfoLabel->setText(
            "Unable to retrieve the calculated route.");

        statusLabel->setText(
            "ERROR | Route path retrieval failed.");

        return;
    }

    // Display route on the map
    mapWidget->setShortestPath(path, pathLength);

    // Build route description
    QString routeText;

    for (int i = 0; i < pathLength; i++)
    {
        const Vertex *vertex =
            graph.getVertex(path[i]);

        if (vertex)
        {
            routeText += QString::fromUtf8(vertex->name);
        }

        if (i < pathLength - 1)
        {
            routeText += "  →  ";
        }
    }

    delete[] path;

    // Get selected exit name
    const Vertex *exitVertex =
        graph.getVertex(selectedExit);

    QString exitName = exitVertex
                           ? QString::fromUtf8(exitVertex->name)
                           : "Emergency Exit";

    // Display final route information
    routeInfoLabel->setText(
        "RECOMMENDED SAFE EXIT: " + exitName +
        "\n\nRoute: " + routeText +
        "\nTotal Distance: " +
        QString::number(selectedDistance) + " m");

    statusLabel->setText(
        "SUCCESS | Nearest available emergency exit selected.");
}

void MainWindow::compareExits()
{
    const int source = sourceCombo->currentData().toInt();
    Dijkstra dijkstra(graph);

    if (!dijkstra.calculate(source))
    {
        statusLabel->setText("ERROR  |  Could not compare exit routes.");
        return;
    }

    const int exitA = dijkstra.getDistance(10);
    const int exitB = dijkstra.getDistance(11);

    QString result = "North Exit: ";
    if (exitA < 0)
        result += "Unavailable";
    else
        result += QString::number(exitA) + " m";

    result += "   |   South Exit: ";
    if (exitB < 0)
        result += "Unavailable";
    else
        result += QString::number(exitB) + " m";

    if (exitA < 0 && exitB < 0)
        result += "\nNo reachable emergency exit.";
    else if (exitA < 0 || (exitB >= 0 && exitB < exitA))
        result += "\nRecommended: South Exit";
    else
        result += "\nRecommended: North Exit";

    routeInfoLabel->setText(result);
    mapWidget->setShortestPath(nullptr, 0);
    statusLabel->setText("EXIT COMPARISON  |  Distances calculated from selected start.");
}

void MainWindow::blockSelectedRoad()
{
    const int source = roadSourceCombo->currentData().toInt();
    const int destination = roadDestinationCombo->currentData().toInt();

    if (source == destination)
    {
        statusLabel->setText("WARNING  |  Select two different locations.");
        return;
    }

    if (graph.blockRoad(source, destination))
    {
        mapWidget->setShortestPath(nullptr, 0);

        routeInfoLabel->setText(
            "Road blocked. Calculate a new route to avoid this passage.");

        refreshPendingRescueCases();

        statusLabel->setText(
            "ROAD STATUS | Road blocked. Pending rescue cases updated.");
    }
    else
    {
        statusLabel->setText("WARNING  |  Road not found or already blocked.");
    }
}

void MainWindow::unblockSelectedRoad()
{
    const int source = roadSourceCombo->currentData().toInt();
    const int destination = roadDestinationCombo->currentData().toInt();

    if (source == destination)
    {
        statusLabel->setText("WARNING  |  Select two different locations.");
        return;
    }

    if (graph.unblockRoad(source, destination))
    {
        mapWidget->setShortestPath(nullptr, 0);

        routeInfoLabel->setText(
            "Road restored. Calculate a route to use this passage again.");

        refreshPendingRescueCases();

        statusLabel->setText(
            "ROAD STATUS | Road restored. Pending rescue cases updated.");
    }
    else
    {
        statusLabel->setText("WARNING  |  Road not found or is already open.");
    }
}

void MainWindow::resetMapView()
{
    mapWidget->fitMap();
}

// Display Rescue Priority Management inside the main interface.
void MainWindow::setupRescueManagement()
{
    if (pageStack)
    {
        pageStack->setCurrentIndex(1);

        // Refresh the Rescue Details Panel
        showRescueCaseDetails(rescueList->currentItem());

        updateRescueList();
        statusLabel->setText("RESCUE MANAGEMENT | Priority queue is ready.");
    }
}

// Add a new rescue case

// Add a new rescue case with automatic exit selection
void MainWindow::addRescueCase()
{
    if (!victimNameInput ||
        !rescueLocationCombo ||
        !dangerModeCombo ||
        !dangerLevelCombo ||
        !signalModeCombo ||
        !signalLevelCombo ||
        !rescueList)
    {
        return;
    }

    QString name = victimNameInput->text().trimmed();

    if (name.isEmpty())
    {
        QMessageBox::warning(
            this, "Invalid Input",
            "Please enter the victim name.");
        return;
    }

    if (rescueHeap->getSize() >= 100)
    {
        QMessageBox::warning(
            this, "Heap Full",
            "Maximum rescue capacity reached.");
        return;
    }

    int locationId =
        rescueLocationCombo->currentData().toInt();

    // =====================================
    // STEP 1: Calculate routes to both exits
    // =====================================

    const int NORTH_EXIT = 10;
    const int SOUTH_EXIT = 11;

    int selectedExit = -1;
    int selectedDistance = -1;

    Dijkstra dijkstra(graph);

    if (dijkstra.calculate(locationId))
    {
        int northDistance = dijkstra.getDistance(NORTH_EXIT);
        int southDistance = dijkstra.getDistance(SOUTH_EXIT);

        // =====================================
        // STEP 2: Select shortest available exit
        // =====================================

        if (northDistance == -1 && southDistance == -1)
        {
            // No safe route currently available.
            // Still allow victim to enter Pending List.
            selectedExit = -1;
            selectedDistance = -1;
        }
        else if (northDistance == -1)
        {
            selectedExit = SOUTH_EXIT;
            selectedDistance = southDistance;
        }
        else if (southDistance == -1)
        {
            selectedExit = NORTH_EXIT;
            selectedDistance = northDistance;
        }
        else if (northDistance <= southDistance)
        {
            selectedExit = NORTH_EXIT;
            selectedDistance = northDistance;
        }
        else
        {
            selectedExit = SOUTH_EXIT;
            selectedDistance = southDistance;
        }
    }
    else
    {
        QMessageBox::warning(
            this, "Route Calculation Warning",
            "Route calculation failed. The victim will be added "
            "to Pending List and checked again during rescue.");
    }

    // =====================================
    // STEP 3: Calculate Danger Score
    // =====================================

    int dangerScore = 0;

    if (dangerModeCombo->currentIndex() == 0)
    {
        // Manual mode
        dangerScore =
            dangerLevelCombo->currentData().toInt();
    }
    else
    {
        // Automatic simulation mode
        dangerScore = QRandomGenerator::global()->bounded(25, 101);
    }

    // =====================================
    // STEP 4: Calculate Emergency Signal Score
    // =====================================

    int signalScore = 0;

    if (signalModeCombo->currentIndex() == 0)
    {
        // Manual mode
        signalScore =
            signalLevelCombo->currentData().toInt();
    }
    else
    {
        // Automatic simulation mode
        signalScore = QRandomGenerator::global()->bounded(25, 101);
    }

    // =====================================
    // STEP 5: Calculate Distance Risk Score
    // =====================================

    const int MAX_DISTANCE = 30;

    int distanceRisk = 0;

    if (selectedDistance != -1)
    {
        distanceRisk =
            (selectedDistance * 100) / MAX_DISTANCE;

        if (distanceRisk > 100)
            distanceRisk = 100;

        if (distanceRisk < 0)
            distanceRisk = 0;
    }

    // =====================================
    // STEP 6: Calculate Final Rescue Priority
    // =====================================

    int priority = static_cast<int>(
        0.50 * dangerScore +
        0.30 * signalScore +
        0.20 * distanceRisk);

    // =====================================
    // STEP 7: Create Rescue Case
    // =====================================

    QByteArray nameBytes = name.toUtf8();

    RescueCase newCase(
        nextCaseId,
        nameBytes.constData(),
        priority,
        locationId,
        dangerScore,
        signalScore,
        distanceRisk,
        selectedExit,
        selectedDistance);

    if (!rescueHeap->insert(newCase))
    {
        QMessageBox::warning(
            this, "Insertion Failed",
            "Could not add the rescue case.");
        return;
    }

    // =====================================
    // STEP 8: Get Exit Name
    // =====================================

    QString exitName = "N/A";

    if (selectedExit != -1)
    {
        const Vertex *exitVertex =
            graph.getVertex(selectedExit);

        exitName = exitVertex
                       ? QString::fromUtf8(exitVertex->name)
                       : "Unknown Exit";
    }

    // =====================================
    // STEP 9: Display Rescue Case
    // =====================================

    QString priorityCategory;

    if (priority >= 80)
        priorityCategory = "CRITICAL";
    else if (priority >= 60)
        priorityCategory = "HIGH";
    else if (priority >= 40)
        priorityCategory = "MEDIUM";
    else
        priorityCategory = "LOW";

    QListWidgetItem *item = new QListWidgetItem;

    item->setText(
        QString("#%1 | %2 | Priority: %3 (%4) | Exit: %5 | Distance: %6")
            .arg(nextCaseId)
            .arg(name)
            .arg(priority)
            .arg(priorityCategory)
            .arg(exitName)
            .arg(selectedDistance == -1
                     ? QString("N/A")
                     : QString::number(selectedDistance)));

    item->setData(Qt::UserRole, nextCaseId);
    item->setData(Qt::UserRole + 1, priority);

    // Store complete rescue information for the Details Panel
    item->setData(Qt::UserRole + 2, name);
    item->setData(Qt::UserRole + 3, locationId);
    item->setData(Qt::UserRole + 4, dangerScore);
    item->setData(Qt::UserRole + 5, signalScore);
    item->setData(Qt::UserRole + 6, distanceRisk);
    item->setData(Qt::UserRole + 7, selectedExit);
    item->setData(Qt::UserRole + 8, selectedDistance);
    item->setData(Qt::UserRole + 9, exitName);
    item->setData(Qt::UserRole + 10, priorityCategory);

    // Insert higher priority cases first
    int position = 0;

    while (position < rescueList->count())
    {
        int existingPriority =
            rescueList->item(position)
                ->data(Qt::UserRole + 1)
                .toInt();

        if (priority > existingPriority)
            break;

        position++;
    }

    rescueList->insertItem(position, item);

    // =====================================
    // STEP 10: Update Interface
    // =====================================

    nextCaseId++;

    victimNameInput->clear();

    updateRescueList();

    statusLabel->setText(
        QString("RESCUE MANAGEMENT | Case added | Priority: %1 | %2")
            .arg(priority)
            .arg(priorityCategory));

    QString routeStatus =
        selectedExit == -1
            ? "NO SAFE ROUTE AVAILABLE"
            : "SAFE ROUTE AVAILABLE";

    QMessageBox::information(
        this,
        "Rescue Case Added",
        QString(
            "Victim: %1\n"
            "Danger Score: %2\n"
            "Emergency Signal Score: %3\n"
            "Distance Risk: %4\n"
            "Final Priority: %5 (%6)\n"
            "Selected Exit: %7\n"
            "Route Distance: %8\n"
            "Route Status: %9\n\n"
            "Victim successfully added to Pending List.")
            .arg(name)
            .arg(dangerScore)
            .arg(signalScore)
            .arg(distanceRisk)
            .arg(priority)
            .arg(priorityCategory)
            .arg(exitName)
            .arg(selectedDistance == -1
                     ? QString("N/A")
                     : QString::number(selectedDistance))
            .arg(routeStatus));
}

// Process the highest priority rescue case

// Process the highest priority rescue case
void MainWindow::processHighestPriority()
{
    if (rescueHeap->isEmpty())
    {
        QMessageBox::information(
            this,
            "No Pending Cases",
            "There are no rescue cases to process.");

        return;
    }

    // Step 1: Find the highest-priority victim with a safe route

    const int NORTH_EXIT = 10;
    const int SOUTH_EXIT = 11;
    const int MAX_SKIPPED = 100;

    RescueCase skippedCases[MAX_SKIPPED];
    int skippedCount = 0;

    RescueCase processed;
    int selectedExit = -1;
    int selectedDistance = -1;

    bool foundSafeVictim = false;

    // Keep checking until a safe victim is found or heap becomes empty
    while (!rescueHeap->isEmpty())
    {
        RescueCase current = rescueHeap->extractMax();

        if (current.caseId == -1)
            continue;

        Dijkstra dijkstra(graph);

        // If route calculation fails, temporarily skip this victim
        if (!dijkstra.calculate(current.locationId))
        {
            if (skippedCount < MAX_SKIPPED)
                skippedCases[skippedCount++] = current;

            continue;
        }

        int northDistance =
            dijkstra.getDistance(NORTH_EXIT);

        int southDistance =
            dijkstra.getDistance(SOUTH_EXIT);

        int currentExit = -1;
        int currentDistance = -1;

        // Select the nearest reachable exit
        if (northDistance != -1 && southDistance != -1)
        {
            if (northDistance <= southDistance)
            {
                currentExit = NORTH_EXIT;
                currentDistance = northDistance;
            }
            else
            {
                currentExit = SOUTH_EXIT;
                currentDistance = southDistance;
            }
        }
        else if (northDistance != -1)
        {
            currentExit = NORTH_EXIT;
            currentDistance = northDistance;
        }
        else if (southDistance != -1)
        {
            currentExit = SOUTH_EXIT;
            currentDistance = southDistance;
        }

        // No safe route: temporarily skip and check next victim
        if (currentExit == -1)
        {
            if (skippedCount < MAX_SKIPPED)
                skippedCases[skippedCount++] = current;

            continue;
        }

        // A safe victim has been found
        processed = current;
        selectedExit = currentExit;
        selectedDistance = currentDistance;

        foundSafeVictim = true;
        break;
    }

    // Restore all skipped victims to the heap
    for (int i = 0; i < skippedCount; i++)
    {
        if (!rescueHeap->insert(skippedCases[i]))
        {
            QMessageBox::critical(
                this,
                "Heap Error",
                "Unable to restore a skipped victim to the heap.");

            return;
        }
    }

    // If no victim has a safe route, stop here
    if (!foundSafeVictim)
    {
        updateRescueList();

        QMessageBox::information(
            this,
            "No Safe Route Available",
            "No pending victim currently has a safe route.\n\n"
            "All victims remain in the Pending List.\n"
            "Restore a road and try again.");

        statusLabel->setText(
            "RESCUE MANAGEMENT | No victim currently has a safe route.");

        return;
    }

    // Step 6: Update the selected exit information
    processed.exitId = selectedExit;
    processed.routeDistance = selectedDistance;

    const Vertex *location =
        graph.getVertex(processed.locationId);

    const Vertex *exit =
        graph.getVertex(selectedExit);

    QString locationName = location
                               ? QString::fromUtf8(location->name)
                               : "Unknown Location";

    QString exitName = exit
                           ? QString::fromUtf8(exit->name)
                           : "Unknown Exit";

    QString victimName =
        QString::fromUtf8(processed.victimName);

    // Step 7: Remove victim from Pending list
    for (int i = 0; i < rescueList->count(); ++i)
    {
        QListWidgetItem *item = rescueList->item(i);

        if (item->data(Qt::UserRole).toInt() == processed.caseId)
        {
            delete rescueList->takeItem(i);
            break;
        }
    }

    // Step 8: Add victim to Successfully Rescued list
    QString rescuedText =
        QString("RESCUED | #%1 | %2\n"
                "Priority: %3 | Location: %4\n"
                "Emergency Exit: %5 | Distance: %6")
            .arg(processed.caseId)
            .arg(victimName)
            .arg(processed.priority)
            .arg(locationName)
            .arg(exitName)
            .arg(processed.routeDistance);

    QListWidgetItem *rescuedItem =
        new QListWidgetItem(rescuedText);

    rescuedItem->setData(
        Qt::UserRole, processed.caseId);

    rescuedItem->setData(
        Qt::UserRole + 1, processed.priority);

    rescuedItem->setData(
        Qt::UserRole + 2, victimName);

    rescuedItem->setData(
        Qt::UserRole + 3, processed.locationId);

    rescuedItem->setData(
        Qt::UserRole + 4, selectedExit);

    rescuedItem->setData(
        Qt::UserRole + 5, selectedDistance);

    rescuedList->addItem(rescuedItem);

    // Step 9: Update list counts
    updateRescueList();
    updateRescuedList();

    // Step 10: Show rescue confirmation
    QString message =
        QString("VICTIM SUCCESSFULLY RESCUED!\n\n"
                "Case ID: %1\n"
                "Victim: %2\n"
                "Priority: %3\n"
                "Current Location: %4\n"
                "Selected Emergency Exit: %5\n"
                "Safe Route Distance: %6")
            .arg(processed.caseId)
            .arg(victimName)
            .arg(processed.priority)
            .arg(locationName)
            .arg(exitName)
            .arg(processed.routeDistance);

    QMessageBox::information(
        this,
        "Rescue Successful",
        message);

    statusLabel->setText(
        "RESCUE MANAGEMENT | Victim successfully rescued.");
}

// Update pending rescue case count
void MainWindow::updateRescueList()
{
    if (rescueCountLabel && rescueList)
    {
        rescueCountLabel->setText(
            QString("Total: %1").arg(rescueList->count()));
    }
}

// Refresh all pending rescue cases after road status changes
void MainWindow::refreshPendingRescueCases()
{
    if (!rescueList || !rescueHeap)
        return;

    const int NORTH_EXIT = 10;
    const int SOUTH_EXIT = 11;
    const int MAX_DISTANCE = 30;

    // Create a new heap for updated rescue priorities
    MaxHeap *updatedHeap = new MaxHeap(100);

    // Preserve currently selected victim
    int selectedCaseId = -1;

    QListWidgetItem *currentItem = rescueList->currentItem();

    if (currentItem)
    {
        selectedCaseId =
            currentItem->data(Qt::UserRole).toInt();
    }

    // Recalculate every pending victim
    for (int i = 0; i < rescueList->count(); i++)
    {
        QListWidgetItem *item = rescueList->item(i);

        int caseId =
            item->data(Qt::UserRole).toInt();

        QString name =
            item->data(Qt::UserRole + 2).toString();

        int locationId =
            item->data(Qt::UserRole + 3).toInt();

        int dangerScore =
            item->data(Qt::UserRole + 4).toInt();

        int signalScore =
            item->data(Qt::UserRole + 5).toInt();

        // Calculate updated routes
        int selectedExit = -1;
        int selectedDistance = -1;

        Dijkstra dijkstra(graph);

        if (dijkstra.calculate(locationId))
        {
            int northDistance =
                dijkstra.getDistance(NORTH_EXIT);

            int southDistance =
                dijkstra.getDistance(SOUTH_EXIT);

            if (northDistance == -1 && southDistance == -1)
            {
                selectedExit = -1;
                selectedDistance = -1;
            }
            else if (northDistance == -1)
            {
                selectedExit = SOUTH_EXIT;
                selectedDistance = southDistance;
            }
            else if (southDistance == -1)
            {
                selectedExit = NORTH_EXIT;
                selectedDistance = northDistance;
            }
            else if (northDistance <= southDistance)
            {
                selectedExit = NORTH_EXIT;
                selectedDistance = northDistance;
            }
            else
            {
                selectedExit = SOUTH_EXIT;
                selectedDistance = southDistance;
            }
        }

        // Recalculate distance risk
        int distanceRisk = 0;

        if (selectedDistance != -1)
        {
            distanceRisk =
                (selectedDistance * 100) / MAX_DISTANCE;

            if (distanceRisk > 100)
                distanceRisk = 100;

            if (distanceRisk < 0)
                distanceRisk = 0;
        }

        // Recalculate final priority
        int priority = static_cast<int>(
            0.50 * dangerScore +
            0.30 * signalScore +
            0.20 * distanceRisk);

        // Keep priority within the valid heap range
        if (priority < 1)
            priority = 1;

        // Determine priority category
        QString category;

        if (priority >= 80)
            category = "CRITICAL";
        else if (priority >= 60)
            category = "HIGH";
        else if (priority >= 40)
            category = "MEDIUM";
        else
            category = "LOW";

        // Get updated exit name
        QString exitName = "N/A";

        if (selectedExit != -1)
        {
            const Vertex *exitVertex =
                graph.getVertex(selectedExit);

            exitName = exitVertex
                           ? QString::fromUtf8(exitVertex->name)
                           : "Unknown Exit";
        }

        // Update visible Pending List information
        item->setText(
            QString("#%1 | %2 | Priority: %3 (%4) | Exit: %5 | Distance: %6")
                .arg(caseId)
                .arg(name)
                .arg(priority)
                .arg(category)
                .arg(exitName)
                .arg(selectedDistance == -1
                         ? QString("N/A")
                         : QString::number(selectedDistance)));

        // Update stored item information
        item->setData(Qt::UserRole + 1, priority);
        item->setData(Qt::UserRole + 6, distanceRisk);
        item->setData(Qt::UserRole + 7, selectedExit);
        item->setData(Qt::UserRole + 8, selectedDistance);
        item->setData(Qt::UserRole + 9, exitName);
        item->setData(Qt::UserRole + 10, category);

        // Create updated RescueCase for the new heap
        QByteArray nameBytes = name.toUtf8();

        RescueCase updatedCase(
            caseId,
            nameBytes.constData(),
            priority,
            locationId,
            dangerScore,
            signalScore,
            distanceRisk,
            selectedExit,
            selectedDistance);

        if (!updatedHeap->insert(updatedCase))
        {
            delete updatedHeap;

            QMessageBox::warning(
                this,
                "Refresh Failed",
                "Could not rebuild the rescue priority heap.");

            return;
        }
    }

    // Replace old heap with updated heap
    delete rescueHeap;
    rescueHeap = updatedHeap;

    // Sort Pending List by updated priority
    for (int i = 0; i < rescueList->count(); i++)
    {
        for (int j = i + 1; j < rescueList->count(); j++)
        {
            int firstPriority =
                rescueList->item(i)
                    ->data(Qt::UserRole + 1)
                    .toInt();

            int secondPriority =
                rescueList->item(j)
                    ->data(Qt::UserRole + 1)
                    .toInt();

            if (secondPriority > firstPriority)
            {
                QListWidgetItem *takenItem =
                    rescueList->takeItem(j);

                rescueList->insertItem(i, takenItem);
            }
        }
    }

    // Restore previously selected victim
    for (int i = 0; i < rescueList->count(); i++)
    {
        QListWidgetItem *item = rescueList->item(i);

        if (item->data(Qt::UserRole).toInt() == selectedCaseId)
        {
            rescueList->setCurrentItem(item);
            break;
        }
    }

    updateRescueList();

    updateRescuedList();

    statusLabel->setText(
        "RESCUE MANAGEMENT | Pending routes and priorities updated.");
}

// ================= BFS ANALYSIS =================

void MainWindow::runBFSAnalysis()
{
    bfsResultList->clear();

    int source = bfsSourceCombo->currentIndex();

    BFS bfs(graph);

    int order[12];

    int count = bfs.getTraversalOrder(source, order, 12);

    if (count <= 0)
    {
        bfsSummaryLabel->setText("No reachable locations found.");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        const Vertex *vertex = graph.getVertex(order[i]);

        if (vertex != nullptr)
        {
            QString locationName = QString::fromUtf8(vertex->name);

            bfsResultList->addItem(
                QString("%1. %2")
                    .arg(i + 1)
                    .arg(locationName));
        }
    }

    bfsSummaryLabel->setText(
        QString("BFS Analysis Complete! %1 locations are reachable.")
            .arg(count));

    statusLabel->setText("BFS connectivity analysis completed.");
}
void MainWindow::showRescueCaseDetails(QListWidgetItem *item)
{
    if (!item)
    {
        caseDetailsLabel->setText(
            "NO CASE SELECTED\n\n"
            "Select a rescue case to view its details.");

        priorityCategoryLabel->setText("PRIORITY: --");
        priorityScoreLabel->setText("-- / 100");
        dangerScoreLabel->setText("-- / 100");
        signalScoreLabel->setText("-- / 100");
        distanceRiskLabel->setText("-- / 100");
        exitDetailsLabel->setText("No exit information available");
        return;
    }

    int caseId = item->data(Qt::UserRole).toInt();
    int priority = item->data(Qt::UserRole + 1).toInt();

    QString name = item->data(Qt::UserRole + 2).toString();
    int locationId = item->data(Qt::UserRole + 3).toInt();
    int danger = item->data(Qt::UserRole + 4).toInt();
    int signal = item->data(Qt::UserRole + 5).toInt();
    int distanceRisk = item->data(Qt::UserRole + 6).toInt();
    int exitId = item->data(Qt::UserRole + 7).toInt();
    int distance = item->data(Qt::UserRole + 8).toInt();
    QString exitName = item->data(Qt::UserRole + 9).toString();
    QString category = item->data(Qt::UserRole + 10).toString();

    const Vertex *locationVertex = graph.getVertex(locationId);

    QString locationName = locationVertex
                               ? QString::fromUtf8(locationVertex->name)
                               : "Unknown Location";

    caseDetailsLabel->setText(
        QString("CASE #%1\nVICTIM: %2\nLOCATION: %3")
            .arg(caseId)
            .arg(name)
            .arg(locationName));

    priorityCategoryLabel->setText(
        "PRIORITY: " + category);

    priorityScoreLabel->setText(
        QString("%1 / 100").arg(priority));

    dangerScoreLabel->setText(
        QString("%1 / 100").arg(danger));

    signalScoreLabel->setText(
        QString("%1 / 100").arg(signal));

    distanceRiskLabel->setText(
        QString("%1 / 100").arg(distanceRisk));

    exitDetailsLabel->setText(
        QString("%1\nRoute Distance: %2")
            .arg(exitName)
            .arg(distance));
}
// Update Successfully Rescued List
void MainWindow::updateRescuedList()
{
    if (rescuedList == nullptr || rescuedCountLabel == nullptr)
        return;

    rescuedCountLabel->setText(
        QString("Total: %1").arg(rescuedList->count()));
}
