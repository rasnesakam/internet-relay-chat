#include "../../inc/Commands.hpp"

int cmd::nick(const vector<string> &input, Server &srv, User& user)
{
	if (user.getActive() <= HALF)
		return (-1);
	user.setActive(ACTIVE);
	user.setNickName(input[1]);
	cout << "nick" << endl;
	return (0);

}
