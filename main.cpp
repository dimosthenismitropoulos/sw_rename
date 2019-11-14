//  Copyright (c) Mitropoulos Dimosthenis
//  Email   mitropoulosdimosthenis@gmail.com
//  License   GPL3
//  Feedback and bug reports are appreciated

#include "mainwindow.h"
#include <QApplication>

#include <QCommandLineParser>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("file"), "Document to open");
    parser.process(a);


    //  Initialize Main Window
    MainWindow w;
    if (parser.positionalArguments().count() > 0)
    {
        //  Set the starting location
        QString startingLocation = parser.positionalArguments().at(0);
        QDir startingDir(startingLocation);
        if (startingDir.exists()){
             w.setLocationUrl(startingLocation);
        }
    }

    w.show();
    return QApplication::exec();
}

