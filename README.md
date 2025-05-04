# DBus-Config-Managed

Проект по реализации D-Bus-сервера на языке программирования С++ с помощью библиотеки QtDBus. Для сборки проекта была 
использована система Cmake. Стилевые настройки, тесты находятся в папке materials.

## Описание

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

## Программные требования

- **Компилятор**: GCC ≥ 5.0 или Clang с поддержкой C++11.
- **Qt**: Qt5 или Qt6, включая модули `QtCore` и `QtBus`
- **Cmake**: версия ≥ 3.10 
- **Linux с поддержкой D-Bus session bus**

---

## Сборка проекта

Для запуска этого проекта из корневой директории используется CMakeLists.txt из нее же. Также в папках server, confManagerApplication1 
присутствуют отдельные CMakeLists.txt для сборки подпрограмм(для сервера и приложения соответственно). 

1. Склонируйте проект в отдельную директорию
2. Соберите проект с помощью представленного в корневой директории скрипта `run.sh`
   На выходе в папке build/ вы получите два исполняемых файла:
      - `server/server`
      - `confManagerApplication1/confManagerApplication1`

## Использование

Перед запуском сервера убедитесь, что имеется папка `~/com.system.configurationManager/` 
с соответствующими файлами конфигурации(`.conf` и `.json`).

1. Запустите сервер в отдельном терминале 

   ```bash
      ./build/server/server
   ```
После запуска сервер выводит объекты, которые были зарегистрированы с `~/com.system.configurationManager/*.conf`

2. В втором терминале запустите приложение 

   ```bash
      ./build/confManagerApplication1/confManagerApplication1
   ```
   
После запуска приложение выводит сообщение об успешном подключении.

   ```bash
    Подписка успешно выполнена. Ожидаем сигнал... 
   ```

3. После запуска сервера и приложения для проверки их работы, в другом терминале необходимо ввести команду, чтобы изменить
параметр через D-Bus:
   ```bash
      dbus-send --session \
      --dest=com.system.configurationManager \
      --type=method_call \
      /com/system/configurationManager/Application/confManagerApplication1 \
      com.system.configurationManager.Application.Configuration.ChangeConfiguration \
      string:"General/TimeoutPhrase" variant:string:"Please stop me"
   ```

Фраза "Please stop me" должна начать периодически появляться в терминале приложения.

## Тестирование

Для запуска автоматических тестов перейдите в `materials/tests`:

   ```bash 
    ./test_client.sh
   ```

Для проверки работы функционала сервера необходимо запустить сервер, а в отдельном терминале:

   ```bash
   ./test.sh
   ```