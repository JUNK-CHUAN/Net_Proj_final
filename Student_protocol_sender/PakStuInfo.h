//
// Created by junk_chuan on 2019/10/31.
//

#ifndef NETWORK_PROJ_PAKSTUINFO_H
#define NETWORK_PROJ_PAKSTUINFO_H

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

/*
provide addIdInfo() function
You need provide two parameters
the first is char*
the second is the length of the char
like this
addIdInfo(manydata, thedatalength);
and I will return a new char *
*/

//check the id
// if the id isn't number
// the checkid return 0
int checkid(char *idtemp) {
    int i;
    for(i = 0;i < 8;i++) {
        if(idtemp[i] < '0' || idtemp[i] > '9') {
            return 0;
        }
    }
    return 1;
}

//dec to binary
int dectobina(int x) {
    int p = 1, y = 0, temp;
    while(1) {
        temp = x % 2;
        x /= 2;
        y += temp * p;
        p *= 10;
        if(x < 2) {
            y += x * p;
            break;
        }
    }
    return y;
}


//binary to dec
int binatodec(int n) {
    int dec = 0, i = 0, remainder;
    while(n != 0) {
        remainder = n % 10;
        n /= 10;
        dec += remainder * pow(2, i);
        i++;
    }
    return dec;
}


// add 4 bytes
//
char* addIdInfo(char* data, int filesize) {
    char *ptr = NULL;
    ///////////////////////////////////////////下面长度加4
    ptr = (char*)malloc((filesize + 8) * sizeof(char));
    memset(ptr,'\0', sizeof(ptr));
    char *idf0 = (char*)"teacher";
    char *idf1 = (char*)"freshman";
    char *idf2 = (char*)"sophomore";
    char *idf3 = (char*)"junior";
    char *idf4 = (char*)"senior";
    char *idf5 = (char*)"graduate";
    char *idf6 = (char*)"doctoral";
    char *stu = (char*)"student";
    char *newdata = data;
    char identity[10] = {};
    char itemp[10] = {};
    /////////////
    ///添加块sentid,sentchar
    char sentchar[5] = {};
    char sentid[9] = {};
    /////////////
    char id[9] = {};
    char binaryString[33] = {};
    char addchar[5] = {};
    int i, j, tempnum;
    printf("Are you a student or a teacher?(Please input teacher or student) \n");
    scanf("%s", identity);
    //////////////////////////////////////////
    ///////////////////////////////////////////2019-11-10，T.K.
    printf("Who do you want to send the message to?\n");
    printf("Please input the number of him:\n");
    scanf("%s", sentid);
    if(checkid(sentid) == 0) {
        printf("Sentid number input error.\n");
        printf("Please input eight figure.\n");
        return NULL;
    }
    if(strcmp(idf0, identity) == 0) {
        //printf("You are a teacher.\n");
        printf("Please input your teacher number:\n");
        scanf("%s",id);
        if(checkid(id) == 0) {
            printf("Teacher number input error.\n");
            printf("Please input eight figure.\n");
            return NULL;
        }
        for(i = 1;i <= 4;i++) {
            tempnum = id[(i - 1) * 2] - '0';
            tempnum *= 10;
            tempnum += id[((i - 1) * 2) + 1] - '0';
            tempnum = dectobina(tempnum);
            for(j = (i * 7) - 1;j >= (i - 1) * 7;j--) {
                binaryString[j] = (tempnum % 10) + '0';
                tempnum /= 10;
            }
        }
        for(i = 28;i < 32;i++) {
            binaryString[i] = '0';
        }
        binaryString[32] = '\0';
        /////////////////////////////修改块1
    }
    else if(strcmp(stu, identity) == 0) {
        printf("Which of the following is your identity?\n");
        printf("freshman,sophomore,junior,senior,graduate,doctoral.\n");
        printf("Please input: ");
        scanf("%s", itemp);
        printf("Please input your student number:");
        scanf("%s", id);
        if(checkid(id) == 0) {
            printf("Student number input error.\n");
            printf("Please input eight figure.\n");
            return NULL;
        }
        for(i = 1;i <= 4;i++) {
            tempnum = id[(i - 1) * 2] - '0';
            tempnum *= 10;
            tempnum += id[((i - 1) * 2) + 1] - '0';
            tempnum = dectobina(tempnum);
            for(j = (i * 7) - 1;j >= (i - 1) * 7;j--) {
                binaryString[j] = (tempnum % 10) + '0';
                tempnum /= 10;
            }
        }
        binaryString[28] = '1';
        if(strcmp(itemp, idf1) == 0) {
            //printf("You are a freshman.\n");
            binaryString[29] = '0';
            binaryString[30] = '0';
            binaryString[31] = '1';
        }
        else if(strcmp(itemp, idf2) == 0) {
            //printf("You are a sophomore.\n");
            binaryString[29] = '0';
            binaryString[30] = '1';
            binaryString[31] = '0';
        }
        else if(strcmp(itemp, idf3) == 0) {
            //printf("You are a junior.\n");
            binaryString[29] = '0';
            binaryString[30] = '1';
            binaryString[31] = '1';
        }
        else if(strcmp(itemp, idf4) == 0) {
            //printf("You are a senior.\n");
            binaryString[29] = '1';
            binaryString[30] = '0';
            binaryString[31] = '0';
        }
        else if(strcmp(itemp, idf5) == 0) {
            //printf("You are a graduate.\n");
            binaryString[29] = '1';
            binaryString[30] = '0';
            binaryString[31] = '1';
        }
        else if(strcmp(itemp, idf6) == 0) {
            //printf("You are a doctoral.\n");
            binaryString[29] = '1';
            binaryString[30] = '1';
            binaryString[31] = '0';
        }
        else {
            printf("Input error.\n");
            printf("Please input these: \nfreshman,sophomore,junior,senior,graduate,doctoral.\n");
            return NULL;
        }
        binaryString[32] = '\0';
    }
    else {
        printf("Input error, only students and teachers can use it.\n");
        return NULL;
    }
    /*for(i = 0;i < 32;i++) {
        printf("%c", binaryString[i]);
    }*/
    printf("\n");
    for(i = 0;i < 4;i++) {
        tempnum = 0;
        for(j = i * 8;j < (i + 1) * 8;j++) {
            tempnum *= 10;
            tempnum += (binaryString[j] - '0');
        }
        tempnum = binatodec(tempnum);
        addchar[i] = char(tempnum);
        //printf("%c\n", addchar[i]);
    }
    addchar[4] = '\0';
    strcat(ptr, addchar);
    ////////////////////////////////
    ///第二个修改块
    for(i = 0;i < 4;i++) {
        tempnum = 0;
        for(j = i * 2;j < (i + 1) * 2;j++) {
            tempnum *= 10;
            tempnum += (sentid[j] -'0');
        }
        sentchar[i] = char(tempnum);
    }
    sentchar[4] = '\0';
    strcat(ptr, sentchar);
    ////////////////////////////
    strcat(ptr, newdata);
    //printf("%s\n", ptr);
    free(data);
    data = NULL;
    newdata = NULL;
    return ptr;
}

#endif //NETWORK_PROJ_PAKSTUINFO_H
