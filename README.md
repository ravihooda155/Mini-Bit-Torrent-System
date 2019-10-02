# Mini-Bit-Torrent-System
## Prerequisites
### openssl for SHA hashing
```
sudo apt-get install openssl
```

### G++ compiler
```
sudo apt-get install g++
```

## Compiling Project
### Navigate to tracker directory
```
make
```

### Navigate to client directory
```
make
```

## Project Execution
### Running Tracker
```
./tracker <tracker_ip> <tracker_port> <log_file>
eg : ./tracker 127.0.0.1 4000  logserver
```

### Running Client
```
./tracker <tracker_ip> <tracker_port> <log_file>
eg : ./client 127.0.0.1 6000  127.0.0.1 4000  client
```

## Functionalities
### - Share File On Network
```
 share <local_file_path> <filename>.mtorrent
 Eg : share  /home/ravihooda155/DownLoads/assign.pdf  assign.mtorrent.
 ```
 
### - File Downloading
```
get <path_to_.mtorrent_file> <dest_path>
 Eg : get assign.pdf.mtorrent Downld_1.pdf
```

### - Status Of Download
```
show_downloads
```

### - Stopping File Sharing
```
remove <filename.mtorrent>
Eg : remove assign.pdf.mtorrent
```

### - Shutdown Client
```
shutdown
```
