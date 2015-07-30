#ifndef FTPLOTOPTIONS_H
#define FTPLOTOPTIONS_H

#include <QWidget>


class FTPlotOptionsPrivate;

class FTPlotOptions : public QWidget
{
	Q_OBJECT
public:
	friend class FTPlotOptionsPrivate;
	explicit FTPlotOptions(QWidget *parent = 0);

	bool uniformVerticalChannelSpacing();

private:
	FTPlotOptionsPrivate *d;

	~FTPlotOptions();

signals:
	void signalOptionsChanged();
	void signalVerticalScaling(float factor);

private slots:
	void slot_vertical_scaling_minus();
	void slot_vertical_scaling_plus();

public slots:
};

#endif // FTPLOTOPTIONS_H
