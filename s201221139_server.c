#include "s201221139_client.c"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAXLINE 4096
#define TOKEN "-"
#define CODE_LOGIN "1"
#define CODE_FILEREAD "2"
#define CODE_FILEWRITE "3"
#define CODE_LOGOUT "4"
#define CODE_EXIT "5"

void server(int,int);
void serverLogin(char[], char[], int, int);
void serverReadFile(char[], char[], int, int);
void serverWriteFile(char[], char[], char[], int, int);
void serverLogout(char[], char[], int, int);


time_t current_timer;
int try=0;

int main(int argc, char *argv[]){

	int pipe1[2],  pipe2[2];

	pid_t childpid;

	pipe(pipe1);
	pipe(pipe2);

	if((childpid=fork())==0){ //child

		close(pipe1[0]);
		close(pipe2[1]);

		client(pipe2[0], pipe1[1]);
		exit(0);
	}

	//parent
	close(pipe1[1]);
	close(pipe2[0]);

	server(pipe1[0], pipe2[1]);
		
	waitpid(childpid, NULL, 0); //wait for child to terminate
	exit(0);
}

void server(int readfd, int writefd){
	char buff[MAXLINE];
	char id[MAXLINE], pw[MAXLINE];
	char *code;
	char fileName[MAXLINE];
	char studentID[MAXLINE], studentName[MAXLINE];

	while(1){
		if((read(readfd, buff, MAXLINE))>0){
			
			code = strtok(buff, "-");
			
			switch(atoi(code)){
				case 1:
					strcpy(id, strtok(NULL,"-"));
					strcpy(pw, strtok(NULL,"-"));
					serverLogin(id, pw, readfd, writefd);
					break;
				case 2:
					strcpy(fileName, strtok(NULL,"-"));
					strcpy(studentID, strtok(NULL,"-"));
					serverReadFile(fileName, studentID, readfd, writefd);
					break;
				case 3:
					strcpy(fileName, strtok(NULL,"-"));
					strcpy(studentID, strtok(NULL,"-"));
					strcpy(studentName, strtok(NULL,"-"));
					serverWriteFile(fileName, studentID, studentName, readfd, writefd);
					break;
				case 4:
					serverLogout(id, pw, readfd, writefd);
					return;
				case 5:
					return;
			}
		}
	}
}

void serverLogin(char id[MAXLINE], char pw[MAXLINE], int readfd, int writefd){

	FILE *fp=fopen("login.txt","r");
	FILE *temp_fp = fopen("temp_login.txt","w");
	struct tm *current_time;
    current_timer = time(NULL);    // 현재 시각을 초 단위로 얻기

	char login_result[MAXLINE];
	char file_id[MAXLINE], file_pw[MAXLINE];
	char file_year[5], file_month[3], file_date[3], file_hour[3], file_minute[3],file_second[3];
	
	login_result[0]='\0';

	while(!feof(fp)){

		char file_id[MAXLINE], file_pw[MAXLINE],fi;
		
		time_t user_timer;
		struct tm user_time;

		fscanf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", file_id, file_pw, file_year, file_month, file_date, file_hour, file_minute, file_second);
		
		user_time.tm_year = atoi(file_year)-1900;
		user_time.tm_mon = atoi(file_month)-1;
		user_time.tm_mday = atoi(file_date);
		user_time.tm_hour = atoi(file_hour);
		user_time.tm_min = atoi(file_minute);
		user_time.tm_sec = atoi(file_second);
		user_time.tm_isdst = 0;

		user_timer = mktime(&user_time);
		current_time = localtime(&current_timer);

		if(strcmp(id,file_id)==0 && strcmp(pw, file_pw)==0){
			if(current_timer-user_timer>0){
				fprintf(temp_fp,"%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n", file_id, file_pw, current_time->tm_year+1900, current_time->tm_mon+1, current_time->tm_mday, current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
				strcpy(login_result,"true");
			}else{
				fprintf(temp_fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", file_id, file_pw, file_year, file_month, file_date, file_hour, file_minute, file_second);
				if(strcmp(login_result,"true")!=0){	
					strcpy(login_result,"false");
				}
			}
		}else{
			fprintf(temp_fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", file_id, file_pw, file_year, file_month, file_date, file_hour, file_minute, file_second);
			if(strcmp(login_result,"true")!=0){
				strcpy(login_result,"false");
			}
		}
	}

	if(strcmp(login_result,"false")==0){
		try++;
		printf("\nServer: [Login is failed. please retry]\n");
		printf("Server: [Remained time to try: %d]\n\n", 3-try);
		remove("./temp_login.txt");
	}else{
		printf("\nServer: [Login Success!]\n\n");
		remove("./login.txt");
		rename("./temp_login.txt","login.txt");
	}
	
	write(writefd,login_result,MAXLINE);

	if(try==3){
		printf("Server: [You have alreay failed three times.]\n");
		printf("Server: [Program exit.]\n\n");
		remove("./temp_login.txt");
	}

	fclose(temp_fp);
	fclose(fp);

	return;
}

void serverReadFile(char fileName[MAXLINE], char studentID[MAXLINE], int readfd, int writefd){
	
	FILE *fp = fopen(fileName,"r");
	char buff[MAXLINE];
	char file_studentID[MAXLINE], file_studentName[MAXLINE];
	char result[MAXLINE];

	strcpy(result, "false");

	if(fp==NULL){
		printf("Server: [Can't find file]\n");
		write(writefd,result,MAXLINE);
		return;
	}

	while(!feof(fp)){
		fscanf(fp, "%s\t%s\n", file_studentID, file_studentName);

		if(strcmp(file_studentID, studentID)==0){
			strcpy(result, "true");
			strcpy(buff, file_studentName);
			break;
		}else{
			strcpy(result,"false");
		}
	}

	if(strcmp(result,"true")==0){
		write(writefd, buff, MAXLINE);
	}else{
		printf("Server: [Can't find a student ID.]\n");
		printf("Server: [Please retry.]");
		write(writefd, result, MAXLINE);
	}

	buff[0]='\0';
	fclose(fp);
	return;
}

void serverWriteFile(char fileName[MAXLINE], char studentID[MAXLINE], char studentName[MAXLINE], int readfd, int writefd){
	FILE *fp = fopen(fileName,"r");
	FILE *temp_fp = fopen("temp.txt","w");

	char result[MAXLINE];
	char file_studentID[MAXLINE], file_studentName[MAXLINE];
	char filePath[MAXLINE+2];

	int count1=0;
	int count2=0;

	strcpy(filePath, "./");

	if(fp==NULL){
		printf("Server: [Can't find file.]\n");
		strcpy(result,"false");
		write(writefd,result,MAXLINE);
		return;
	}

	while(!feof(fp)){
		count1++;
		fscanf(fp,"%s\t%s\n", file_studentID, file_studentName);

		if(strcmp(file_studentID,studentID)==0){
			fprintf(temp_fp, "%s\t%s\n", file_studentID, studentName);
			printf("Server: [Update student name.]\n");
			strcpy(result,"update");
		}else{
			count2++;
			fprintf(temp_fp, "%s\t%s\n", file_studentID, file_studentName);
		}
	}
	
	if(count1==count2){
		fprintf(temp_fp, "%s\t%s\n", studentID, studentName);
		printf("Server: [Add student ID & student name.]\n");
		strcpy(result,"add");
	}

	strcat(filePath, fileName);

	remove(filePath);
	rename("./temp.txt", fileName);

	fclose(fp);
	fclose(temp_fp);

	write(writefd, result, MAXLINE);
}

void serverLogout(char id[MAXLINE], char pw[MAXLINE], int readfd, int writefd){
	FILE *fp = fopen("login.txt","r");
	FILE *temp_fp = fopen("temp_login.txt","w");
	struct tm *current_time;
	current_timer = time(NULL);

	char file_id[MAXLINE], file_pw[MAXLINE], file_year[MAXLINE], file_month[MAXLINE], file_date[MAXLINE], file_hour[MAXLINE], file_minute[MAXLINE], file_second[MAXLINE];
	char result[MAXLINE];

	char cur_year[MAXLINE], cur_mon[MAXLINE], cur_date[MAXLINE], cur_hour[MAXLINE], cur_min[MAXLINE], cur_sec[MAXLINE];

	current_time = localtime(&current_timer);

	sprintf(cur_year,"%d",current_time->tm_year+1900);
	sprintf(cur_mon,"%d",current_time->tm_mon+1);
	sprintf(cur_date,"%d",current_time->tm_mday);
	sprintf(cur_hour,"%d",current_time->tm_hour);
	sprintf(cur_min,"%d",current_time->tm_min);
	sprintf(cur_sec,"%d",current_time->tm_sec);

	while(!feof(fp)){
		fscanf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", file_id, file_pw, file_year, file_month, file_date, file_hour, file_minute, file_second);

		if(strcmp(file_id, id)==0){
			fprintf(temp_fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",file_id, file_pw, cur_year, cur_mon, cur_date, cur_hour, cur_min, cur_sec);
		}else{
			fprintf(temp_fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",file_id, file_pw, file_year, file_month, file_date, file_hour, file_minute, file_second);
		}
	}

	remove("./login.txt");
	rename("./temp_login.txt","./login.txt");

	strcpy(result,"true");
	write(writefd,result,MAXLINE);
	
	fclose(fp);
	fclose(temp_fp);
	return;
}
