# Minimal Networking Reference (Simple Server Only)

This document summarizes everything you implemented for the **server portion** of your blockchain project. It includes:

* The exact functions you used
* What each function does
* Which header/library it comes from
* The overall flow and where the server fits compared to your blockchain loop

Use this as a quick reference later when extending into **P2P networking**.

---

## 🔌 1. Headers / Libraries Used

### **`<sys/types.h>`**

* Base system types (sockets on Linux/Mac). *On Windows this is included through WinSock.*

### **`<sys/socket.h>`**  *(Linux/Mac)*

* Core socket functions: `socket()`, `bind()`, `listen()`, `accept()`.
* **On Windows**, these come from **WinSock (winsock2.h)**.

### **`<netinet/in.h>`**  *(Linux/Mac)*

* `sockaddr_in` structure (IP + port).
* Constants like `AF_INET` and `INADDR_ANY`.

### **`<unistd.h>`** *(Linux/Mac)*

* `close()` for closing sockets.
* On Windows use `closesocket()`.

### **`<winsock2.h>`**  *(Windows)*

* Windows socket API.
* Provides all networking functions.
* Must call:

  ```cpp
  WSAStartup(MAKEWORD(2, 2), &wsaData);
  ```

  at program start.

### **`<ws2tcpip.h>`** *(Windows)*

* Allows IPv4/IPv6 compatibility.

---

## 🧱 2. Core Networking Functions You Used

Below is a list of every function involved in creating your basic server.

### ### **1. `WSAStartup()`** *(Windows only)*

Initializes the WinSock library. Required **before** any socket functions.

### **2. `socket()`**

Creates a new socket.

* Parameters:

  * `AF_INET` → IPv4
  * `SOCK_STREAM` → TCP
* Returns a socket descriptor.

### **3. `bind()`**

Attaches your socket to:

* An IP (`INADDR_ANY` → all local interfaces)
* A port (example: `5000`)

Uses the struct:

```cpp
sockaddr_in serverAddr;
```

Fields:

* `serverAddr.sin_family = AF_INET;`
* `serverAddr.sin_addr.s_addr = INADDR_ANY;`
* `serverAddr.sin_port = htons(5000);`

### **4. `listen()`**

Tells the OS your socket will accept incoming connections.

* You set backlog = `5`.

### **5. `accept()`**

Waits until a client connects.

* When someone connects, returns a **new socket** dedicated for talking to that client.

### **6. `send()`**

Sends data to the connected client.

* Example:

  ```cpp
  send(clientSocket, "Hello from server!", 19, 0);
  ```

### **7. `recv()`** *(not used yet, but next step)*

Receives data from client.

### **8. `closesocket()` / `close()`**

Shuts down sockets.

* Windows: `closesocket()`
* Linux/Mac: `close()`

### **9. `WSACleanup()`** *(Windows)*

Shuts down WinSock.

---

## ⚙️ 3. Where the Server Code Goes in Your File

You placed it correctly:

### **1. At the very top:**

* Include headers
* Initialize WinSock with `WSAStartup()`

### **2. After your blockchain class definitions**

* Setup server socket
* Bind → listen

### **3. Inside the infinite blockchain UI loop:**

You placed:

```cpp
int clientSocket = accept(serverSocket, ...);
if (clientSocket >= 0) {
    send(clientSocket, "Connected to blockchain server", ...);
    closesocket(clientSocket);
}
```

This is fine because:

* Your server continuously runs
* Accepts new connections
* Sends response
* Goes back to the blockchain loop

---

## 🗺️ 4. High-Level Flow Summary

```
START PROGRAM
│
├── Initialize WinSock (Windows)
│
├── Create socket()
│
├── Bind socket to port 5000
│
├── Listen for connections
│
└── Enter blockchain main loop
    ├── Handle SHA256, add blocks, verify chain, etc.
    ├── Check if a client connected → accept()
    ├── Send a message ("Connected to blockchain server")
    └── Continue loop
```

---

## 📌 5. Testing Your Server

### **Windows PowerShell / CMD**

```
tnc localhost -port 5000
```

If connection succeeds → your server is working.

### **Linux/Mac**

```
telnet localhost 5000
```

or

```
nc localhost 5000
```

---

## 🧩 6. What Comes Next (for P2P)

When we extend this into peer‑to‑peer networking, you will add:

* `recv()` to listen for data
* A message parser
* A struct like:

  ```cpp
  struct NetworkMessage {
      std::string command;
      std::string payload;
  };
  ```
* A loop dedicated to accepting multiple peers
* Client code (`connect()`)
* A simple protocol: NEW_BLOCK, REQUEST_CHAIN, SEND_CHAIN

But this document is **only for the base server**.

---

If you want I can add diagrams, code snippets, or a section that explains the WinSock lifecycle in a simpler way.
