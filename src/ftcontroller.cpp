#include "ftcontroller.h"
#include "sstimeserieswidget.h"
#include "sscommon.h"
#include "diskarraymodel.h"
#include <QDebug>
#include "sstimeseriesview.h"
#include "sslabelview.h"
#include "mdaobject.h"
#include "firetrackwidget.h"

FTController::FTController()
{

}

FTController::~FTController()
{

}

QWidget *FTController::createTimeSeriesWidget() {
	SSTimeSeriesWidget *W=new SSTimeSeriesWidget();
	W->setAttribute(Qt::WA_DeleteOnClose);
	W->showNormal();
	W->resize(1000,500);
	W->move(300,300);
	return W;
}

QWidget *FTController::createTimeSeriesView() {
	SSTimeSeriesView *V=new SSTimeSeriesView();
	return V;
}

QWidget *FTController::createLabelView()
{
	SSLabelView *V=new SSLabelView();
	return V;
}

QObject *FTController::loadArray(QString path) {
	SSARRAY *X=new SSARRAY();
	X->setPath(path.toLatin1().data());

	return X;
}

QObject *FTController::readArray(QString path)
{
	DiskReadMda *ret=new DiskReadMda;
	ret->setPath(path);
	return ret;
}

QWidget *FTController::createFireTrackerWidget()
{
	FireTrackWidget *W=new FireTrackWidget();
	W->setAttribute(Qt::WA_DeleteOnClose);
	W->showNormal();
	W->resize(1300,500);
	W->move(300,300);
	return W;
}
