#include "editor.h"
#include "stackmenu.h"

QString Editor::getStackName(QString stackPath) {
    QStringList path = stackPath.split('/');
    QString name = path.last();
    name.chop(5);
    return name;
}

void Editor::readStack(QString stackPath) {
    QFile stackFile(stackPath);
    stackFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = stackFile.readAll();
    stackFile.close();
    mStackObj = QJsonDocument::fromJson(data.toUtf8()).object();
    mMaxNumber = mStackObj["count"].toInt();
}

void Editor::makeUI(QWidget* parent, QApplication* app, QString stackPath) {
    // UI
    this->setWindowIcon(parent->windowIcon());

    QString stackName = getStackName(stackPath);
    this->setWindowTitle(stackName);

    this->setStyleSheet(parent->styleSheet());

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* stackLayout = new QHBoxLayout();

    QLabel* stackNameLabel = new QLabel(stackName);
    stackNameLabel->setObjectName("stackName");
    stackLayout->addWidget(stackNameLabel);

    stackLayout->addSpacerItem(new QSpacerItem(0, 0,
                                               QSizePolicy::MinimumExpanding,
                                               QSizePolicy::Ignored));

    mUnsavedIconLabel = new QLabel();
    mUnsavedIconLabel->setPixmap(QIcon(":/icons/media-record.png").pixmap(24, 24));
    mUnsavedIconLabel->setVisible(false);
    stackLayout->addWidget(mUnsavedIconLabel);

    mLastSaveLabel = new QLabel("Last save: Never");
    stackLayout->addWidget(mLastSaveLabel);

    QPushButton* saveButton = new QPushButton("Save");
    saveButton->setIcon(QIcon(":/icons/document-save.png"));
    stackLayout->addWidget(saveButton);

    mainLayout->addLayout(stackLayout);

    QHBoxLayout* toolbarLayout = new QHBoxLayout();

    QPushButton* boldButton = new QPushButton("B");
    boldButton->setObjectName("boldButton");
    boldButton->setFixedWidth(24);
    toolbarLayout->addWidget(boldButton);

    QPushButton* italicButton = new QPushButton("I");
    italicButton->setObjectName("italicButton");
    italicButton->setFixedWidth(24);
    toolbarLayout->addWidget(italicButton);

    QPushButton* underlineButton = new QPushButton("U");
    underlineButton->setObjectName("underlineButton");
    underlineButton->setFixedWidth(24);
    toolbarLayout->addWidget(underlineButton);

    QPushButton* setTextColorButton = new QPushButton();
    setTextColorButton->setIcon(QIcon(":/icons/color-wheel.png"));
    toolbarLayout->addWidget(setTextColorButton);

    QPushButton* clearStyleButton = new QPushButton();
    clearStyleButton->setIcon(QIcon(":/icons/edit-clear.png"));
    toolbarLayout->addWidget(clearStyleButton);

    toolbarLayout->addSpacerItem(new QSpacerItem(0, 0,
                                                 QSizePolicy::MinimumExpanding,
                                                 QSizePolicy::Ignored));

    QPushButton* addButton = new QPushButton("Add");
    addButton->setIcon(QIcon(":/icons/list-add.png"));
    toolbarLayout->addWidget(addButton);

    QPushButton* deleteButton = new QPushButton("Delete");
    deleteButton->setIcon(QIcon(":/icons/list-remove.png"));
    toolbarLayout->addWidget(deleteButton);

    mainLayout->addLayout(toolbarLayout);

    QScrollArea* scrollArea = new QScrollArea();

    QWidget* flashcardsWidget = new QWidget();
    flashcardsWidget->setObjectName("flashcardsWidget");
    flashcardsWidget->setStyleSheet(this->styleSheet());
    QVBoxLayout* flashcardsLayout = new QVBoxLayout(flashcardsWidget);

    for (int i = 1; i <= mStackObj["count"].toInt(); ++i) {
        QWidget* flashcardWidget = new QWidget();
        flashcardWidget->setObjectName("flashcardWidget");
        flashcardWidget->setStyleSheet(this->styleSheet());
        QHBoxLayout* flashcardLayout = new QHBoxLayout(flashcardWidget);

        QCheckBox* checkBox = new QCheckBox();
        flashcardLayout->addWidget(checkBox);

        QLabel* numberLabel = new QLabel(QString::number(i));
        flashcardLayout->addWidget(numberLabel);

        QJsonObject flashcardObj = mStackObj[QString::number(i)].toObject();

        QTextEdit* frontTextEdit = new QTextEdit();
        QFont font(mCfgMan->mFontFamily, 13);
        frontTextEdit->setFont(font);
        frontTextEdit->setMinimumHeight(300);
        frontTextEdit->setMaximumHeight(300);
        QString front = flashcardObj["f"].toString();
        frontTextEdit->setHtml(front);
        flashcardLayout->addWidget(frontTextEdit);

        QTextEdit* backTextEdit = new QTextEdit();
        backTextEdit->setFont(font);
        backTextEdit->setMinimumHeight(300);
        backTextEdit->setMaximumHeight(300);
        QString back = flashcardObj["b"].toString();
        backTextEdit->setHtml(back);
        flashcardLayout->addWidget(backTextEdit);

        Flashcard flashcard = {flashcardWidget, checkBox, numberLabel, frontTextEdit, backTextEdit};
        mFlashcardByNumber[i] = flashcard;
        ++mCount;

        flashcardsLayout->addWidget(flashcardWidget);

        connect(frontTextEdit, &QTextEdit::textChanged, this, [this]() {
            mSaved = false;
            mUnsavedIconLabel->setVisible(true);
        });
        connect(backTextEdit, &QTextEdit::textChanged, this, [this]() {
            mSaved = false;
            mUnsavedIconLabel->setVisible(true);
        });
    }

    scrollArea->setWidget(flashcardsWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);


    // Geometry
    int width = 800, height = 600;
    QScreen* screen = app->primaryScreen();
    QRect geometry = screen->geometry();
    int x = (geometry.width() - width) / 2;
    int y = (geometry.height() - height) / 2;
    this->setGeometry(x, y, width, height);


    // Making connections
    connect(boldButton, &QPushButton::clicked, this, [this]() {
        foreach (int i, mFlashcardByNumber.keys()) {
            QTextEdit* textEditA = mFlashcardByNumber[i].frontTextEdit;
            bold(textEditA);
            QTextEdit* textEditB = mFlashcardByNumber[i].backTextEdit;
            bold(textEditB);
        }
    });

    connect(italicButton, &QPushButton::clicked, this, [this]() {
        foreach (int i, mFlashcardByNumber.keys()) {
            QTextEdit* textEditA = mFlashcardByNumber[i].frontTextEdit;
            italic(textEditA);
            QTextEdit* textEditB = mFlashcardByNumber[i].backTextEdit;
            italic(textEditB);
        }
    });

    connect(underlineButton, &QPushButton::clicked, this, [this]() {
        foreach (int i, mFlashcardByNumber.keys()) {
            QTextEdit* textEditA = mFlashcardByNumber[i].frontTextEdit;
            underline(textEditA);
            QTextEdit* textEditB = mFlashcardByNumber[i].backTextEdit;
            underline(textEditB);
        }
    });

    connect(setTextColorButton, &QPushButton::clicked, this, [this, parent]() {
        foreach (int i, mFlashcardByNumber.keys()) {
            QTextEdit* textEditA = mFlashcardByNumber[i].frontTextEdit;
            setColor(textEditA,
                     static_cast<StackMenu*>(parent)->mCfgMan,
                     static_cast<StackMenu*>(parent)->mCfgDir);

            QTextEdit* textEditB = mFlashcardByNumber[i].backTextEdit;
            setColor(textEditB,
                     static_cast<StackMenu*>(parent)->mCfgMan,
                     static_cast<StackMenu*>(parent)->mCfgDir);
        }
    });

    connect(clearStyleButton, &QPushButton::clicked, this, [this]() {
        foreach (int i, mFlashcardByNumber.keys()) {
            QTextEdit* textEditA = mFlashcardByNumber[i].frontTextEdit;
            clearStyle(textEditA);
            QTextEdit* textEditB = mFlashcardByNumber[i].backTextEdit;
            clearStyle(textEditB);
        }
    });

    connect(addButton, &QPushButton::clicked, this, [this, flashcardsLayout]() {
        QWidget* flashcardWidget = new QWidget();
        flashcardWidget->setObjectName("flashcardWidget");
        flashcardWidget->setStyleSheet(this->styleSheet());
        QHBoxLayout* flashcardLayout = new QHBoxLayout(flashcardWidget);

        QCheckBox* checkBox = new QCheckBox();
        flashcardLayout->addWidget(checkBox);

        QLabel* numberLabel = new QLabel(QString::number(++mCount));
        flashcardLayout->addWidget(numberLabel);

        QTextEdit* frontTextEdit = new QTextEdit();
        QFont font(mCfgMan->mFontFamily, 13);
        frontTextEdit->setFont(font);
        frontTextEdit->setMinimumHeight(300);
        frontTextEdit->setMaximumHeight(300);
        flashcardLayout->addWidget(frontTextEdit);

        QTextEdit* backTextEdit = new QTextEdit();
        backTextEdit->setFont(font);
        backTextEdit->setMinimumHeight(300);
        backTextEdit->setMaximumHeight(300);
        flashcardLayout->addWidget(backTextEdit);

        Flashcard flashcard = {flashcardWidget, checkBox, numberLabel, frontTextEdit, backTextEdit};
        mFlashcardByNumber[mCount] = flashcard;
        if (mCount > mMaxNumber) {
            mMaxNumber = mCount;
        }
        flashcardsLayout->addWidget(flashcardWidget);

        mSaved = false;
        mUnsavedIconLabel->setVisible(true);
        connect(frontTextEdit, &QTextEdit::textChanged, this, [this]() {
            mSaved = false;
            mUnsavedIconLabel->setVisible(true);
        });
        connect(backTextEdit, &QTextEdit::textChanged, this, [this]() {
            mSaved = false;
            mUnsavedIconLabel->setVisible(true);
        });
    });

    connect(deleteButton, &QPushButton::clicked, this, [this]() {
        QList<int> forDeletion;
        foreach (int i, mFlashcardByNumber.keys()) {
            if (mFlashcardByNumber[i].checkBox->isChecked()) {
                forDeletion.append(i);
            }
        }

        if (forDeletion.length() > 0) {
            QString warning = QString("Are you sure you want to delete %1 flashcards: ").arg(forDeletion.length());
            foreach (int i, forDeletion) {
                warning += "#" + QString::number(i) + " ";
            }
            warning += "?";

            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Flashcards deletion");
            msgBox.setText(warning);
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setIcon(QMessageBox::Warning);

            if (msgBox.exec() == QMessageBox::Yes) {
                foreach (int i, forDeletion) {
                    delete mFlashcardByNumber[i].checkBox;
                    delete mFlashcardByNumber[i].numberLabel;
                    delete mFlashcardByNumber[i].backTextEdit;
                    delete mFlashcardByNumber[i].frontTextEdit;
                    delete mFlashcardByNumber[i].widget;
                    mFlashcardByNumber.remove(i);
                    --mCount;
                }
            }
            resetOrder();
            mSaved = false;
            mUnsavedIconLabel->setVisible(true);
        }
        else {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Flashcards deletion");
            msgBox.setText("You did not select flashcards for deletion.");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
    });

    connect(saveButton, &QPushButton::clicked, this, [this, stackPath]() {
        saveStack(stackPath, false);
    });
}

void Editor::resetOrder() {
    QHash<int, Flashcard> flashcardByNumber;
    int j = 1;
    for (int i = 1; i <= mMaxNumber; ++i) {
        if (mFlashcardByNumber.contains(i)) {
            mFlashcardByNumber[i].numberLabel->setText(QString::number(j));
            flashcardByNumber[j] = mFlashcardByNumber[i];
            ++j;
        }
    }
    mFlashcardByNumber.clear();
    mFlashcardByNumber = flashcardByNumber;
}

void Editor::bold(QTextEdit* textEdit) {
    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    textEdit->textCursor().mergeCharFormat(format);
}

void Editor::italic(QTextEdit* textEdit) {
    QTextCharFormat format;
    format.setFontItalic(true);
    textEdit->textCursor().mergeCharFormat(format);
}

void Editor::underline(QTextEdit* textEdit) {
    QTextCharFormat format;
    format.setFontUnderline(true);
    textEdit->textCursor().mergeCharFormat(format);
}

void Editor::setColor(QTextEdit* textEdit, ConfigMan* cfgMan, QString cfgDir) {
    if (textEdit->textCursor().hasSelection()) {
        QColorDialog* colorDialog = new QColorDialog();

        for (int i = 0; i < cfgMan->mCustomColors.count(); ++i) {
            QString customColor = cfgMan->mCustomColors.at(i).toString();
            QStringList colorList = customColor.split(',');
            int red = colorList.at(0).toInt();
            int green = colorList.at(1).toInt();
            int blue = colorList.at(2).toInt();
            QColor color(red, green, blue);
            colorDialog->setCustomColor(i, color);
        }

        if (colorDialog->exec() == QColorDialog::Accepted) {
            QTextCharFormat format;
            format.setForeground(colorDialog->currentColor());
            textEdit->textCursor().mergeCharFormat(format);
        }

        QJsonArray customColors;
        for (int i = 0; i < colorDialog->customCount(); ++i) {
            int red = colorDialog->customColor(i).red();
            int green = colorDialog->customColor(i).green();
            int blue = colorDialog->customColor(i).blue();
            QString color = QString("%1,%2,%3").arg(QString::number(red),
                                                    QString::number(green),
                                                    QString::number(blue));
            customColors.append(QJsonValue(color));
        }
        cfgMan->mCustomColors = customColors;
        cfgMan->saveConfig(cfgDir);
    }
}

void Editor::clearStyle(QTextEdit* textEdit) {
    if (textEdit->textCursor().hasSelection()) {
        textEdit->textCursor().setCharFormat(QTextCharFormat());
    }
}

void Editor::saveStack(QString stackPath, bool unattended) {
    QJsonObject stackObj;
    for (int i = 1; i <= mCount; ++i) {
        QJsonObject flashcardObj;

        QString frontHtml = mFlashcardByNumber[i].frontTextEdit->toHtml();
        flashcardObj["f"] = QJsonValue(frontHtml);

        QString backHtml = mFlashcardByNumber[i].backTextEdit->toHtml();
        flashcardObj["b"] = QJsonValue(backHtml);

        stackObj[QString::number(i)] = QJsonValue(flashcardObj);

        qDebug() << mFlashcardByNumber[i].frontTextEdit->toPlainText();

    }
    stackObj["count"] = QJsonValue(mCount);

    QJsonDocument doc(stackObj);
    QFile file(stackPath);
    file.open(QFile::WriteOnly);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    mLastSaveAgo = 0;
    mLastSaveLabel->setText("Last save: Now");
    mSaved = true;
    mUnsavedIconLabel->setVisible(false);
}

void Editor::setupAutosave(QString stackPath) {
    mAutosaveTimer = new QTimer(this);
    mAutosaveTimer->setInterval(mCfgMan->mAutosaveIntervalMin * 60 * 1000);
    connect(mAutosaveTimer, &QTimer::timeout, this, [this, stackPath]() {
        saveStack(stackPath, true);
    });
    mAutosaveTimer->start();

    mUpdateLastSaveTimer = new QTimer(this);
    mUpdateLastSaveTimer->setInterval(30000);
    connect(mUpdateLastSaveTimer, &QTimer::timeout, this, [this]() {
        if (mLastSaveAgo != -1) {
            ++mLastSaveAgo;
            QString text = QString("Last save: %1 min ago").arg(mLastSaveAgo);
            mLastSaveLabel->setText(text);
        }
    });
    mUpdateLastSaveTimer->start();
}


Editor::Editor(QWidget* parent,
               QApplication* app,
               QString stackPath,
               ConfigMan* cfgMan) : QWidget(nullptr) {
    mParent = parent;
    mCfgMan = cfgMan;
    mStackPath = stackPath;
    this->setObjectName("editor");
    readStack(stackPath);
    makeUI(parent, app, stackPath);
    setupAutosave(stackPath);
}

void Editor::closeEvent(QCloseEvent* event) {
    if (mSaved) {
        mParent->show();
        mAutosaveTimer->stop();
        mUpdateLastSaveTimer->stop();
        event->accept();
    }
    else {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Stack is not saved!");
        msgBox.setText("Do you want to save last changes?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Question);

        int result = msgBox.exec();

        if (result == QMessageBox::Yes) {
            saveStack(mStackPath, true);
            mAutosaveTimer->stop();
            mUpdateLastSaveTimer->stop();
            mParent->show();
            event->accept();
        }
        else if (result == QMessageBox::Cancel) {
            event->ignore();
        }
        else {  // QMessageBox::No
            qDebug() << msgBox.result();
            mAutosaveTimer->stop();
            mUpdateLastSaveTimer->stop();
            mParent->show();
            event->accept();
        }
    }
}

Editor::~Editor() {

}
