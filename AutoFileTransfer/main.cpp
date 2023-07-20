#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSharedMemory>
#include <QPixmap>

int main(int argc, char *argv[])
{
    //防多开
    QSharedMemory shared("AutoFileTransfer");//随便填个名字就行
    if (shared.attach())
    {
        return 0;
    }
    shared.create(1);

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "AutoFileTransfer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;

    QIcon appIcon(":/res/prefix/autoupload.ico");
    a.setWindowIcon(appIcon);
    w.setWindowIcon(appIcon);

    w.show();
    return a.exec();
}
