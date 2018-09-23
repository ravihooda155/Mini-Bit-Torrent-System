#include<iostream>
#include<fstream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include<netdb.h> 
#include <string.h>
#include<bits/stdc++.h> 
using namespace std;
struct ClienTMesG
{
string CIAddr;
string  port;
string filename;
string shahash;
};
void displayCLIENT(struct ClienTMesG cliMsg)
{

                cout<<"client data->"<<endl;
                cout<<"Client IP->"<<cliMsg.CIAddr<<endl;
                cout<<"Client Port->"<<cliMsg.port<<endl;
                cout<<"Client Filename->"<<cliMsg.filename<<endl;
                cout<<"Client Hash->"<<cliMsg.shahash<<endl;

                
}
int main(int argc, char  *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0};     
    struct hostent *tracker1;

      vector<ClienTMesG> clientDetails;

       if (argc !=3)
     {
      cout<<"enter all parameters"<<endl;
    exit(1);
    }
    if((tracker1=gethostbyname(argv[1])) == NULL)
{
    perror("gethostbyname()");
    exit(1);
}
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr = *((struct in_addr *)tracker1->h_addr); 
    address.sin_port =  htons((unsigned short)strtoul(argv[2], NULL, 0));

    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
  

    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    else 
    {
      //  printf("Listening to Client\n");
        cout<<"Listening to Client"<<endl;
    }

while(1) 
{
  
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 


	    int num;
   
        if ((num=recv(new_socket, buffer,1024,0))== -1)
	    {
            perror("recv");
            exit(1);
        }   
        else if (num == 0)
         {
            printf("Connection Closed");
            cout<<"Connection Closed"<<endl;
            return 0;
        }
        buffer[num] = '\0';
      
      cout<<"DATA RECEIVED"<<endl;
     
       std::vector<std::string> tokens;
       tokens.clear();
        std::string token;
        token="";
        std::istringstream tokenStream(buffer);
        while (std::getline(tokenStream, token, '#'))
        {
            tokens.push_back(token);
        }
        
        if(tokens[0]=="share_file")
        {
                struct ClienTMesG cliMsg ;
                cliMsg.CIAddr=tokens[1];
                cliMsg.port=tokens[2];
                cliMsg.filename=tokens[3];
                cliMsg.shahash=tokens[4];
                clientDetails.push_back(cliMsg);
                displayCLIENT(cliMsg);
            
        }
        else if(tokens[0]=="get")
        {
            //cout<<"ip"<<tokens[1]<<endl;
            //cout<<"port"<<tokens[2]<<endl;
            //cout<<"sha"<<tokens[3]<<endl;
             string mesg="";
            string retClientIP;
            string retClientPort;
            string file_sha=tokens[1];
            bool flag=false;
            for(int i=0;i<clientDetails.size();i++)
            {
                if(clientDetails[i].shahash==file_sha)
                {
                 retClientIP=clientDetails[i].CIAddr;
                 retClientPort=clientDetails[i].port;
                 flag=true;
                 break;
                }
            }
            if(!flag)
            {
                cout<<"File not shared"<<endl;
                 send(new_socket , mesg.c_str(),mesg.length() , 0 );
               goto exitprogram;
            }
            //cout<<retClientIP<<retClientPort;
           
            mesg+=retClientIP;
            mesg+="-";
            mesg+=retClientPort;
             send(new_socket , mesg.c_str(),mesg.length() , 0 );

        }
        else if(tokens[0]=="remove")
        {
            int i;
             string mesg="";
            string file_sha=tokens[1];
            bool flag=false;
            for( i=0;i<clientDetails.size();i++)
            {
                if(clientDetails[i].shahash==file_sha)
                {
            
                 flag=true;
                 break;
                }
            }
            
            if(!flag)
            {
                cout<<"File not shared"<<endl;
                 send(new_socket , mesg.c_str(),mesg.length() , 0 );
                goto exitprogram;
            }
            else
            {
                clientDetails.erase(clientDetails.begin()+i);
            }
        
            mesg+="torrent entry removed";
             send(new_socket , mesg.c_str(),mesg.length() , 0 );
        }
        exitprogram:
        close(new_socket);
    }
close(server_fd);

    return 0; 

}
