#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <zconf.h>
#include <uuid/uuid.h>
#include <dirent.h>
#include "PakType.h"
#include "PakStuInfo.h"
#include "PakSlice.h"
#include "PakCheck.h"

#define PATHLEN 100U  // 输入路径的长度限制
#define PROTLEN 10U  // 协议的长度，即'student://'
#define MAXSIZE 4096
#define IP "127.0.0.1"
#define SERV_PORT 8000

// BASE_DIR为临时文件存放的文件夹
const char BASE_DIR[PATHLEN] = "/home/junk_chuan/Desktop/send/";

//---------------------------------------文件操作函数--------------------------------
/*
 * 获取文件大小
 * 需要用sys/stat.h库，该方法仅限Linux
 */
int file_size(char * filename)
{
    struct stat statbuf;
    stat(filename, &statbuf);
    int size = statbuf.st_size;

    return size;
}

/*
 * 将文件加载到字节数组
 * 函数返回字符串char *
 */
char *loadfile(char * filename)
{
    printf("%s\n", filename);
    int fd = open(filename, O_RDONLY);
    //printf("文件描述符： %d\n", fd);

    unsigned int FileLen = file_size(filename);
    char *buffer = (char *) malloc(sizeof(char) * FileLen); // 根据文件大小动态分配内存

    int readBytes = read(fd, buffer, sizeof(char) * FileLen);
    if (readBytes == sizeof(char) * FileLen)
    {
        printf("Read file successfully.\n");
        return buffer;
    } else
    {
        printf("The size of file: %d.\n", (int)(sizeof(char) * FileLen));
        printf("The Bytes actually read: %d.\n", readBytes);
        printf("Exit.\n");
        exit(0);
    }
}


//-------------------------------------I/O模块-----------------------------------
//输入字符串函数
int getline(char line[], int max)
{
    int ch;
    int i = 0;
    while (max > 0 &&
           (ch = getchar()) != EOF && ch != '\n')
    {
        line[i] = ch;
        i++;
        max--;
    }
    if (ch == '\n')
        line[i++] = '\n';
    line[i] = '\0';
    if (i > 0)
    {
        return 1;
    }
    else
        return 0;
}


//-------------------------------------I/O模块-----------------------------------
// socket 错误报错函数
void sys_err(const char *ptr,int num)
{
    perror(ptr);
    exit(num);
}


void socket_send(char *src)
{
    int sockfd;
    char buf[MAXSIZE];
    struct sockaddr_in addr;

    //建立socket套接字
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0)
        sys_err("socket",-1);

    bzero(&addr,sizeof(addr));

    //初始化ip+port
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERV_PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    //connect将sockfd套接字描述符与服务器端的ip+port联系起来
    if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr)) < 0)
        sys_err("connect",-2);

    // 打开读的文件
    int fd = open(src,O_RDONLY);
    if(fd < 0)
        sys_err("open",-3);

    while(1)
    {
        int len = read(fd,buf,sizeof(buf));
        if(len == 0)
            break;
        
        int _tmp = 0;
        //考虑若socket缓冲区小于len的情况
        while(1)
        {
            int ret = write(sockfd,buf + _tmp, len - _tmp);
            if(ret > 0 )
                _tmp += ret;
            if(_tmp == ret)
                break;
            if(ret < 0)
            {
                perror("write");
                break;
            }
        }
    
    }
    
    close(sockfd);
}


//-------------------------------------主函数---------------------------------
/*
 * 将读取的文件添加头部
 * 进行分组
 * 再添加校验码
 * 最后分块存入临时文件
 */

// 设置临时文件的后缀为.pak
const char POST_FIX[5] = ".pak";

int main()
{
    printf("Please input the file you want to transfer with the protocol.\n");
    char * path = (char *)malloc(sizeof(char) * PATHLEN);
    getline(path, PATHLEN);                     // 获取输入的路径(带协议名)
    char *protocol;   // 创建一个protocol字符串储存输入的协议
    protocol = strprotocol(path, PROTLEN);      // 字符串分割，protocol储存协议，path储存地址
    strcpy(path, path + PROTLEN);
    int i = 0;
    while (path[i] != '\n')
        i++;
    path[i] = '\0';
    char standard[PROTLEN + 1] = "student://";  // 创建一个标准字符串，表示我们唯一可识别的协议
    if (strmatch(protocol, standard))           // 若协议符合学生协议的standard
    {
        /* 若协议正确则读取文件数据 */
        char * data = loadfile(path);
        // 读取文件后释放内存
        free(protocol);
        protocol = NULL;
        unsigned int DataSize = file_size(path);

        // 自顶而下的第一层封装
        data = PackageTypeInfo(data, path, &DataSize);  // 实现顶层数据封装

        // 自顶而下的第二层封装
        data = addIdInfo(data, DataSize);
        DataSize += 8;

        // 自顶而下的第三层封装
        unsigned int piece = 0;
        unsigned int left = 0;
        // 将数据分组之后存入二维数组
        char **SliceData = PakSlice(data, &DataSize, &piece, &left);

        // 添加CRC校验码
        for (int i = 0; i < piece-1; ++i) {
            SliceData[i] = CRC(SliceData[i]);
        }
        SliceData[piece-1] = CRC(SliceData[piece-1], left);

        // 删除以前缓存文件
        system("rm -f /home/junk_chuan/Desktop/send/*");
        sleep(3);
        // -------------------------cache
        // 利用uuid生成唯一标识符，以免存储时文件名重复
        uuid_t uuid;
        char *filepath = (char *)malloc(sizeof(char) * PATHLEN);
        char str[37];
        int fd, count;
        for (int i = 0; i < piece-1; ++i)
        {
            // 生成唯一标识符
            uuid_generate(uuid);
            uuid_unparse(uuid, str);
            // 利用唯一标识符生成唯一的文件路径
            memset(filepath, '\0', sizeof(filepath));
            strcat(filepath, BASE_DIR);
            strcat(filepath, str);
            strcat(filepath, POST_FIX);
            // 创建文件并以读写方式打开
            fd = open(filepath, O_RDWR | O_CREAT, S_IRWXU | S_IRUSR | S_IWUSR);
            // 将每组数据写入临时文件
            count = write(fd, SliceData[i], 1004);
            close(fd);
        }

        // 生成缓存文件
        // 生成唯一标识符
        uuid_generate(uuid);
        uuid_unparse(uuid, str);
        // 利用唯一标识符生成唯一的文件路径
        memset(filepath, '\0', sizeof(filepath));
        strcat(filepath, BASE_DIR);
        strcat(filepath, str);
        strcat(filepath, POST_FIX);
        // 创建文件并以读写方式打开
        fd = open(filepath, O_RDWR | O_CREAT, S_IRWXU | S_IRUSR | S_IWUSR);
        // 将每组数据写入临时文件
        count = write(fd, SliceData[piece-1], left+1);
        close(fd);
        for (int j = 0; j < piece; ++j) {
            free(SliceData[j]);
            SliceData[j] = NULL;
        }
        free(SliceData);
        SliceData = NULL;
        /* doing */

        // -------------------------socket
        // socket发送文件
    	// 循环遍历文件夹
		DIR *dir;
    	struct dirent *ptr;
		if ((dir=opendir(BASE_DIR)) == NULL)
    	{
        	perror("Open dir error...");
        	exit(1);
    	}

		// 循环读取文件，通过d_type判断是否为文件
    	while ((ptr=readdir(dir)) != NULL)
    	{
    		if(ptr->d_type == 8)    //type refers to file
    		{
				// 获取当前文件的文件路径
    			memset(filepath, '\0', sizeof(filepath));
    			strcat(filepath, BASE_DIR);
    			strcat(filepath, ptr->d_name);

    			socket_send(filepath);
    		}
    	}
    	closedir(dir);

    	char comfirmMsg[5] = "OK";

    	int sockfd;
    	char buf[MAXSIZE];
    	struct sockaddr_in addr;

    	// 建立socket套接字
    	sockfd = socket(AF_INET,SOCK_STREAM,0);
    	if(sockfd < 0)
        	sys_err("socket",-1);

    	bzero(&addr,sizeof(addr));

    	// 初始化ip+port
    	addr.sin_family = AF_INET;
    	addr.sin_port = htons(SERV_PORT);
    	addr.sin_addr.s_addr = inet_addr(IP);

    	// connect将sockfd套接字描述符与服务器端的ip+port联系起来
    	if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr)) < 0)
        	sys_err("connect",-2);
        int ret = write(sockfd, comfirmMsg, 5);
		if(ret < 0)
        {
            perror("write");
            exit(0);
        }
        else
        	close(sockfd);
        free(filepath);
        filepath = NULL;
    }
    else // 若协议与学生协议不符合
    {
        printf("We cannot recognize your protocol. Please use other dalao's program.\n");
        free(protocol);
        free(path);
        exit(0);
    }

    return 0;
}
