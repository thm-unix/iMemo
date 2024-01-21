#include "quickeditor.h"
#include "reader.h"
#include "editor.h"

void QuickEditor::makeUI(QWidget* parent,
                         ConfigMan* cfgMan,
                         QString stackPath,
                         QJsonObject* stackObj,
                         int i) {
    this->setWindowTitle("Go to flashcard");
    this->setWindowIcon(parent->windowIcon());

    this->setStyleSheet(parent->styleSheet());

    // UI
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

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

    mainLayout->addLayout(toolbarLayout);

    QHBoxLayout* flashcardLayout = new QHBoxLayout();

    QTextEdit* frontTextEdit = new QTextEdit();
    QFont font(cfgMan->mFontFamily, 13);
    frontTextEdit->setFont(font);
    flashcardLayout->addWidget(frontTextEdit);

    QTextEdit* backTextEdit = new QTextEdit();
    backTextEdit->setFont(font);
    flashcardLayout->addWidget(backTextEdit);

    mainLayout->addLayout(flashcardLayout);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();

    buttonsLayout->addSpacerItem(new QSpacerItem(0, 0,
                                                 QSizePolicy::MinimumExpanding,
                                                 QSizePolicy::Ignored));

    QPushButton* cancelButton = new QPushButton("Cancel");
    buttonsLayout->addWidget(cancelButton);

    QPushButton* saveButton = new QPushButton("Save");
    buttonsLayout->addWidget(saveButton);

    mainLayout->addLayout(buttonsLayout);

    // Set contents
    QString front = stackObj->operator[](QString::number(i)).toObject()["f"].toString();
    frontTextEdit->setHtml(front);

    QString back = stackObj->operator[](QString::number(i)).toObject()["b"].toString();
    backTextEdit->setHtml(back);

    // Making connections
    connect(boldButton, &QPushButton::clicked, this, [frontTextEdit, backTextEdit]() {
        Editor::bold(frontTextEdit);
        Editor::bold(backTextEdit);
    });

    connect(italicButton, &QPushButton::clicked, this, [frontTextEdit, backTextEdit]() {
        Editor::italic(frontTextEdit);
        Editor::italic(backTextEdit);
    });

    connect(underlineButton, &QPushButton::clicked, this, [frontTextEdit, backTextEdit]() {
        Editor::underline(frontTextEdit);
        Editor::underline(backTextEdit);
    });

    connect(setTextColorButton, &QPushButton::clicked, this, [frontTextEdit,
                                                              backTextEdit,
                                                              parent]() {
        Editor::setColor(frontTextEdit,
                         static_cast<Reader*>(parent)->mCfgMan,
                         static_cast<Reader*>(parent)->mCfgDir);
        Editor::setColor(backTextEdit,
                         static_cast<Reader*>(parent)->mCfgMan,
                         static_cast<Reader*>(parent)->mCfgDir);
    });

    connect(clearStyleButton, &QPushButton::clicked, this, [frontTextEdit, backTextEdit]() {
        Editor::clearStyle(frontTextEdit);
        Editor::clearStyle(backTextEdit);
    });

    connect(frontTextEdit, &QTextEdit::textChanged, this, [this]() {
        mChanged = true;
    });

    connect(backTextEdit, &QTextEdit::textChanged, this, [this]() {
        mChanged = true;
    });

    connect(cancelButton, &QPushButton::clicked, this, [this]() {
        if (mChanged) {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("There are unsaved changes");
            msgBox.setText("Do you want to revert changes you made to this flashcard?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            msgBox.setIcon(QMessageBox::Question);

            if (msgBox.exec() == QMessageBox::Yes) {
                this->hide();
                delete this;
            }
        }
        else {
            this->hide();
            delete this;
        }
    });

    connect(saveButton, &QPushButton::clicked, this, [this, stackObj, i,
                                                      frontTextEdit, backTextEdit,
                                                      stackPath, parent]() {
        QJsonObject flashcardObj;

        QString frontHtml = frontTextEdit->toHtml();
        flashcardObj["f"] = QJsonValue(frontHtml);

        QString backHtml = backTextEdit->toHtml();
        flashcardObj["b"] = QJsonValue(backHtml);

        stackObj->operator[](QString::number(i)) = QJsonValue(flashcardObj);

        QJsonDocument doc(*stackObj);
        QFile file(stackPath);
        file.open(QFile::WriteOnly);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();

        bool isTurnedOver = static_cast<Reader*>(parent)->mTurnedOver;
        static_cast<Reader*>(parent)->loadFlashcard(i, isTurnedOver);

        this->hide();
        delete this;
    });
}

void QuickEditor::closeEvent(QCloseEvent* event) {
    if (mChanged) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("There are unsaved changes");
        msgBox.setText("Do you want to revert changes you made to this flashcard?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Question);

        if (msgBox.exec() == QMessageBox::Yes) {
            event->accept();
        }
        else {
            event->ignore();
        }
    }
    else {
        event->accept();
    }
}

void QuickEditor::setWindowGeometry() {
    int width = 800, height = 330;
    QScreen* screen = qApp->primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width) / 2;
    int y = (screenGeometry.height() - height) / 2;
    this->setGeometry(x, y, width, height);
    this->setFixedSize(width, height);
}

QuickEditor::QuickEditor(QWidget* parent,
                         ConfigMan* cfgMan,
                         QString stackPath,
                         QJsonObject* stackObj,
                         int i) {
    this->setObjectName("quickEditor");
    makeUI(parent, cfgMan, stackPath, stackObj, i);
    setWindowGeometry();
}
