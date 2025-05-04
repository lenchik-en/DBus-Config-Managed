#include "configurationobject.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

ConfigurationObject::ConfigurationObject(const QString& configFilePath,
                                         QObject* parent)
    : QObject(parent), configFilePath_(configFilePath) {}

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