#include "ftelectrodearraywidget.h"
#include "ftelectrodearrayview.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QAction>
#include <QSlider>
#include <QTimer>
#include <QLabel>
#include "ftoptionswidget.h"
#include "fthelpwidget.h"

class FTElectrodeArrayWidgetPrivate {
public:
	FTElectrodeArrayWidget *q;
	FTElectrodeArrayView *m_view;
	QSlider *m_slider;
	QPushButton *m_animate_button;
	QLabel *m_timepoint_label;
	FTOptionsWidget *m_options_widget;
	FTHelpWidget *m_help_widget;

	void update_animate_button();
};

FTElectrodeArrayWidget::FTElectrodeArrayWidget(QWidget *parent) : QWidget(parent)
{
	d=new FTElectrodeArrayWidgetPrivate;
	d->q=this;

	d->m_options_widget=new FTOptionsWidget(0);
	d->m_options_widget->setWindowFlags(Qt::WindowStaysOnTopHint);
	connect(d->m_options_widget,SIGNAL(signalOptionsChanged()),this,SLOT(slot_options_changed()));

	d->m_help_widget=new FTHelpWidget(0);
	d->m_help_widget->setWindowFlags(Qt::WindowStaysOnTopHint);

	d->m_view=new FTElectrodeArrayView;
	connect(d->m_view,SIGNAL(signalSelectedElectrodesChanged()),this,SIGNAL(signalSelectedElectrodesChanged()));
	connect(d->m_view,SIGNAL(signalElectrodeLeftClicked(int)),this,SIGNAL(signalElectrodeLeftClicked(int)));

	QVBoxLayout *layout=new QVBoxLayout;
	layout->addWidget(d->m_view);

	d->m_slider=new QSlider; d->m_slider->setOrientation(Qt::Horizontal);
	connect(d->m_slider,SIGNAL(sliderMoved(int)),this,SLOT(slot_slider_moved()));
	connect(d->m_slider,SIGNAL(actionTriggered(int)),this,SLOT(slot_slider_action_triggered()));

	QString sheet="QSlider::groove:horizontal {border: 1px solid #999999;height: 18px;border-radius: 9px; background-color:lightgray;} QSlider::handle:horizontal {width: 30px; background-color:black; border-radius:9px;}";
	d->m_slider->setStyleSheet(sheet);

	d->m_timepoint_label=new QLabel; d->m_timepoint_label->setFixedHeight(20);

	QHBoxLayout *button_layout=new QHBoxLayout;
	{
		QPushButton *B0=new QPushButton("Animate");
		connect(B0,SIGNAL(clicked()),this,SLOT(slot_animate()));
		button_layout->addWidget(B0);
		d->m_animate_button=B0;
	}
	{
		QPushButton *B0=new QPushButton("Rewind");
		connect(B0,SIGNAL(clicked()),this,SLOT(slot_rewind()));
		button_layout->addWidget(B0);
	}
	button_layout->addWidget(d->m_slider);
	button_layout->addWidget(d->m_timepoint_label);
	{
		QPushButton *B0=new QPushButton("Options...");
		connect(B0,SIGNAL(clicked()),this,SLOT(slot_options()));
		button_layout->addWidget(B0);
	}
	{
		QPushButton *B0=new QPushButton("Help...");
		connect(B0,SIGNAL(clicked()),this,SLOT(slot_help()));
		button_layout->addWidget(B0);
	}
	button_layout->addStretch();
	layout->addLayout(button_layout);

	this->setLayout(layout);

	connect(d->m_view,SIGNAL(signalTimepointChanged()),this,SLOT(slot_timepoint_changed()));
}

void FTElectrodeArrayWidget::setWaveform(const Mda &X)
{
	d->m_view->setWaveform(X);
	d->m_slider->setRange(-1,X.N2()-1);
	d->m_slider->setValue(-1);
	d->m_view->setTimepoint(-1);
}

void FTElectrodeArrayWidget::setElectrodeLocations(const Mda &X)
{
	d->m_view->setElectrodeLocations(X);
}

void FTElectrodeArrayWidget::animate()
{
	d->m_view->animate();
	d->update_animate_button();
}

int FTElectrodeArrayWidget::timepoint()
{
	return d->m_view->timepoint();
}

void FTElectrodeArrayWidget::setTimepoint(int t)
{
	d->m_view->setTimepoint(t);
	d->m_slider->setValue(t);
}

QList<int> FTElectrodeArrayWidget::selectedElectrodeIndices()
{
	return d->m_view->selectedElectrodeIndices();
}

void FTElectrodeArrayWidget::setSelectedElectrodeIndices(const QList<int> &X)
{
	d->m_view->setSelectedElectrodeIndices(X);
}

void FTElectrodeArrayWidget::wheelEvent(QWheelEvent *evt)
{
	if (d->m_view->timepoint()<0) {
		d->m_view->setTimepoint(d->m_view->waveform()->N2()/2);
		return;
	}
	int dd=1;
	if (evt->delta()<0) dd=-1;
	d->m_view->setTimepoint(d->m_view->timepoint()+dd);
}

void FTElectrodeArrayWidget::slot_animate()
{
	if (!d->m_view->isAnimating()) {
		d->m_view->animate();
		d->update_animate_button();
	}
	else {
		d->m_view->pauseAnimation();
		d->update_animate_button();
	}
}

void FTElectrodeArrayWidget::slot_pause()
{
	d->m_view->pauseAnimation();
	d->update_animate_button();
}

void FTElectrodeArrayWidget::slot_rewind()
{
	d->m_view->rewindAnimation();
}

void FTElectrodeArrayWidget::slot_timepoint_changed()
{
	int t0=d->m_view->timepoint();
	d->m_slider->setValue(t0);
	if (t0<0) d->m_timepoint_label->setText("(max)");
	else d->m_timepoint_label->setText(QString("%1").arg(t0+1));
	d->update_animate_button();
	emit signalTimepointChanged();
}

void FTElectrodeArrayWidget::slot_slider_moved()
{
	d->m_view->setTimepoint(d->m_slider->value());
	d->m_view->pauseAnimation();
	d->update_animate_button();
}

void FTElectrodeArrayWidget::slot_slider_action_triggered()
{
	QTimer::singleShot(10,this,SLOT(slot_slider_moved())); //this is necessary because value has not yet been set
}

void FTElectrodeArrayWidget::slot_options()
{
	if (!d->m_options_widget->isVisible()) {
		d->m_options_widget->resize(300,300);
		d->m_options_widget->move(this->topLevelWidget()->pos().x()+50,this->topLevelWidget()->pos().y()+50);
	}
	d->m_options_widget->show();
	d->m_options_widget->raise();
}

void FTElectrodeArrayWidget::slot_help()
{
	if (!d->m_help_widget->isVisible()) {
		d->m_help_widget->resize(600,300);
		d->m_help_widget->move(this->topLevelWidget()->pos().x()+this->topLevelWidget()->width()-50-d->m_help_widget->width(),this->topLevelWidget()->pos().y()+50);
	}
	d->m_help_widget->show();
	d->m_help_widget->raise();
}

void FTElectrodeArrayWidget::slot_options_changed()
{
	d->m_view->setShowChannelNumbers(d->m_options_widget->showChannelNumbers());
	d->m_view->setAutoSelectChannels(d->m_options_widget->autoSelectChannels());
}

FTElectrodeArrayWidget::~FTElectrodeArrayWidget()
{
	delete d->m_options_widget;
	delete d->m_help_widget;
	delete d;
}



void FTElectrodeArrayWidgetPrivate::update_animate_button()
{
	if (m_view->isAnimating()) m_animate_button->setText("Pause");
	else m_animate_button->setText("Animate");
}
