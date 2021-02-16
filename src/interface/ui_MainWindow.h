/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "Buffered_viewer.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLoad_New_File;
    QAction *actionLoad_Additional_File;
    QAction *actionLoad_Points;
    QAction *actionQuit;
    QAction *actionVertices;
    QAction *actionEdges;
    QAction *actionTriangles;
    QAction *actionPrimitives;
    QAction *actionEdge_collapse;
    QAction *actionPlanes;
    QAction *actionPlanar_primitives;
    QAction *actionProject_on_primitives;
    QAction *actionLight;
    QAction *actionSet_cost_placement_strategy;
    QAction *actionSave;
    QAction *actionNoise;
    QAction *actionPromptPrimitives;
    QAction *actionPrompt_complex;
    QAction *actionBorder_edges;
    QAction *actionLoad_segmentation;
    QAction *actionMesh_stats;
    QAction *actionKeep_only_one_CC;
    QAction *actionSize_vertices_edges;
    QAction *actionMore;
    QAction *actionGraph_Perturbation;
    QAction *actionPrimitives_perturbation;
    QAction *actionPrimitives_expansion;
    QAction *actionGraph_detection;
    QAction *actionPrompt_detection_params;
    QAction *actionSave_segmentation;
    QAction *actionLoad_segmenetation;
    QAction *actionPrompt_contraction_params;
    QAction *actionNormalise;
    QWidget *centralwidget;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    Buffered_viewer *viewer;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuRender;
    QMenu *menuDetection;
    QMenu *menuSimplification;
    QMenu *menuPoint_set_operations;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(736, 510);
        actionLoad_New_File = new QAction(MainWindow);
        actionLoad_New_File->setObjectName(QStringLiteral("actionLoad_New_File"));
        actionLoad_Additional_File = new QAction(MainWindow);
        actionLoad_Additional_File->setObjectName(QStringLiteral("actionLoad_Additional_File"));
        actionLoad_Points = new QAction(MainWindow);
        actionLoad_Points->setObjectName(QStringLiteral("actionLoad_Points"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        actionVertices = new QAction(MainWindow);
        actionVertices->setObjectName(QStringLiteral("actionVertices"));
        actionEdges = new QAction(MainWindow);
        actionEdges->setObjectName(QStringLiteral("actionEdges"));
        actionTriangles = new QAction(MainWindow);
        actionTriangles->setObjectName(QStringLiteral("actionTriangles"));
        actionPrimitives = new QAction(MainWindow);
        actionPrimitives->setObjectName(QStringLiteral("actionPrimitives"));
        actionEdge_collapse = new QAction(MainWindow);
        actionEdge_collapse->setObjectName(QStringLiteral("actionEdge_collapse"));
        actionPlanes = new QAction(MainWindow);
        actionPlanes->setObjectName(QStringLiteral("actionPlanes"));
        actionPlanar_primitives = new QAction(MainWindow);
        actionPlanar_primitives->setObjectName(QStringLiteral("actionPlanar_primitives"));
        actionProject_on_primitives = new QAction(MainWindow);
        actionProject_on_primitives->setObjectName(QStringLiteral("actionProject_on_primitives"));
        actionLight = new QAction(MainWindow);
        actionLight->setObjectName(QStringLiteral("actionLight"));
        actionSet_cost_placement_strategy = new QAction(MainWindow);
        actionSet_cost_placement_strategy->setObjectName(QStringLiteral("actionSet_cost_placement_strategy"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionNoise = new QAction(MainWindow);
        actionNoise->setObjectName(QStringLiteral("actionNoise"));
        actionPromptPrimitives = new QAction(MainWindow);
        actionPromptPrimitives->setObjectName(QStringLiteral("actionPromptPrimitives"));
        actionPrompt_complex = new QAction(MainWindow);
        actionPrompt_complex->setObjectName(QStringLiteral("actionPrompt_complex"));
        actionBorder_edges = new QAction(MainWindow);
        actionBorder_edges->setObjectName(QStringLiteral("actionBorder_edges"));
        actionLoad_segmentation = new QAction(MainWindow);
        actionLoad_segmentation->setObjectName(QStringLiteral("actionLoad_segmentation"));
        actionMesh_stats = new QAction(MainWindow);
        actionMesh_stats->setObjectName(QStringLiteral("actionMesh_stats"));
        actionKeep_only_one_CC = new QAction(MainWindow);
        actionKeep_only_one_CC->setObjectName(QStringLiteral("actionKeep_only_one_CC"));
        actionSize_vertices_edges = new QAction(MainWindow);
        actionSize_vertices_edges->setObjectName(QStringLiteral("actionSize_vertices_edges"));
        actionMore = new QAction(MainWindow);
        actionMore->setObjectName(QStringLiteral("actionMore"));
        actionGraph_Perturbation = new QAction(MainWindow);
        actionGraph_Perturbation->setObjectName(QStringLiteral("actionGraph_Perturbation"));
        actionPrimitives_perturbation = new QAction(MainWindow);
        actionPrimitives_perturbation->setObjectName(QStringLiteral("actionPrimitives_perturbation"));
        actionPrimitives_expansion = new QAction(MainWindow);
        actionPrimitives_expansion->setObjectName(QStringLiteral("actionPrimitives_expansion"));
        actionGraph_detection = new QAction(MainWindow);
        actionGraph_detection->setObjectName(QStringLiteral("actionGraph_detection"));
        actionPrompt_detection_params = new QAction(MainWindow);
        actionPrompt_detection_params->setObjectName(QStringLiteral("actionPrompt_detection_params"));
        actionSave_segmentation = new QAction(MainWindow);
        actionSave_segmentation->setObjectName(QStringLiteral("actionSave_segmentation"));
        actionLoad_segmenetation = new QAction(MainWindow);
        actionLoad_segmenetation->setObjectName(QStringLiteral("actionLoad_segmenetation"));
        actionPrompt_contraction_params = new QAction(MainWindow);
        actionPrompt_contraction_params->setObjectName(QStringLiteral("actionPrompt_contraction_params"));
        actionNormalise = new QAction(MainWindow);
        actionNormalise->setObjectName(QStringLiteral("actionNormalise"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        vboxLayout = new QVBoxLayout(centralwidget);
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        viewer = new Buffered_viewer(centralwidget);
        viewer->setObjectName(QStringLiteral("viewer"));

        hboxLayout->addWidget(viewer);


        vboxLayout->addLayout(hboxLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 736, 34));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuRender = new QMenu(menubar);
        menuRender->setObjectName(QStringLiteral("menuRender"));
        menuDetection = new QMenu(menubar);
        menuDetection->setObjectName(QStringLiteral("menuDetection"));
        menuSimplification = new QMenu(menubar);
        menuSimplification->setObjectName(QStringLiteral("menuSimplification"));
        menuPoint_set_operations = new QMenu(menubar);
        menuPoint_set_operations->setObjectName(QStringLiteral("menuPoint_set_operations"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuRender->menuAction());
        menubar->addAction(menuDetection->menuAction());
        menubar->addAction(menuSimplification->menuAction());
        menubar->addAction(menuPoint_set_operations->menuAction());
        menuFile->addAction(actionLoad_New_File);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionLoad_segmentation);
        menuFile->addAction(actionSave_segmentation);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);
        menuRender->addAction(actionVertices);
        menuRender->addAction(actionEdges);
        menuRender->addAction(actionTriangles);
        menuRender->addAction(actionPlanar_primitives);
        menuRender->addAction(actionLight);
        menuRender->addAction(actionMore);
        menuDetection->addAction(actionPlanes);
        menuDetection->addAction(actionPrimitives_expansion);
        menuDetection->addAction(actionGraph_detection);
        menuDetection->addAction(actionPromptPrimitives);
        menuDetection->addAction(actionPrompt_detection_params);
        menuDetection->addSeparator();
        menuDetection->addAction(actionGraph_Perturbation);
        menuDetection->addAction(actionPrimitives_perturbation);
        menuSimplification->addAction(actionEdge_collapse);
        menuSimplification->addAction(actionPrompt_contraction_params);
        menuSimplification->addSeparator();
        menuSimplification->addAction(actionProject_on_primitives);
        menuPoint_set_operations->addAction(actionNoise);
        menuPoint_set_operations->addAction(actionNormalise);
        menuPoint_set_operations->addAction(actionPrompt_complex);
        menuPoint_set_operations->addAction(actionMesh_stats);
        menuPoint_set_operations->addAction(actionKeep_only_one_CC);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Urban simplification", Q_NULLPTR));
        actionLoad_New_File->setText(QApplication::translate("MainWindow", "Load mesh", Q_NULLPTR));
        actionLoad_Additional_File->setText(QApplication::translate("MainWindow", "Load Additional File", Q_NULLPTR));
        actionLoad_Points->setText(QApplication::translate("MainWindow", "Load Points", Q_NULLPTR));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", Q_NULLPTR));
        actionVertices->setText(QApplication::translate("MainWindow", "Vertices", Q_NULLPTR));
        actionEdges->setText(QApplication::translate("MainWindow", "Edges", Q_NULLPTR));
        actionTriangles->setText(QApplication::translate("MainWindow", "Triangles", Q_NULLPTR));
        actionPrimitives->setText(QApplication::translate("MainWindow", "Primitives", Q_NULLPTR));
        actionEdge_collapse->setText(QApplication::translate("MainWindow", "Edge collapse", Q_NULLPTR));
        actionPlanes->setText(QApplication::translate("MainWindow", "Primitives detection", Q_NULLPTR));
        actionPlanar_primitives->setText(QApplication::translate("MainWindow", "Planar primitives", Q_NULLPTR));
        actionProject_on_primitives->setText(QApplication::translate("MainWindow", "Project on primitives", Q_NULLPTR));
        actionLight->setText(QApplication::translate("MainWindow", "Light", Q_NULLPTR));
        actionSet_cost_placement_strategy->setText(QApplication::translate("MainWindow", "Set cost/placement strategy", Q_NULLPTR));
        actionSave->setText(QApplication::translate("MainWindow", "Save mesh", Q_NULLPTR));
        actionNoise->setText(QApplication::translate("MainWindow", "Uniform noise", Q_NULLPTR));
        actionPromptPrimitives->setText(QApplication::translate("MainWindow", "Prompt primitives", Q_NULLPTR));
        actionPrompt_complex->setText(QApplication::translate("MainWindow", "Prompt complex", Q_NULLPTR));
        actionBorder_edges->setText(QApplication::translate("MainWindow", "Border edges", Q_NULLPTR));
        actionLoad_segmentation->setText(QApplication::translate("MainWindow", "Load segmentation", Q_NULLPTR));
        actionMesh_stats->setText(QApplication::translate("MainWindow", "Mesh stats", Q_NULLPTR));
        actionKeep_only_one_CC->setText(QApplication::translate("MainWindow", "Keep only one CC", Q_NULLPTR));
        actionSize_vertices_edges->setText(QApplication::translate("MainWindow", "Size vertices/edges", Q_NULLPTR));
        actionMore->setText(QApplication::translate("MainWindow", "More", Q_NULLPTR));
        actionGraph_Perturbation->setText(QApplication::translate("MainWindow", "Graph perturbation", Q_NULLPTR));
        actionPrimitives_perturbation->setText(QApplication::translate("MainWindow", "Primitives perturbation", Q_NULLPTR));
        actionPrimitives_expansion->setText(QApplication::translate("MainWindow", "Primitives expansion", Q_NULLPTR));
        actionGraph_detection->setText(QApplication::translate("MainWindow", "Graph detection", Q_NULLPTR));
        actionPrompt_detection_params->setText(QApplication::translate("MainWindow", "Prompt detection params", Q_NULLPTR));
        actionSave_segmentation->setText(QApplication::translate("MainWindow", "Save segmentation", Q_NULLPTR));
        actionLoad_segmenetation->setText(QApplication::translate("MainWindow", "Load segmentation", Q_NULLPTR));
        actionPrompt_contraction_params->setText(QApplication::translate("MainWindow", "Prompt contraction params", Q_NULLPTR));
        actionNormalise->setText(QApplication::translate("MainWindow", "Normalise", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", Q_NULLPTR));
        menuRender->setTitle(QApplication::translate("MainWindow", "Render", Q_NULLPTR));
        menuDetection->setTitle(QApplication::translate("MainWindow", "Detection", Q_NULLPTR));
        menuSimplification->setTitle(QApplication::translate("MainWindow", "Simplification", Q_NULLPTR));
        menuPoint_set_operations->setTitle(QApplication::translate("MainWindow", "Triangulation", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
