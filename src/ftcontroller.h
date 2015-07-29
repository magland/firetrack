#ifndef FTCONTROLLER_H
#define FTCONTROLLER_H

#include <QObject>

class FTController : public QObject
{
	Q_OBJECT
public:
	FTController();
	~FTController();
	Q_INVOKABLE QWidget *createTimeSeriesWidget();
	Q_INVOKABLE QWidget *createTimeSeriesView();
	Q_INVOKABLE QWidget *createLabelView();
	Q_INVOKABLE QObject *loadArray(QString path);
	Q_INVOKABLE QObject *readArray(QString path);

	Q_INVOKABLE QWidget *createFireTrackWidget();
};


#endif // FTCONTROLLER_H
