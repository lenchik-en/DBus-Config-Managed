#ifndef CONFIGURATIONOBJECT_H
#define CONFIGURATIONOBJECT_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QDBusVariant>



class ConfigurationObject : public QObject {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface",
              "com.system.configurationManager.Application.Configuration")
    Q_PROPERTY(QString configFileName READ configFileName WRITE setConfigFileName)


public:
  ConfigurationObject(const QString& configFilePath, QObject* parent = nullptr);

 public slots:
  QString readConfig() const;
  QString configFileName() const;
  void setConfigFileName(const QString& name);
  void reloadConfig();

    QMap<QString, QVariant> GetConfiguration() const;
    void ChangeConfiguration(const QString& key, const QDBusVariant& value);
 signals:
  void configUpdated(const QString& newContent);

 private:
  QString configFilePath_;

};

#endif  // CONFIGURATIONOBJECT_H
