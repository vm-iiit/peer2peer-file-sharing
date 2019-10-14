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
#include<climits>
#include <openssl/sha.h>

#define BUFF_SIZE 100
#define THREAD_COUNT 20
#define num_trackers 2
#define group_limit 25
using namespace std;

struct local_f{
    char ip[BUFF_SIZE];
    int port;
    char path[BUFF_SIZE];
    char SHA_hash[BUFF_SIZE];

    char fname[BUFF_SIZE];
    char dpath[BUFF_SIZE];
};

struct f_info{
    string u;
    int g;
    string fname;
    string hash;
    string path;
};

pair<string, string> break_path(string path)
{
    string temp(path);
    reverse(temp.begin(), temp.end());
    string p, f;
    f = temp.substr(0, temp.find("/"));
    p = temp.substr(temp.find("/"));
    reverse(f.begin(), f.end());
    reverse(p.begin(), p.end());
    p = p.substr(0, p.length()-1);
    return make_pair(p, f);
    
}

string calc_sha(char *path)
{
    //cout<<path<<endl;
    FILE *f = fopen(path, "rb");
    if(f == NULL)
        cout<<"cant open file\n";
    fseek(f, 0, SEEK_END);
    ssize_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *strin = (char *)malloc(fsize + 1);
    fread(strin, fsize, 1, f);
    fclose(f);
    //cout<<"closed file\n";
    strin[fsize] = 0;

    const unsigned char *str = (unsigned char *)strin;
    unsigned char hash[2*SHA_DIGEST_LENGTH]; // == 20
    char buf[2*SHA_DIGEST_LENGTH];

    SHA1(str, sizeof(str), hash);
    //cout<<hash<<endl;

    for (int i=0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf((char*)&(buf[i*2]), "%02x", hash[i]);
    }

    string temp;
    temp.assign(buf);
    return temp;
    // do some stuff with the hash
}