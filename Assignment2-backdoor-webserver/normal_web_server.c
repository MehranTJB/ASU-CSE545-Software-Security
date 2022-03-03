#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <string.h>

int portnum = 0;
int srvr_socket_desc;

void accept_command(struct sockaddr_in srvr_addr);
void SIGINT_callback_handler(int signal_num);
void extract_command(char * input_str);

int main(int argc, char *argv[]){
  //check command line arguments
  if(argc < 2){
    printf("Command to Execute: ./normal_web_server <port_number>\nMissing port number.\n");
    exit(1);
  }
  portnum = atoi(argv[1]);

  //Create a new socket descriptor
  srvr_socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  //check if there was an error creating socket.
  if (srvr_socket_desc == 0)
  {
      perror("failed to get socket");
      exit(1);
  }

  //register handler for Ctrl + C
  signal(SIGINT, SIGINT_callback_handler);

  //create server address
  struct sockaddr_in srvr_address;
  srvr_address.sin_family = AF_INET;
  srvr_address.sin_port = htons(portnum);
  srvr_address.sin_addr.s_addr = INADDR_ANY;

  //bind socket and port
  int bind_status = bind(srvr_socket_desc, (struct sockaddr *)&srvr_address, sizeof(srvr_address));
  if(bind_status < 0){
    perror("Failed to bind socket to port");
    exit(1);
  }

  //listen to socket
  int listen_status = listen(srvr_socket_desc, 3);
  if(listen_status < 0){
    perror("Failed to listen to port");
    exit(1);
  }

  accept_command(srvr_address);

  return 0;
}

void accept_command(struct sockaddr_in srvr_address){
  char * ok_response_header = "HTTP/1.1 200 OK\n"
                              "Content-Type: text/html\n"
                              "Accept-Ranges: bytes\n"
                              "Connection: close\n"
                              "\n";
  char * not_found_response_header = "HTTP/1.1 404 NOTFOUND\n"
                                    "Content-Type: text/html\n"
                                    "Accept-Ranges: bytes\n"
                                    "Connection: close\n"
                                    "\n";
  char * get_request = "GET";

  while(1){
    //accept incoming requests
    int len_of_address = sizeof(srvr_address);
    int client_socket_desc = accept(srvr_socket_desc, (struct sockaddr *)&srvr_address, (socklen_t *)&len_of_address);

    char in_buffer[1000];
    char input_str[1000];

    memset(input_str, 0, sizeof(input_str));
 

    read(client_socket_desc, in_buffer, 1000);
    
    char command_result[1000];
    char response_header[1000];

    if(strlen(in_buffer)<= 10 || strncmp(get_request, in_buffer, strlen(get_request)) != 0){
      strcpy(response_header, not_found_response_header);
      strcpy(command_result, "Invalid request only GET is allowed.");
    }
    else{
      int i, j=0;
     
      for(i = 4; i<strlen(in_buffer); i++){
        if(in_buffer[i] == ' '){
          break;
        }
        input_str[j++] = in_buffer[i];
      }
      input_str[j] = '\0';
      extract_command(input_str);
      printf("Command recieved: %s\n", input_str);

      if(strlen(input_str) == 0){
        strcpy(response_header, not_found_response_header);
        strcpy(command_result, "Invalid command!");
      }
      else{
        strcat(input_str, " 2>&1");
        char tmp_buffer[1000];
        FILE* file = popen(input_str, "r");
        if(file == NULL){
          strcpy(response_header, not_found_response_header);
          strcpy(command_result, "Invalid command!");
        }
        else{
          int flag = 0;
          while (fgets(tmp_buffer, sizeof(tmp_buffer), file) != NULL)
          {
            if(flag == 0){
                strcpy(command_result, tmp_buffer);
                flag++;
            }
            else{
                strcat(command_result, tmp_buffer);
            }
          }
          strcpy(response_header, ok_response_header);
        }

        if (pclose (file) != 0)
        {
          strcpy(response_header, not_found_response_header);
          file = NULL;
        }

        memset(tmp_buffer, 0, sizeof(tmp_buffer));
      }
    }

    char * response = NULL;
    response = malloc(strlen(response_header) + strlen(command_result) + 10);
    strcpy(response, response_header);
    strcat(response, command_result);

    send(client_socket_desc, response, strlen(response), 0);
    close(client_socket_desc);
    printf("Message sent:\n%s\n", response);

    //clear all buffers
    memset(in_buffer, 0, sizeof(in_buffer));
    memset(input_str, 0, sizeof(input_str));
    memset(response, 0, sizeof(response));
    memset(command_result, 0, sizeof(command_result));
    memset(response_header, 0, sizeof(response_header));
  }
}

void SIGINT_callback_handler(int signal_num){
  //Release the port
  close(srvr_socket_desc);
  shutdown(srvr_socket_desc, 2);
  //exit program
  exit(0);
}

void extract_command(char * input_str){
  char * tmp_ptr;
  tmp_ptr = malloc(strlen(input_str));
  
  int i, j=0;

  for(i = 0; i<strlen(input_str); i++){
    if(*(input_str+i) == '+'){
      tmp_ptr[j++] = ' ';
    }
    else if(*(input_str+i) != '%'){
      tmp_ptr[j++] = *(input_str+i);
    }
    else if(!isxdigit(*(input_str+i+1)) || !isxdigit(*(input_str+i+2))){
      tmp_ptr[j++] = *(input_str+i);
    }
    else{
      char first_char = tolower(*(input_str+i+1));
      char second_char = tolower(*(input_str+i+2));
      if(first_char <= '9'){
        first_char = first_char - '0';
      }
      else{
        first_char = first_char - 'a' + 10;
      }
      if(second_char <= '9'){
        second_char = second_char - '0';
      }
      else{
        second_char = second_char - 'a' + 10;
      }

      tmp_ptr[j++] = (16 * first_char + second_char);
      i += 2;
    }
  }
  tmp_ptr[j] = '\0';
  strcpy(input_str, tmp_ptr);

  char * exec = "/exec/";
  if(strlen(input_str) <= 6 || strncmp(exec, tmp_ptr, strlen(exec)) != 0){
    memset(tmp_ptr, 0, sizeof(tmp_ptr));
    strcpy(input_str, tmp_ptr);
    return;
  }

  char command[1000];
  strcpy(command, input_str+6);
  strcpy(input_str, command);
}

