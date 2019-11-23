//
// Created by junk_chuan on 2019/10/31.
//

#ifndef NETWORK_PROJ_PAKTYPE_H
#define NETWORK_PROJ_PAKTYPE_H


//----------------------------------字符串操作函数------------------------------------
/*
 * 字符串操作：
 * 系统库函数：
 *     char *strcat(char *str_des, char *str_sou);
 *     将字符串str_sou接在字符串str_des后面（放在str_des的最后字符和“\0”之间）
 * 自定义函数：
 *     字符串匹配函数int strmatch(char line[], char *mat)
 *     字符串切割函数char *strcut(char *str_des, unsigned int *size1, unsigned int *size2)
 */

/*
 * 自定义字符串切割函数
 * param.str_des:待分割字符串,函数执行后变为数组后半段
 * param.size1:分割后前半段的长度(不计'\0')
 * param.size2:待分割字符串的总的长度(不计'\0')
 * return:前半段字符串
 */
char *strprotocol(char *str_des, unsigned int size1)
{
    // 分配内存截取前半段字符串
    char  *str_pre = (char *)malloc(sizeof(char) * (size1 + 1));
    str_pre = strncpy(str_pre, str_des, size1);
    str_pre[size1] = '\0';
    // debug dump
    // printf("%s\n", str_pre);

    return str_pre;
}

/*
 * 字符串匹配函数
 * param.line: 输入字符串
 * param.mat: 匹配对应字符串
 * 完全相同返回1，
 * 否则返回0
 */
int strmatch(char line[], char *mat)
{
    int i = strlen(line), j = 0, k = 0;
    if (i == strlen(mat))
    {
        for (; j < strlen(mat); j++, k++)
        {
            if (line[k] != *(mat + j))
                break;
        }
        if (*(mat + j) == '\0' && k>0)
            return 1;
    }
    return 0;
}


char *PackageTypeInfo(char * data, char * path, unsigned int *DataSize)
{
    char *split_str = (char *)".";
    char *postfix;
    char *tmp;
    tmp = strtok(path, split_str);
    while (tmp != NULL)
    {
        postfix = tmp;  // 一直替换直到换至后缀
        tmp = (char *) strtok(NULL, split_str);
    }
    char *type0 = (char *)"txt";
    char *type1 = (char *)"html";
    char *type2 = (char *)"jpg";
    char *type3 = (char *)"png";
    char *type4 = (char *)"json";
    char *type5 = (char *)"mp3";
    char *type6 = (char *)"mp4";
    char *type7 = (char *)"xml";
    char *type8 = (char *)"md";
    char prePak[2] = { 0 };
    prePak[1] = '\0';   // 设置字符串结尾符
    if (strmatch(postfix, type0))
        prePak[0] = char(64);
    else if (strmatch(postfix, type1))
        prePak[0] = char(65);
    else if (strmatch(postfix, type2))
        prePak[0] = char(66);
    else if (strmatch(postfix, type3))
        prePak[0] = char(67);
    else if (strmatch(postfix, type4))
        prePak[0] = char(68);
    else if (strmatch(postfix, type5))
        prePak[0] = char(69);
    else if (strmatch(postfix, type6))
        prePak[0] = char(70);
    else if (strmatch(postfix, type7))
        prePak[0] = char(71);
    else if (strmatch(postfix, type8))
        prePak[0] = char(72);
    else                // 文件类型不在索引列表内
    {
        printf("Unknown document type."); // 打印错误信息并退出程序
        free(path);
        path = NULL;
        free(data);
        data = NULL;
        exit(0);
    }
    printf("The postfix of the file is %s.\n", postfix);

    free(path);         // 释放内存
    path = NULL;
    *DataSize += 1;
    char * pakData = (char *)malloc(sizeof(char) * *DataSize);
    strcpy(pakData, prePak);
    strcat(pakData, data);
    //free(data);         // 释放封装前数据的内存
    //data = NULL;

    return pakData;
}


#endif //NETWORK_PROJ_PAKTYPE_H
