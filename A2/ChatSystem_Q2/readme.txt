Design a chat application that facilitates communication among multiple clients and a single
server. Utilize fork and exec commands along with shared memory to implement the
application. Ensure that the chat application meets the following requirements:
Client Features
• Clients can communicate with each other through the server.
• Clients can create group chats.
• Clients can choose the chat where they want to send messages.
• Clients can communicate privately with other users.
• Clients that create a group chat can also choose the other clients that they want to be added in
the chat. And then make a request to the server.

Server Features:
• The server takes the number of clients as input and opens a new terminal for each client.
// Use exec command for opening multiple terminals from one terminal.
• Each terminal displays the chat information of the respective client.
• The server manages group chat requests from clients.

• The server ensures that clients not belonging to a group cannot send or receive messages from
that group.

Communication Mechanism:
• Shared memory is used for communication between clients.
• Clients send messages to the server, which forwards them to the intended recipient(s).
Implementation Guidelines:
• Use forked processes for managing client-server communication.
• You can define custom data structures for representing messages, group information, etc.
Additional Notes:
• Clients do not need server confirmation to receive messages.
• The server must handle client requests to create groups and manage group membership
securely.
• Store all the messages of the chat in shared memory. When a new user sends a message it is
appended to the previous chat which would be displayed to clients all the time.
• System should ask the user every time whether to read the chat or send the message.
• Consider implementing error handling and validation mechanisms to enhance application
robustness.
• Ensure proper cleanup of resources after client disconnection or termination.
