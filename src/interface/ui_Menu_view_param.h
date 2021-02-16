/********************************************************************************
** Form generated from reading UI file 'Menu_view_param.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MENU_VIEW_PARAM_H
#define UI_MENU_VIEW_PARAM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>

QT_BEGIN_NAMESPACE

class Ui_Menu_view_param
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QLabel *label_2;
    QSlider *horizontalSlider_theta;
    QSlider *horizontalSlider_phi;
    QLabel *label;
    QCheckBox *checkBox_relative_light;
    QGroupBox *groupBox_2;
    QSlider *horizontalSlider_vertex_size;
    QLabel *label_3;
    QSlider *horizontalSlider_vertex_style;
    QLabel *label_5;
    QGroupBox *groupBox_3;
    QLabel *label_4;
    QSlider *horizontalSlider_edge_size;
    QLabel *label_6;
    QSlider *horizontalSlider_edge_style;
    QSlider *horizontalSlider_edge_luminosity;
    QLabel *label_8;
    QGroupBox *groupBox_4;
    QSlider *horizontalSlider_triangle_style;
    QLabel *label_7;
    QLabel *label_9;
    QSlider *horizontalSlider_triangle_luminosity;

    void setupUi(QDialog *Menu_view_param)
    {
        if (Menu_view_param->objectName().isEmpty())
            Menu_view_param->setObjectName(QStringLiteral("Menu_view_param"));
        Menu_view_param->resize(334, 525);
        buttonBox = new QDialogButtonBox(Menu_view_param);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 470, 211, 50));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(Menu_view_param);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(30, 20, 271, 111));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 40, 65, 21));
        horizontalSlider_theta = new QSlider(groupBox);
        horizontalSlider_theta->setObjectName(QStringLiteral("horizontalSlider_theta"));
        horizontalSlider_theta->setGeometry(QRect(160, 20, 111, 19));
        horizontalSlider_theta->setOrientation(Qt::Horizontal);
        horizontalSlider_phi = new QSlider(groupBox);
        horizontalSlider_phi->setObjectName(QStringLiteral("horizontalSlider_phi"));
        horizontalSlider_phi->setGeometry(QRect(160, 40, 111, 19));
        horizontalSlider_phi->setOrientation(Qt::Horizontal);
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 20, 65, 21));
        checkBox_relative_light = new QCheckBox(groupBox);
        checkBox_relative_light->setObjectName(QStringLiteral("checkBox_relative_light"));
        checkBox_relative_light->setGeometry(QRect(10, 60, 171, 26));
        checkBox_relative_light->setChecked(true);
        groupBox_2 = new QGroupBox(Menu_view_param);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(30, 130, 281, 101));
        horizontalSlider_vertex_size = new QSlider(groupBox_2);
        horizontalSlider_vertex_size->setObjectName(QStringLiteral("horizontalSlider_vertex_size"));
        horizontalSlider_vertex_size->setGeometry(QRect(160, 20, 111, 20));
        horizontalSlider_vertex_size->setValue(10);
        horizontalSlider_vertex_size->setOrientation(Qt::Horizontal);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 20, 81, 21));
        horizontalSlider_vertex_style = new QSlider(groupBox_2);
        horizontalSlider_vertex_style->setObjectName(QStringLiteral("horizontalSlider_vertex_style"));
        horizontalSlider_vertex_style->setGeometry(QRect(159, 50, 111, 20));
        horizontalSlider_vertex_style->setMaximum(4);
        horizontalSlider_vertex_style->setOrientation(Qt::Horizontal);
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 50, 81, 21));
        groupBox_3 = new QGroupBox(Menu_view_param);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(30, 210, 281, 121));
        label_4 = new QLabel(groupBox_3);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(14, 30, 71, 21));
        horizontalSlider_edge_size = new QSlider(groupBox_3);
        horizontalSlider_edge_size->setObjectName(QStringLiteral("horizontalSlider_edge_size"));
        horizontalSlider_edge_size->setGeometry(QRect(170, 30, 101, 19));
        horizontalSlider_edge_size->setValue(50);
        horizontalSlider_edge_size->setOrientation(Qt::Horizontal);
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 50, 71, 21));
        horizontalSlider_edge_style = new QSlider(groupBox_3);
        horizontalSlider_edge_style->setObjectName(QStringLiteral("horizontalSlider_edge_style"));
        horizontalSlider_edge_style->setGeometry(QRect(169, 60, 101, 20));
        horizontalSlider_edge_style->setMaximum(4);
        horizontalSlider_edge_style->setOrientation(Qt::Horizontal);
        horizontalSlider_edge_luminosity = new QSlider(groupBox_3);
        horizontalSlider_edge_luminosity->setObjectName(QStringLiteral("horizontalSlider_edge_luminosity"));
        horizontalSlider_edge_luminosity->setGeometry(QRect(170, 80, 101, 20));
        horizontalSlider_edge_luminosity->setValue(25);
        horizontalSlider_edge_luminosity->setSliderPosition(25);
        horizontalSlider_edge_luminosity->setOrientation(Qt::Horizontal);
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(10, 80, 111, 21));
        groupBox_4 = new QGroupBox(Menu_view_param);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(30, 340, 281, 80));
        horizontalSlider_triangle_style = new QSlider(groupBox_4);
        horizontalSlider_triangle_style->setObjectName(QStringLiteral("horizontalSlider_triangle_style"));
        horizontalSlider_triangle_style->setGeometry(QRect(169, 30, 101, 20));
        horizontalSlider_triangle_style->setMaximum(5);
        horizontalSlider_triangle_style->setOrientation(Qt::Horizontal);
        label_7 = new QLabel(groupBox_4);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(10, 30, 91, 21));
        label_9 = new QLabel(groupBox_4);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 60, 131, 21));
        horizontalSlider_triangle_luminosity = new QSlider(groupBox_4);
        horizontalSlider_triangle_luminosity->setObjectName(QStringLiteral("horizontalSlider_triangle_luminosity"));
        horizontalSlider_triangle_luminosity->setGeometry(QRect(169, 60, 101, 20));
        horizontalSlider_triangle_luminosity->setValue(65);
        horizontalSlider_triangle_luminosity->setOrientation(Qt::Horizontal);

        retranslateUi(Menu_view_param);
        QObject::connect(buttonBox, SIGNAL(accepted()), Menu_view_param, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Menu_view_param, SLOT(reject()));

        QMetaObject::connectSlotsByName(Menu_view_param);
    } // setupUi

    void retranslateUi(QDialog *Menu_view_param)
    {
        Menu_view_param->setWindowTitle(QApplication::translate("Menu_view_param", "Rendering options", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("Menu_view_param", "Light", Q_NULLPTR));
        label_2->setText(QApplication::translate("Menu_view_param", "Phi", Q_NULLPTR));
        label->setText(QApplication::translate("Menu_view_param", "Theta", Q_NULLPTR));
        checkBox_relative_light->setText(QApplication::translate("Menu_view_param", "Relative light", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("Menu_view_param", "Vertex", Q_NULLPTR));
        label_3->setText(QApplication::translate("Menu_view_param", "Vertex size", Q_NULLPTR));
        label_5->setText(QApplication::translate("Menu_view_param", "Vertex style", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("Menu_view_param", "Edge", Q_NULLPTR));
        label_4->setText(QApplication::translate("Menu_view_param", "Edge size", Q_NULLPTR));
        label_6->setText(QApplication::translate("Menu_view_param", "Edge style", Q_NULLPTR));
        label_8->setText(QApplication::translate("Menu_view_param", "Edge luminosity", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("Menu_view_param", "Triangle", Q_NULLPTR));
        label_7->setText(QApplication::translate("Menu_view_param", "Triangle style", Q_NULLPTR));
        label_9->setText(QApplication::translate("Menu_view_param", "Triangle luminosity", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Menu_view_param: public Ui_Menu_view_param {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MENU_VIEW_PARAM_H
