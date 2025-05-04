#include "configurationobject.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDBusError>
#include <QSettings>


ConfigurationObject::ConfigurationObject(const QString& configFilePath, QObject* parent)
        : QObject(parent), configFilePath_(configFilePath)
{}

//
//void ConfigurationObject::parseConfig()
//{
//    config_.clear();
//    QFile file(configFilePath_);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;
//
//    QTextStream in(&file);
//    while (!in.atEnd()) {
//        const QString line = in.readLine().trimmed();
//        if (line.isEmpty() || line.startsWith('#')) continue;
//
//        const QStringList parts = line.split('=');
//        if (parts.size() == 2)
//            config_[parts[0].trimmed()] = parts[1].trimmed();
//    }
//}
//
//void ConfigurationObject::saveConfig() const
//{
//    QFile file(configFilePath_);
//    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
//        qWarning() << "Could not write config file:" << configFilePath_;
//        throw QDBusError(QDBusError::Failed, "Cannot write to config file.");
//    }
//
//    QTextStream out(&file);
//    for (auto it = config_.begin(); it != config_.end(); ++it)
//        out << it.key() << "=" << it.value().toString() << "\n";
//}

QString ConfigurationObject::readConfig() const {
  QFile file(configFilePath_);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return QStringLiteral("Failed to open config: ") + configFilePath_;

  QTextStream in(&file);
  return in.readAll();
}

QString ConfigurationObject::configFileName() const { return configFilePath_; }

void ConfigurationObject::setConfigFileName(const QString& name) {
  configFilePath_ = name;
}

void ConfigurationObject::reloadConfig() {
    emit configUpdated(readConfig());
  qDebug() << "Config reloaded:" << configFilePath_;
}

QMap<QString, QVariant> ConfigurationObject::GetConfiguration() const
{
    QSettings settings(configFilePath_, QSettings::IniFormat);
    QVariantMap result;

    const QStringList groups = settings.childGroups();
    qDebug() << "Groups found:" << groups;

    for (const QString& group : groups) {
        settings.beginGroup(group);

        QStringList keys = settings.childKeys();
        qDebug() << "Group:" << group << "Keys:" << keys;

        QVariantMap groupMap;
        for (const QString& key : keys) {
            groupMap.insert(key, settings.value(key));
        }

        settings.endGroup();
        result.insert(group, groupMap);
    }

    return result;
}

void ConfigurationObject::ChangeConfiguration(const QString& key, const QDBusVariant& value)
{
    QSettings settings(configFilePath_, QSettings::IniFormat);
    settings.setValue(key, value.variant());
    settings.sync();

    emit configUpdated(readConfig());
}