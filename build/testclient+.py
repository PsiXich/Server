import socket
import time
import os
import subprocess
import sys

def send_log(host="localhost", port=4000, client_name="client", logs=None):
    if logs is None:
        logs = ['log record 1', 'log record 2']
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((host, port))
            print(f"Connected to {host}:{port} as {client_name}")
            s.sendall(f"{client_name}\n".encode('utf-8'))
            for log in logs:
                s.sendall(f"{log}\n".encode('utf-8'))
                print(f"Sent log: {log}")
            time.sleep(1)
    except Exception as e:
        print(f"Error in send_log: {e}")
        return False
    return True

def check_log_file(log_dir, client_name, expected_logs):
    log_file = os.path.join(log_dir, f"{client_name}.log")
    if not os.path.exists(log_file):
        print(f"Log file {log_file} not found")
        return False
    with open(log_file, 'r', encoding='utf-8') as f:
        content = f.read().strip()
    expected = f"[0001] ---=== Client {client_name} ===---\n" + \
               "\n".join(f"[000{i+2}] {log}" for i, log in enumerate(expected_logs)) + \
               f"\n[000{len(expected_logs)+2}] ^^^ ^^^ ^^^ Client stopped ^^^ ^^^ ^^^"
    if content == expected:
        print(f"Log file {log_file} matches expected content")
        return True
    else:
        print(f"Log file {log_file} does not match")
        print("Expected:")
        print(expected)
        print("Got:")
        print(content)
        return False

def test_single_client(server_dir):
    print("Testing single client...")
    server_path = os.path.join(server_dir, "build", "server.exe" if sys.platform == "win32" else "server")
    server = subprocess.Popen([server_path], cwd=server_dir)
    time.sleep(2)  # Даём серверу время на запуск
    logs = ["log record 1", "log record 2"]
    success = send_log(client_name="test_client", logs=logs)
    time.sleep(1)
    log_success = check_log_file(os.path.join(server_dir, "test"), "test_client", logs)
    # Останавливаем сервер
    if sys.platform == "win32":
        subprocess.run("echo stop & echo pass | build\\server.exe", cwd=server_dir, shell=True)
    else:
        subprocess.run("echo -e 'stop\npass' | ./build/server", cwd=server_dir, shell=True)
    server.terminate()
    time.sleep(1)  # Ensure server terminates
    return success and log_success

def test_multiple_clients(server_dir):
    print("Testing multiple clients...")
    server_path = os.path.join(server_dir, "build", "server.exe" if sys.platform == "win32" else "server")
    server = subprocess.Popen([server_path], cwd=server_dir)
    time.sleep(2)
    clients = [
        ("client1", ["log1", "log2"]),
        ("client2", ["logA", "logB"]),
    ]
    success = True
    for client_name, logs in clients:
        success &= send_log(client_name=client_name, logs=logs)
        time.sleep(1)
    for client_name, logs in clients:
        success &= check_log_file(os.path.join(server_dir, "test"), client_name, logs)
    if sys.platform == "win32":
        subprocess.run("echo stop & echo pass | build\\server.exe", cwd=server_dir, shell=True)
    else:
        subprocess.run("echo -e 'stop\npass' | ./build/server", cwd=server_dir, shell=True)
    server.terminate()
    time.sleep(1)
    return success

if __name__ == "__main__":
    print("Running test suite...")
    # Assume server_dir is the parent directory of the test script
    server_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    single_client_passed = test_single_client(server_dir)
    print(f"Single client test: {'PASSED' if single_client_passed else 'FAILED'}")
    multiple_clients_passed = test_multiple_clients(server_dir)
    print(f"Multiple clients test: {'PASSED' if multiple_clients_passed else 'FAILED'}")