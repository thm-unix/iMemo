#include "stackmenu.h"

#include "reader.h"
#include "editor.h"
#include "stackexporter.h"
#include "mainmenu.h"

void StackMenu::makeUI(QWidget* parent,
                       QApplication* app,
                       QString stackPath) {
    // UI
    QString iconPath = ":/icons/app-icon-generic.png";
    if (static_cast<MainMenu*>(parent)->mPlatform == Mac) {
        iconPath = ":/icons/app-icon-mac.png";
    }
    this->setWindowIcon(QIcon(iconPath));

    QStringList path = stackPath.split('/');
    QString name = path.last();
    name.chop(5);
    this->setWindowTitle(name);

    QFont font;
    font.setPointSize(11);
    this->setFont(font);

    this->setStyleSheet(parent->styleSheet());

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* upperLayout = new QHBoxLayout();

    QPushButton* backPushButton = new QPushButton("Back");
    backPushButton->setMinimumWidth(60);
    backPushButton->setMaximumWidth(60);
    backPushButton->setIcon(QIcon(":/icons/go-previous.png"));
    upperLayout->addWidget(backPushButton);

    QLabel* stackNameLabel = new QLabel(name);
    stackNameLabel->setObjectName("stackName");
    stackNameLabel->setAlignment(Qt::AlignHCenter);
    upperLayout->addWidget(stackNameLabel);

    mainLayout->addLayout(upperLayout);

    mainLayout->addSpacerItem(new QSpacerItem(0, 0,
                                              QSizePolicy::Ignored,
                                              QSizePolicy::MinimumExpanding));

    QPushButton* readerPushButton = new QPushButton("Reader");
    readerPushButton->setIcon(QIcon(":/icons/edit-find.png"));
    mainLayout->addWidget(readerPushButton);

    QPushButton* editorPushButton = new QPushButton("Editor");
    editorPushButton->setIcon(QIcon(":/icons/applications-office.png"));
    mainLayout->addWidget(editorPushButton);

    QPushButton* exportPushButton = new QPushButton("Export to PDF (A6 format)");
    exportPushButton->setIcon(QIcon(":/icons/document-print.png"));
    mainLayout->addWidget(exportPushButton);

    mainLayout->addSpacerItem(new QSpacerItem(0, 0,
                                              QSizePolicy::Ignored,
                                              QSizePolicy::MinimumExpanding));

    QPushButton* deletePushButton = new QPushButton("Delete");
    deletePushButton->setIcon(QIcon(":/icons/edit-delete.png"));
    mainLayout->addWidget(deletePushButton);


    // Make connections
    connect(backPushButton, &QPushButton::clicked, this, [this, parent]() {
        QString req = static_cast<MainMenu*>(mParent)->mSearchBox->text();
        static_cast<MainMenu*>(mParent)->buildList(req);
        this->hide();
        parent->setGeometry(this->geometry());
        parent->show();
        delete this;
    });

    connect(readerPushButton, &QPushButton::clicked, this, [this, app, stackPath]() {
        Reader* reader = new Reader(this, app, stackPath, mCfgMan, mCfgDir);
        reader->show();
        this->hide();
    });

    connect(editorPushButton, &QPushButton::clicked, this, [this, app, stackPath]() {
        Editor* editor = new Editor(this, app, stackPath, mCfgMan);
        editor->show();
        this->hide();
    });

    connect(exportPushButton, &QPushButton::clicked, this, [this, stackPath]() {
        new StackExporter(mCfgMan, stackPath);
    });

    connect(deletePushButton, &QPushButton::clicked, this, [this, name, parent, stackPath]() {
        QString warning = QString("Are you sure you want to delete %1?").arg(name);

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Stack deletion");
        msgBox.setText(warning);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setIcon(QMessageBox::Warning);

        if (msgBox.exec() == QMessageBox::Yes) {
            QMessageBox msgBox2(this);
            msgBox2.setWindowTitle("Stack deletion");
            msgBox2.setText("Last warning! This action cannot be undone.");
            msgBox2.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox2.setIcon(QMessageBox::Warning);
            if (msgBox2.exec() == QMessageBox::Yes) {
                this->hide();
                parent->setGeometry(this->geometry());
                parent->show();
                int index = findInArray(&mCfgMan->mStacksList, stackPath);
                if (index != -1) {
                    mCfgMan->mStacksList.removeAt(index);
                }
                try {
                    QFile::remove(stackPath);
                }
                catch (std::exception& ex) {
                    QString text = QString("Unable to delete %1. %2").arg(stackPath, ex.what());
                    QMessageBox::critical(this, "Error", text);
                }
                mCfgMan->saveConfig(static_cast<MainMenu*>(parent)->mCfgDir);
                QString req = static_cast<MainMenu*>(parent)->mSearchBox->text();
                static_cast<MainMenu*>(parent)->buildList(req);
            }
        }
    });
}

int StackMenu::findInArray(QJsonArray* array, QString elem) {
    for (int i = 0; i < array->count(); ++i) {
        if (!array->at(i).toString().compare(elem)) {
            return i;
        }
    }
    return -1;
}

void StackMenu::closeEvent(QCloseEvent* event) {
    QString req = static_cast<MainMenu*>(mParent)->mSearchBox->text();
    static_cast<MainMenu*>(mParent)->buildList(req);
    mParent->show();
    event->accept();
}

StackMenu::StackMenu(QWidget* parent,
                     QApplication* app,
                     QString stackPath,
                     ConfigMan* cfgMan,
                     QString cfgDir) : QWidget(nullptr) {
    mParent = parent;
    mCfgMan = cfgMan;
    mCfgDir = cfgDir;
    this->setObjectName("stackMenu");
    makeUI(parent, app, stackPath);
    this->setGeometry(parent->geometry());
}
