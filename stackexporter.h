#ifndef STACKEXPORTER_H
#define STACKEXPORTER_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPrinter>
#include <QFileDialog>
#include <QTextDocument>
#include <QPainter>
#include <QTextDocumentFragment>
#include <QTextOption>
#include <QStaticText>
#include <QDesktopServices>

#include "configman.h"

class StackExporter : public QObject {
    Q_OBJECT
public:
    StackExporter(ConfigMan* cfgMan,
                  QString stackPath);
    void readStack(QString stackPath);
    int getTextHeight(QString text, int ptSize);
    int getMaximumFontSize(QString text, int width, int height);
    void exportStack();

private:
    ConfigMan* mCfgMan;
    QJsonObject mStackObj;

};

#endif // STACKEXPORTER_H
