#!/bin/bash

set -e

SERVICE="com.system.configurationManager"
APP="testapp"
OBJ="/com/system/configurationManager/Application/$APP"
IFACE="com.system.configurationManager.Application.Configuration"
CONFIG_DIR="$HOME/com.system.configurationManager"
CONFIG_FILE="$CONFIG_DIR/$APP.conf"

echo "Подготовка..."
ls -l "$CONFIG_FILE"

echo "Ждём запуск сервиса..."
sleep 1

echo "Проверка readConfig..."
gdbus call --session --dest "$SERVICE" --object-path "$OBJ" --method "$IFACE.readConfig"

echo "Проверка GetConfiguration..."
gdbus call --session --dest "$SERVICE" --object-path "$OBJ" --method "$IFACE.GetConfiguration"

echo "Изменение конфигурации через ChangeConfiguration (на 1.4.4)..."
dbus-send --session \
  --dest="$SERVICE" \
  --type=method_call \
  "$OBJ" \
  "$IFACE.ChangeConfiguration" \
  string:"General/version" variant:string:"1.4.4"

echo "Проверка изменения файла..."
sleep 1
if grep -q "version=1.4.4" "$CONFIG_FILE"; then
  echo "Файл успешно обновлён (найден version=1.4.4)"
else
  echo "Ошибка: файл не содержит version=1.4.4"
  echo "Содержимое файла:"
  cat "$CONFIG_FILE"
  exit 1
fi


#if grep -Eq "^\s*version *= *1\.4\.4" "$CONFIG_FILE"; then
#  echo "Файл успешно обновлён (найден version=1.4.4)"
#else
#  echo "Ошибка: файл не содержит version=1.4.4"
#  echo "Содержимое файла:"
#  cat "$CONFIG_FILE"
#  exit 1
#fi

echo "Проверка свойства configFileName..."
gdbus call --session --dest "$SERVICE" --object-path "$OBJ" \
  --method org.freedesktop.DBus.Properties.Get "$IFACE" configFileName

echo "Проверка сигналов configUpdated и configurationChanged..."

SIGNAL_LOG=$(mktemp)

# подписываемся на оба сигнала
( timeout 5 dbus-monitor "interface=$IFACE" > "$SIGNAL_LOG" ) &

MONITOR_PID=$!
sleep 0.5

echo "Второе изменение конфигурации через ChangeConfiguration (на 1.2.3)..."
dbus-send --session \
  --dest="$SERVICE" \
  --type=method_call \
  "$OBJ" \
  "$IFACE.ChangeConfiguration" \
  string:"General/version" variant:string:"1.2.3"

wait $MONITOR_PID || true

# Проверка сигналов
CONFIG_UPDATED_OK=false
CONFIG_CHANGED_OK=false

if grep -q "member=configUpdated" "$SIGNAL_LOG"; then
  echo "Сигнал configUpdated получен"
  CONFIG_UPDATED_OK=true
else
  echo "Сигнал configUpdated не получен вовремя"
fi

if grep -q "member=configurationChanged" "$SIGNAL_LOG"; then
  echo "Сигнал configurationChanged получен"
  CONFIG_CHANGED_OK=true
else
  echo "Сигнал configurationChanged не получен вовремя"
fi

rm "$SIGNAL_LOG"

if ! $CONFIG_UPDATED_OK || ! $CONFIG_CHANGED_OK; then
  echo "Ошибка: один или оба сигнала не были получены"
  exit 1
fi

echo "Все тесты пройдены!"
