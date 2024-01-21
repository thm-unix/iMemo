#include "configman.h"

void ConfigMan::readConfig(QString cfgDir, bool firstRun) {
    QFile configFile(cfgDir + "/config.json");
    QJsonObject configObj;
    if (!firstRun) {
        configFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QString data = configFile.readAll();
        configFile.close();

        configObj = QJsonDocument::fromJson(data.toUtf8()).object();
        mAppTheme = configObj["theme"].toString();
        mFontFamily = configObj["fontFamily"].toString();
        mStacksList = configObj["stacks"].toArray();
        mAutosaveIntervalMin = configObj["autosaveIntervalMin"].toInt();
        mCustomColors = configObj["customColors"].toArray();
    }
    else {
        // Setting default values
        configObj["theme"] = QJsonValue("light");
        mFontFamily = mFont.defaultFamily();
        configObj["fontFamily"] = QJsonValue(mFontFamily);
        configObj["stacks"] = QJsonArray();
        configObj["autosaveIntervalMin"] = QJsonValue(10);
        configObj["customColors"] = QJsonValue(QJsonArray());

        QDir d(cfgDir);
        if (!d.exists(cfgDir)) {
            d.mkdir(cfgDir);
        }
        QJsonDocument doc(configObj);
        configFile.open(QFile::WriteOnly);
        configFile.write(doc.toJson(QJsonDocument::Indented));
        configFile.close();
    }
}

void ConfigMan::saveConfig(QString cfgDir) {
    QJsonObject config;
    config["theme"] = QJsonValue(mAppTheme);
    config["fontFamily"] = QJsonValue(mFontFamily);
    config["stacks"] = QJsonValue(mStacksList);
    config["autosaveIntervalMin"] = QJsonValue(mAutosaveIntervalMin);
    config["customColors"] = QJsonValue(mCustomColors);

    QJsonDocument doc(config);
    QFile jsonFile(cfgDir + "/config.json");
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(doc.toJson(QJsonDocument::Indented));
    jsonFile.close();
}

ConfigMan::ConfigMan() {

}
