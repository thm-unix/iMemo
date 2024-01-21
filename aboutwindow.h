#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "configman.h"
#include "mainmenu.h"

class AboutWindow : public QWidget {
    Q_OBJECT
public:
    AboutWindow(QWidget* parent = nullptr,
                Platform platform = Unknown);
    void makeUI(QWidget* parent, Platform paltform);
    void setTheme();

private:
    QWidget* mParent;

protected:
    void closeEvent(QCloseEvent* event);
};

#endif // ABOUTWINDOW_H
