#!/bin/bash

set -e

SERVICE="com.system.configurationManager"
APP="confManagerApplication1"
OBJ="/com/system/configurationManager/Application/$APP"
IFACE="com.system.configurationManager.Application.Configuration"
CONFIG_DIR="$HOME/com.system.configurationManager"
CONFIG_FILE="$CONFIG_DIR/$APP.conf"
JSON_CONFIG="$CONFIG_DIR/$APP.json"

echo "Подготовка..."
mkdir -p "$CONFIG_DIR"

# Конфигурация INI
cat > "$CONFIG_FILE" <<EOF
[%General]
name=confManagerApplication1
version=1.0.0
EOF

# Управляемая JSON-конфигурация
cat > "$JSON_CONFIG" <<EOF
{
  "Timeout": 1000,
  "TimeoutPhrase": "Конфигурация успешно применена!"
}
EOF

echo "Запуск сервера..."
./build/server/server &
SERVER_PID=$!
sleep 1

echo "Запуск клиента..."
./build/confManagerApplication1/confManagerApplication1 > client_output.log 2>&1 &
CLIENT_PID=$!
sleep 1

echo "Отправка ChangeConfiguration..."
dbus-send --session \
  --dest="$SERVICE" \
  --type=method_call \
  "$OBJ" \
  "$IFACE.ChangeConfiguration" \
  string:"General/version" variant:string:"2.0.1"

sleep 2

echo "Проверка, что клиент получил сигнал configurationChanged..."
if grep -q "configurationChanged" client_output.log; then
  echo "Сигнал успешно получен клиентом!"
else
  echo "Клиент не получил сигнал."
  cat client_output.log
  kill $SERVER_PID $CLIENT_PID
  exit 1
fi

echo "Проверка, что выведена фраза из JSON (TimeoutPhrase)..."
if grep -q "Конфигурация успешно применена!" client_output.log; then
  echo "TimeoutPhrase выведена"
else
  echo "TimeoutPhrase не выведена"
  cat client_output.log
  kill $SERVER_PID $CLIENT_PID
  exit 1
fi

echo "Завершаем процессы..."
kill $SERVER_PID $CLIENT_PID

echo "Все тесты клиента пройдены успешно!"


#dbus-send --session \
#>   --dest=com.system.configurationManager \
#>   --type=method_call \
#>   /com/system/configurationManager/Application/confManagerApplication1 \
#>   com.system.configurationManager.Application.Configuration.ChangeConfiguration \
#>   string:"General/TimeoutPhrase" variant:string:"Please stop me"
