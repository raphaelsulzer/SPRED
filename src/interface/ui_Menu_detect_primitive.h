/********************************************************************************
** Form generated from reading UI file 'Menu_detect_primitive.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MENU_DETECT_PRIMITIVE_H
#define UI_MENU_DETECT_PRIMITIVE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Menu_detect_primitive
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *label_15;
    QLabel *label_2;
    QLabel *label_5;
    QLabel *label_11;
    QFrame *line;
    QLabel *label;
    QLabel *label_4;
    QFrame *line_3;
    QLabel *label_10;
    QFrame *line_5;
    QLabel *label_3;
    QLabel *label_6;
    QVBoxLayout *verticalLayout_2;
    QComboBox *comboBox_detect_method;
    QDoubleSpinBox *doubleSpinBox_max_angle;
    QDoubleSpinBox *doubleSpinBox_min_area;
    QDoubleSpinBox *doubleSpinBox_maxdist;
    QFrame *line_2;
    QDoubleSpinBox *doubleSpinBox_coplanarity;
    QDoubleSpinBox *doubleSpinBox_max_distance;
    QFrame *line_4;
    QDoubleSpinBox *doubleSpinBox_min_area_filter;
    QFrame *line_6;
    QSpinBox *spinBox_nb_neighbors;
    QDoubleSpinBox *doubleSpinBox_max_graph_length;
    QPushButton *pushButton_detect;
    QPushButton *pushButton_regularize;
    QPushButton *pushButton_small_proxies;
    QPushButton *pushButton_recompute_graph;

    void setupUi(QDialog *Menu_detect_primitive)
    {
        if (Menu_detect_primitive->objectName().isEmpty())
            Menu_detect_primitive->setObjectName(QStringLiteral("Menu_detect_primitive"));
        Menu_detect_primitive->setEnabled(true);
        Menu_detect_primitive->resize(454, 629);
        verticalLayout_3 = new QVBoxLayout(Menu_detect_primitive);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_15 = new QLabel(Menu_detect_primitive);
        label_15->setObjectName(QStringLiteral("label_15"));

        verticalLayout->addWidget(label_15);

        label_2 = new QLabel(Menu_detect_primitive);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);

        label_5 = new QLabel(Menu_detect_primitive);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout->addWidget(label_5);

        label_11 = new QLabel(Menu_detect_primitive);
        label_11->setObjectName(QStringLiteral("label_11"));

        verticalLayout->addWidget(label_11);

        line = new QFrame(Menu_detect_primitive);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        label = new QLabel(Menu_detect_primitive);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        label_4 = new QLabel(Menu_detect_primitive);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout->addWidget(label_4);

        line_3 = new QFrame(Menu_detect_primitive);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_3);

        label_10 = new QLabel(Menu_detect_primitive);
        label_10->setObjectName(QStringLiteral("label_10"));

        verticalLayout->addWidget(label_10);

        line_5 = new QFrame(Menu_detect_primitive);
        line_5->setObjectName(QStringLiteral("line_5"));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_5);

        label_3 = new QLabel(Menu_detect_primitive);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout->addWidget(label_3);

        label_6 = new QLabel(Menu_detect_primitive);
        label_6->setObjectName(QStringLiteral("label_6"));

        verticalLayout->addWidget(label_6);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        comboBox_detect_method = new QComboBox(Menu_detect_primitive);
        comboBox_detect_method->setObjectName(QStringLiteral("comboBox_detect_method"));

        verticalLayout_2->addWidget(comboBox_detect_method);

        doubleSpinBox_max_angle = new QDoubleSpinBox(Menu_detect_primitive);
        doubleSpinBox_max_angle->setObjectName(QStringLiteral("doubleSpinBox_max_angle"));
        doubleSpinBox_max_angle->setDecimals(2);
        doubleSpinBox_max_angle->setMaximum(90);
        doubleSpinBox_max_angle->setSingleStep(0.1);
        doubleSpinBox_max_angle->setValue(20);

        verticalLayout_2->addWidget(doubleSpinBox_max_angle);

        doubleSpinBox_min_area = new QDoubleSpinBox(Menu_detect_primitive);
        doubleSpinBox_min_area->setObjectName(QStringLiteral("doubleSpinBox_min_area"));
        doubleSpinBox_min_area->setDecimals(5);
        doubleSpinBox_min_area->setMinimum(0);
        doubleSpinBox_min_area->setMaximum(10000);
        doubleSpinBox_min_area->setSingleStep(2.5);
        doubleSpinBox_min_area->setValue(10);

        verticalLayout_2->addWidget(doubleSpinBox_min_area);

        doubleSpinBox_maxdist = new QDoubleSpinBox(Menu_detect_primitive);
        doubleSpinBox_maxdist->setObjectName(QStringLiteral("doubleSpinBox_maxdist"));
        doubleSpinBox_maxdist->setDecimals(5);
        doubleSpinBox_maxdist->setMaximum(1e+9);
        doubleSpinBox_maxdist->setValue(9999);

        verticalLayout_2->addWidget(doubleSpinBox_maxdist);

        line_2 = new QFrame(Menu_detect_primitive);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line_2);

        doubleSpinBox_coplanarity = new QDoubleSpinBox(Menu_detect_primitive);
        doubleSpinBox_coplanarity->setObjectName(QStringLiteral("doubleSpinBox_coplanarity"));
        doubleSpinBox_coplanarity->setDecimals(1);
        doubleSpinBox_coplanarity->setMaximum(90);
        doubleSpinBox_coplanarity->setSingleStep(0.1);
        doubleSpinBox_coplanarity->setValue(10);

        verticalLayout_2->addWidget(doubleSpinBox_coplanarity);

        doubleSpinBox_max_distance = new QDoubleSpinBox(Menu_detect_primitive);
        doubleSpinBox_max_distance->setObjectName(QStringLiteral("doubleSpinBox_max_distance"));
        doubleSpinBox_max_distance->setDecimals(2);
        doubleSpinBox_max_distance->setMinimum(0);
        doubleSpinBox_max_distance->setMaximum(9999);
        doubleSpinBox_max_distance->setSingleStep(0.1);
        doubleSpinBox_max_distance->setValue(0.1);

        verticalLayout_2->addWidget(doubleSpinBox_max_distance);

        line_4 = new QFrame(Menu_detect_primitive);
        line_4->setObjectName(QStringLiteral("line_4"));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line_4);

        doubleSpinBox_min_area_filter = new QDoubleSpinBox(Menu_detect_primitive);
        doubleSpinBox_min_area_filter->setObjectName(QStringLiteral("doubleSpinBox_min_area_filter"));
        doubleSpinBox_min_area_filter->setDecimals(5);
        doubleSpinBox_min_area_filter->setMaximum(9999);
        doubleSpinBox_min_area_filter->setSingleStep(0.1);
        doubleSpinBox_min_area_filter->setValue(10);

        verticalLayout_2->addWidget(doubleSpinBox_min_area_filter);

        line_6 = new QFrame(Menu_detect_primitive);
        line_6->setObjectName(QStringLiteral("line_6"));
        line_6->setFrameShape(QFrame::HLine);
        line_6->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line_6);

        spinBox_nb_neighbors = new QSpinBox(Menu_detect_primitive);
        spinBox_nb_neighbors->setObjectName(QStringLiteral("spinBox_nb_neighbors"));
        spinBox_nb_neighbors->setMaximum(50);
        spinBox_nb_neighbors->setValue(5);

        verticalLayout_2->addWidget(spinBox_nb_neighbors);

        doubleSpinBox_max_graph_length = new QDoubleSpinBox(Menu_detect_primitive);
        doubleSpinBox_max_graph_length->setObjectName(QStringLiteral("doubleSpinBox_max_graph_length"));
        doubleSpinBox_max_graph_length->setDecimals(4);
        doubleSpinBox_max_graph_length->setMaximum(1e+8);
        doubleSpinBox_max_graph_length->setValue(1.5);

        verticalLayout_2->addWidget(doubleSpinBox_max_graph_length);


        horizontalLayout->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout);

        pushButton_detect = new QPushButton(Menu_detect_primitive);
        pushButton_detect->setObjectName(QStringLiteral("pushButton_detect"));

        verticalLayout_3->addWidget(pushButton_detect);

        pushButton_regularize = new QPushButton(Menu_detect_primitive);
        pushButton_regularize->setObjectName(QStringLiteral("pushButton_regularize"));

        verticalLayout_3->addWidget(pushButton_regularize);

        pushButton_small_proxies = new QPushButton(Menu_detect_primitive);
        pushButton_small_proxies->setObjectName(QStringLiteral("pushButton_small_proxies"));

        verticalLayout_3->addWidget(pushButton_small_proxies);

        pushButton_recompute_graph = new QPushButton(Menu_detect_primitive);
        pushButton_recompute_graph->setObjectName(QStringLiteral("pushButton_recompute_graph"));

        verticalLayout_3->addWidget(pushButton_recompute_graph);


        retranslateUi(Menu_detect_primitive);

        QMetaObject::connectSlotsByName(Menu_detect_primitive);
    } // setupUi

    void retranslateUi(QDialog *Menu_detect_primitive)
    {
        Menu_detect_primitive->setWindowTitle(QApplication::translate("Menu_detect_primitive", "Plane detection (by region growing)", Q_NULLPTR));
        label_15->setText(QApplication::translate("Menu_detect_primitive", "Detection method", Q_NULLPTR));
        label_2->setText(QApplication::translate("Menu_detect_primitive", "Angle tolerance", Q_NULLPTR));
        label_5->setText(QApplication::translate("Menu_detect_primitive", "Minimum area", Q_NULLPTR));
        label_11->setText(QApplication::translate("Menu_detect_primitive", "Max distance", Q_NULLPTR));
        label->setText(QApplication::translate("Menu_detect_primitive", "Coplanarity angle", Q_NULLPTR));
        label_4->setText(QApplication::translate("Menu_detect_primitive", "Max distance", Q_NULLPTR));
        label_10->setText(QApplication::translate("Menu_detect_primitive", "Minimum area", Q_NULLPTR));
        label_3->setText(QApplication::translate("Menu_detect_primitive", "Graph neighbors", Q_NULLPTR));
        label_6->setText(QApplication::translate("Menu_detect_primitive", "Max distance edge", Q_NULLPTR));
        comboBox_detect_method->clear();
        comboBox_detect_method->insertItems(0, QStringList()
         << QApplication::translate("Menu_detect_primitive", "NN region growing", Q_NULLPTR)
         << QApplication::translate("Menu_detect_primitive", "David region growing", Q_NULLPTR)
         << QApplication::translate("Menu_detect_primitive", "Mesh region growing", Q_NULLPTR)
         << QApplication::translate("Menu_detect_primitive", "Florent region growing", Q_NULLPTR)
        );
        pushButton_detect->setText(QApplication::translate("Menu_detect_primitive", "detect primitives", Q_NULLPTR));
        pushButton_regularize->setText(QApplication::translate("Menu_detect_primitive", "Regularize primitive", Q_NULLPTR));
        pushButton_small_proxies->setText(QApplication::translate("Menu_detect_primitive", "Filter small proxies", Q_NULLPTR));
        pushButton_recompute_graph->setText(QApplication::translate("Menu_detect_primitive", "Recompute graph", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Menu_detect_primitive: public Ui_Menu_detect_primitive {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MENU_DETECT_PRIMITIVE_H
