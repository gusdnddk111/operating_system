#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 4096
#define TOKEN "-"
#define CODE_LOGIN "1"
#define CODE_FILEREAD "2"
#define CODE_FILEWRITE "3"
#define CODE_LOGOUT "4"
#define CODE_EXIT "5"

void client(int, int);
int clientLogin(char[], int, int);
int clientFuction(char[], int, int);

void client(int readfd, int writefd){
	char buff[MAXLINE];
	
	clientLogin(buff, readfd, writefd);
	clientFunction(buff, readfd, writefd);
}

int clientLogin(char buff[MAXLINE], int readfd, int writefd){

	char user_info[MAXLINE];
	char login_check[MAXLINE];
	char id[MAXLINE], pw[MAXLINE];

	int try=0;

	while(1){
	
		printf("\nClient:[Enter your ID]\n");
		printf("Client: ");
		scanf("%s", id);
		printf("\nClient:[Enter your Password]\n");
		printf("Client: ");
		scanf("%s", pw);
	
		strcpy(user_info,CODE_LOGIN);
		strcat(user_info,TOKEN);
		strcat(user_info,id);
		strcat(user_info,TOKEN);
		strcat(user_info,pw);
		
		write(writefd, user_info, MAXLINE);
		user_info[0]='\0';
		
		//로그인 결과 받아오기
		read(readfd,buff,MAXLINE);
		strcpy(login_check, buff);
		buff[0]='\0';

		if(strcmp(login_check,"true")==0){
			return 0;
		}else if(strcmp(login_check,"false")==0){
			try++;
			if(try==3){
				strcpy(buff, CODE_EXIT);

				write(writefd, buff, MAXLINE);
				exit(0);
			}
		}
	}
	return 0;
}

int  clientFunction(char buff[MAXLINE], int readfd, int writefd){
	
	char fileName[MAXLINE];
	char studentID[MAXLINE], studentName[MAXLINE];
	char code[MAXLINE];
	char result[MAXLINE];

	while(1){
		printf("┌─────────────────────────────────────┐\n");
		printf("│          What do you want?          │\n");
		printf("│                                     │\n");
		printf("│  \"r\" : Read File                    │\n");
		printf("│  \"w\" : Write File                   │\n");
		printf("│  \"logout\" : Logout & program exit   │\n");
		printf("│                                     │\n");
		printf("└─────────────────────────────────────┘\n\n");
		printf("Client: ");
		scanf("%s",code);

		if(strcmp(code,"r")==0 || strcmp(code,"R")==0){
			strcpy(buff,CODE_FILEREAD);	

			printf("Client: [Enter a file name]\n");
			printf("Client: ");
			scanf("%s",fileName);
			printf("Client: [Enter a student ID]\n");
			printf("Client: ");
			scanf("%s",studentID);

			strcat(buff,TOKEN);
			strcat(buff,fileName);
			strcat(buff,TOKEN);
			strcat(buff,studentID);
			
			write(writefd, buff, MAXLINE);

			read(readfd, result, MAXLINE);

			if(strcmp(result,"false")==0){
				printf("\nClient: [Server can't complete your request.]\n\n");
			}else if(strcmp(result,"true")==0){
				printf("\nClient: [Server complete your request.]\n\n");
			}
		}else if(strcmp(code,"w")==0 || strcmp(code,"W")==0){
			strcpy(buff,CODE_FILEWRITE);

			printf("Client: [Enter a file name]\n");
			printf("Client: ");
			scanf("%s",fileName);
			printf("Client: [Enter a student ID]\n");
			printf("Client: ");
			scanf("%s",studentID);
			printf("Client: [Enter a student name]\n");
			printf("Client: ");
			scanf("%s",studentName);

			strcat(buff,TOKEN);
			strcat(buff,fileName);
			strcat(buff,TOKEN);
			strcat(buff,studentID);
			strcat(buff,TOKEN);
			strcat(buff,studentName);

			write(writefd, buff, MAXLINE);

			read(readfd, result, MAXLINE);

			if(strcmp(result,"add")==0){
				printf("\nClient: [Server add studentID & name.]\n\n");
			}else if(strcmp(result,"update")==0){
				printf("\nClient: [Server update student name about %s.]\n\n", studentID);
			}else{
				printf("\nClient: [Server can't complete your request.]\n\n");
			}
	
		}else if(strcmp(code,"logout")==0 || strcmp(code,"LOGOUT")==0){
			printf("\nClient: [You selected Logout.]\n");
			printf("Client: [Program exit.]\n\n");

			char temp[10];
			strcpy(temp,CODE_LOGOUT);
			write(writefd,temp,MAXLINE);

			read(readfd, buff, MAXLINE);

			if(strcmp(buff,"true")==0){
				exit(0);
			}
		}else{
			printf("\nClient: [Maybe you typed wrong.]\n");
			printf("Client: [Please retry.]\n\n");
		}

		buff[0]='\0';
		code[0]='\0';
	}
	return 0;
}
