/********************************************************************************
** Form generated from reading UI file 'Menu_edge_collapse.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MENU_EDGE_COLLAPSE_H
#define UI_MENU_EDGE_COLLAPSE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Menu_edge_collapse
{
public:
    QGridLayout *gridLayout;
    QLabel *txt_nb_vertices;
    QLabel *label_3;
    QPushButton *pushButton_collapse;
    QLabel *label_4;
    QLabel *label_5;
    QSlider *horizontalSlider;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout;
    QComboBox *m_simplificationMethod;
    QSpinBox *spinBox_nb_remaining_vertices;
    QLabel *txt_nb_collapses;
    QGroupBox *groupBox_3;
    QFormLayout *formLayout_2;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBox_abstraction_weight;
    QLabel *label_9;
    QDoubleSpinBox *doubleSpinBox_boundary_weight;
    QGroupBox *groupBox;
    QCheckBox *checkBox_preserve_normals;
    QDoubleSpinBox *doubleSpinBox_preserve_normals;
    QCheckBox *checkBox_link_condition;

    void setupUi(QDialog *Menu_edge_collapse)
    {
        if (Menu_edge_collapse->objectName().isEmpty())
            Menu_edge_collapse->setObjectName(QStringLiteral("Menu_edge_collapse"));
        Menu_edge_collapse->resize(487, 580);
        Menu_edge_collapse->setMinimumSize(QSize(487, 580));
        Menu_edge_collapse->setMaximumSize(QSize(9999, 99999));
        gridLayout = new QGridLayout(Menu_edge_collapse);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        txt_nb_vertices = new QLabel(Menu_edge_collapse);
        txt_nb_vertices->setObjectName(QStringLiteral("txt_nb_vertices"));

        gridLayout->addWidget(txt_nb_vertices, 1, 2, 1, 1, Qt::AlignRight);

        label_3 = new QLabel(Menu_edge_collapse);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 2);

        pushButton_collapse = new QPushButton(Menu_edge_collapse);
        pushButton_collapse->setObjectName(QStringLiteral("pushButton_collapse"));

        gridLayout->addWidget(pushButton_collapse, 0, 1, 1, 2);

        label_4 = new QLabel(Menu_edge_collapse);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 4, 0, 1, 2);

        label_5 = new QLabel(Menu_edge_collapse);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 1, 0, 1, 1);

        horizontalSlider = new QSlider(Menu_edge_collapse);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setMinimum(0);
        horizontalSlider->setMaximum(100);
        horizontalSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSlider, 0, 0, 1, 1);

        groupBox_2 = new QGroupBox(Menu_edge_collapse);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout = new QVBoxLayout(groupBox_2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        m_simplificationMethod = new QComboBox(groupBox_2);
        m_simplificationMethod->setObjectName(QStringLiteral("m_simplificationMethod"));

        verticalLayout->addWidget(m_simplificationMethod);


        gridLayout->addWidget(groupBox_2, 6, 0, 1, 3);

        spinBox_nb_remaining_vertices = new QSpinBox(Menu_edge_collapse);
        spinBox_nb_remaining_vertices->setObjectName(QStringLiteral("spinBox_nb_remaining_vertices"));
        spinBox_nb_remaining_vertices->setMinimum(1);
        spinBox_nb_remaining_vertices->setMaximum(999999999);
        spinBox_nb_remaining_vertices->setValue(1);

        gridLayout->addWidget(spinBox_nb_remaining_vertices, 4, 2, 1, 1);

        txt_nb_collapses = new QLabel(Menu_edge_collapse);
        txt_nb_collapses->setObjectName(QStringLiteral("txt_nb_collapses"));

        gridLayout->addWidget(txt_nb_collapses, 2, 2, 1, 1, Qt::AlignRight);

        groupBox_3 = new QGroupBox(Menu_edge_collapse);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setEnabled(true);
        groupBox_3->setMinimumSize(QSize(0, 300));
        groupBox_3->setMaximumSize(QSize(16777215, 300));
        formLayout_2 = new QFormLayout(groupBox_3);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_6);

        doubleSpinBox_abstraction_weight = new QDoubleSpinBox(groupBox_3);
        doubleSpinBox_abstraction_weight->setObjectName(QStringLiteral("doubleSpinBox_abstraction_weight"));
        doubleSpinBox_abstraction_weight->setDecimals(5);
        doubleSpinBox_abstraction_weight->setMaximum(1);
        doubleSpinBox_abstraction_weight->setSingleStep(0.1);
        doubleSpinBox_abstraction_weight->setValue(0.5);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, doubleSpinBox_abstraction_weight);

        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName(QStringLiteral("label_9"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_9);

        doubleSpinBox_boundary_weight = new QDoubleSpinBox(groupBox_3);
        doubleSpinBox_boundary_weight->setObjectName(QStringLiteral("doubleSpinBox_boundary_weight"));
        doubleSpinBox_boundary_weight->setDecimals(2);
        doubleSpinBox_boundary_weight->setSingleStep(0.5);
        doubleSpinBox_boundary_weight->setValue(0.8);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, doubleSpinBox_boundary_weight);

        groupBox = new QGroupBox(groupBox_3);
        groupBox->setObjectName(QStringLiteral("groupBox"));

        formLayout_2->setWidget(8, QFormLayout::LabelRole, groupBox);

        checkBox_preserve_normals = new QCheckBox(groupBox_3);
        checkBox_preserve_normals->setObjectName(QStringLiteral("checkBox_preserve_normals"));
        checkBox_preserve_normals->setChecked(true);

        formLayout_2->setWidget(9, QFormLayout::LabelRole, checkBox_preserve_normals);

        doubleSpinBox_preserve_normals = new QDoubleSpinBox(groupBox_3);
        doubleSpinBox_preserve_normals->setObjectName(QStringLiteral("doubleSpinBox_preserve_normals"));
        doubleSpinBox_preserve_normals->setDecimals(2);
        doubleSpinBox_preserve_normals->setMaximum(180);
        doubleSpinBox_preserve_normals->setSingleStep(0.1);
        doubleSpinBox_preserve_normals->setValue(150);

        formLayout_2->setWidget(9, QFormLayout::FieldRole, doubleSpinBox_preserve_normals);

        checkBox_link_condition = new QCheckBox(groupBox_3);
        checkBox_link_condition->setObjectName(QStringLiteral("checkBox_link_condition"));
        checkBox_link_condition->setChecked(false);

        formLayout_2->setWidget(10, QFormLayout::LabelRole, checkBox_link_condition);


        gridLayout->addWidget(groupBox_3, 7, 0, 1, 3);


        retranslateUi(Menu_edge_collapse);

        QMetaObject::connectSlotsByName(Menu_edge_collapse);
    } // setupUi

    void retranslateUi(QDialog *Menu_edge_collapse)
    {
        Menu_edge_collapse->setWindowTitle(QApplication::translate("Menu_edge_collapse", "Edge collapse", Q_NULLPTR));
        txt_nb_vertices->setText(QApplication::translate("Menu_edge_collapse", "0", Q_NULLPTR));
        label_3->setText(QApplication::translate("Menu_edge_collapse", "Number of collapses", Q_NULLPTR));
        pushButton_collapse->setText(QApplication::translate("Menu_edge_collapse", "Perform collapse", Q_NULLPTR));
        label_4->setText(QApplication::translate("Menu_edge_collapse", "Number of vertices after collapse", Q_NULLPTR));
        label_5->setText(QApplication::translate("Menu_edge_collapse", "Number of vertices", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("Menu_edge_collapse", "Policy", Q_NULLPTR));
        m_simplificationMethod->clear();
        m_simplificationMethod->insertItems(0, QStringList()
         << QApplication::translate("Menu_edge_collapse", "Structure preserving (memoryless)", Q_NULLPTR)
         << QApplication::translate("Menu_edge_collapse", "Structure preserving", Q_NULLPTR)
         << QApplication::translate("Menu_edge_collapse", "Garland Heckbert", Q_NULLPTR)
         << QApplication::translate("Menu_edge_collapse", "Min length and midpoint", Q_NULLPTR)
         << QApplication::translate("Menu_edge_collapse", "Lindstrom Turk", Q_NULLPTR)
        );
        txt_nb_collapses->setText(QApplication::translate("Menu_edge_collapse", "1", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("Menu_edge_collapse", "Simplification parameters", Q_NULLPTR));
        label_6->setText(QApplication::translate("Menu_edge_collapse", "Abstraction weight", Q_NULLPTR));
        label_9->setText(QApplication::translate("Menu_edge_collapse", "Border weight", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("Menu_edge_collapse", "Valid criterions", Q_NULLPTR));
        checkBox_preserve_normals->setText(QApplication::translate("Menu_edge_collapse", "Preserve normals angle", Q_NULLPTR));
        checkBox_link_condition->setText(QApplication::translate("Menu_edge_collapse", "Link condition", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Menu_edge_collapse: public Ui_Menu_edge_collapse {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MENU_EDGE_COLLAPSE_H
