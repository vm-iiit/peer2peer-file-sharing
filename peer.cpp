#include"header.h"

pair<string, int> tinfo[num_trackers+1];
int current_tracker;

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
		perror("accept");
		exit(0);
	}
	//cout<<"waiting to get filename\n";
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
	//cout<<"wil start at port "<<PORT<<endl;
	int tracker_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tracker_sock == -1)
	{
		cout<<"cannot create socket\n";
		exit(0);
	}
	//char source[]="127.0.0.1";
	//char dest=
	//inet_pton(AF_INET, )

	struct sockaddr_in sadd;
	sadd.sin_family = AF_INET;
	sadd.sin_port = htons(PORT);
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
	socklen_t sstruct;
	sstruct = sizeof(sadd);
	while(1)
	{
		//cout<<"waiting for connection request at port number "<<PORT<<endl;
		int acc_fd = accept(tracker_sock, (struct sockaddr *)&sadd, &sstruct);
		perror("accept");
		if(acc_fd < 0)
		{
			perror("accept");
			if(errno == EAGAIN or errno == EWOULDBLOCK)
				cout<<"EAGAIN\n";
			else if(errno == EBADF)
				cout<<"EBADF\n";
			else if(errno == ECONNABORTED)
				cout<<"ECONNABORTED\n";
			else if(errno == EBADF)
				cout<<"EBADF\n";
			else if(errno == EFAULT)
				cout<<"EFAULT\n";
			else if(errno == EINTR)
				cout<<"EINTR\n";
			else if(errno == EINTR)
				cout<<"EINTR\n";
			else if(errno == EINVAL)
				cout<<"EINVAL\n";
			else if(errno == ENOBUFS  || errno == ENOMEM)
				cout<<"ENOMEM\n";
			else if(errno == ENOTSOCK)
				cout<<"ENOTSOCK\n";
			else if(errno == EOPNOTSUPP)
				cout<<"EOPNOTSUPP\n";
			else if(errno == EPERM)
				cout<<"EPERM\n";
			else if(errno == EPROTO)
				cout<<"EPROTO\n";
			else
				cout<<"some other error\n";
		}
		cout<<"incoming connection for download accepted "<<acc_fd<<" sent"<<endl;
		int succ = pthread_create( &threads[count], NULL, send_file, &acc_fd);
		if(succ != 0)
			cout<<"couldn't create send file thread\n";
		++count;

		//pthread_exit(NULL);
	}

}

void *take_files(void *p)
{
	//cout<<"Inside take_files\n";
	srand(time(0));
	auto pack = *(vector<local_f> *)p;
	int ri = rand()%pack.size();

	local_f obj = pack[ri];

	char *argv[3];
	
	argv[1] = new char[BUFF_SIZE];
	argv[2] = new char[BUFF_SIZE];

	strcpy(argv[1], obj.path);
	strcpy(argv[2], obj.dpath);
	
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
	track_addr.sin_port = htons(obj.port);
	inet_pton(AF_INET, obj.ip, &track_addr.sin_addr);
	int succ = connect(client_sock, (struct sockaddr *)&track_addr, sizeof(track_addr));
	if(succ == -1)
	{
		cout<<"cannot connect to tracker\n";
		exit(0);
	}

	strcpy(buffer, argv[1]);
	buffer[strlen(buffer)] = '/';
	buffer[strlen(buffer)+1] = '\0';

	strcat(buffer, obj.fname);
	cout<<"sending path "<<buffer<<" to server"<<endl;                                      
	send(client_sock, buffer, sizeof(buffer), 0);

	int fsize;
	recv(client_sock, &fsize, sizeof(fsize), 0);
	cout<<"received "<<fsize<<" as size of file\n";
	string filename = findname(argv[1]);
	string downpath = argv[2];
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
	cout<<"file downloaded. Verifying integrity by SHA1\n";
	strcpy(argv[1], comppath.c_str());
	if(strcmp(calc_sha(argv[1]), obj.SHA_hash) == 0)
		cout<<"Integrity verified\n";
	else
		cout<<"SHA of original and downloaded file differs\n";
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

bool create_user(pair<string, string> p, int client_sock)
{
	int one=1;
	send(client_sock, &one, sizeof(int), 0);
	bool b;
	//cout<<"sending username "<<p.first<<endl;
	send(client_sock, p.first.c_str(), BUFF_SIZE, 0);
	//cout<<"sending password "<<p.second<<endl;
	send(client_sock, p.second.c_str(), BUFF_SIZE, 0);
	cout<<"waiting to receive\n";
	recv(client_sock, &b, sizeof(bool), 0);
	cout<<"returning\n";
	return b;
}

bool log_in(pair<string, string> p, int client_sock)
{
	int two=2;
	send(client_sock, &two, sizeof(int), 0);
	bool b;
	//cout<<"sending username "<<p.first<<endl;
	send(client_sock, p.first.c_str(), BUFF_SIZE, 0);
	//cout<<"sending password "<<p.second<<endl;
	send(client_sock, p.second.c_str(), BUFF_SIZE, 0);
	recv(client_sock, &b, sizeof(bool), 0);
	return b;
}

bool log_out(int client_sock)
{
	int twelve=12;
	send(client_sock, &twelve, sizeof(int), 0);
	//cout<<"sent 12\n";
	bool b;
	//cout<<"sending username "<<p.first<<endl;
	//send(client_sock, p.first.c_str(), BUFF_SIZE, 0);
	//cout<<"sending password "<<p.second<<endl;
	//send(client_sock, p.second.c_str(), BUFF_SIZE, 0);
	recv(client_sock, &b, sizeof(bool), 0);
	cout<<"received bool\n";
	return b;
}

bool create_group(int client_sock)
{
	int three=3;
	send(client_sock, &three, sizeof(int), 0);
	cout<<"Enter group id :";
	int i;
	cin>>i;
	send(client_sock, &i, sizeof(int), 0);
	bool b;
	recv(client_sock, &b, sizeof(bool), 0);
	//cout<<"received and returning "<<i<<endl;
	return b;
}

int main(int argc, char *argv[])
{
	char *sha_val;
	get_ip_port(argv[2]);
	srand(time(0));
	int tracker_id = rand()%num_trackers + 1;
	int t_port = tinfo[tracker_id].second;

	int client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(client_sock == -1)
	{
		cout<<"cannot open client socket\n";
		exit(0);
	}
	char buffer[BUFF_SIZE];
	struct sockaddr_in track_addr;
	track_addr.sin_family = AF_INET;
	track_addr.sin_port = htons(t_port);
	inet_pton(AF_INET, tinfo[tracker_id].first.c_str(), &track_addr.sin_addr);
	int succ = connect(client_sock, (struct sockaddr *)&track_addr, sizeof(track_addr));
	if(succ == -1)
	{
		cout<<"cannot connect to tracker\n";
		exit(0);
	}
	cout<<"connected to tracker #"<<tracker_id<<endl;
	pthread_t server_thread;
	int port_num ;//= conv_to_num(argv[1]);

	string params(argv[1]);
	string serv_port = params.substr(params.find(":")+1);
	string ip_addr = params.substr(0, params.find(":"));
	/*char temparray[20];
	strcpy(temparray, serv_port.c_str());*/
	//cout<<"obtained "<<serv_port<<" as string port\n";
	port_num = stoi(serv_port);

	//cout<<"sending "<<port_num<<" to server thread\n";
	cout<<"launching peer's server at port# "<<port_num<<endl;
	succ = pthread_create(&server_thread, NULL, serve_files, &port_num);
	if(succ != 0)
	{
		cout<<"cannot launch peer's server\n";
		exit(0);
	}
	pthread_t client_threads[THREAD_COUNT];
	int count = 0;
	int rq;
	char *temps;
	bool login_status = false;
	bool group = false;
	string luser="", stemp;
	string username = "", password, rpath;
	vector<int> group_ids ;
	int choice, g;
	pair<string, string> p;
	int fifteen = 15;
	int four = 4, five=5;
	int seven=7, six = 6, eight = 8, ten = 10, eleven=11;
	bool s;
	//cout<<"buffer being cleared\n";
	cin.clear();
	fflush(stdin);
	//cout<<"after buffer cleared\n";
	while(1)
	{
		cout<<"----------------------------------------\n\n";
		if(!login_status)
			cout<<"Not logged in\n";
		else
			cout<<"logged in as "<<luser<<endl;

		/*if(group_ids.size() == 0)
			cout<<"Group not assigned\n\n";
		else
		{
			cout<<"\nassociated group IDs :";
			for(int lv=0; lv<group_ids.size(); lv++)
				cout<<group_ids[lv]<<' ';
			cout<<endl<<endl;
		}*/
		
		if(login_status)
		{
			cout<<"3. Create group\n";
			cout<<"4. Join group\n";
			cout<<"5. Leave group\n";
			cout<<"6. List pending join requests\n";
			cout<<"7. Accept group join requests\n";
			cout<<"8. List all group in network\n";
			cout<<"9. List all sharable files in group\n";
			cout<<"10. Upload file\n";
			cout<<"11. Download file\n";
			cout<<"12. Logout\n";
			cout<<"13. Show downloads\n";
			cout<<"14. Stop sharing\n";
			cout<<"15. Exit\n\n";
		}
		
		else
		{
			cout<<"1. Create user/sign in\n";
			cout<<"2. Login\n";
			cout<<"15. Exit\n";
		}

		cout<<"Enter choice :";
		cin>>choice;
		cout<<endl;

		if(login_status && (choice <3 || choice >15))
		{
			cout<<"incorrect choice !!! try again\n";
			continue;
		}
		else if(!login_status && choice != 1 && choice != 2 && choice != 15)
		{
			
			cout<<"incorrect choice !!! try again\n";
			continue;
		}
		switch(choice)
		{
			case 1: cout<<"Enter username :";
					cin>>username;
					cout<<"\nEnter password :";
					cin>>password;
					cout<<endl;
					p = make_pair(username, password);
					if(create_user(p, client_sock))
						cout<<"User "<<username<<" successfully created\n";
					else
						cout<<"Unable to create user with this name/ username already exists\n";
					break;

			case 2: cout<<"\nEnter username :";
					cin>>username;
					cout<<"\nEnter password :";
					cin>>password;
					cout<<endl;
					p = make_pair(username, password);
					if(log_in(p, client_sock))
					{
						cout<<"login successful\n";
						login_status = true;
						luser = username;
						strcpy(buffer, ip_addr.c_str());
						send(client_sock, buffer, BUFF_SIZE, 0);
						send(client_sock, &port_num, sizeof(int), 0);
						cout<<"sent ip address and port number of server part\n";
					}
					else
					{
						cout<<"wrong password / YOu are already logged in \n";
					}
					break;

			case 3: g = create_group(client_sock);
					if(g)
					{
						cout<<"group successfully created\n";
						group = true;
						//cout<<"got group id "<<g<<endl;
						//group_ids.push_back(g);
					}
					else
						cout<<"cannot create group\n";
					break;

			case 4: send(client_sock, &four, sizeof(int), 0);
					cout<<"Enter id of group to be joined :";
					cin>>g;
					send(client_sock, &g, sizeof(int), 0);
					recv(client_sock, &s, sizeof(bool), 0);
					if(s == true)
						cout<<"\nGroup join request sent to owner\n";
					else
						cout<<"\nCould not process request/ User logged out\n";
					break;

			case 5: send(client_sock, &five, sizeof(int), 0);
					cout<<"Enter id of group to be left :";
					cin>>g;
					send(client_sock, &g, sizeof(int), 0);
					recv(client_sock, &s, sizeof(bool), 0);
					if(s)
						cout<<"Left group "<<g<<" successfully\n";
					else
						cout<<"cannot leave group\n";
					break;

			case 6: send(client_sock, &six, sizeof(int), 0);
					cout<<"Enter group id\n";
					cin>>g;
					send(client_sock, &g, sizeof(int), 0);
					recv(client_sock, &rq, sizeof(int), 0);
					cout<<"There are "<<rq<<" pending requests :";
					temps = (char *)malloc(sizeof(char)*100);
					while(rq--)
					{
						recv(client_sock, temps, 100, 0);
						cout<<temps<<' ';
					}
					delete temps;
					cout<<endl;
					break;

			case 7: send(client_sock, &seven, sizeof(int), 0);
					cout<<"Enter group id and user id to be joined :";
					cin>>g>>stemp;
					cout<<endl;
					temps = (char *)malloc(sizeof(char)*100);
					strcpy(temps, stemp.c_str());
					send(client_sock, &g, sizeof(int), 0);
					send(client_sock, temps, 100, 0);
					recv(client_sock, &s, sizeof(bool), 0);
					delete temps;
					if(s)
						cout<<"\nGroup join request accepted\n";
					else
						cout<<"\nCould not process request/ User logged out/Join req doesn't exist\n";
					break;

			case 8: send(client_sock, &eight, sizeof(int), 0);
					recv(client_sock, &g, sizeof(int), 0);
					cout<<"There are currently "<<g<<" groups in the network :";
					while(g--)
					{
						recv(client_sock, &rq, sizeof(int), 0);
						cout<<rq<<' ';
					}
					cout<<endl;
					break;

			case 10:send(client_sock, &ten, sizeof(int), 0);
					cout<<"Enter file with complete path to be uploaded :";
					cin.ignore();
					cin.getline(buffer, BUFF_SIZE);
					cout<<"\nEnter group id :";
					cin>>g;
					send(client_sock, buffer, BUFF_SIZE, 0);
					send(client_sock, &g, sizeof(int), 0);
					cout<<"calc SHA1 of "<<buffer<<endl;
					sha_val = calc_sha(buffer);
					cout<<"csending sha "<<sha_val<<endl;
					//strcpy(buffer, stemp.c_str());
					send(client_sock, sha_val, BUFF_SIZE, 0);
					recv(client_sock, &s, sizeof(bool), 0);
					if(s)
						cout<<"file successfully uploaded\n";
					else
						cout<<"could not upload the file\n";
					break;

			case 11:send(client_sock, &eleven, sizeof(int), 0);
					cout<<"Enter group id :";
					cin>>g;
					cout<<"\nEnter filename :";
					cin>>stemp;
					cout<<"\nEnter destination path :";
					cin>>rpath;
					send(client_sock, &g, sizeof(int), 0);
					send(client_sock, stemp.c_str(), BUFF_SIZE, 0);
					send(client_sock, rpath.c_str(), BUFF_SIZE, 0);
					recv(client_sock, &s, sizeof(bool), 0);
					if(s)
					{
						cout<<"\nDownloading the file\n";
						recv(client_sock, &g, sizeof(int), 0);
						cout<<endl<<g<<" users have the file\n";
						vector<local_f> tvec;
						local_f rec;
						while(g)
						{
							cout<<g<<"th iteration\n";
							recv(client_sock, buffer, BUFF_SIZE, 0);
							cout<<"recv 1\n";
							strcpy(rec.ip, buffer);
							recv(client_sock, &rq, sizeof(rq), 0);
							cout<<"recv 2\n";
							rec.port = rq;
							recv(client_sock, buffer, BUFF_SIZE, 0);
							cout<<"recv 3\n";
							strcpy(rec.path, buffer);
							recv(client_sock, buffer, BUFF_SIZE, 0);
							cout<<"recv 4\n";
							strcpy(rec.SHA_hash, buffer);
							strcpy(rec.fname, stemp.c_str());
							strcpy(rec.dpath, rpath.c_str());
							tvec.push_back(rec);
							--g;
						}
						cout<<"received the complete vector\n";
						pthread_t d_thread;
						pthread_create(&d_thread, NULL, take_files, &tvec);
						pthread_join(d_thread, NULL);
						cout<<"Download function called\n";
					}
					else
						cout<<"File not found in your group\n";
					break;

			case 12:cout<<"calling logout\n";
					if(log_out(client_sock))
					{
						login_status = false;
						luser = "";
						cout<<"logged out successfully\n";
						//group_ids.clear();
					}
					else
					{
						cout<<"unable to log out\n";
					}
					break;

			case 15:send(client_sock, &fifteen, sizeof(int), 0);
					close(client_sock);
					exit(0);
					break;

		}

		/*cout<<"Enter path of file to be downloaded and download path:\n";
		char file[100], path[100];
		cin.getline(file, sizeof(file));
		cin.getline(path, sizeof(path));

		auto ele = make_pair(file, path);
		cout<<"sending filepath as "<<file<<endl;
		cout<<"sending download path as "<<path<<endl;
		pthread_create(&client_threads[count], NULL, take_files, &ele);
		++count;*/
	}

}