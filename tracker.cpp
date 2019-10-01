#include"header.h"

void *receive_file(void *afd)
{
	int acc_fd = *(int *)afd;
	char buffer[BUFF_SIZE];
	if(acc_fd < 0)
	{
		cout<<"connection accept failure\n";
		exit(0);
	}
	else
	{
		cout<<"conn accepted\n";
	}
	recv(acc_fd, buffer, BUFF_SIZE, 0);
	string filename (buffer);
	cout<<"filename is "<<filename<<endl;
	int fsize;
	recv(acc_fd, &fsize, sizeof(fsize), 0);
	cout<<"got filesize "<<fsize<<endl;

	string path = "/home/virat/Desktop/";
	path = path + filename; 
	cout<<"creating file at "<<path.c_str()<<endl;
	FILE *fp = fopen(path.c_str(), "wb");
	cout<<"opened/created\n";
	
	//ssize_t received = 0;
	ssize_t n;
	cout<<"receiving\n";
	while(( n = recv(acc_fd, buffer, BUFF_SIZE, 0)) > 0  &&  fsize > 0)
	{	
		
		fwrite (buffer , sizeof(char), n, fp);
		memset ( buffer , '\0', BUFF_SIZE);
		fsize = fsize - n;
	} 
	cout<<"received\n";

	/*while(received < fsize)
	{
		received += recv(acc_fd, &buffer, 100, 0);
		write(opf, buffer, 100);
	}*/ 
	chmod(path.c_str(), S_IRWXG|S_IRWXO|S_IRWXU);
	cout<<"closing file\n";
	
	fclose(fp);
	//close(opf);
	
	
	/*if(sizeof(buffer) == 0)
		break;
	cout<<"client :"<<buffer<<endl;
	cout<<"enter :";
	cin.getline(buffer, sizeof(buffer));
	cout<<"sending "<<buffer<<endl;
	send(acc_fd, buffer, sizeof(buffer), 0);*/
	close(acc_fd);
	pthread_exit(NULL);
	cout<<"ham chutiya hai\n";
}

int main()
{
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
	sadd.sin_port = htons(5245);
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

	
	
	
	//bool falg = true;
	//buffer[0]='\0';

	pthread_t threads[THREAD_COUNT];
	int count = 0;
	while(1)
	{
		cout<<"waiting for connection request\n";
		int acc_fd = accept(tracker_sock, (struct sockaddr *)&sadd, (socklen_t*)&sadd);

		int succ = pthread_create( &threads[count], NULL, receive_file, &acc_fd);
		
		++count;

		//pthread_exit(NULL);
	}
	
	
	close(tracker_sock);
}