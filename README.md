# Windows Server & Client application for transmitting data<br />
Access via console (which is automatically generated with default/test parameters if not launched throught cmd)<br /><br />
# Usage:
1) Launch the server (no parameters for now)
2) Launch the client (1 parameter - ip, defaults to localhost or "getkey" for new Master key in text form)
3) Type in command and its parameters (on the client side)

# Commands: <br />
- R FileName.extension //reads specified file from Server and sends it to Client
- W FileName.extension //reads specified file from Client and sends it to Server
- @echo test //@ followed by windows command that will be executed on the Server
- E //exit communication with server and Shutdown Client
- B //Shutdown the Server and Client

# Compilation:
- `cd Win_Server-Client`
- `mkdir build`
- `cd build`
- `cmake ..`

# Current shortcomings:
1) no ability to specify the folder to store data(file/folder) retrieved from the server
2) no ability to easily browse data outside of server folder
3) no data streaming capabilities
4) no GUI

# Security:
I had fun, it's not secure.<br />
Both Client and Server are enrypting sensetive data such as Commands and Files/Folders with randomly generated key the size of the message itself (up to 256 bytes), the random key is included into the message while encrypted with Symmetric Master Key (the copy of which is hardcoded into both parties).<br />
Data will be encrypted (besides 1 byte operation status packets) but that still leaves possibility for MITM attack as you can simply copy old encrypted requests for server to perform certain action.<br />
Encryption itself is probaly silly as I don't know much about cryptography<br />
//in this case '^' means "xor data with key and key << 256"<br />
//         [(1      + 255 )      + 256      ] = 512<br />
//packet - [(header + data)^keyR + keyR^keyM]<br />
At least it's more fun than blindly copying AES or smth :)
