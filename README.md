# Server-Client
Server/Client implementation in C.

**For Linux:**

1. Enter the project the folder in a terminal and then enter the following command:
```
$ ./run.sh
```

**For Mac:**
1. Enter the project folder in a terminal and compile the server and clients file using these commands:
```
$ gcc server.c -o server
$ gcc clients.c -o clients
```
2. Grant guest, user and admin full access to the files by entering the following commands:
```
$ chmod 777 ./server
$ chmod 777 ./clients
```
3. Open 2 new terminals and enter the Server-Client folder in each one of them
4. Enter the following command into the first terminal:
```
$ ./server
```
5. Enter following command into the second terminal:
```
$ ./clients
```
5. Success???
