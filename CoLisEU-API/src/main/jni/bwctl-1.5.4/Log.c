

extern char* tmp_dir;

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <android/log.h>

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

char *output_file = "/output.jni";
extern char* glob_var;

char* read_file(char* file_path){
    char* file_contents;
    long input_file_size;
    FILE *input_file = fopen(file_path, "rb");

    fseek(input_file, 0, SEEK_END);
    input_file_size = ftell(input_file);
    rewind(input_file);

    file_contents = malloc(input_file_size * (sizeof(char)));
    fread(file_contents, sizeof(char), input_file_size, input_file);
    fclose(input_file);

    return file_contents;
}


int send_to_socket(char* output_tex)
{
    int sock;
    struct sockaddr_in server;
    //char message[1000] , server_reply[2000];
    char server_reply[20];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
        return 1;
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //keep communicating with server
    //while(1)
    //{
        //printf("Enter message : ");
        //scanf("%s" , message);

        //Send some data
        if( send(sock , output_tex , strlen(output_tex) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        close(sock);
        return 0;

    //}


}

void log_to_file(char* output_tex){
    //send_to_socket(output_tex);
    __android_log_print(ANDROID_LOG_DEBUG, "LOG_TO_FILE", "%s", output_tex);

    strcat(glob_var, output_tex);

    /*FILE *output_jni;
    char *output_path = (char *) malloc(1 + strlen(tmp_dir)+ strlen(output_file) );
    strcpy(output_path, tmp_dir);
    strcat(output_path, output_file);

    if((output_jni = fopen(output_path,"a+"))){
        fprintf(output_jni, "%s", output_tex);
        __android_log_print(ANDROID_LOG_DEBUG, "LOG_TO_FILE", "%s", output_tex);
    }

    fclose(output_file);*/
}


char* get_log(){
    char *output_path = (char *) malloc(1 + strlen(tmp_dir)+ strlen(output_file) );
    strcpy(output_path, tmp_dir);
    strcat(output_path, output_file);

    char *result = read_file(output_path);
    unlink(output_path);
    return result;
}

