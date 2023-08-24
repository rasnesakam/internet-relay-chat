#include <Server.hpp>
#include <Commands.hpp>

Server::Server() {}

Server::~Server() { cout << "it is not done but work destructor" << endl; }

const string						Server::getPassword() const { return (this->password); }

map< string, vector<Person *> >&	Server::getChannels() { return (this->channels); }

vector<Person *>&					Server::getChannel(const string &channel) { return (this->channels[channel]); }

string&								Server::getRawString() { return (raw_string); }

string&								Server::getHostname() { return (hostname); }

vector<Person *>					Server::getUsers() 
{ 
	vector<Person *> persons;

	for (int i = 0; i < int(users.size()); i++){
		if (this->users[i])
			persons.push_back(this->users[i]);
		else
			persons.push_back(NULL);
	}
	return persons;
}

Person*		Server::getUserNick(string nick)
{
	if (users.size() == 0)
		return (NULL);
	for (int i = 0; i < int(users.size()); i++)
	{
		if (users[i] && users[i]->getNickName() == nick)
			return (users[i]);
	}
	return (NULL);
}

Person*   Server::getOrCreateUser(int fd)
{
	if (int(users.size()) <= fd || users[fd] == 0)
		users[fd] = new Person(fd);
	return (users[fd]);
}

void	Server::setRawString(string set) { raw_string = set; }

void	Server::setPort(int port) { this->port = port; }

void	Server::setPassword(string pass) { this->password = pass; }

void	Server::setHostname()
{
	char	hostname_c[1024];
	int		return_number = gethostname(hostname_c, 1024);
	checkSocket(return_number, "gethostname");
	this->hostname = hostname_c;


}

void	Server::deleteUser(int fd)
{
	if (this->users[fd] != 0)
	{
		vector<string>&	wh_op = this->users[fd]->getWhichChannel();

		cout << "fora girecek" << endl;
		for (int i = 0; i < int(wh_op.size()); i++)
		{
			cout << "for icine girdi" << endl;
			removeUserFrom(wh_op[i], *this->users[fd]);
			cout << "removeUserFrom bitti" << endl;
		}
		cout << "this is image" << endl;
		if (!this->users[fd])
		{
			cout << "before delete users" << endl;
			delete this->users[fd];
			cout << "after delete users" << endl;
		}
		cout << "end of the image" << endl;
		this->users[fd] = NULL;
	}
	else
		cout << "bana dahi ugramadi" << endl;
}

void	Server::removeUserFrom(const string &channel, Person &user)
{
	int fd = user.getFd();
	int i = 0;

	cout << "in remove funcition " << endl;
	for (; i < int(channels[channel].size()); i++)
	{
		cout << "ben buradayim" << endl;
		cout << "FD: " << channels[channel][i]->getFd() << "-----" << fd << endl;
		if (channels[channel][i]->getFd() == fd)
		{
			channels[channel].erase(channels[channel].begin() + i);
			cout << "kor musun" << endl;
			break;
		}
		cout << "sen neredesin" << endl;
	}
	cout << "Channel name : " << channel << endl;
	cout << "channel(size) : " << channels.size() << endl;
	cout << "index : " << i << endl;
	cout << "ciktim" << endl;
	cout << "bittim amk" << endl;
}

void	Server::addUserTo(const string &group, Person &user) 
{
	if (!find_channel(channels[group], user.getNickName()))
		Response::createMessage().from(user).to(user).content("JOIN").addContent(group).send();
	else
	{
		Response::withCode(ERR_USERONCHANNEL).to(user).content(user.getNickName() + " " + group + ER_ALREADY_JOIN).send();
		return ;
	}
	if (channels[group].size() == 0)
		Response::createMessage().from(user).to(user).content("MODE").addContent(group + " +o " + user.getNickName()).send();

	user.addOperator(group);
	channels[group].push_back((Person *)&user);
	string nickname = user.getNickName();
	vector<Person *> users = channels[group];

	for (int i = 0; i != int(users.size()); i++)
	{		
		Response::createMessage().from(user).to(*users[i]).content("JOIN").addContent(group).send();
	}
	Response::createReply(RPL_NAMEREPLY).to(user).addContent("= " + group + showInChannelNames(channels[group])).send();
	Response::createReply(RPL_ENDOFNAMES).to(user).addContent(group + " :End of /NAMES list").send();
}

void	Server::toBegin()
{
	cout << "Port: " << port << endl;
	cout << "Password: " << password << endl;
	setHostname();
	printServer("Server started ");
	setUpSocket();
}
