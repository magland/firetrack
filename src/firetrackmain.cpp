#include <QApplication>
#include <QScriptEngine>
#include <QDebug>
#include <qdatetime.h>
#include "ftcontroller.h"
#include <QStringList>
#include "textfile.h"
#include "usagetracking.h"
#include "cvcommon.h"
#include "ftelectrodearrayview.h"

/*
 * TO DO:
 * Clean up temporary files
 * */


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	//MainWindow w;
	//w.show();

	QStringList args;
	for (int i=1; i<argc; i++) {
		args << QString(argv[i]);
	}

	qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

	//args << "/tmp/tp903426a1_f13b_43a2_a1b8_5ff229f26018_run_java_script.js";

//	FTElectrodeArrayView *V=new FTElectrodeArrayView;
//	int M=100,T=30;
//	Mda L; L.allocate(M,2);
//	Mda W; W.allocate(M,T);
//	for (int i=0; i<M; i++) {
//		L.setValue(i/6 + ((i%6)%2)*0.5,i,0);
//		L.setValue(i%6,i,1);
//		for (int t=0; t<T; t++) {
//			W.setValue(i+t,i,t);
//		}
//	}
//	V->setElectrodeLocations(L);
//	V->setWaveform(W);
//	V->show();
//	return a.exec();

	QString script_path;
	QString waveforms_path;
	QString locations_path;
	for (int i=0; i<args.count(); i++) {
		QString str=args[i];
		if (str=="--waveforms") {
			waveforms_path=args.value(i+1);
		}
		if (str=="--locations") {
			locations_path=args.value(i+1);
		}
	}

	QScriptEngine *engine=new QScriptEngine;

	//qScriptRegisterMetaType(engine, myObjectToScriptValue, myObjectFromScriptValue);


	FTController FIRETRACK;
	QScriptValue FIRETRACK_value = engine->newQObject(&FIRETRACK);
	   //QScriptValue FIRETRACK_value = engine->newQObject(new QObject());
	engine->globalObject().setProperty("FIRETRACK", FIRETRACK_value);

	QString script;
	if (!script_path.isEmpty()) {
		script=read_text_file(script_path);
	} else {
		if (waveforms_path.isEmpty()) {
			waveforms_path="/home/magland/matlab/scda_ss/jfm/core/scratch/test_all_channels/waveforms_first_5e5_points.mda";
		}
		if (locations_path.isEmpty()) {
			locations_path="/home/magland/matlab/scda_ss/jfm/core/scratch/test_all_channels/locations.mda";
		}
		script="";
		script+=QString("var V%1=FIRETRACK.createFireTrackerWidget();\n").arg(0);
		script+=QString("var X%1=FIRETRACK.readArray('%2');\n").arg(0).arg(waveforms_path);
		script+=QString("var L%1=FIRETRACK.readArray('%2');\n").arg(0).arg(locations_path);
		script+=QString("V%1.setWaveforms(X%1);\n").arg(0);
		script+=QString("V%1.setElectrodeLocations(L%1);\n").arg(0);
		//script+=QString("V%1.setTitle(\"This is a test %1.\");\n").arg(0);
		script+=QString("V%1.show();\n").arg(0);
		script+="\n";
	}

	QScriptValue result = engine->evaluate(script);
	if (result.isError()) {
		qWarning() << "Error running script: "+result.toString();
	}

	CleanupObject cleanup_object;
	QObject::connect(&a, SIGNAL(aboutToQuit()), &cleanup_object, SLOT(closing()));

	int ret=a.exec();

	engine->collectGarbage();

	delete engine;
	printf("Number of files open: %d, number of unfreed mallocs: %d, number of unfreed megabytes: %g\n",jnumfilesopen(),jmalloccount(),(int)jbytesallocated()*1.0/1000000);

	return ret;
}
