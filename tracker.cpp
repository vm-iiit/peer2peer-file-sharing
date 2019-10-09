#include"header.h"
pair<string, int> tinfo[num_trackers+1];
map<string, string> credentials;

void *req_handler(void *arg)
{
	int acc_fd = *(int *)arg;
	int choice;
	char buffer[BUFF_SIZE];
	while(1)
	{
		cout<<"waiting for request number\n";
		recv(acc_fd, &choice, sizeof(int), 0);
		if(choice < 1 || choice > 9)
			pthread_exit(NULL);
		switch(choice)
		{
			case 1: bool success;
				    recv(acc_fd, buffer, BUFF_SIZE, 0);
				    string usr(buffer);
				    cout<<"got username "<<usr<<endl;
				    recv(acc_fd, buffer, BUFF_SIZE, 0);
				    string pass(buffer);
				    cout<<"got password "<<pass<<endl;
				    if(credentials[usr] != "")
				        success = false;
				    else
				    {
				    	success = true;
				    	credentials[usr] = pass;
				    }
				    send(acc_fd, &success, sizeof(bool), 0);

		}
		cout<<"request served\n";
	}
	
	
}

void get_ip_port(char *filename)
{
	ifstream ipf;
	int port;
	ipf.open(filename);
	int lv=1;
	string temp;
	while(1)
	{
		ipf>>temp;
		if(ipf.tellg() == -1)
			break;
		int index = temp.find(":");
		port = stoi(temp.substr(index+1));
		temp = temp.substr(0, index);
		//cout<<"storing "<<temp<<" and "<<port<<" at "<<lv<<endl;
		tinfo[lv] = make_pair(temp, port);
		++lv;
	}
}

int main(int argc, char *argv[])
{
	credentials.clear();
	//int PORT = *(int *)P;
	int tracker_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tracker_sock == -1)
	{
		cout<<"cannot create socket\n";
		exit(0);
	}
	get_ip_port(argv[1]);
	string id(argv[2]);
	cout<<"initializing tracker#"<<argv[2]<<" with ip "<<tinfo[stoi(id)].first<<" and port "<<tinfo[stoi(id)].second<<endl;
	
	struct sockaddr_in sadd;
	sadd.sin_family = AF_INET;
	sadd.sin_port = htons(tinfo[stoi(id)].second);
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

	cout<<"tracker #"<<argv[2]<<" initiated\n";
	while(1)
	{
		cout<<"waiting for connection request\n";
		int acc_fd = accept(tracker_sock, (struct sockaddr *)&sadd, (socklen_t*)&sadd);
		cout<<"incoming connection accepted\n";
		int succ = pthread_create( &threads[count], NULL, req_handler, &acc_fd);

		++count;

		//pthread_exit(NULL);
	}
}