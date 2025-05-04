#!/bin/bash

set -e

SERVICE="com.system.configurationManager"
APP="confManagerApplication1"
OBJ="/com/system/configurationManager/Application/$APP"
IFACE="com.system.configurationManager.Application.Configuration"
CONFIG_DIR="$HOME/com.system.configurationManager"
INI_FILE="$CONFIG_DIR/$APP.conf"
JSON_FILE="$CONFIG_DIR/$APP.json"
PHRASE="Please stop me"
TIMEOUT=1000

echo "Подготовка..."
mkdir -p "$CONFIG_DIR"
cat > "$INI_FILE" <<EOF
[%General]
name=confManagerApplication1
version=1.0.0
EOF

cat > "$JSON_FILE" <<EOF
{
  "Timeout": $TIMEOUT,
  "TimeoutPhrase": "$PHRASE"
}
EOF

echo "Запуск сервера..."
./build/server/server > /dev/null 2>&1 &
SERVER_PID=$!
sleep 1

echo "Запуск клиента..."
./build/confManagerApplication1/confManagerApplication1 > client_output.log 2>&1 &
CLIENT_PID=$!
sleep 1

echo "Отправка параметров Timeout и TimeoutPhrase..."
dbus-send --session \
  --dest="$SERVICE" \
  --type=method_call \
  "$OBJ" \
  "$IFACE.ChangeConfiguration" \
  string:"General/Timeout" variant:uint32:$TIMEOUT

dbus-send --session \
  --dest="$SERVICE" \
  --type=method_call \
  "$OBJ" \
  "$IFACE.ChangeConfiguration" \
  string:"General/TimeoutPhrase" variant:string:"$PHRASE"

echo "Ожидание вывода клиентом фразы..."
sleep 2

echo "Проверка вывода клиента..."
if grep -q "$PHRASE" client_output.log; then
  echo "Фраза успешно выведена клиентом."
else
  echo "Клиент не вывел ожидаемую фразу."
  cat client_output.log
  kill $SERVER_PID $CLIENT_PID
  exit 1
fi

echo "Завершаем процессы..."
kill $SERVER_PID $CLIENT_PID

echo "Все проверки пройдены!"
