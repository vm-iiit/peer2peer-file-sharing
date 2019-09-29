#include"header.h"

int main()
{
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(client_sock == -1)
	{
		cout<<"cannot open client socket\n";
		exit(0);
	}
	char message[100]="qwertyuiop";
	
	struct sockaddr_in track_addr;
	track_addr.sin_family = AF_INET;
	track_addr.sin_port = htons(5245);
	inet_pton(AF_INET, "127.0.0.1", &track_addr.sin_addr);
	int succ = connect(client_sock, (struct sockaddr *)&track_addr, sizeof(track_addr));
	if(succ == -1)
	{
		cout<<"cannot connect to tracker\n";
		exit(0);
	}
	bool flag = true;
	while(flag)
	{
		cout<<"enter :";
		cin.getline(message, sizeof(message));
		if(strcmp(message, "exit") == 0)
			flag = false;
		cout<<"sending "<<message<<endl;
		send(client_sock, &message, sizeof(message), 0);
		succ = recv(client_sock, &message, 100, 0);
		cout<<"tracker :"<<message<<endl;
	}
	
	close(client_sock);

}