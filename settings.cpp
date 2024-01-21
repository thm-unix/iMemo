#include "settings.h"

#include "mainmenu.h"

void Settings::makeUI(QWidget* parent, ConfigMan* cfgMan) {
    this->setWindowIcon(parent->windowIcon());
    this->setWindowTitle("iMemo Settings");

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

    upperLayout->addSpacerItem(new QSpacerItem(0, 0,
                                               QSizePolicy::MinimumExpanding,
                                               QSizePolicy::Ignored));

    mainLayout->addLayout(upperLayout);

    QLabel* themeLabel = new QLabel("Theme");

    QVBoxLayout* themeLayout = new QVBoxLayout();

    QRadioButton* lightRadioButton = new QRadioButton("Light");
    themeLayout->addWidget(lightRadioButton);

    QRadioButton* darkRadioButton = new QRadioButton("Dark");
    themeLayout->addWidget(darkRadioButton);

    mainLayout->addLayout(themeLayout);

    QLabel* fontFamilyLabel = new QLabel("Font Family");
    mainLayout->addWidget(fontFamilyLabel);

    QFontComboBox* fontComboBox = new QFontComboBox();
    mainLayout->addWidget(fontComboBox);

    QLabel* autosaveInterval = new QLabel("Auto-Save interval");
    mainLayout->addWidget(autosaveInterval);

    QHBoxLayout* intervalLayout = new QHBoxLayout();

    QSpinBox* spinBox = new QSpinBox();
    spinBox->setMinimum(1);
    spinBox->setMaximum(20);
    intervalLayout->addWidget(spinBox);

    QLabel* minLabel = new QLabel("min");
    intervalLayout->addWidget(minLabel);

    mainLayout->addLayout(intervalLayout);

    mainLayout->addSpacerItem(new QSpacerItem(0, 0,
                                              QSizePolicy::Ignored,
                                              QSizePolicy::MinimumExpanding));

    QPushButton* saveButton = new QPushButton("Save");
    mainLayout->addWidget(saveButton);

    // Set current settings
    if (!cfgMan->mAppTheme.compare("light")) {
        lightRadioButton->setChecked(true);
    }
    else {  // Dark
        darkRadioButton->setChecked(true);
    }

    fontComboBox->setCurrentFont(QFont(cfgMan->mFontFamily));

    spinBox->setValue(cfgMan->mAutosaveIntervalMin);

    // Making connections
    connect(backPushButton, &QPushButton::clicked, this, [this, parent]() {
        QString req = static_cast<MainMenu*>(mParent)->mSearchBox->text();
        static_cast<MainMenu*>(mParent)->buildList(req);
        parent->show();
        this->hide();
        delete this;
    });

    connect(saveButton, &QPushButton::clicked, this, [this, cfgMan,
                                                      lightRadioButton,
                                                      fontComboBox,
                                                      spinBox, parent]() {
        if (lightRadioButton->isChecked()) {
            cfgMan->mAppTheme = "light";
        }
        else {  // Dark
            cfgMan->mAppTheme = "dark";
        }

        cfgMan->mFontFamily = fontComboBox->currentFont().family();

        cfgMan->mAutosaveIntervalMin = spinBox->value();

        cfgMan->saveConfig(static_cast<MainMenu*>(parent)->mCfgDir);

        static_cast<MainMenu*>(parent)->setTheme();
        QString req = static_cast<MainMenu*>(parent)->mSearchBox->text();
        static_cast<MainMenu*>(parent)->buildList(req);

        parent->show();
        this->hide();
        delete this;
    });
}

void Settings::closeEvent(QCloseEvent* event) {
    QString req = static_cast<MainMenu*>(mParent)->mSearchBox->text();
    static_cast<MainMenu*>(mParent)->buildList(req);
    mParent->show();
    this->hide();
    delete this;
}

Settings::Settings(QWidget* parent, ConfigMan* cfgMan) : QWidget(nullptr) {
    mParent = parent;
    makeUI(parent, cfgMan);
    this->setObjectName("settings");
    this->setGeometry(parent->geometry());
}
