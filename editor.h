#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QCheckBox>
#include <QTextEdit>
#include <QApplication>
#include <QScreen>
#include <QColorDialog>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCloseEvent>
#include <QTimer>

#include "configman.h"

struct Flashcard {
    QWidget* widget;
    QCheckBox* checkBox;
    QLabel* numberLabel;
    QTextEdit* frontTextEdit;
    QTextEdit* backTextEdit;
};

class Editor : public QWidget {
public:
    Editor(QWidget* parent = nullptr,
           QApplication* app = nullptr,
           QString stackPath = "",
           ConfigMan* cfgMan = nullptr);
    QString getStackName(QString stackPath);
    void makeUI(QWidget* parent, QApplication* app, QString stackPath);
    void readStack(QString stackPath);
    void setTheme();
    static void bold(QTextEdit* textEdit);
    static void italic(QTextEdit* textEdit);
    static void underline(QTextEdit* textEdit);
    static void setColor(QTextEdit* textEdit, ConfigMan* cfgMan, QString cfgDir);
    static void clearStyle(QTextEdit* textEdit);
    void resetOrder();
    void saveStack(QString stackPath, bool unattended);
    void setupAutosave(QString stackPath);
    ~Editor();

private:
    ConfigMan* mCfgMan;
    QWidget* mParent;

    QString mStackPath;
    QJsonObject mStackObj;

    QHash<int, Flashcard> mFlashcardByNumber;
    int mCount = 0;
    int mMaxNumber = 0;

    QLabel* mLastSaveLabel;
    QLabel* mUnsavedIconLabel;
    int mLastSaveAgo = -1;
    bool mSaved = true;
    QTimer* mAutosaveTimer;
    QTimer* mUpdateLastSaveTimer;

protected:
    void closeEvent(QCloseEvent* event);

};

#endif // EDITOR_H
