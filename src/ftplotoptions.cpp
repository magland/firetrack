#include "ftplotoptions.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>

class FTPlotOptionsPrivate {
public:
	FTPlotOptions *q;
	QCheckBox *m_uniform_vertical_spacing;
    QSpinBox *m_animation_speed;
};


FTPlotOptions::FTPlotOptions(QWidget *parent) : QWidget(parent)
{
	d=new FTPlotOptionsPrivate;
	d->q=this;

	QGridLayout *layout=new QGridLayout;
	setLayout(layout);

	int row=0;

	{
		QCheckBox *CB=new QCheckBox("Uniform vertical spacing"); CB->setChecked(true);
		layout->addWidget(CB,row,0);
		connect(CB,SIGNAL(stateChanged(int)),this,SIGNAL(signalOptionsChanged()));
		d->m_uniform_vertical_spacing=CB;
		row++;
	}
	{
		QHBoxLayout *LL=new QHBoxLayout;
		LL->addWidget(new QLabel("Vertical scaling:"));
		QToolButton *Bminus=new QToolButton(); Bminus->setText(" - ");
		QToolButton *Bplus=new QToolButton(); Bplus->setText(" + ");
		LL->addWidget(Bminus);
		LL->addWidget(Bplus);
		LL->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding));
		layout->addLayout(LL,row,0);
		connect(Bminus,SIGNAL(clicked()),this,SLOT(slot_vertical_scaling_minus()));
		connect(Bplus,SIGNAL(clicked()),this,SLOT(slot_vertical_scaling_plus()));
		row++;
	}
    {
        QHBoxLayout *LL=new QHBoxLayout;
        LL->addWidget(new QLabel("Animation speed (Hz):"));
        QSpinBox *SB=new QSpinBox;
        SB->setRange(1,20000);
        SB->setValue(10);
        LL->addWidget(SB);
        LL->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding));
        layout->addLayout(LL,row,0);
        connect(SB,SIGNAL(valueChanged(int)),this,SIGNAL(signalOptionsChanged()));
        d->m_animation_speed=SB;
        row++;
    }

	QSpacerItem *SI=new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding);
	layout->addItem(SI,row,0);

	this->setWindowTitle("FireTrack Options");
}

FTPlotOptions::~FTPlotOptions()
{
	delete d;
}

void FTPlotOptions::slot_vertical_scaling_minus()
{
	emit signalVerticalScaling(1.0/1.2);
}
void FTPlotOptions::slot_vertical_scaling_plus()
{
	emit signalVerticalScaling(1.2);
}

bool FTPlotOptions::uniformVerticalChannelSpacing()
{
    return d->m_uniform_vertical_spacing->isChecked();
}

float FTPlotOptions::animationSpeed()
{
    return d->m_animation_speed->value();
}

