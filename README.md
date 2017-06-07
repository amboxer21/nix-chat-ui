## [**Description**]

> This is a GTK2 chat client with a select server used to handle the data. The server is written in the C programming language as well as the rest of the code in this repo. It is LAN based chat at this point in time and all development has been done on Linux with no thought of MS Windows in mind. 

<br><br/>
## [**How do I run this program?**]

**Briefing**

> You must first run the server and this is done from the command line at the moment. You can either use the fully functional synchronous server or the select server which could potentially have bugs or might not work the way you need it to. Then you can run the GTK client after the server is up and running.

**Examples:**

| Program       | IP Address    | Port  |
| ------------- |:-------------:| -----:|
| ./server      | 127.0.01      | 1234  |
| ./gtk_client  | 127.0.0.1     | 1234  |

<br><br/>
## [**How to compile the GTK application**]

**Briefing**

> In order to successfully compile a C programming GTK2 application on Linux you must append the following to the end of your GCC command line compilation args: **\`pkg-config --cflags --libs gtk+-2.0\`**

**Example**

| **Compilation Args (With the assumption that the file is named gtk_client.c)       |
| ------------------------- |
| [anthony@ghost nix-chat-ui]$ gcc gtk_client.c -o gtk_client \`pkg-config --cflags --libs gtk+-2.0\` |

<br><br/>
## [**Changes**]
**As of Sunday August 5th 2012**
- The select server
  Recieves data from the client the first time around and sends it out to all clients connected. Then after, it does nothing. In turn, the client produces a bad file descriptor error on the third time around. Indicating the server is closing the FD somewhere.

- The GTK_client
  Needs more functionality. Most importantly, it needs to take the data in the text entry buffer and send it to the server. I am unsure at this moment of the exact steps that need to be taken in order to achieve this. I assume I would need to convert the standard GTK UTF-8 to binary. Also, the client must display sent, as well as recieved, data in the top textview/scrollable window.  
  
<br><br/>
**As of Tuesday June 6th 2017** 
- I added the ability to send the typed text into the main view which is on the top. This is done once you press the send button and the typed text is also deleted upon sending as well.
- I added the ability to read incoming data from a socket on the main UI with threads. 

## [**Images**]

![alt text][logo]

[logo]: https://github.com/amboxer21/nix-chat-ui/tree/master/pics/gtk_client.png "Main UI"

<br><br/>
<br><br/>
**Any ideas, feel free to send an E-mail at amboxer21@gmail.com or submit a pull request.**
