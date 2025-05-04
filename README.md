# DBus-Config-Managed

Проект по реализации D-Bus-сервера на языке программирования С++ с помощью библиотеки QtDBus. Для сборки проекта была 
использована система Cmake. Стилевые настройки находятся в папке materials.

## Описание(Тех задание)

1. Реализован DBus-сервис с именем `com.system.configurationManager` на сессионной шине, который:

    - при запуске считывает файлы конфигураций приложений из папки `~/com.system.configurationManager/` и  
      создаёт для каждого файла конфигурации D-Bus-объект  
      `com.system.configurationManager.Application.{applicationName}` с интерфейсом  
      `com.system.configurationManager.Application.Configuration`;
    - предоставляет в этом интерфейсе методы:
        - `void ChangeConfiguration(const QString &key, const QDBusVariant &value)` — изменяет  
          указанный параметр в конфиге и выдаёт D-Bus-ошибку в случае неудачи;
        - `QVariantMap GetConfiguration()` — возвращает полную карту конфигурации приложения;
    - при изменении любой настройки эмитит сигнал  
      `configurationChanged(QVariantMap conf)`, где `conf` имеет D-Bus-тип `a{sv}`.

2. Реализовано консольное приложение `confManagerApplication1`, которое:

    - подписывается на сигнал `configurationChanged` на объекте  
      `com.system.configurationManager.Application.confManagerApplication1` сервиса  
      `com.system.configurationManager` и применяет новые параметры в случае их изменения;
    - хранит свой конфиг в файле  
      `~/com.system.configurationManager/confManagerApplication1.json` с полями:
        1. `"Timeout"` (uint, миллисекунды) — интервал между выводами строки;
        2. `"TimeoutPhrase"` (string) — фраза, выводимая каждые `Timeout` мс.

---

