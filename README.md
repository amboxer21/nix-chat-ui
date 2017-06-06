[**Description**]

  A GTK2.xx chat client with a select server to handle that data. The server is written in the C programming language. It is LAN based chat at this point in time. All deveopment has been done on *nix with no thought of MS Windows in mind. 

  I have written a basic synchronous server to test the chat with which is included with the repo.

[**As of Sunday Aug 5th**]
- The select server
  Recieves data from the client the first time around and sends it out to all clients connected. Then after, it does nothing. In turn, the client produces a bad file descriptor error on the third time around. Indicating the server is closing the FD somewhere.

- The GTK_client
  Needs more functionality. Most importantly, it needs to take the data in the text entry buffer and send it to the server. I am unsure at this momnet of the exact steops that need to be taken in order to achieve this. I assume i would need to convert the standard GTK UTF-8 to binary. Also, the client must display sent, as well as recieved, data in the top textview/scrollable window. 

  Any ideas, feel free to add. 

[**As of Tuesday June 6th 2017**]
- I added the ability to send the typed text into the main view which is on the top. This is done once you press the send button and the typed text is also deleted upon sending as well.
- I added the ability to read incoming data from a socket on the main UI with threads.
