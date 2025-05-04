#include <QCoreApplication>
#include <QtDBus/QtDBus>
#include <QDebug>

class ConfigReceiver : public QObject {
    Q_OBJECT
public slots:
            void onConfigurationChanged(const QVariantMap& config) {
        qDebug() << "[confManagerApplication1] Получен сигнал configurationChanged:";
        for (auto it = config.constBegin(); it != config.constEnd(); ++it) {
            qDebug() << " • " << it.key() << "=" << it.value().toString();
        }
    }
};

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    const QString service = "com.system.configurationManager";
    const QString path = "/com/system/configurationManager/Application/confManagerApplication1";
    const QString interface = "com.system.configurationManager.Application.Configuration";

    ConfigReceiver receiver;

    bool ok = QDBusConnection::sessionBus().connect(
            service,
            path,
            interface,
            "configurationChanged",
            &receiver,
            SLOT(onConfigurationChanged(QVariantMap))
    );

    if (!ok) {
        qCritical() << "❌ Не удалось подписаться на сигнал.";
        return 1;
    }

    qDebug() << "[confManagerApplication1] Подписка успешно выполнена. Ожидаем сигнал...";
    return app.exec();
}

#include "main.moc"
