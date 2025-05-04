#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QtDBus/QtDBus>

class ConfigReceiver : public QObject {
  Q_OBJECT

 public:
  ConfigReceiver(QObject* parent = nullptr) : QObject(parent) {
    applyManagedConfig();
  }

 public slots:
  void onConfigurationChanged(const QVariantMap& config) {
    qDebug()
        << "Получен сигнал configurationChanged:";
    for (auto it = config.constBegin(); it != config.constEnd(); ++it) {
      qDebug() << " • " << it.key() << "=" << it.value().toString();
    }
    applyManagedConfig();
  }

 private:
  void applyManagedConfig() {
    QString configFilePath =
        QDir::homePath() +
        "/com.system.configurationManager/confManagerApplication1.json";
    QFile file(configFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning() << "Не удалось открыть конфигурационный файл:"
                 << configFilePath;
      return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
      qWarning() << "Ошибка парсинга JSON:" << error.errorString();
      return;
    }

    QJsonObject obj = doc.object();
    int timeout = obj.value("Timeout").toInt();
    QString phrase = obj.value("TimeoutPhrase").toString();

    if (timeout > 0 && !phrase.isEmpty()) {
      if (timer_) {
        timer_->stop();
        delete timer_;
      }

      timer_ = new QTimer(this);
      connect(timer_, &QTimer::timeout, this, [phrase]() {
        qDebug() << "[confManagerApplication1] =>" << phrase;
      });
      timer_->start(timeout);
    } else {
      qDebug() << "Конфигурация неполная или "
                  "некорректная.";
    }
  }

  QTimer* timer_ = nullptr;
};

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);

  const QString service = "com.system.configurationManager";
  const QString path =
      "/com/system/configurationManager/Application/confManagerApplication1";
  const QString interface =
      "com.system.configurationManager.Application.Configuration";

  ConfigReceiver receiver;

  bool ok = QDBusConnection::sessionBus().connect(
      service, path, interface, "configurationChanged", &receiver,
      SLOT(onConfigurationChanged(QVariantMap)));

  if (!ok) {
    qCritical() << "Не удалось подписаться на сигнал.";
    return 1;
  }

  qDebug() << "Подписка успешно выполнена. Ожидаем "
              "сигнал...";
  return app.exec();
}

#include "main.moc"
