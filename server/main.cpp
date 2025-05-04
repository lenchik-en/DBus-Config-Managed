#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>
#include <QtDBus>

#include "configurationobject.h"

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);

  const QString serviceName = "com.system.configurationManager";
  const QString basePath =
      QDir::homePath() + "/com.system.configurationManager";

  if (!QDBusConnection::sessionBus().registerService(serviceName)) {
    qCritical() << "❌ Could not register D-Bus service";
    return 1;
  }

  QDir configDir(basePath);
  QStringList configFiles =
      configDir.entryList(QStringList() << "*.conf", QDir::Files);

  QFileSystemWatcher watcher;
  QMap<QString, ConfigurationObject*> objectMap;

  for (const QString& file : configFiles) {
    QString appName = file.section('.', 0, 0);
    QString objectPath =
        "/com/system/configurationManager/Application/" + appName;

    QString fullPath = configDir.filePath(file);
    auto* obj = new ConfigurationObject(fullPath);
    objectMap[fullPath] = obj;

    QDBusConnection::sessionBus().registerObject(
        objectPath, obj,
        QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals |
            QDBusConnection::ExportAllProperties);

    watcher.addPath(fullPath);
    qDebug() << "✅ Registered D-Bus object with watcher:" << objectPath;
  }

  QObject::connect(&watcher, &QFileSystemWatcher::fileChanged,
                   [&](const QString& path) {
                     if (objectMap.contains(path)) {
                       qDebug() << "📝 Detected change in config:" << path;
                       objectMap[path]->reloadConfig();
                       watcher.addPath(path);  // повторно отслеживаем
                     }
                   });

  return app.exec();
}

// #include "main.moc"
