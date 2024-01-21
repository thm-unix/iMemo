#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QToolButton>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QIcon>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFont>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

#include "configman.h"

enum Platform {
    Linux,
    Mac,
    Windows,
    Unknown
};

class MainMenu : public QWidget {
    Q_OBJECT

public:
    bool isFirstRun();
    void makeUI(bool remake);
    void setTheme();
    void buildList(QString searchRequest);
    int countTerms(QString path);
    void import(QStringList paths);
    void newStack();
    MainMenu(QWidget *parent = nullptr,
             QApplication* app = nullptr);
    ~MainMenu();

    Platform mPlatform;
    bool mFirstRun;
    QString mCfgDir;
    QFont mFont;
    QVBoxLayout* mMainLayout;
    QLineEdit* mSearchBox;
    QScrollArea* mScrollArea;
    QGridLayout* mGridLayout;
    QList<QPushButton*> mStacksButtonList;

    QWidget* mFirstRunWidget;
    bool mNoStacks;

private:
    ConfigMan* mCfgMan;
    QApplication* mApp;
    bool mDarkTheme;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent* event);
    void closeEvent(QCloseEvent* event);
};
#endif // MAINMENU_H
