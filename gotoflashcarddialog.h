#ifndef GOTOFLASHCARDDIALOG_H
#define GOTOFLASHCARDDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>

#include "reader.h"

class GoToFlashcardDialog : public QDialog {
public:
    GoToFlashcardDialog(Reader* parent, int count);
};

#endif // GOTOFLASHCARDDIALOG_H
