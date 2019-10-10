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
#include<fstream>
#include<stdlib.h>
#include<map>
#include<vector>
#include<list>
#define BUFF_SIZE 100
#define THREAD_COUNT 20
#define num_trackers 2
#define group_limit 15
using namespace std;