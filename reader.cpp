#include "reader.h"
#include "gotoflashcarddialog.h"
#include "quickeditor.h"

QString Reader::getStackName(QString stackPath) {
    QStringList path = stackPath.split('/');
    QString name = path.last();
    name.chop(5);
    return name;
}

void Reader::makeUI(QWidget* parent, QApplication* app, QString stackPath) {
    // UI
    this->setWindowIcon(parent->windowIcon());

    QString stackName = getStackName(stackPath);
    this->setWindowTitle(stackName);

    QFont font;
    font.setPointSize(11);
    this->setFont(font);

    this->setStyleSheet(parent->styleSheet());
    qDebug() << parent->styleSheet();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* stackNameLabel = new QLabel(stackName);
    stackNameLabel->setObjectName("stackName");
    mainLayout->addWidget(stackNameLabel);

    QHBoxLayout* flashcardLayout = new QHBoxLayout();

    mBackButton = new QPushButton();
    mBackButton->setIcon(QIcon(":/icons/go-previous.png"));
    mBackButton->setIconSize(QSize(32, 32));
    mBackButton->setMinimumWidth(40);
    mBackButton->setMaximumWidth(40);
    mBackButton->setEnabled(false);
    flashcardLayout->addWidget(mBackButton);

    mFlashcardView = new QTextEdit();
    mFlashcardView->setObjectName("flashcard");
    mFlashcardView->setFontFamily(mCfgMan->mFontFamily);
    mFlashcardView->setReadOnly(true);
    mFlashcardView->setMinimumSize(437, 310);  // A6 minimum pixel size
    mFlashcardView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    flashcardLayout->addWidget(mFlashcardView);

    mForwardButton = new QPushButton();
    mForwardButton->setIcon(QIcon(":/icons/go-next.png"));
    mForwardButton->setIconSize(QSize(32, 32));
    mForwardButton->setMinimumWidth(40);
    mForwardButton->setMaximumWidth(40);
    mForwardButton->setEnabled(false);
    flashcardLayout->addWidget(mForwardButton);

    mainLayout->addLayout(flashcardLayout);

    QHBoxLayout* toolbarLayout = new QHBoxLayout();

    mTurnOverButton = new QPushButton("Turn over");
    toolbarLayout->addWidget(mTurnOverButton);

    mQuickEditButton = new QPushButton("Quick edit");
    toolbarLayout->addWidget(mQuickEditButton);

    toolbarLayout->addSpacerItem(new QSpacerItem(0, 0,
                                                 QSizePolicy::MinimumExpanding,
                                                 QSizePolicy::Ignored));

    mProgressBar = new QProgressBar();
    mProgressBar->setFormat("");
    toolbarLayout->addWidget(mProgressBar);

    mProgressButton = new QPushButton("0/0");
    toolbarLayout->addWidget(mProgressButton);

    mainLayout->addLayout(toolbarLayout);


    // Geometry
    int width = 800, height = 600;
    QScreen* screen = app->primaryScreen();
    QRect geometry = screen->geometry();
    int x = (geometry.width() - width) / 2;
    int y = (geometry.height() - height) / 2;
    this->setGeometry(x, y, width, height);


    // Make connections
    connect(mBackButton, &QPushButton::clicked, this, [this]() {
        previous();
    });
    connect(mForwardButton, &QPushButton::clicked, this, [this]() {
        next();
    });
    connect(mTurnOverButton, &QPushButton::clicked, this, [this]() {
        turnOver();
    });
    connect(mQuickEditButton, &QPushButton::clicked, this, [this, stackPath]() {
        QuickEditor* quickEditor = new QuickEditor(this,
                                                   mCfgMan,
                                                   stackPath,
                                                   &mStackObj,
                                                   mCurrentIndex);
        quickEditor->show();
    });
    connect(mProgressButton, &QPushButton::clicked, this, [this]() {
        auto goToFlashcardDialog = new GoToFlashcardDialog(this, mStackObj["count"].toInt());
        goToFlashcardDialog->show();
    });
}

void Reader::readStack(QString stackPath) {
    QFile stackFile(stackPath);
    stackFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = stackFile.readAll();
    stackFile.close();
    mStackObj = QJsonDocument::fromJson(data.toUtf8()).object();
}

void Reader::loadFlashcard(int i, bool turnOver) {
    QJsonObject flashcard = mStackObj[QString::number(i)].toObject();

    QString text;
    if (!turnOver) {
        text = flashcard["f"].toString();
    }
    else {
        text = flashcard["b"].toString();
    }

    text = text.replace("font-size:13pt;", "");
    mText = QTextDocumentFragment::fromHtml(text).toPlainText();
    //mText = mText.replace("\n", "<br>");

    int ptSize = getMaximumFontSize(mText, mFlashcardView);
    mFlashcardView->setText(text);
    mFlashcardView->setFont(QFont(mCfgMan->mFontFamily, ptSize));
    int margin = (mFlashcardView->height() - getTextHeight(mText, ptSize)) / 2;

    QTextFrame* frame = mFlashcardView->document()->rootFrame();
    QTextFrameFormat format = frame->frameFormat();

    format.setTopMargin(margin);
    format.setBottomMargin(margin);
    frame->setFrameFormat(format);

    mCurrentIndex = i;
    setProgress();

    if (i == 1) {
        mBackButton->setEnabled(false);
    }
    else {
        mBackButton->setEnabled(true);
    }
    if (i == mStackObj["count"].toInt()) {
        mForwardButton->setEnabled(false);
    }
    else {
        mForwardButton->setEnabled(true);
    }
}

int Reader::getMaximumFontSize(QString text, QWidget* flashcardView) {
    //text = text.replace("<br>", "\n");
    //text = QTextDocumentFragment::fromHtml(text).toPlainText();
    //text = text.replace("\n", "<br>");
    //qDebug() << text;

    int fontSize = 36;
    QFont font;
    font.setFamily(mCfgMan->mFontFamily);
    font.setPointSize(48);
    QFontMetrics fontMetrics(font);

    // Find the longest line
    QString maxLine;
    foreach (QString line, text.split("\n")) {
        if (fontMetrics.horizontalAdvance(line) >
            fontMetrics.horizontalAdvance(maxLine)) {
            maxLine = line;
        }
    }

    // Getting maximum possible font size for current size of QTextEdit
    while ((fontMetrics.horizontalAdvance(maxLine) >= mFlashcardView->width() - 50) ||
           (getTextHeight(text, fontSize) >= mFlashcardView->height() - 50)) {
        --fontSize;
        font.setPointSize(fontSize);
        fontMetrics = QFontMetrics(font);
    }

    return fontSize;
}

int Reader::getTextHeight(QString text, int ptSize) {
    QFontMetrics fontMetrics = QFontMetrics(QFont(mCfgMan->mFontFamily, ptSize));
    int lineHeight = fontMetrics.height();
    int countLines = text.count("\n") + 1;
    return (lineHeight * countLines);
}

void Reader::setProgress() {
    int percentage = mCurrentIndex * 100 / mStackObj["count"].toInt();
    QString progress = QString("%1/%2").arg(QString::number(mCurrentIndex),
                                            QString::number(mStackObj["count"].toInt()));
    mProgressBar->setValue(percentage);
    mProgressButton->setText(progress);
}

void Reader::previous() {
    loadFlashcard(mCurrentIndex - 1, false);
    mTurnedOver = false;
}

void Reader::next() {
    loadFlashcard(mCurrentIndex + 1, false);
    mTurnedOver = false;
}

void Reader::turnOver() {
    loadFlashcard(mCurrentIndex, !mTurnedOver);
    mTurnedOver = !mTurnedOver;
}

void Reader::resizeEvent(QResizeEvent* event) {
    int ptSize = getMaximumFontSize(mText, mFlashcardView);
    mFlashcardView->setFont(QFont(mCfgMan->mFontFamily, ptSize));

    int margin = (mFlashcardView->height() - getTextHeight(mText, ptSize)) / 2;

    QTextFrame* frame = mFlashcardView->document()->rootFrame();
    QTextFrameFormat format = frame->frameFormat();

    format.setTopMargin(margin);
    format.setBottomMargin(margin);
    frame->setFrameFormat(format);

    event->accept();
}

Reader::Reader(QWidget* parent,
               QApplication* app,
               QString stackPath,
               ConfigMan* cfgMan,
               QString cfgDir) : QWidget(nullptr) {
    this->setObjectName("reader");
    mParent = parent;
    mCfgMan = cfgMan;
    mCfgDir = cfgDir;
    makeUI(parent, app, stackPath);
    readStack(stackPath);
    if (mStackObj["count"].toInt() != 0) {
        mProgressButton->setEnabled(true);
        mTurnOverButton->setEnabled(true);
        mQuickEditButton->setEnabled(true);
        loadFlashcard(1, false);
    }
    else {
        mFlashcardView->setHtml("<p style=\"text-align: center\"><b>Empty.</b></p>");
        mProgressButton->setEnabled(false);
        mTurnOverButton->setEnabled(false);
        mQuickEditButton->setEnabled(false);
    }
}

void Reader::closeEvent(QCloseEvent* event) {
    mParent->show();
    event->accept();
}

Reader::~Reader() {

}
