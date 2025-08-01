#!/bin/bash

build/server &
SERVER_PID=$!
echo "Server started with PID: $SERVER_PID"

sleep 1

echo -e "test_client\nlog record 1\nlog record 2\n" | nc -q 0 localhost 4000
echo "Data sent to server"

sleep 1

LOG_DIR="test"
echo "Checking directory: $LOG_DIR"
ls -l $LOG_DIR

LOG_FILE=$(ls $LOG_DIR | grep "test_client.log")
if [ -z "$LOG_FILE" ]; then
    echo "Лог-файл не найден"
    exit 1
fi

CONTENT=$(cat $LOG_DIR/$LOG_FILE)
EXPECTED="[0001] ---=== Client "test_client" ===---
[0002] log record 1
[0003] log record 2
[0004] ^^^ ^^^ ^^^ Client stopped ^^^ ^^^ ^^^"

# Сохраняем ожидаемое содержимое во временный файл
echo "$EXPECTED" > expected.txt
# Сравниваем файлы, игнорируя пробелы и переносы строк
if diff -w <(echo "$CONTENT") expected.txt > /dev/null; then
    echo "Тест пройден"
else
    echo "Тест не пройден"
    echo "Ожидалось:"
    echo "$EXPECTED"
    echo "Получено:"
    echo "$CONTENT"
fi

rm expected.txt
kill $SERVER_PID
echo "Server stopped"