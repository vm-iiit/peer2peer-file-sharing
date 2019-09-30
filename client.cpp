#include"header.h"

int main(int argc, char *argv[])
{
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(client_sock == -1)
	{
		cout<<"cannot open client socket\n";
		exit(0);
	}
	char Buffer[BUFF_SIZE];
	
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

	FILE *fp = fopen ( argv[1]  , "rb" );

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	rewind(fp);

	/*int ipf = open(argv[1], O_RDONLY);
	if(ipf < 0)
	{
		cout<<"cant open file\n";
		exit(0);
	}
	ssize_t fsize = lseek(ipf, 0, SEEK_END);
	cout<<"file size :"<<fsize<<endl;
	lseek(ipf, 0, SEEK_SET);
	ssize_t bytes_read=0;
	ssize_t currbytes;*/
	//send(client_sock, &fsize, sizeof(fsize), 0);
	send(client_sock, &size, sizeof(size), 0);
	ssize_t n;
	while ((n=fread( Buffer , sizeof(char) , BUFF_SIZE, fp ) ) > 0  && size > 0 )
	{
			send (client_sock, Buffer, n, 0 );
	   	 	memset ( Buffer , '\0', BUFF_SIZE);
			size = size - n ;
	}

	/*while(1)
	{
		currbytes = read(ipf, message, sizeof(message));
		if(currbytes == 0)
			break;
		bytes_read += currbytes;


		if(strcmp(message, "exit") == 0)
			flag = false;
		cout<<"sending chunk "<<endl;
		send(client_sock, &message, sizeof(message), 0);
		//succ = recv(client_sock, &message, 100, 0);
		//cout<<"tracker :"<<message<<endl;
	}*/
	cout<<"sent file\n";
	
	close(client_sock);

}