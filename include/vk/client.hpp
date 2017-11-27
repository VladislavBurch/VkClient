#include <map>
#include <string>
#include <vk/json.hpp>
#include <iostream>
#include <curl/curl.h>
namespace Vk {
	class VkClient {
	public:
		using dict_t = std::map<std::string, std::string>;

		VkClient() {};
		VkClient(dict_t settings) : settings_(settings) {};
		auto check_connection() -> bool;
		auto get_groups()-> nlohmann::json;
		auto start_streaming(int n, int resp)->void;
		static auto do_threads(size_t i)-> void;
		static auto print_groups(nlohmann::json j_resp)->void;
		static auto func(char* ptr, size_t size, size_t nmemb, std::string* link)->size_t;
	private:
		dict_t settings_;
	};
}
