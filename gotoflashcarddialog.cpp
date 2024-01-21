#include "gotoflashcarddialog.h"

GoToFlashcardDialog::GoToFlashcardDialog(Reader* parent, int count) {
    this->setObjectName("goToFlashcard");
    this->setWindowTitle("Go to flashcard");
    this->setWindowIcon(parent->windowIcon());

    // Geometry
    int width = 300, height = 150;
    QScreen* screen = qApp->primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width) / 2;
    int y = (screenGeometry.height() - height) / 2;
    this->setGeometry(x, y, width, height);
    this->setFixedSize(width, height);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);

    this->setStyleSheet(parent->styleSheet());

    // UI
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* specifyLabel = new QLabel("Specify which flashcard you want to go to");
    mainLayout->addWidget(specifyLabel);

    QHBoxLayout* fieldLayout = new QHBoxLayout();

    QSpinBox* spinBox = new QSpinBox();
    spinBox->setMinimum(1);
    spinBox->setMaximum(count);
    fieldLayout->addWidget(spinBox);

    QLabel* maxLabel = new QLabel(QString("/%1").arg(QString::number(count)));
    fieldLayout->addWidget(maxLabel);

    mainLayout->addLayout(fieldLayout);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();

    buttonsLayout->addSpacerItem(new QSpacerItem(0, 0,
                                                 QSizePolicy::MinimumExpanding,
                                                 QSizePolicy::Ignored));

    QPushButton* cancelButton = new QPushButton("Cancel");
    buttonsLayout->addWidget(cancelButton);

    QPushButton* goButton = new QPushButton("Go");
    goButton->setDefault(true);
    goButton->setAutoDefault(true);
    buttonsLayout->addWidget(goButton);

    mainLayout->addLayout(buttonsLayout);

    // Making connections
    connect(cancelButton, &QPushButton::clicked, this, [this]() {
        this->hide();
        delete this;
    });

    connect(goButton, &QPushButton::clicked, this, [this, spinBox, parent]() {
        parent->loadFlashcard(spinBox->value(), false);
        parent->mTurnedOver = false;
        this->hide();
        delete this;
    });
}
