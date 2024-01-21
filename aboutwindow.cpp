#include "aboutwindow.h"
#include "mainmenu.h"

void AboutWindow::makeUI(QWidget* parent, Platform platform) {
    // UI
    this->setWindowTitle("About iMemo");
    this->setWindowIcon(parent->windowIcon());

    QFont font;
    font.setPointSize(11);
    this->setFont(font);

    QFont appNameFont = font;
    appNameFont.setPointSize(16);

    this->setStyleSheet(parent->styleSheet());

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* upperLayout = new QHBoxLayout();

    QPushButton* backPushButton = new QPushButton("Back");
    backPushButton->setIcon(QIcon(":/icons/go-previous.png"));
    upperLayout->addWidget(backPushButton);

    upperLayout->addSpacerItem(new QSpacerItem(0, 0,
                                               QSizePolicy::MinimumExpanding,
                                               QSizePolicy::Ignored));

    mainLayout->addLayout(upperLayout);

    QHBoxLayout* iconLayout = new QHBoxLayout();

    iconLayout->addSpacerItem(new QSpacerItem(0, 0,
                                              QSizePolicy::MinimumExpanding,
                                              QSizePolicy::Ignored));

    QString iconPath = ":/icons/app-icon-generic.png";
    if (platform == Mac) {
        iconPath = ":/icons/app-icon-mac.png";
    }

    QLabel* iconLabel = new QLabel();
    iconLabel->setFixedSize(128, 128);
    iconLabel->setPixmap(QPixmap(iconPath));
    iconLabel->setScaledContents(true);
    iconLayout->addWidget(iconLabel);

    iconLayout->addSpacerItem(new QSpacerItem(0, 0,
                                              QSizePolicy::MinimumExpanding,
                                              QSizePolicy::Ignored));

    mainLayout->addLayout(iconLayout);

    QLabel* appNameLabel = new QLabel("<b>iMemo</b>");
    appNameLabel->setTextFormat(Qt::RichText);
    appNameLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(appNameLabel);

    QLabel* versionLabel = new QLabel("v. 0.1");
    versionLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(versionLabel);

    QLabel* developerLabel = new QLabel("\niMemo was brought to you by thm-unix");
    developerLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(developerLabel);

    QLabel* websiteLabel = new QLabel("<a href=\"https://thm-unix.github.io/\" style=\"color: #378edf\">https://thm-unix.github.io/</a>");
    websiteLabel->setTextFormat(Qt::RichText);
    websiteLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(websiteLabel);

    QLabel* githubLabel = new QLabel("<a href=\"https://github.com/thm-unix/iMemo/\" style=\"color: #378edf\">https://github.com/thm-unix/iMemo/</a>");
    githubLabel->setTextFormat(Qt::RichText);
    githubLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(githubLabel);

    QLabel* issuesLabel = new QLabel("Found bug/want to help us/have an idea?\n"
                                     "Contact us via GitHub Issues.");
    issuesLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(issuesLabel);

    QLabel* licenseLabel = new QLabel("<br><br><a href=\"https://www.gnu.org/licenses/gpl-3.0.html\" style=\"color: #378edf\">Learn more about GNU General Public License v3...</a>");
    licenseLabel->setTextFormat(Qt::RichText);
    licenseLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(licenseLabel);


    mainLayout->addSpacerItem(new QSpacerItem(0, 0,
                                              QSizePolicy::Ignored,
                                              QSizePolicy::MinimumExpanding));


    // Make connections
    connect(backPushButton, &QPushButton::clicked, this, [this, parent]() {
        QString req = static_cast<MainMenu*>(mParent)->mSearchBox->text();
        static_cast<MainMenu*>(mParent)->buildList(req);
        this->hide();
        parent->setGeometry(this->geometry());
        parent->show();
        delete this;
    });
}

void AboutWindow::closeEvent(QCloseEvent* event) {
    QString req = static_cast<MainMenu*>(mParent)->mSearchBox->text();
    static_cast<MainMenu*>(mParent)->buildList(req);
    mParent->show();
    event->accept();
}

AboutWindow::AboutWindow(QWidget *parent,
                         Platform platform) : QWidget(nullptr) {
    mParent = parent;
    this->setObjectName("about");
    makeUI(parent, platform);
    this->setGeometry(parent->geometry());
}
