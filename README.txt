== FIRETRACK ==

Copyright (C) 2015 Jeremy Magland, Simons Center for Data Analysis (SCDA)
Released under GPL: license.txt

== How to compile and run the demo on a Unix system ==

Prequisite
First, you need to have Qt4 or Qt5 development environment installed.
See: http://doc.qt.io/qt-5/gettingstarted.html

Run from a console:
> cd src
> qmake
> make
> cd ..
> ./demo1.sh

[[ NOTE: the demos are temporarily disabled for this software ]]

This will download some test data (if needed) and show it.

You can also run "./demo1.sh B" which will run the second demo.

The first demo is displaying raw data.
The second demo is displaying >300 detected neuron profiles.
