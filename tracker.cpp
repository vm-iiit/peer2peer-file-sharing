#include"header.h"
pair<string, int> tinfo[num_trackers+1];

void *req_handler(void *arg)
{
	cout<<"request served\n";
	pthread_exit(NULL);
}

void get_ip_port()
{
	ifstream ipf;
	int port;
	ipf.open("config");
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
	//int PORT = *(int *)P;
	int tracker_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tracker_sock == -1)
	{
		cout<<"cannot create socket\n";
		exit(0);
	}
	get_ip_port();
	string id(argv[1]);
	cout<<"initializing tracker#"<<argv[1]<<" with ip "<<tinfo[stoi(id)].first<<" and port "<<tinfo[stoi(id)].second<<endl;
	
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

	cout<<"tracker #"<<argv[1]<<" initiated\n";
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