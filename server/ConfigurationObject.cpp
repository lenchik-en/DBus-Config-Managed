#include "configurationobject.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDBusError>


ConfigurationObject::ConfigurationObject(const QString& configFilePath, QObject* parent)
        : QObject(parent), configFilePath_(configFilePath)
{
    parseConfig();
}


void ConfigurationObject::parseConfig()
{
    config_.clear();
    QFile file(configFilePath_);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;

        const QStringList parts = line.split('=');
        if (parts.size() == 2)
            config_[parts[0].trimmed()] = parts[1].trimmed();
    }
}

void ConfigurationObject::saveConfig() const
{
    QFile file(configFilePath_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "❌ Could not write config file:" << configFilePath_;
        throw QDBusError(QDBusError::Failed, "Cannot write to config file.");
    }

    QTextStream out(&file);
    for (auto it = config_.begin(); it != config_.end(); ++it)
        out << it.key() << "=" << it.value().toString() << "\n";
}

QString ConfigurationObject::readConfig() const {
  QFile file(configFilePath_);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return QStringLiteral("❌ Failed to open config: ") + configFilePath_;

  QTextStream in(&file);
  return in.readAll();
}

QString ConfigurationObject::configFileName() const { return configFilePath_; }

void ConfigurationObject::setConfigFileName(const QString& name) {
  configFilePath_ = name;
}

void ConfigurationObject::reloadConfig() {
  QString updated = readConfig();
  emit configUpdated(updated);  // 🔔 Уведомление через D-Bus
  qDebug() << "♻️ Config reloaded:" << configFilePath_;
}