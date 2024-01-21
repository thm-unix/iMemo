#ifndef READER_H
#define READER_H

#include <QWidget>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QScreen>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFont>
#include <QFontMetrics>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QTextDocumentFragment>

#include "configman.h"

class Reader : public QWidget {
    Q_OBJECT
public:
    Reader(QWidget* parent = nullptr,
           QApplication* app = nullptr,
           QString stackPath = "",
           ConfigMan* cfgMan = nullptr,
           QString cfgDir = "");
    void makeUI(QWidget* parent, QApplication* app, QString stackPath);
    QString getStackName(QString stackPath);
    void readStack(QString stackPath);
    void loadFlashcard(int i, bool turnOver );
    int getMaximumFontSize(QString text, QWidget* flashcardView);
    int getTextHeight(QString text, int ptSize);
    void setProgress();
    void previous();
    void next();
    void turnOver();
    ~Reader();

    bool mTurnedOver = false;

    ConfigMan* mCfgMan;
    QString mCfgDir;

private:
    QWidget* mParent;

    QJsonObject mStackObj;

    int mCurrentIndex = 1;

    QTextEdit* mFlashcardView;
    QProgressBar* mProgressBar;
    QPushButton* mProgressButton;

    QPushButton* mBackButton;
    QPushButton* mForwardButton;
    QPushButton* mTurnOverButton;
    QPushButton* mQuickEditButton;

    QString mText;

protected:
    void resizeEvent(QResizeEvent* event);
    void closeEvent(QCloseEvent* event);
};

#endif // READER_H
