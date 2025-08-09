import socket
import time

def send_log(host="localhost", port=4000, client_name="client", logs=None):
    if logs is None:
        logs = ['log record 1', 'log record 2']
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((host, port))
            print(f"Connected to {host}:{port}")
            # Отправляем имя клиента
            s.sendall(f"{client_name}\n".encode('utf-8'))
            print(f"Sent client name: {client_name}")
            # Отправляем записи логов
            for log in logs:
                s.sendall(f"{log}\n".encode('utf-8'))
                print(f"Sent log: {log}")
            # Даём серверу время обработать данные
            time.sleep(1)
    except Exception as e:
        print(f"Error in send_log: {e}")

if __name__ == "__main__":
    send_log(client_name="test_client")