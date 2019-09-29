#include"header.h"
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

	succ = listen(tracker_sock, 5);
	if(succ != 0)
	{
		cout<<"listen failure\n";
		exit(0);
	}

	int acc_fd = accept(tracker_sock, (struct sockaddr *)&sadd, (socklen_t*)&sadd);
	if(acc_fd < 0)
	{
		cout<<"connection accept failure\n";
		exit(0);
	}
	else
	{
		cout<<"conn accepted\n";
	}
	
	char buffer[100];
	//bool falg = true;
	//buffer[0]='\0';
	while(1)
	{
		succ = recv(acc_fd, &buffer, 100, 0);
		if(strcmp(buffer, "exit") == 0)
			break;
		cout<<"client :"<<buffer<<endl;
		cout<<"enter :";
		cin.getline(buffer, sizeof(buffer));
		cout<<"sending "<<buffer<<endl;
		send(acc_fd, buffer, sizeof(buffer), 0);
	}
	
	close(acc_fd);
	close(tracker_sock);
}