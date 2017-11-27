#include <vk/client.hpp>
#include <iostream>
#include <map>
#include <string>
#include <exception>
#include <thread>

int main() {

	setlocale(LC_ALL, "rus");
	std::string flag = "-v";
	std::string users_flag;
	int resp;
	std::string access_token;
	std::cout << "Hello! If you want to get the access_token please check the link below" << std::endl;
	std::cout << "https://oauth.vk.com/authorize?client_id=6191736&display=page&redirect_uri=http://vk.com/callback&scope=friends&response_type=token&v=5.59&state=1" << std::endl;
	std::cout << "Access_token: ";
	try {
		if (std::cin >> access_token) 
		{
			std::cout << "Loading..."<<std::endl;
		}
		else throw std::logic_error("Invalid argument!");
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}
	Vk::VkClient data({ { "token", access_token } });
		if (data.check_connection())
	{
		data.print_groups(data.get_groups());
		std::cout << "Please enter the number of threads: " << std::endl;
		int n;
		if (std::cin >> n)
		{
			std::cout << "Do you want to use flag? Enter 1 for 'yes', 0 for 'no'" << std::endl;
			if (std::cin >> resp) 
			{
				if (resp == 1)
				{
					std::cout << "Flag: ";
					std::cin >> users_flag;

					if (flag == users_flag)
					{
						data.start_streaming(n, resp);
					}
					else std::cout << "You should use -v as a flag";
				}
				else data.start_streaming(n, resp);
			}
		}
	}
	else std::cout << "Didn't manage to connect" << std::endl;
	system("pause");
	return 0;
}
