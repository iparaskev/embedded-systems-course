Server
----------------------------------------------

1. Bind a port with a socket and open a new socket for listening.
2. Wait for new connections and for every new connection begin a new process/
   thread.
3. Receives address of the connected client.
4. Sends to to the connected client the unread messages that it has, if no 
   remaining messages sends a blank line.
5. Waits the response of the client. 
6. If the response is blank terminates connection
7. Else the first line is the address which the client want to send a message.
   Then it adds the message to the receiver's heap and terminates the connection.
##############################################
 
Client
----------------------------------------------

1. Connects to the server.
2. Sends his address to the server and waits for incoming messages. 
3. If the incoming messages is a blank line stops waiting for new.
4. If it has any message to send it sends it and terminates the connection. 
