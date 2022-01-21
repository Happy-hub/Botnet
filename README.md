# Botnet
Simple botnet implemented in Python and C++

__Warning: Misuse of this software can raise legal and ethical issues which I don't support nor can be held responsible for.__

# Running the botnet
To run the botnet, you first have to compile and create an executable of the client:
```
g++ BotnetClient.cpp -o client.exe -fpermissive
```
Then, run the server using the command:
```
python BotnetServer.py
```
Now just run the executable of the client, and it should connect to the server.

