#include <vk/client.hpp>
#include <iostream>
#include <string>
#include <exception> 
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <curl/curl.h>
#include <vk/json.hpp>
namespace Vk {
	std::mutex m;
	std::vector<std::thread> threads;
	std::vector<std::string> group_names;
	std::vector<int> group_ids;
	std::vector<int> group_privacy;
	auto VkClient::check_connection()-> bool
	{
		CURL *curl = curl_easy_init();
		if (curl) {
			std::string data_to_send = "access_token=" + settings_["token"] + "&v=5.59";
			CURLcode res;
			curl_easy_setopt(curl, CURLOPT_URL, "https://api.vk.com/method/account.getInfo?");
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_to_send.c_str());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_to_send.length());

			res = curl_easy_perform(curl);
			if (res == CURLE_OK) {
				long response_code;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
				std::cout << std::endl;
				std::cout << response_code << std::endl;
				return true;
			}
			curl_easy_cleanup(curl);
		}
		return false;
	};
	auto VkClient::get_groups()->nlohmann::json
	{

		CURL *curl = curl_easy_init();
		if (curl)
		{
			std::string data_to_send = "extended=1&offset=5&count=4&access_token=" + settings_["token"] + "&v=5.59";
			CURLcode res;
			std::string link = "";
			curl_easy_setopt(curl, CURLOPT_URL, "https://api.vk.com/method/groups.get?");
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_to_send.c_str());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_to_send.length());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, func);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &link);
			res = curl_easy_perform(curl);
			if (res == CURLE_OK)
			{
				try
				{
					nlohmann::json j_result = nlohmann::json::parse(link.c_str());
					nlohmann::json j_resp = j_result["response"];
					if (!j_resp.empty())
					{
						size_t count = j_resp["count"];
						return j_resp;
					}
					else
					{
						nlohmann::json j_err = j_result["error"];
						std::cerr << "error: " << j_err << std::endl;
						return nullptr;
					}
				}
				catch (std::exception&)
				{
					std::cerr << "Object is not JSON!!!" << std::endl;
				}
				return nullptr;
			};
			curl_easy_cleanup(curl);
		}

	};
	auto VkClient::print_groups(nlohmann::json j_resp)->void
	{
		if (!j_resp.empty())
		{
			size_t count = j_resp["count"];
			std::cout << "response {" << std::endl; 
			std::cout << "count: " << count << std::endl;
			if (count)
			{
				nlohmann::json g_r = j_resp["items"];
				for (nlohmann::json::iterator it = g_r.begin(); it != g_r.end(); ++it)
				{
					int g_id = it.value()["id"];
					group_ids.push_back(g_id);
					std::cout << "id: " << g_id << std::endl;
					std::string g_name = it.value()["name"];
					group_names.push_back(g_name);
					std::cout << "name: " << "'" << g_name << "'" << std::endl;
					std::string g_screen_name = it.value()["screen_name"];
					std::cout << "screen_name: " << "'" << g_screen_name << "'" << std::endl;
					int g_closed = it.value()["is_closed"];
					std::cout << "is_closed: " << g_closed << std::endl;
					group_privacy.push_back(g_closed);
					std::string g_type = it.value()["type"];
					std::cout << "type: " << "'" << g_type << "'" << std::endl;
				}
				std::cout << "}" << std::endl;
			}
		}
	}
	auto VkClient::do_threads(size_t i)-> void
	{
		    std::lock_guard<std::mutex> lck(m);
			std::time_t time_start = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::cout << "id: " << group_ids[i] << std::endl;
			std::cout << "name: " << group_names[i] << std::endl;
			std::cout << "is_closed: " << group_privacy[i] << std::endl;
			std::cout << "Время начала: " << ctime(&time_start) << std::endl;
	};

auto VkClient::start_streaming(int n, int resp)->void
	{
		auto hardware_conc = std::thread::hardware_concurrency();
		try {
			if (n >= 1 && n <= hardware_conc && group_ids.size() <= n)
			{
				for (int i = 0; i < n; ++i)
				{
					std::cout << "I'm  " << (i + 1) << " thread" << std::endl;
					threads.push_back(std::thread(do_threads, i));
					if (threads[i].joinable())
					{
						std::time_t time_start = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
						threads[i].join();
						std::time_t time_end = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
						if (resp == 1)
						{
							std::cout << "Время начала: " << ctime(&time_start) << std::endl;
							std::cout << "Время конца: " << ctime(&time_end) << std::endl;
						}

					};
				}
				
			}
			else throw std::logic_error("Invalid number of threads");
		   }
			catch (std::exception &e)
			{
				std::cerr << e.what();
			}
	}

	auto VkClient::func(char* ptr, size_t size, size_t nmemb, std::string* link) -> size_t
	{
		*link += ptr;
		return size*nmemb;
	}
}
