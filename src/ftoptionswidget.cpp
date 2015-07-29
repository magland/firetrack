#include "ftoptionswidget.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSpacerItem>

class FTOptionsWidgetPrivate {
public:
	FTOptionsWidget *q;

	QCheckBox *m_show_channel_numbers;
	QCheckBox *m_auto_select_channels;
};


FTOptionsWidget::FTOptionsWidget(QWidget *parent) : QWidget(parent)
{
	d=new FTOptionsWidgetPrivate;
	d->q=this;

	QGridLayout *layout=new QGridLayout;
	setLayout(layout);

	int row=0;

	{
		QCheckBox *CB=new QCheckBox("Show channel numbers"); CB->setChecked(true);
		layout->addWidget(CB,row,0);
		connect(CB,SIGNAL(stateChanged(int)),this,SIGNAL(signalOptionsChanged()));
		d->m_show_channel_numbers=CB;
		row++;
	}
	{
		QCheckBox *CB=new QCheckBox("Auto select channels"); CB->setChecked(true);
		layout->addWidget(CB,row,0);
		connect(CB,SIGNAL(stateChanged(int)),this,SIGNAL(signalOptionsChanged()));
		d->m_auto_select_channels=CB;
		row++;
	}

	QSpacerItem *SI=new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding);
	layout->addItem(SI,row,0);

	this->setWindowTitle("FireTrack Options");
}

FTOptionsWidget::~FTOptionsWidget()
{
	delete d;
}

bool FTOptionsWidget::showChannelNumbers()
{
	return d->m_show_channel_numbers->isChecked();
}

bool FTOptionsWidget::autoSelectChannels()
{
	return d->m_auto_select_channels->isChecked();
}

