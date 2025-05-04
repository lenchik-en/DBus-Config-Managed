#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>
#include <QtDBus>
#include <QDBusMetaType>

#include "ConfigurationObject.h"

int main(int argc, char* argv[]) {
    qRegisterMetaType<QVariant>("QVariant");
    qDBusRegisterMetaType<QVariantMap>();

    QCoreApplication app(argc, argv);

    const QString serviceName = "com.system.configurationManager";
    const QString basePath = QDir::homePath() + "/com.system.configurationManager";

    if (!QDBusConnection::sessionBus().registerService(serviceName)) {
        qCritical() << "Could not register D-Bus service";
        return 1;
    }

    QDir configDir(basePath);
    QFileSystemWatcher watcher;
    QMap<QString, ConfigurationObject*> objectMap;

    for (const QString& file : configDir.entryList({"*.conf"}, QDir::Files)) {
        QString appName = file.section('.', 0, 0);
        QString objectPath = "/com/system/configurationManager/Application/" + appName;
        QString fullPath = configDir.filePath(file);

        auto* obj = new ConfigurationObject(fullPath);
        objectMap[fullPath] = obj;

        QDBusConnection::sessionBus().registerObject(
                objectPath, obj,
                QDBusConnection::ExportAllSlots |
                QDBusConnection::ExportAllSignals |
                QDBusConnection::ExportAllProperties);

        watcher.addPath(fullPath);
        qDebug() << "Registered D-Bus object:" << objectPath;
    }

    QObject::connect(&watcher, &QFileSystemWatcher::fileChanged,
                     [&](const QString& path) {
                         if (objectMap.contains(path)) {
                             qDebug() << "Detected change in config:" << path;
                             objectMap[path]->reloadConfig();
                             watcher.addPath(path);
                         }
                     });

    return app.exec();
}
