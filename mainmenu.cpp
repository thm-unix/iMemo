#include "mainmenu.h"

#include "stackmenu.h"
#include "aboutwindow.h"
#include "settings.h"

bool MainMenu::isFirstRun() {
#ifdef Q_OS_LINUX
    qDebug() << "Running on Linux";
    mCfgDir = QString("%1/.config/iMemo").arg(getenv("HOME"));
    mPlatform = Platform::Linux;
#elif defined Q_OS_MACOS
    qDebug() << "Running on macOS";
    mCfgDir = QString("%1/Library/Preferences/iMemo").arg(getenv("HOME"));
    mPlatform = Platform::Mac;
#elif defined Q_OS_WIN
    qDebug() << "Running on Windows";
    mCfgDir = QString("%1/iMemo").arg(getenv("APPDATA")));
    mPlatform = Platform::Windows;
#else
    qDebug() << "Running on UNKNOWN. Please report this!";
    mCfgDir = QDir::currentPath();
    mPlatform = Platform::Unknown;
#endif
    return !QFile(mCfgDir + "/config.json").exists();
}

void MainMenu::makeUI(bool remake) {
    // Window & geometry
    if (!remake) {
        QScreen* screen = mApp->primaryScreen();
        int width = 450, height = 600;
        QRect geometry = screen->geometry();
        int x = (geometry.width() - width) / 2;
        int y = (geometry.height() - height) / 2;
        this->setGeometry(x, y, width, height);

        mMainLayout = new QVBoxLayout();
        this->setLayout(mMainLayout);
        mMainLayout->setContentsMargins(5, 5, 5, 5);
        mMainLayout->setSpacing(10);
        this->setWindowTitle("iMemo");
        QString iconPath = ":/icons/app-icon-generic.png";
        if (mPlatform == Mac) {
            iconPath = ":/icons/app-icon-mac.png";
        }
        this->setWindowIcon(QIcon(iconPath));
        this->setObjectName("mainMenu");
        this->setAcceptDrops(true);
    }

    // UI
    if (mCfgMan->mStacksList.empty()) {
        mNoStacks = true;

        mFirstRunWidget = new QWidget();
        QVBoxLayout* firstRunLayout = new QVBoxLayout(mFirstRunWidget);
        firstRunLayout->setContentsMargins(0, 0, 0, 0);
        firstRunLayout->setSpacing(10);

        QSpacerItem* spacerItem1 = new QSpacerItem(
                                   0, 0,
                                   QSizePolicy::Ignored,
                                   QSizePolicy::MinimumExpanding);
        firstRunLayout->addSpacerItem(spacerItem1);

        QHBoxLayout* buttonLayout = new QHBoxLayout();

        buttonLayout->addSpacerItem(new QSpacerItem(0, 0,
                                                    QSizePolicy::MinimumExpanding,
                                                    QSizePolicy::Ignored));

        QToolButton* rectButton = new QToolButton();
        QString iconPath = ":/etc/add-light.png";
        if (mDarkTheme) {
            iconPath = ":/etc/add-dark.png";
        }
        rectButton->setIcon(QIcon(iconPath));
        rectButton->setFixedSize(256, 256);
        rectButton->setIconSize(QSize(200, 200));
        rectButton->setAutoRaise(true);
        buttonLayout->addWidget(rectButton);

        buttonLayout->addSpacerItem(new QSpacerItem(0, 0,
                                                    QSizePolicy::MinimumExpanding,
                                                    QSizePolicy::Ignored));

        firstRunLayout->addLayout(buttonLayout);

        QLabel* hintLabel = new QLabel("\nStart with a new stack or\n"
                                       "drop an existing one");
        hintLabel->setObjectName("hintLabel");
        hintLabel->setAlignment(Qt::AlignCenter);
        firstRunLayout->addWidget(hintLabel);

        QSpacerItem* spacerItem2 = new QSpacerItem(
                                   0, 0,
                                   QSizePolicy::Ignored,
                                   QSizePolicy::MinimumExpanding);
        firstRunLayout->addSpacerItem(spacerItem2);

        mMainLayout->addWidget(mFirstRunWidget);

        connect(rectButton, &QPushButton::clicked, this, [this]() {
            newStack();
        });
    }
    else {
        mNoStacks = false;
        QHBoxLayout* actionsLayout = new QHBoxLayout();
        actionsLayout->setContentsMargins(4, 4, 4, 4);

        QLabel* appTitle = new QLabel("iMemo");
        appTitle->setObjectName("appName");
        actionsLayout->addWidget(appTitle);

        actionsLayout->addSpacerItem(new QSpacerItem(
                                     0, 0,
                                     QSizePolicy::MinimumExpanding,
                                     QSizePolicy::Ignored));

        QPushButton* newStackButton = new QPushButton();
        newStackButton->setIcon(QIcon(":/icons/list-add.png"));
        newStackButton->setIconSize(QSize(20, 20));
        newStackButton->setToolTip("Create new stack");
        newStackButton->setMinimumWidth(50);
        actionsLayout->addWidget(newStackButton);

        QPushButton* importStackButton = new QPushButton();
        importStackButton->setIcon(QIcon(":/icons/folder.png"));
        importStackButton->setIconSize(QSize(20, 20));
        importStackButton->setToolTip("Import an existing stack(s) from file(s)");
        importStackButton->setMinimumWidth(50);
        actionsLayout->addWidget(importStackButton);

        QPushButton* settingsButton = new QPushButton();
        settingsButton->setIcon(QIcon(":/icons/emblem-system.png"));
        settingsButton->setIconSize(QSize(20, 20));
        settingsButton->setToolTip("Settings");
        settingsButton->setMinimumWidth(50);
        actionsLayout->addWidget(settingsButton);

        QPushButton* aboutButton = new QPushButton();
        aboutButton->setIcon(QIcon(":/icons/help-browser.png"));
        aboutButton->setIconSize(QSize(20, 20));
        aboutButton->setToolTip("About software");
        aboutButton->setMinimumWidth(50);
        actionsLayout->addWidget(aboutButton);
        mMainLayout->addLayout(actionsLayout);

        mSearchBox = new QLineEdit();
        mSearchBox->setPlaceholderText("🔎 Search");  // U+01F50E - magnifier icon
        mMainLayout->addWidget(mSearchBox);

        QWidget* stacksWidget = new QWidget();
        stacksWidget->setObjectName("stacksWidget");
        mGridLayout = new QGridLayout();
        mGridLayout->setContentsMargins(10, 4, 10, 4);
        stacksWidget->setLayout(mGridLayout);

        mScrollArea = new QScrollArea();
        mScrollArea->setWidgetResizable(true);

        buildList("");
        mScrollArea->setWidget(stacksWidget);
        mMainLayout->addWidget(mScrollArea);

        // Make connections
        connect(newStackButton, &QPushButton::clicked, this, [this]() {
            newStack();
        });

        connect(importStackButton, &QPushButton::clicked, this, [this]() {
            QFileDialog dialog;
            dialog.setWindowTitle("Select stack(s)");
            dialog.setDirectory(QDir::homePath());
            dialog.setNameFilter("Stacks (*.json)");

            if (dialog.exec() == QFileDialog::Accepted) {
                QStringList files = dialog.selectedFiles();
                import(files);
            }
        });

        connect(mSearchBox, &QLineEdit::textChanged, this, [this]() {
            buildList(mSearchBox->text());
        });

        connect(settingsButton, &QPushButton::clicked, this, [this]() {
            Settings* settings = new Settings(this, mCfgMan);
            this->hide();
            settings->show();
        });

        connect(aboutButton, &QPushButton::clicked, this, [this]() {
            AboutWindow* aboutWindow = new AboutWindow(this, mPlatform);
            this->hide();
            aboutWindow->show();
        });
    }
}

void MainMenu::setTheme() {
    QString stylePath = QString(":/styles/%1.qss").arg(mCfgMan->mAppTheme);
    QFile stylesheetFile(stylePath);
    stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text);
        this->setStyleSheet(stylesheetFile.readAll());
    stylesheetFile.close();
    mDarkTheme = stylePath.contains("dark.qss");
}

void MainMenu::buildList(QString searchRequest) {
    foreach (QPushButton* button, mStacksButtonList) {
        delete button;
    }
    mStacksButtonList.clear();

    int row = 0, column = 0;
    foreach (QJsonValue stackPath, mCfgMan->mStacksList) {
        QStringList path = stackPath.toString().split('/');
        QString name = path.last();
        name.chop(5);
        if (name.toLower().contains(searchRequest.toLower())) {
            QString amountOfTerms = QString::number(countTerms(stackPath.toString()));
            QString buttonText = QString("%1\n\n%2 terms").arg(name, amountOfTerms);
            QPushButton* stackButton = new QPushButton(buttonText);
            stackButton->setObjectName("stackButton");
            stackButton->setMinimumSize(180, 90);
            mGridLayout->addWidget(stackButton,
                                   row,
                                   column);
            mStacksButtonList.append(stackButton);
            if (column < 1) {
                ++column;
            }
            else {
                ++row;
                column = 0;
            }

            connect(stackButton, &QPushButton::clicked, this, [this, stackPath]() {
                StackMenu* stackMenu = new StackMenu(this,
                                                     mApp,
                                                     stackPath.toString(),
                                                     mCfgMan,
                                                     mCfgDir);
                this->hide();
                stackMenu->show();
            });
        }
    }
}

int MainMenu::countTerms(QString path) {
    QFile stackFile(path);
    QJsonObject stackObj;
    stackFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = stackFile.readAll();
    stackFile.close();

    stackObj = QJsonDocument::fromJson(data.toUtf8()).object();
    return stackObj["count"].toInt();
}

void MainMenu::import(QStringList paths) {
    bool empty = mCfgMan->mStacksList.empty();
    foreach (QString path, paths) {
        mCfgMan->mStacksList.append(QJsonValue(path));
    }
    mCfgMan->saveConfig(mCfgDir);
    if (!mNoStacks) {
        mSearchBox->clear();
        buildList("");

        QScrollBar* vScrollBar = mScrollArea->verticalScrollBar();
        vScrollBar->setSliderPosition(vScrollBar->maximum());
    }
    else {
        delete mFirstRunWidget;
        makeUI(true);
    }
}

void MainMenu::newStack() {
    QFileDialog dialog;
    QString filename = dialog.getSaveFileName(nullptr,
                                              "Save stack",
                                              QDir::homePath(),
                                              "Stack (*.json)");
    if (!filename.isEmpty()) {
        if (!filename.endsWith(".json")) {
            filename += ".json";
        }

        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write("{ }");
        file.close();

        mCfgMan->mStacksList.append(QJsonValue(filename));
        mCfgMan->saveConfig(mCfgDir);

        if (mNoStacks) {
            delete mFirstRunWidget;
            makeUI(true);
        }

        StackMenu* stackMenu = new StackMenu(this, mApp, filename, mCfgMan);
        this->hide();
        stackMenu->show();
    }
}

void MainMenu::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}

void MainMenu::dropEvent(QDropEvent* event) {
    event->acceptProposedAction();

    QString data = event->mimeData()->text();
    QString terminator = "\r\n";
    if (data.endsWith(terminator)) {
        data.chop(terminator.length());
    }
    QStringList paths;
    foreach (QString srcPath, data.split(terminator)) {
        QUrl url(srcPath);
        paths.append(url.toLocalFile());
    }
    import(paths);
}

MainMenu::MainMenu(QWidget* parent,
                   QApplication* app) : QWidget(parent) {
    setAcceptDrops(true);
    mApp = app;
    mFirstRun = isFirstRun();
    mCfgMan = new ConfigMan();
    mCfgMan->readConfig(mCfgDir, mFirstRun);
    setTheme();
    makeUI(false);
}

void MainMenu::closeEvent(QCloseEvent* event) {
    qApp->quit();
}

MainMenu::~MainMenu() {

}
