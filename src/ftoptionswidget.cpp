#include "ftoptionswidget.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSpacerItem>
#include <QSlider>

class FTOptionsWidgetPrivate {
public:
	FTOptionsWidget *q;

	QCheckBox *m_show_channel_numbers;
	QCheckBox *m_auto_select_channels;
	QCheckBox *m_normalize_intensity;
	QSlider *m_brightness_slider;
};


FTOptionsWidget::FTOptionsWidget(QWidget *parent) : QWidget(parent)
{
	d=new FTOptionsWidgetPrivate;
	d->q=this;

	QGridLayout *layout=new QGridLayout;
	setLayout(layout);

	int row=0;

	{
		QCheckBox *CB=new QCheckBox("Show electrode numbers"); CB->setChecked(true);
		layout->addWidget(CB,row,0);
		connect(CB,SIGNAL(stateChanged(int)),this,SIGNAL(signalOptionsChanged()));
		d->m_show_channel_numbers=CB;
		row++;
	}
	{
		QCheckBox *CB=new QCheckBox("Auto select electrodes"); CB->setChecked(true);
		layout->addWidget(CB,row,0);
		connect(CB,SIGNAL(stateChanged(int)),this,SIGNAL(signalOptionsChanged()));
		d->m_auto_select_channels=CB;
		row++;
	}
	{
		QCheckBox *CB=new QCheckBox("Normalize intensity per neuron"); CB->setChecked(true);
		layout->addWidget(CB,row,0);
		connect(CB,SIGNAL(stateChanged(int)),this,SIGNAL(signalOptionsChanged()));
		d->m_normalize_intensity=CB;
		row++;
	}
	{
		QSlider *SS=new QSlider(Qt::Horizontal);
		SS->setRange(-100,100);
		QLayout *LL=new QHBoxLayout;
		LL->addWidget(new QLabel("Brightness:"));
		LL->addWidget(SS);
		layout->addLayout(LL,row,0);
		connect(SS,SIGNAL(sliderMoved(int)),this,SIGNAL(signalOptionsChanged()));
		d->m_brightness_slider=SS;
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

bool FTOptionsWidget::normalizeIntensity()
{
	return d->m_normalize_intensity->isChecked();
}

float FTOptionsWidget::brightness()
{
	return d->m_brightness_slider->value();
}

