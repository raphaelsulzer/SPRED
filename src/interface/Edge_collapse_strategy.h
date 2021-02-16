#ifndef EDGE_COLLAPSE_STRATEGY_H
#define EDGE_COLLAPSE_STRATEGY_H
#include "ui_Edge_collapse_strategy.h"
#include <CGAL/Qt/DemosMainWindow.h>
#include <QSlider>
#include <QFileDialog>
#include <string>
class QWidget;


class Edge_collapse_strategy : public QDialog, private Ui::StrategyCollapseDialog
{
	Q_OBJECT

private:
	int num_edges;

public:
	Edge_collapse_strategy(QObject* parent,std::string current_placement,std::string current_cost);

	void connectActions(QObject* parent);

	public slots:
		void accept();
		void reject();			
		//void update_strategy();

		
};




#endif
