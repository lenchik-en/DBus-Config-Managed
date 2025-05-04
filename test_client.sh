#!/bin/bash

set -e

SERVICE="com.system.configurationManager"
APP="confManagerApplication1"
OBJ="/com/system/configurationManager/Application/$APP"
IFACE="com.system.configurationManager.Application.Configuration"
CONFIG_FILE="$HOME/com.system.configurationManager/$APP.conf"

echo "Подготовка..."
mkdir -p "$(dirname "$CONFIG_FILE")"
cat > "$CONFIG_FILE" <<EOF
[%General]
name=confManagerApplication1
version=1.0.0
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

sleep 1

echo "Проверка, что клиент получил сигнал..."
if grep -q "configurationChanged" client_output.log; then
  echo "Сигнал успешно получен клиентом!"
else
  echo "Клиент не получил сигнал."
  cat client_output.log
  kill $SERVER_PID $CLIENT_PID
  exit 1
fi

echo "Завершаем процессы..."
kill $SERVER_PID $CLIENT_PID

echo "Все тесты пройдены!"
