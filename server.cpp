// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#define PORT 8080
#define MAXCHAR 1000
char user[1000];
char* User;
char from[100], to[100], sub[100];

bool verifyM(char *str){

	char s1[1000];
	int i=0;
	while(str[i] != ':'){
		s1[i] = str[i];
		i++;
	}
	s1[i] = '\0';
	if ( !strcmp(s1, "MAIL FROM") && str[i+1] == '<' && str[strlen(str) - 1 ] == '>'){
		return true;
	}
	return false;
}

bool verifyR(char *str){

	char s1[1000];
	int i=0;
	while(str[i] != ':'){
		s1[i] = str[i];
		i++;
	}
	s1[i] = '\0';
	if ( !strcmp(s1, "RCPT TO") && str[i+1] == '<' && str[strlen(str) - 1 ] == '>'){
		return true;
	}
	return false;
}

void extUser(char addr[1000]){
	int start = 0, i=0, k=0;
	for (int j=0; addr[j]; j++){
		if (addr[j] == '<'){
			i = j+1;
			break;
		}
	}
	while(addr[i] != '@'){
		user[k] = addr[i];
		i++;
		k++;
	}
	user[k] = '\0';
}

bool exist(char filename[1000]){
    /* try to open file to read */
    FILE *file;
    if (file = fopen(filename, "r")){
        fclose(file);
        return true;
    }
    return false;
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
		// ---Connection Established---

	    // ---Fomalities---
	send(new_socket, "200 OK", 6, 0);

	read(new_socket, buffer, 1024);
	if (!strcmp("HELO",buffer)){
		printf("Client: %s\n",buffer);
		printf("Status code: 220\n");
		send(new_socket, "220", 3, 0);
		read(new_socket,buffer, 1024);
		strcpy(from, buffer);

		if (verifyM(buffer)){
			printf("Client: %s\n", buffer);
			printf("Status code: 220\n");
			send(new_socket, "220", 3, 0);
			read(new_socket, buffer, 1024);
			strcpy(to, buffer);

			if(verifyR(buffer)){
				printf("Client: %s\n",buffer);
				extUser(buffer);
				User = strcat(user, ".txt");
				printf("User file: %s\n", User);
				if(exist(User)){
					printf("Status code: 220\nUser found. Ready to transfer files...\n");
					send(new_socket, "220", 3, 0);
					read(new_socket,buffer, 1024);
					strcpy(sub,buffer);
					printf("%s\n%s\nSubject: %s\n\n", to, from, sub);
					//  --- File transfer ---


					read( new_socket ,buffer, 1024);

					FILE *fwrite;
					    fwrite = fopen(User, "w");

						read( new_socket ,buffer, 1024);
						int jj=0;
                        char* fstr[1000];
						char* strr = strtok(buffer, "\n");
						fstr[jj] = strr;
						while (strr){
							jj++;

							fstr[jj] = strr;
							strr = strtok(NULL, "\n");
					    }

						for (int i=0; i<jj; i++){
							if (!strcmp(fstr[i], ".")){
								break;
							}
							printf("%s\n", fstr[i]);
							fprintf(fwrite, "%s\n", fstr[i]);
						}

					    fprintf(fwrite, "\n\n(Copied)");
					    printf("\n\nFile recieved. Closing connection...\n");
					    fclose(fwrite);

				}
				else{
					send(new_socket, "400", 3, 0);
					printf("Status code: 400  File not found\nClosing connection...\n");
				}
			}
		}

	}

	return 0;
 }

