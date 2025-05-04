#include "configurationobject.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QSettings>

ConfigurationObject::ConfigurationObject(const QString& configFilePath, QObject* parent)
        : QObject(parent), configFilePath_(configFilePath)
{}

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
        const QStringList keys = settings.childKeys();

        QVariantMap groupMap;
        for (const QString& key : keys) {
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
//    qDebug() << "Reloaded keys:" << configuration_;
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

    reloadConfig();
//    qDebug() << "ChangeConfiguration: key =" << key << "value =" << value.variant();
}
