#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QFontComboBox>
#include <QSpinBox>
#include <QPushButton>

#include "configman.h"

class Settings : public QWidget {
public:
    Settings(QWidget *parent = nullptr,
             ConfigMan* cfgMan = nullptr);
    void makeUI(QWidget* parent, ConfigMan* cfgMan);

    QWidget* mParent;

protected:
    void closeEvent(QCloseEvent* event);
};

#endif // SETTINGS_H
