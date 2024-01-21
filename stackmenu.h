#ifndef STACKMENU_H
#define STACKMENU_H

#include <QWidget>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFile>

#include "configman.h"

class StackMenu : public QWidget {
    Q_OBJECT
public:
    StackMenu(QWidget* parent = nullptr,
              QApplication* app = nullptr,
              QString stackPath = "",
              ConfigMan* cfgMan = nullptr,
              QString cfgDir = "");
    void makeUI(QWidget* parent,
                QApplication* app,
                QString stackPath);
    void setTheme();
    int findInArray(QJsonArray* array, QString elem);

    ConfigMan* mCfgMan;
    QString mCfgDir;

private:
    QWidget* mParent;

protected:
    void closeEvent(QCloseEvent* event);
};

#endif // STACKMENU_H
