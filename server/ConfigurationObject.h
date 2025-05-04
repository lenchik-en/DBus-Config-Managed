#ifndef CONFIGURATIONOBJECT_H
#define CONFIGURATIONOBJECT_H

#include <QObject>
#include <QString>


class ConfigurationObject : public QObject {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface",
              "com.system.configurationManager.Application.Configuration")

 public:
  ConfigurationObject(const QString& configFilePath, QObject* parent = nullptr);

 public slots:
  QString readConfig() const;
  QString configFileName() const;
  void setConfigFileName(const QString& name);
  void reloadConfig();

 signals:
  void configUpdated(const QString& newContent);

 private:
  QString configFilePath_;
};

#endif  // CONFIGURATIONOBJECT_H
