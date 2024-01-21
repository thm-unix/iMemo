#include "stackexporter.h"

void StackExporter::readStack(QString stackPath) {
    QFile stackFile(stackPath);
    stackFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data = stackFile.readAll();
    stackFile.close();
    mStackObj = QJsonDocument::fromJson(data.toUtf8()).object();
}

int StackExporter::getTextHeight(QString text, int ptSize) {
    text = QTextDocumentFragment::fromHtml(text).toPlainText();
    //text = text.replace("\n", "<br>");

    QFontMetrics fontMetrics = QFontMetrics(QFont(mCfgMan->mFontFamily, ptSize));
    int lineHeight = fontMetrics.height();
    int countLines = text.count("\n") + 1;
    return (lineHeight * countLines);
}

int StackExporter::getMaximumFontSize(QString text, int width, int height) {
    text = QTextDocumentFragment::fromHtml(text).toPlainText();
    //text = text.replace("\n", "<br>");

    int fontSize = 26;
    QFont font;
    font.setFamily(mCfgMan->mFontFamily);
    font.setPointSize(26);
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
    while ((fontMetrics.horizontalAdvance(maxLine) >= width) ||
           (getTextHeight(text, fontSize) >= height)) {
        --fontSize;
        font.setPointSize(fontSize);
        fontMetrics = QFontMetrics(font);
    }

    return fontSize;
}

void StackExporter::exportStack() {
    QString filename = QFileDialog::getSaveFileName(nullptr, "Export stack", QDir::homePath(), "PDF (*.pdf)");
    if (!filename.isEmpty()) {
        if (!filename.endsWith(".pdf")) {
            filename += ".pdf";
        }

        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filename);
        printer.setPageSize(QPageSize::A6);
        printer.setOrientation(QPrinter::Landscape);

        QPainter painter(&printer);
        QTextDocument doc;

        QTextOption options;
        options.setWrapMode(QTextOption::NoWrap);

        for (int i = 1; i <= mStackObj["count"].toInt(); ++i) {
            QString front = mStackObj[QString::number(i)].toObject()["f"].toString();
            front = front.replace("font-size:13pt;", "");

            int frontSize = getMaximumFontSize(front, 499, 316);
            painter.setFont(QFont(mCfgMan->mFontFamily, frontSize));

            int frontHeight = getTextHeight(front, frontSize);
            int frontVMargin = (300 - frontHeight) / 2;

            QStaticText finalFront(front);
            finalFront.setTextOption(options);

            painter.drawStaticText(20, 20 + frontVMargin, finalFront);

            printer.newPage();

            QString back = mStackObj[QString::number(i)].toObject()["b"].toString();
            back = back.replace("font-size:13pt;", "");

            int backSize = getMaximumFontSize(back, 499, 316);
            painter.setFont(QFont(mCfgMan->mFontFamily, backSize));

            int backHeight = getTextHeight(back, backSize);
            int backVMargin = (300 - backHeight) / 2;

            QStaticText backFinal(back);
            backFinal.setTextOption(options);

            painter.drawStaticText(20, 20 + backVMargin, backFinal);

            if (i < mStackObj["count"].toInt()) {
                printer.newPage();
            }
        }

        QDesktopServices::openUrl(filename);
    }
}

StackExporter::StackExporter(ConfigMan* cfgMan,
                             QString stackPath) {
    mCfgMan = cfgMan;
    readStack(stackPath);
    exportStack();
}
