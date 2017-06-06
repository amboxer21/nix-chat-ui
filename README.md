## [**Description**]

> This is a GTK2 chat client with a select server used to handle the data. The server is written in the C programming language as well as the rest of the code in this repo. It is LAN based chat at this point in time and all development has been done on Linux with no thought of MS Windows in mind. 
<br><br/>
---
## [**How do I run this program?**]

**Brief introduction**

`You must first run the server and this is done from the command line at the moment. You can either use the fully functional synchronous server or the select server which could potentially have bugs. Then you can run the GTK client after the server is up and running.`

| Program       | IP Address    | Port  |
| ------------- |:-------------:| -----:|
| ./server      | 127.0.01      | 1234  |
| ./gtk_client  | 127.0.0.1     | 1234  |

[**As of Sunday August 5th 2012**]
- The select server
  Recieves data from the client the first time around and sends it out to all clients connected. Then after, it does nothing. In turn, the client produces a bad file descriptor error on the third time around. Indicating the server is closing the FD somewhere.

- The GTK_client
  Needs more functionality. Most importantly, it needs to take the data in the text entry buffer and send it to the server. I am unsure at this moment of the exact steps that need to be taken in order to achieve this. I assume I would need to convert the standard GTK UTF-8 to binary. Also, the client must display sent, as well as recieved, data in the top textview/scrollable window.  

[**As of Tuesday June 6th 2017**]
- I added the ability to send the typed text into the main view which is on the top. This is done once you press the send button and the typed text is also deleted upon sending as well.
- I added the ability to read incoming data from a socket on the main UI with threads. 
<br><br/>
<br><br/>

**Any ideas, feel free to send an E-mail at amboxer21@gmail.com or submit a pull request.**
