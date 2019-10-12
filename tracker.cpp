#include"header.h"

pair<string, int> tinfo[num_trackers+1];
map<string, string> credentials;
map<string, bool> logged_in;
map<string, bool>::iterator mi_log;
map<string, vector<int>> ownership;
map<string, vector<int>>::iterator mi_ownership;
map<string, vector<int>> membership;
map<string, vector<int>>::iterator mi_membership;
map<int,string> g_owner;
map<int, vector<string>> g_members;
int empty_group ;
list<string> group_members[group_limit+1];
map<int, vector<string>> requests;
map<int, vector<string>>::iterator ri;
vector<string>::iterator vsi;

void load_credentials()
{
	ifstream cred_file;
	ofstream temp;
	string u, p;
	credentials.clear();
	cred_file.open("credentials.txt");
	if(!cred_file)
	{
		cout<<"creating credentials file\n";
		temp.open("credentials.txt");
		cred_file.open("credentials.txt");

	}
		while(1)
		{
			cred_file>>u;
			if(cred_file.tellg() == -1)
				break;
			else
			{
				cred_file>>p;
				credentials[u] = p;
			}
		}
		cred_file.close();	
	
	//cout<<"credentials map loaded\n";
}

void load_login()
{
	ifstream login_file;
	ofstream temp;
	string u;
	bool b;
	logged_in.clear();
	login_file.open("login_status.txt");
	if(!login_file)
	{
		cout<<"creating login status file\n";
		temp.open("login_status.txt");
		login_file.open("credentials.txt");

	}
		while(1)
		{
			login_file>>u;
			if(login_file.tellg() == -1)
				break;
			else
			{
				login_file>>b;
				logged_in[u] = b;
			}
		}
		login_file.close();
}

void update_login()
{
	ofstream login_file;
	string u;
	bool b;
	login_file.open("login_status.txt", ios::trunc);
	for(mi_log = logged_in.begin(); mi_log != logged_in.end(); mi_log++)
	{
		login_file<<mi_log->first<<' '<<mi_log->second<<endl;
	}
	login_file.close();
}

void update_membership()
{
	ofstream f_memb;
	f_memb.open("membership.txt", ios::trunc);
	for(mi_membership = membership.begin(); mi_membership != membership.end(); mi_membership++)
	{
		f_memb<<mi_membership->first<<' ';
		for(int lv = 0; lv<membership[mi_membership->first].size(); lv++)
			f_memb<<membership[mi_membership->first][lv]<<' ';
		f_memb<<endl;
	}
	f_memb.close();
	//cout<<"update done\n";
}

void read_memberships()
{
	ifstream member_f;
	string str, entry;
	char *temp, *U;
	char *extra;
	member_f.open("membership.txt");
	if(!member_f.good())
	{
		ofstream t;
		t.open("membership.txt");
		t.close();
		member_f.open("membership.txt");
	}
	membership.clear();
	g_members.clear();
	int i;
	while(getline(member_f, str, '\n'))
	{
		extra = &str[0];
		U = strtok(extra, " ");
		entry.assign(U);
		while(U)
		{
			U = strtok(NULL, " ");
			if(U)
			{
				i = atoi(U);
				membership[entry].push_back(i);
				g_members[i].push_back(entry);
			}
		}
	}
	member_f.close();
	//cout<<"read done\n";
}

void update_ownerships()
{
	ofstream f_memb;
	f_memb.open("ownership.txt", ios::trunc);
	for(mi_ownership = ownership.begin(); mi_ownership != ownership.end(); mi_ownership++)
	{
		f_memb<<mi_ownership->first<<' ';
		for(int lv = 0; lv<ownership[mi_ownership->first].size(); lv++)
			f_memb<<ownership[mi_ownership->first][lv]<<' ';
		f_memb<<endl;
	}
	f_memb.close();
	//cout<<"update done\n";
}

void read_ownership()
{
	ifstream member_f;
	string str, entry;
	char *temp, *U;
	char *extra;
	member_f.open("ownership.txt");
	ownership.clear();
	g_owner.clear();
	int i;
	while(getline(member_f, str, '\n'))
	{
		extra = &str[0];
		U = strtok(extra, " ");
		entry.assign(U);
		while(U)
		{
			U = strtok(NULL, " ");
			if(U)
			{
				i = atoi(U);
				ownership[entry].push_back(i);
				g_owner[i] = entry;
			}
		}
	}
	member_f.close();
	//cout<<"read done\n";
}

void load_join_requests()
{
	requests.clear();
	ifstream irq;
	irq.open("join_requests.txt");
	if(!irq.good())
	{
		ofstream tmp;
		tmp.open("join_requests.txt");
		tmp.close();
		irq.open("join_requests.txt");
	}
	string line;
	char *s;
	char *extra;
	string t;
	while(getline(irq, line, '\n'))
	{
		extra = &line[0];
		s = strtok(extra, " ");
		int var = atoi(s);
		while(s)
		{
			s = strtok(NULL, " ");
			if(s)
			{
				t.assign(s);
				requests[var].push_back(s);
			}
		}
	}
	irq.close();
}

void update_join_requests()
{
	ofstream orq;
	orq.open("join_requests.txt", ios::trunc);
	for(auto it = requests.begin(); it != requests.end(); it++)
	{
		orq<<it->first<<' ';
		for(int lv=0; lv < requests[it->first].size(); lv++)
			orq<<requests[it->first][lv]<<' ';
		orq<<endl;
	}
	orq.close();
}


void *req_handler(void *arg)
{
	int acc_fd = *(int *)arg;
	int choice;
	char buffer[BUFF_SIZE];
	bool success;
	string usr, pass, jt;
	int g;
	string ruser;
	while(1)
	{
		cout<<"\n\nwaiting for "<<usr<<"'s request\n";
		recv(acc_fd, &choice, sizeof(int), 0);
		
		switch(choice)
		{
			case 1: load_credentials();
					cout<<"account creation request\n";
					recv(acc_fd, buffer, BUFF_SIZE, 0);
				    usr.assign(buffer);
				    //cout<<"got username "<<usr<<endl;
				    recv(acc_fd, buffer, BUFF_SIZE, 0);
				    pass.assign(buffer);
				    //cout<<"got password "<<pass<<endl;
				    if(credentials[usr] != "")
				        success = false;
				    else
				    {
				    	success = true;
				    	credentials[usr] = pass;
				    	ofstream cred_file;
				    	cred_file.open("credentials.txt", ios::app);
				    	if(!cred_file.good())
				    		cout<<"cannot write to file\n";
				    	else
				    	{
				    		cred_file.seekp(ios_base::end);
				    		cred_file<<usr<<' '<<pass<<"\n";
				    		//cout<<"wrote creds in file\n";
				    		cred_file.close();	
				    	}

				    	ofstream login_file;
				    	login_file.open("login_status.txt", ios::app);
				    	login_file.seekp(ios_base::end);
				    	login_file<<usr<<' '<<0<<"\n";
				    	login_file.close();	
				    }
				    send(acc_fd, &success, sizeof(bool), 0);
				    update_ownerships();
				    break;

			case 2: load_credentials();
					load_login();
					cout<<"login request\n";
					recv(acc_fd, buffer, BUFF_SIZE, 0);
				    usr.assign(buffer);
				    //cout<<"got username "<<usr<<endl;
				    recv(acc_fd, buffer, BUFF_SIZE, 0);
				    pass.assign(buffer);
				    //cout<<"got password "<<pass<<endl;
				    if(credentials[usr] == pass)
				    {
				    	success = true;
				    	load_login();
				    	logged_in[usr] = true;
				    	update_login();
				    }
				    else
				    {
				    	success = false;
				    	//credentials[usr] = pass;
				    }
				    send(acc_fd, &success, sizeof(bool), 0);
				    break;

			case 3: cout<<"Group creation request\n";
					load_login();
					if(logged_in[usr])
					{	
						read_ownership();
						int empty_group;
						recv(acc_fd, &empty_group, sizeof(empty_group), 0);
						if(g_owner[empty_group] == "")
						{
							ownership[usr].push_back(empty_group);
							update_ownerships();
							cout<<"sending "<<empty_group<<"\n";
							send(acc_fd, &empty_group, sizeof(int), 0);
							
							ofstream own_ind;
							own_ind.open("owners_indexed.txt", ios::app);
							own_ind<<empty_group<<' '<<usr<<endl;
							own_ind.close();
							g_owner[empty_group] = usr;

						}
						else
						{
							bool b= false;
							send(acc_fd, &(b), sizeof(bool), 0);		
						}					
					}
					else
					{
						bool b= false;
						send(acc_fd, &(b), sizeof(bool), 0);
					}
					break;

			case 4: read_ownership();
					cout<<"Group join request\n";
					load_login();
					if(logged_in[usr])
					{
						int gid;
						recv(acc_fd, &gid, sizeof(int), 0);
						if(g_owner[gid] == "")
						{
							success = false;
							cout<<"failure, group doesn't exist\n";
							send(acc_fd, &success, sizeof(bool), 0);
						}
						else
						{
							load_join_requests();
							requests[gid].push_back(usr);
							//cout<<"updating join_requests file\n";
							update_join_requests();
							success = true;
							send(acc_fd, &success, sizeof(bool), 0);
							cout<<"sent group join request\n";
						}
					}
					else
					{
						int gid;
						recv(acc_fd, &gid, sizeof(int), 0);
						success = false;
						send(acc_fd, &success, sizeof(bool), 0);
					}
					break;

			case 7: recv(acc_fd, &g, sizeof(int), 0);
					char tt[100];
					recv(acc_fd, &tt, 100, 0);
					load_join_requests();
					jt.assign(tt);
					vsi = find(requests[g].begin(), requests[g].end(), jt);
					load_login();
					read_ownership();
					if(vsi == requests[g].end() || g_owner[g] != usr || logged_in[usr] == false)
					{
						if(vsi == requests[g].end())
							cout<<"not found in vector\n";
						if(g_owner[g] != usr)
							cout<<"current user is not the owner\n";
						if(logged_in[usr] == false)
							cout<<"user not logged in\n";
						success = false;
					}
					else
					{
						load_join_requests();
						requests[g].erase(vsi);
						update_join_requests();
						read_memberships();
						membership[jt].push_back(g);
						g_members[g].push_back(jt);
						update_membership();
						success = true;
						cout<<"join request accepted\n";
					}
					send(acc_fd, &success, sizeof(bool), 0);
					break;

			case 12:load_login();
					if(logged_in[usr] == true)
					{
						logged_in[usr] = false;
						update_login();
						success = true;
					}
					else
						success = false;
					cout<<"sending\n";
					send(acc_fd, &success, sizeof(bool), 0);
					cout<<"sent\n";
					break;

			case 15:close(acc_fd);
					cout<<"peer closed connection\n";
					load_login();
					logged_in[usr] = false;
					update_login();
					pthread_exit(NULL);
		}
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