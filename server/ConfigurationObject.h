#ifndef CONFIGURATIONOBJECT_H
#define CONFIGURATIONOBJECT_H

#include <QDBusVariant>
#include <QMap>
#include <QObject>
#include <QVariant>

class ConfigurationObject : public QObject {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface",
              "com.system.configurationManager.Application.Configuration")
  Q_PROPERTY(QString configFileName READ configFileName WRITE setConfigFileName)

 public:
  explicit ConfigurationObject(const QString& configFilePath,
                               QObject* parent = nullptr);

 public slots:
  QString readConfig() const;
  QString configFileName() const;
  void setConfigFileName(const QString& name);
  void reloadConfig();
  QMap<QString, QVariant> GetConfiguration() const;
  void ChangeConfiguration(const QString& key, const QDBusVariant& value);

 signals:
  void configurationChanged(const QVariantMap& configuration);
  void configUpdated(const QString& newContent);

 private:
  QString configFilePath_;
  QVariantMap configuration_;
  QString name_;
  QString version_;

  void updateJsonFileIfNeeded(const QString& key,
                              const QVariant& value);  // ← добавлено
};

#endif  // CONFIGURATIONOBJECT_H
