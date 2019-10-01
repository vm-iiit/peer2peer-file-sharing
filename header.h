#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<fstream>
#include<pthread.h>
#include<algorithm>
#define BUFF_SIZE 100
#define THREAD_COUNT 20
using namespace std;