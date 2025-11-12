# ⚙️ TCP-Based RPC Calculator (C Implementation)

This project implements a **simple Remote Procedure Call (RPC)** calculator using **TCP socket programming in C**.  
It demonstrates how a client can invoke arithmetic operations (addition, subtraction, multiplication, division) remotely on a server.

---

## 🧠 Concept

- **TCP (Transmission Control Protocol)** provides reliable, ordered, and error-checked data transmission between two endpoints.
- **RPC (Remote Procedure Call)** allows a client to request that a procedure (function) be executed remotely on the server.

Here, the client sends an operation request (e.g., `5 * 3`) to the server, which performs the computation and returns the result.

---

## 🏗️ Components

| File | Description |
|------|--------------|
| `server.c` | Creates a TCP server that waits for client requests, performs arithmetic operations, and returns results. |
| `client.c` | Connects to the server, sends numbers and an operator, and displays the result returned by the server. |

---

