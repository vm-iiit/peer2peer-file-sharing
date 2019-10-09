#include"header.h"

string findname(char *path)
{
	string n = "";
	for(int lv = strlen(path)-1; path[lv] != '/'; lv--)
	{
		string s(1, path[lv]);
		n.append(s);
	}
	reverse(n.begin(), n.end());
	return n;

}

void *send_file(void *afd)
{
	int acc_fd = *(int *)afd;
	char buffer[BUFF_SIZE];
	if(acc_fd < 0)
	{
		cout<<"connection accept failure\n";
		exit(0);
	}

	recv(acc_fd, buffer, BUFF_SIZE, 0);
	cout<<"client wants "<<buffer<<endl;
	string filepath (buffer);
	//cout<<"filepath is "<<filepath<<endl;        //got filepath
	
	FILE *fp = fopen ( buffer, "rb" );
	if(fp==NULL)
	{
		cout<<"cant open file\n";
		pthread_exit(NULL);
	}
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);                       //calculate filesize
	rewind(fp);
	cout<<"size of file is "<<size<<endl;
	send(acc_fd, &size, sizeof(size), 0);        //tell the size to other peer
	cout<<"sent filesize "<<size<<endl;
	ssize_t n;
	while ((n=fread( buffer , sizeof(char) , BUFF_SIZE, fp ) ) > 0  && size > 0 )
	{
			send (acc_fd, buffer, n, 0 );
	   	 	memset ( buffer , '\0', BUFF_SIZE);
			size = size - n ;
	}

	cout<<"sent file\n";
	close(acc_fd);
	pthread_exit(NULL);

}

void *serve_files(void *P)
{
	int PORT = *(int *)P;
	int tracker_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tracker_sock == -1)
	{
		cout<<"cannot create socket\n";
		exit(0);
	}
	char source[]="127.0.0.1";
	//char dest=
	//inet_pton(AF_INET, )

	struct sockaddr_in sadd;
	sadd.sin_family = AF_INET;
	sadd.sin_port = htons(5000);
	sadd.sin_addr.s_addr = INADDR_ANY;

	int succ;
	succ = bind(tracker_sock, (struct sockaddr *)&sadd, sizeof(sadd));
	if(succ != 0)
	{
		cout<<"binding failure\n";
		exit(0);
	}
	//cout<<"listening for new connections\n";
	succ = listen(tracker_sock, 5);
	//cout<<"new connection arrived\n";
	if(succ != 0)
	{
		cout<<"listen failure\n";
		exit(0);
	}

	pthread_t threads[THREAD_COUNT];
	int count = 0;

	while(1)
	{
		cout<<"waiting for connection request\n";
		int acc_fd = accept(tracker_sock, (struct sockaddr *)&sadd, (socklen_t*)&sadd);
		cout<<"incoming connection accepted\n";
		int succ = pthread_create( &threads[count], NULL, send_file, &acc_fd);

		++count;

		//pthread_exit(NULL);
	}

}

void *take_files(void *p)
{
	auto pack = *(pair<char *, char *> *)p;
	cout<<"got filepath as "<<pack.first<<endl;
	cout<<"got download path as "<<pack.second<<endl;
	char *argv[3];
	argv[1] = new char[100];
	argv[2] = new char[100];
	strcpy(argv[1], pack.first);
	strcpy(argv[2], pack.second);
	cout<<"got filepath as "<<argv[1]<<endl;
	cout<<"got download path as "<<argv[2]<<endl;
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(client_sock == -1)
	{
		cout<<"cannot open client socket\n";
		exit(0);
	}
	char buffer[BUFF_SIZE];
	struct sockaddr_in track_addr;
	track_addr.sin_family = AF_INET;
	track_addr.sin_port = htons(6234);
	inet_pton(AF_INET, "127.0.0.1", &track_addr.sin_addr);
	int succ = connect(client_sock, (struct sockaddr *)&track_addr, sizeof(track_addr));
	if(succ == -1)
	{
		cout<<"cannot connect to tracker\n";
		exit(0);
	}

	strcpy(buffer, argv[1]);
	cout<<"sending path "<<buffer<<" to server"<<endl;                                      
	send(client_sock, buffer, sizeof(buffer), 0);

	int fsize;
	recv(client_sock, &fsize, sizeof(fsize), 0);
	cout<<"received "<<fsize<<" as size of file\n";
	string filename = findname(argv[1]);
	string downpath = pack.second;
	string comppath = downpath+"/"+filename;
	cout<<filename<<" will be saved to "<<comppath<<endl;
	FILE *fp = fopen ( comppath.c_str(), "wb" );
	ssize_t n;

	while(( n = recv(client_sock, buffer, BUFF_SIZE, 0)) > 0  &&  fsize > 0)
	{	
		
		fwrite (buffer , sizeof(char), n, fp);
		memset ( buffer , '\0', BUFF_SIZE);
		fsize = fsize - n;
	} 
	cout<<"received\n";
	close(client_sock);
	delete argv[1];
	delete argv[2];
	pthread_exit(NULL);
}

int conv_to_num(char *carray)
{
	int number=0;
	for(int lv=strlen(carray)-1; lv>=0;lv--)
		number = number*10 + carray[lv]-'0';
	return number;
}

int main()
{
	pthread_t server_thread;
	int port_num ;//= conv_to_num(argv[1]);
	int succ = pthread_create(&server_thread, NULL, serve_files, &port_num);

	pthread_t client_threads[THREAD_COUNT];
	int count = 0;

	char ch;
	while(1)
	{
		cout<<"Enter path of file to be downloaded and download path:\n";
		char file[100], path[100];
		cin.getline(file, sizeof(file));
		cin.getline(path, sizeof(path));

		auto ele = make_pair(file, path);
		cout<<"sending filepath as "<<file<<endl;
		cout<<"sending download path as "<<path<<endl;
		pthread_create(&client_threads[count], NULL, take_files, &ele);
		++count;
	}

}