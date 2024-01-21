#ifndef QUICKEDITOR_H
#define QUICKEDITOR_H

#include <QDialog>

#include "configman.h"

class QuickEditor : public QDialog {
public:
    QuickEditor(QWidget* parent,
                ConfigMan* cfgMan,
                QString stackPath,
                QJsonObject* stackObj,
                int i);
    void makeUI(QWidget* parent,
                ConfigMan* cfgMan,
                QString stackPath,
                QJsonObject* stackObj,
                int i);
    void setWindowGeometry();

private:
    bool mChanged = false;

protected:
    void closeEvent(QCloseEvent* event);
};

#endif // QUICKEDITOR_H
