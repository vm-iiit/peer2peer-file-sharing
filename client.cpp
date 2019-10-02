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

int main(int argc, char *argv[])
{
	//1st cmdline argument - file to be downloaded
	//2nd cmdline arg - path where to download
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(client_sock == -1)
	{
		cout<<"cannot open client socket\n";
		exit(0);
	}
	char buffer[BUFF_SIZE];
	
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
	
	strcpy(buffer, argv[1]);
	cout<<"sending path "<<buffer<<" to server"<<endl;                                      
	send(client_sock, buffer, sizeof(buffer), 0);

	int fsize;
	recv(client_sock, &fsize, sizeof(fsize), 0);
	cout<<"received "<<fsize<<" as size of file\n";
	string filename = findname(argv[1]);
	string downpath (argv[2]);
	string comppath = downpath+filename;
	cout<<filename<<" will be saved to "<<comppath<<endl;
	FILE *fp = fopen ( comppath.c_str(), "wb" );
	/*
	
	
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	rewind(fp);
	char fn[BUFF_SIZE];

	int lv;
	for(lv=0; filename[lv] != '\0'; lv++)
		fn[lv] = filename[lv];
	fn[lv] = '\0';

	cout<<"filename is "<<fn<<endl;
	
	send(client_sock, &size, sizeof(size), 0);
	cout<<"sent filesize "<<size<<endl;*/
	ssize_t n;

	while(( n = recv(client_sock, buffer, BUFF_SIZE, 0)) > 0  &&  fsize > 0)
	{	
		
		fwrite (buffer , sizeof(char), n, fp);
		memset ( buffer , '\0', BUFF_SIZE);
		fsize = fsize - n;
	} 
	cout<<"received\n";
	/*while ((n=fread( buffer , sizeof(char) , BUFF_SIZE, fp ) ) > 0  && size > 0 )
	{
			send (client_sock, buffer, n, 0 );
	   	 	memset ( buffer , '\0', BUFF_SIZE);
			size = size - n ;
	}*/

	
	//cout<<"sent file\n";
	
	close(client_sock);

}