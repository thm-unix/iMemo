#ifndef CONFIGMAN_H
#define CONFIGMAN_H

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFont>


class ConfigMan {
public:
    ConfigMan();
    void readConfig(QString cfgDir, bool firstRun);
    void saveConfig(QString cfgDir);

    QString mAppTheme = "light";
    QString mFontFamily;
    QJsonArray mStacksList = {};
    int mAutosaveIntervalMin = 10;
    QJsonArray mCustomColors = {};  // { "r,g,b", "r,g,b", ... }

private:
    QFont mFont;
};

#endif // CONFIGMAN_H
