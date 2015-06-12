/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <android/log.h>

int sendData( char *message)
{
    int sock;
    struct sockaddr_in server;
    char  server_reply[2000];
    //message[1000] ,
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
         __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI","Could not create socket");
    }
    __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 6000 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI","connect failed. Error");
        return 1;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI","Connected\n");

    //keep communicating with server
   // while(1)
    //{
        //printf("Enter message : ");
        //scanf("%s" , message);

        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI","Send failed");
            return 1;
        }

        //Receive a reply from the server
        //if( recv(sock , server_reply , 2000 , 0) < 0)
        //{
        //    __android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI","recv failed");
           // break;
        //}

       //__android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", "Server reply :");
       //__android_log_print(ANDROID_LOG_DEBUG, "LOG_JNI", server_reply);
    //}

    close(sock);
    return 0;
}