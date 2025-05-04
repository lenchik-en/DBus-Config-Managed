#include "ConfigurationObject.h"

#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

ConfigurationObject::ConfigurationObject(const QString& configFilePath,
                                         QObject* parent)
        : QObject(parent), configFilePath_(configFilePath) {}

QString ConfigurationObject::readConfig() const {
    QFile file(configFilePath_);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QStringLiteral("Failed to open config: ") + configFilePath_;
    QTextStream in(&file);
    return in.readAll();
}

QString ConfigurationObject::configFileName() const {
    return configFilePath_;
}

void ConfigurationObject::setConfigFileName(const QString& name) {
    configFilePath_ = name;
}

void ConfigurationObject::reloadConfig() {
    QSettings settings(configFilePath_, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    configuration_.clear();
    const QStringList groups = settings.childGroups();

    for (const QString& group : groups) {
        settings.beginGroup(group);
        QVariantMap groupMap;
        for (const QString& key : settings.childKeys()) {
            groupMap.insert(key, settings.value(key));
        }
        settings.endGroup();
        configuration_.insert(group, groupMap);
    }

    name_ = configuration_.value("General").toMap().value("name").toString();
    version_ = configuration_.value("General").toMap().value("version").toString();

    emit configUpdated(readConfig());
    emit configurationChanged(GetConfiguration());
    qDebug() << "Config reloaded:" << configFilePath_;
}

QMap<QString, QVariant> ConfigurationObject::GetConfiguration() const {
    QVariantMap flat;
    for (const QString& section : configuration_.keys()) {
        const QVariantMap group = configuration_.value(section).toMap();
        for (const QString& key : group.keys()) {
            flat.insert(section + "/" + key, group.value(key));
        }
    }
    return flat;
}

void ConfigurationObject::ChangeConfiguration(const QString& key, const QDBusVariant& value) {
    QSettings settings(configFilePath_, QSettings::IniFormat);
    settings.setValue(key, value.variant());
    settings.sync();

    updateJsonFileIfNeeded(key, value.variant());
    reloadConfig();
}

void ConfigurationObject::updateJsonFileIfNeeded(const QString& key, const QVariant& value) {
    // Только для приложения confManagerApplication1
    if (!configFilePath_.contains("confManagerApplication1.conf"))
        return;

    QString jsonPath = QDir::homePath() + "/com.system.configurationManager/confManagerApplication1.json";
    QFile jsonFile(jsonPath);
    QJsonObject json;

    if (jsonFile.open(QIODevice::ReadOnly)) {
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll(), &err);
        if (err.error == QJsonParseError::NoError && doc.isObject())
            json = doc.object();
        jsonFile.close();
    }

    QString section = key.section('/', 0, 0);
    QString subkey = key.section('/', 1);

    if (section == "General" && subkey == "Timeout" && value.canConvert<int>())
        json["Timeout"] = value.toInt();
    else if (section == "General" && subkey == "TimeoutPhrase" && value.canConvert<QString>())
        json["TimeoutPhrase"] = value.toString();

    if (jsonFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonDocument doc(json);
        jsonFile.write(doc.toJson(QJsonDocument::Indented));
        jsonFile.close();
    }
}
