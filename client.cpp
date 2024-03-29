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
#include<pthread.h>
#include <ctime>
#include<dirent.h>
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress (double percentage)
{
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}
const time_t ctt = time(0);
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
string createfile(string src,string filename,string trackerIP,string trackerPort)
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

    filleDetail+=filename+"#";

    stat(filename.c_str(),&buf);
    filesize=buf.st_size;
    string SHAFILE=generateSHA(filename);

    filleDetail+=SHAFILE;
    ft<<"Tracker1="<<trackerIP<<endl;
    ft<<"Tracker1Port="<<trackerPort<<endl;
     ft<<"Filename="<<filename<<endl;
    ft<<"FileSize="<<filesize<<endl;
    ft<<"SHA="<<SHAFILE;
    
    ft.close();
    return filleDetail;
}

void serv_peer(int* new_socket)
{
    char buffer[1024]={0};
    int num=recv(*new_socket, buffer,1024,0);
   
        if (num== -1)
	    {
            perror("recv");
            exit(1);
        }   
        else if (num == 0)
         {
            printf("Connection Closed");
            //return 0;
        }
        
        buffer[num] = '\0';
    
        cout<<buffer<<endl;
        ///////////////////sending file
        ////////////////////////////////////////////////

  struct stat filestatus;
  stat(buffer, &filestatus);

  size_t total_size = filestatus.st_size;
  double percentage;
        char sendData[512*1024];int b=0;int total=0;
         FILE *fp = fopen(buffer, "rb");
          if(fp == NULL)
          {
             perror("File");
            }
            cout<<"Uploading......."<<endl;
             while( (b = fread(sendData, 1, sizeof(sendData), fp))>0 )
             {
                 send(*new_socket, sendData, b, 0);
                 total=total+b;

                 percentage=(double)total/total_size;
                    printProgress(percentage);
                           cout<<endl;        
             //  cout<<"Uploading-->"<<percentage*100<<"%"<<endl;
            }

         close(*new_socket);
           fclose(fp);
        //////////////////////////////////////////////////////////
}
void listenprocess(string cliIP,string port)
{
    int client_d,new_socket;
    struct sockaddr_in cli_addr;
     char buffer[1024]={0};

    memset(&cli_addr, '0', sizeof(cli_addr));  
    cli_addr.sin_family = AF_INET; 
    cli_addr.sin_addr.s_addr = inet_addr(cliIP.c_str()); 
    cli_addr.sin_port =  htons((unsigned short)strtoul(port.c_str(), NULL, 0)); 
    int addrlen = sizeof(cli_addr); 

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
  
    if ((new_socket = accept(client_d, (struct sockaddr *)&cli_addr,(socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 


 std::thread t(serv_peer,&new_socket);
t.detach();
/*	    int num=recv(new_socket, buffer,1024,0);
   
        if (num== -1)
	    {
            perror("recv");
            exit(1);
        }   
        else if (num == 0)
         {
            printf("Connection Closed");
            //return 0;
        }
        
        buffer[num] = '\0';
    
        cout<<buffer<<endl;*/
}
 
}
void receivingfile(int* new_socket,string destPath,string filesize)
{
                             char buffe[1024*512];
                             double percentage;
                             size_t size=stoi(filesize);
                            FILE* fp = fopen( destPath.c_str(), "wb");
                            int totByte=0;
                            int b;
                            cout<<"Downloading......."<<endl;
                            if(fp != NULL){
                            while( (b = recv(*new_socket, buffe, 512*1024,0))> 0 ) 
                            {
                                totByte+=b;
                                fwrite(buffe, 1, b, fp);
                                percentage=(double)totByte/size;
                           printProgress(percentage);
                           cout<<endl;
                               //cout<<"Downloading-->"<<percentage*100<<"%"<<endl;
                            }

                                cout<<"Byte Received"<<totByte<<endl;
                                if (b<0)
                                perror("Receiving");

                                fclose(fp);
                            } 
                            else
                             {
                                perror("File");
                            }
                            close(*new_socket);

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
     char logfilePath[256];
     realpath(argv[5], logfilePath);
    ofstream logfile;
    logfile.open(logfilePath,std::ios_base::app);
      logfile<<"------------------------------------------------------------"<<endl;
       logfile << asctime(localtime(&ctt)); 
        logfile<<"------------------------------------------------------------"<<endl;
    if (argc !=6)
     {
         logfile << asctime(localtime(&ctt)) <<"####"<<"enter all parameters in arguments"<<endl;
      cout<<"enter all parameters"<<endl;
    exit(1);
    }
   
    
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

 std::thread t(listenprocess,argv[1],argv[2]);

while(1) 
{           
            string str;
            cout<<"Enter Command:";
            getline(cin,str);
			vector<string> tokens;
			tokens.clear();
			stringstream check1(str);
			string rawcode;
			while(getline(check1, rawcode, ' '))
			{
				tokens.push_back(rawcode);
			}	
            if(tokens.size()==3)
            {
           if(tokens[0]=="share")
            {
                            
                             logfile << asctime(localtime(&ctt))<<"####"<<"Inside Share Portal"<<endl; 
                            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                            { 
                            cout<<"\n Socket creation error \n"; 
                            logfile << asctime(localtime(&ctt)) <<"####"<<"Socket creation error ";
                                return -1; 
                            }    
                            if (connect(sock, (struct sockaddr *)&tracker_addr, sizeof(tracker_addr)) < 0) 
                            { 
                            cout<<"\nConnection Failed \n"; 
                             logfile << asctime(localtime(&ctt)) <<"####"<<"Connection Failed "; 
                                return -1; 
                            } 
                          //  string filepath=tokens[1];
                            logfile<<asctime(localtime(&ctt))<<"####"<<"Connected to Tracker"<<endl; 
                          char  filepath[256];
                          realpath((tokens[1]).c_str(), filepath);
                            string filetorrpath;
                            string basepath;
                            basepath=getcwd(buffer, 256);
                            filetorrpath=basepath+"/"+tokens[2];
                            mesg="";
                            mesg+="share_file";
                            mesg+="#";
                            mesg+=(argv[1]);
                            mesg+="#";
                            mesg+=(argv[2]);
                            mesg+="#";
                            string retmesg;
                            
                             retmesg=createfile(filetorrpath,filepath,argv[3],argv[4]);
                            
                            mesg+=retmesg;
                            send(sock , mesg.c_str(),mesg.length() , 0 ); 
                            
                            cout<<"Torrent File created\n"; 
                             logfile << asctime(localtime(&ctt))<<"####"<<"Torrent File created"<<endl; 
                            close(sock);
                            
                             logfile << asctime(localtime(&ctt))<<"####"<<" Share Portal Closed"<<endl; 
                }  
                else if(tokens[0]=="get")
                {
                             
                             logfile << asctime(localtime(&ctt))<<"####"<<"Inside Get Portal"<<endl; 
                              char  torrentpath[256];
                             realpath((tokens[1]).c_str(), torrentpath);
                             string destPath=tokens[2];

                             struct sockaddr_in tracke11_addr; 
                             struct sockaddr_in peer_addr;
                            ifstream input;
                            input.open(torrentpath);
                            string output;
                            while(input)
                            {
                                string line;
                                getline(input,line,'=');
                                string val;
                                input>>val;
                                output+=val+"#";
                                //cout<<line<<"-----"<<val;
                            }
                            input.close();
                            std::vector<std::string> tokens;
                             tokens.clear();
                             std::string token;
                             token="";
                              std::istringstream tokenStream(output);
                            while (std::getline(tokenStream, token, '#'))
                           {
                            tokens.push_back(token);
                            }
                            string tracker_IP=tokens[0];
                            string tracker_Port=tokens[1];
                            string fileSourcePath=tokens[2];
                            string filesize=tokens[3];
                            string file_Sha=tokens[4];

                          // cout<<file_Sha<<tracker_IP<<tracker_Port;

                            memset(&tracke11_addr, '0', sizeof(tracke11_addr));  
                            tracke11_addr.sin_family = AF_INET; 
                            tracke11_addr.sin_addr.s_addr = inet_addr(tracker_IP.c_str()); 
                            tracke11_addr.sin_port =   htons((unsigned short)strtoul(tracker_Port.c_str(), NULL, 0)); 

                            ////////////////////request to tracker/////////////////////////////
                            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                            { 
                            cout<<"Socket creation error\n"; 
                              logfile << asctime(localtime(&ctt))<<"####";
                            logfile<<"Socket creation error"<<endl; 
                                return -1; 
                            }    
                            if (connect(sock, (struct sockaddr *)&tracke11_addr, sizeof(tracke11_addr)) < 0) 
                            { 
                            cout<<"\nConnection Failed \n"; 
                             logfile << asctime(localtime(&ctt))<<"####"<<"\nConnection Failed \n"; 
                                return -1; 
                            } 
                              logfile << asctime(localtime(&ctt)) <<"####"<<"Connected to Tracker"<<endl; 
                            mesg="";
                            mesg+="get";
                          
                            mesg+="#";
                            mesg+=(file_Sha);
                            
                            send(sock , mesg.c_str(),mesg.length() , 0 ); 
                            cout<<"Send to tracker\n"; 
                             logfile << asctime(localtime(&ctt))<<"####"<<"File extraction details send to tracker"<<endl; 
                            /////////////////////////peer details////////////////////////////
                             int num;
                            
                            if ((num=recv(sock, buffer,1024,0))== -1)
	                          {
                               perror("recv");
                                exit(1);
                               }   
                            else if (num == 0)
                            {
                             cout<<"Connection Closed\n";
                              logfile << asctime(localtime(&ctt))<<"####"<<"Connection Closed\n";
                             exit(1);
                             }
                               logfile << asctime(localtime(&ctt)) <<"####"<<"Peer Details Received From Tracker"<<endl; 
                             buffer[num] = '\0';
                             // std::vector<std::string> tokens;
                             tokens.clear();
                            // std::string token;
                             token="";
                             string buff(buffer);
                              std::istringstream tokenStream1(buff);
                            while (std::getline(tokenStream1, token, '-'))
                           {
                            tokens.push_back(token);
                            }
                            string peer_IP=tokens[0];
                            string peer_PORT=tokens[1];
                             cout<<peer_IP<<peer_PORT;
                             cout<<"peer details received"<<endl;
                            close(sock);

                                /////////////////connecting to peer //////////////////////////
                          
                             memset(&peer_addr, '0', sizeof(peer_addr));  
                            peer_addr.sin_family = AF_INET; 
                            peer_addr.sin_addr.s_addr = inet_addr(peer_IP.c_str()); 
                            peer_addr.sin_port =   htons((unsigned short)strtoul(peer_PORT.c_str(), NULL, 0));
                                    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                            { 
                            cout<<"\n Socket creation error \n"; 
                             logfile << asctime(localtime(&ctt))<<"####"<<"\n Socket creation error \n"; 
                                return -1; 
                            }    
                            if (connect(sock, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) < 0) 
                            { 
                            cout<<"\nConnection Failed \n"; 
                             logfile << asctime(localtime(&ctt))<<"####"<<"\nConnection Failed \n"; 
                                return -1; 
                            }  
                             mesg="";
                            mesg+=fileSourcePath;
                           
                            send(sock , mesg.c_str(),mesg.length() , 0 ); 
                            cout<<"Send to peer"<<endl;
                             logfile << asctime(localtime(&ctt))<<"####"<< "Sending File  to Peer"<<endl;

                            ////////
                           /////////////////////////// receiving file//////////////////////////////

                            
                             std::thread t(receivingfile,&sock,destPath,filesize);
                             t.detach();
                            ///////////////
                          /*  char buffe[1025];
                            FILE* fp = fopen( destPath.c_str(), "wb");
                            int totByte=0;int b;
                            if(fp != NULL){
                            while( (b = recv(sock, buffe, 1024,0))> 0 ) 
                            {
                                totByte+=b;
                                fwrite(buffe, 1, b, fp);
                            }

                                cout<<"Byte Received"<<totByte<<endl;
                                if (b<0)
                                perror("Receiving");

                                fclose(fp);
                            } 
                            else
                             {
                                perror("File");
                            }
                            /////////////
                            close(sock);*/
                            
                             logfile << asctime(localtime(&ctt))<<"####"<<" Get Portal closed"<<endl; 
                }
                else
                {
                    cout<<"command not found"<<endl;
                }
            }
            else if(tokens.size()==2)
            {
                if(tokens[0]=="remove")
                {

                             logfile << asctime(localtime(&ctt))<<"####"<<"Inside Remove Portal"<<endl; 
                              char  torrentpath[256];
                             realpath((tokens[1]).c_str(), torrentpath);
                             struct sockaddr_in tracke11_addr; 
                             struct sockaddr_in peer_addr;
                            ifstream input;
                            input.open(torrentpath);
                            string output;
                            while(input)
                            {
                                string line;
                                getline(input,line,'=');
                                string val;
                                input>>val;
                                output+=val+"#";
                                //cout<<line<<"-----"<<val;
                            }
                            input.close();
                            std::vector<std::string> tokens;
                             tokens.clear();
                             std::string token;
                             token="";
                              std::istringstream tokenStream(output);
                            while (std::getline(tokenStream, token, '#'))
                           {
                            tokens.push_back(token);
                            }
                            string tracker_IP=tokens[0];
                            string tracker_Port=tokens[1];
                            string fileSourcePath=tokens[2];
                            string file_Sha=tokens[4];

                          // cout<<file_Sha<<tracker_IP<<tracker_Port;

                               logfile << asctime(localtime(&ctt)) <<":"<<"Connecting To Tracker"<<endl; 
                            memset(&tracke11_addr, '0', sizeof(tracke11_addr));  
                            tracke11_addr.sin_family = AF_INET; 
                            tracke11_addr.sin_addr.s_addr = inet_addr(tracker_IP.c_str()); 
                            tracke11_addr.sin_port =   htons((unsigned short)strtoul(tracker_Port.c_str(), NULL, 0)); 

                            ////////////////////request to tracker/////////////////////////////
                            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                            { 
                            cout<<"\n Socket creation error \n"; 
                             logfile << asctime(localtime(&ctt)) <<"####"<<"\n Socket creation error \n"; 
                                return -1; 
                            }    
                            
                            if (connect(sock, (struct sockaddr *)&tracke11_addr, sizeof(tracke11_addr)) < 0) 
                            { 
                            cout<<"\nConnection Failed \n"; 
                             logfile << asctime(localtime(&ctt)) <<"####"<<"\nConnection Failed \n"; 
                                return -1; 
                            } 

                               logfile << asctime(localtime(&ctt)) <<"####"<<"Connected To Tracker"<<endl; 
                            mesg="";
                            mesg+="remove";
                          
                            mesg+="#";
                            mesg+=(file_Sha);
                            
                            send(sock , mesg.c_str(),mesg.length() , 0 ); 
                            cout<<"Send to tracker\n"; 
                             logfile << asctime(localtime(&ctt)) <<"####"<<"File Details send to tracker"<<endl; 
                            /////////////////////////peer details////////////////////////////
                             int num;
                            
                            if ((num=recv(sock, buffer,1024,0))== -1)
	                          {
                               perror("recv");
                                exit(1);
                               }   
                            else if (num == 0)
                            {
                             cout<<"Connection Closed\n";
                              logfile << asctime(localtime(&ctt)) <<"####";
                             logfile<<"Connection Closed\n";
                             exit(1);
                             }
                             buffer[num] = '\0';
                             cout<<buffer<<endl;

                               logfile << asctime(localtime(&ctt)) <<"####"<<buffer<<endl; 
                             close(sock);

                             logfile << asctime(localtime(&ctt))<<"####"<<"Remove Portal Closed"<<endl; 
                }
                else  if(tokens[0]=="show")
                {
                     logfile << asctime(localtime(&ctt))<<"####"<<"Inside Show Portal"<<endl; 
                    char  dirname[256];
                    realpath((tokens[1]).c_str(), dirname);
                    DIR *dir;
                	dirent *pdir;
                    vector<string>DirectryList;
                    dir=opendir(dirname);
                    while((pdir=readdir(dir))!=NULL)
                    {	string filename((pdir->d_name));
                        DirectryList.push_back(filename);
                    }
                    for(int i=0;i<DirectryList.size();i++)
                    {
                        if(DirectryList[i]!="."&&DirectryList[i]!="..")
                        cout<<DirectryList[i]<<endl;
                    }
                     logfile << asctime(localtime(&ctt))<<"####"<<" Show Portal Closed"<<endl; 

                }
                else
                {
                    cout<<"command not found"<<endl;
                }
            }
            else
            {
                cout<<"command not found"<<endl;
            }
            }
  close(client_d);
  logfile.close();
    return 0; 
} 
 