#include<iostream>
#include<bits/stdc++.h>
#include<fstream>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include<unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include<openssl/sha.h>
#include<netdb.h>
#include <cstdlib>
using namespace std;

int sha1(  char * name, unsigned char * out )
{
    FILE * pf;
    unsigned char buf[ 512 ];
    SHA_CTX ctxt;
    SHA1_Init( &ctxt );
     SHA1_Update( &ctxt, name, 512 );
    SHA1_Final( out, &ctxt );

    return 0;
}
void bin2hex( unsigned char * src, int len, char * hex )
{
    int i, j;

    for( i = 0, j = 0; i < len; i++, j+=2 )
        sprintf( &hex[j], "%02x", src[i] );
}
string generateSHA(string filename)
{
    ifstream file(filename.c_str(), ifstream::binary);

  if (not file)
    {
      cerr << "file: " << filename << " failed to open" << endl;
      return NULL;
    }

  struct stat filestatus;
  stat(filename.c_str(), &filestatus);

  size_t total_size = filestatus.st_size;
  size_t chunk_size = 0;

  if (not (chunk_size > 0))
    { chunk_size = 512 * 1024; }
 // cout << "using chunk size: " << chunk_size << endl;

  size_t total_chunks = total_size / chunk_size;
  size_t last_chunk_size = total_size % chunk_size;

  if (last_chunk_size != 0) 
    {
      ++total_chunks;
    }
  else
    {
      last_chunk_size = chunk_size;
    }
    string SHA1FILE="";
  for (size_t chunk = 0; chunk < total_chunks; ++chunk)
    {
      	size_t this_chunk_size = chunk == total_chunks - 1 ? last_chunk_size: chunk_size;
      	char chunk_data[this_chunk_size];
     	file.read(&chunk_data[0],this_chunk_size); 
        unsigned char digest[ SHA_DIGEST_LENGTH ];
        char str[ (SHA_DIGEST_LENGTH * 2) + 1 ];
        if( sha1( chunk_data, digest ) )
        {
            printf("Error!\n");
            return NULL;
        }
        bin2hex( digest, sizeof(digest), str );
         SHA1FILE=SHA1FILE+str ;

    }

return SHA1FILE;
}
string createfile(string src,string filename)
{
    struct stat buf;
    string filleDetail;
    int filesize;
	ofstream ft;
	char ch;
	ft.open(src);
	if(!ft)
	{
		cout<<"Error in opening  file..!";
		getchar();
		exit(2);
	}

    filleDetail+=src+"#";

    stat(filename.c_str(),&buf);
    filesize=buf.st_size;
    string SHAFILE=generateSHA(filename);

    filleDetail+=SHAFILE;
    ft<<"Tracker1"<<":\n";
    ft<<"Tracker1"<<":\n";
    ft<<"FileSize:"<<filesize<<"\n";
    ft<<"SHA:"<<SHAFILE;
    
    ft.close();
    return filleDetail;
}
int main(int argc, char  *argv[]) 
{ 
    
    int sock = 0,new_socket,client_d; 
    struct sockaddr_in tracker_addr; 
     struct sockaddr_in cli_addr; 
    int addrlen = sizeof(cli_addr); 
    struct hostent*tracker1;
    struct hostent*client;
    string  mesg ;
    char buffer[1024]={0};
    int choice;
    //////////////////client ///////////////////////////////
    
     
    memset(&cli_addr, '0', sizeof(cli_addr));  
    cli_addr.sin_family = AF_INET; 
    cli_addr.sin_addr.s_addr = inet_addr(argv[1]); 
    cli_addr.sin_port =  htons((unsigned short)strtoul(argv[2], NULL, 0)); 
    ////////////////////////////////////////////////////
   

    //////////////////tracker1 connection/////////////////////////////////
     if((tracker1=gethostbyname(argv[3])) == NULL)
    {
        perror("gethostbyname()");
        exit(1);
    }  
   
    memset(&tracker_addr, '0', sizeof(tracker_addr));  
    tracker_addr.sin_family = AF_INET; 
    tracker_addr.sin_addr = *((struct in_addr *)tracker1->h_addr); 
    tracker_addr.sin_port =  htons((unsigned short)strtoul(argv[4], NULL, 0)); 
    ////////////////////////////////////////////////////////////////////////////


    if ((client_d = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (bind(client_d, (struct sockaddr *)&cli_addr,sizeof(cli_addr))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
  
       if (listen(client_d, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
while(1) 
{           
            string str;
            cout<<"Enter Command:";
            cout<<"1.share";
           getline(cin,str);
			vector<string> tokens;
			tokens.clear();
			stringstream check1(str);
			string rawcode;
			while(getline(check1, rawcode, ' '))
			{
				tokens.push_back(rawcode);
			}	
           if(tokens[0]=="share")
            {
                            cout<<"share portal";
                            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                            { 
                            cout<<"\n Socket creation error \n"; 
                                return -1; 
                            }    
                            if (connect(sock, (struct sockaddr *)&tracker_addr, sizeof(tracker_addr)) < 0) 
                            { 
                            cout<<"\nConnection Failed \n"; 
                                return -1; 
                            } 
                            string filepath=tokens[1];
                            string filetorrpath;
                            string basepath;
                            basepath=getcwd(buffer, 256);
                            filetorrpath=basepath+"/"+tokens[2];
                            mesg+="share_file";
                            mesg+="#";
                            mesg+=(argv[1]);
                           // mesg.append(argv[1]);
                            mesg+="#";
                            mesg+=(argv[2]);
                            //mesg.append(argv[2]);
                            mesg+="#";
                            string retmesg;
                            
                             retmesg=createfile(filetorrpath,filepath);
                            
                            mesg+=retmesg;
                            send(sock , mesg.c_str(),mesg.length() , 0 ); 
                            cout<<"File created\n"; 
                            close(sock);
                            
                }   
                
            }
  close(client_d);
    return 0; 
} 
 