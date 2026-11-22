
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "graph.h"

class QLabel;
class QPushButton;
class QComboBox;
class QLineEdit;
class QSpinBox;
class QListWidget;
class QListWidgetItem;
class MapWidget;
class QString;
class MaxHeap;
class QStackedWidget;

class MainWindow : public QWidget
{
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // Existing UI components
    QLabel *statusLabel;
    QLabel *routeInfoLabel;

    QComboBox *sourceCombo;
    QComboBox *roadSourceCombo;
    QComboBox *roadDestinationCombo;

    Graph graph;
    MapWidget *mapWidget;
    QStackedWidget *pageStack;

    // Rescue Priority Management
    MaxHeap *rescueHeap;

    QLineEdit *victimNameInput;
    QSpinBox *priorityInput;
    QComboBox *rescueLocationCombo;

    // Pending Rescue List
    QListWidget *rescueList;
    QLabel *rescueCountLabel;

    // Successfully Rescued List
    QListWidget *rescuedList;
    QLabel *rescuedCountLabel;

    // Futuristic Rescue Details Panel
    QLabel *caseDetailsLabel;
    QLabel *priorityScoreLabel;
    QLabel *dangerScoreLabel;
    QLabel *signalScoreLabel;
    QLabel *distanceRiskLabel;
    QLabel *exitDetailsLabel;
    QLabel *priorityCategoryLabel;

    // Danger Level Controls
    QComboBox *dangerModeCombo;
    QComboBox *dangerLevelCombo;

    // Emergency Signal Controls
    QComboBox *signalModeCombo;
    QComboBox *signalLevelCombo;

    int nextCaseId;

    // Existing functions
    QPushButton *createButton(const QString &text,
                              const QString &accent);

    void setupUI();
    void setupSampleGraph();

    void findSafeRoute();
    void compareExits();
    void blockSelectedRoad();
    void unblockSelectedRoad();
    void resetMapView();
    void runBFSAnalysis();

    // Rescue Management functions
    void setupRescueManagement();
    void addRescueCase();
    void processHighestPriority();
    void updateRescueList();
    void refreshPendingRescueCases();
    void updateRescuedList();
    void showRescueCaseDetails(QListWidgetItem *item);
    QComboBox *bfsSourceCombo;
    QListWidget *bfsResultList;
    QLabel *bfsSummaryLabel;
};

#endif
