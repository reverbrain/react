#include <thread>

#include "../react.hpp"

using namespace ioremap::react;

actions_set_t actions_set; // Define set of actions that will be monitored
const int ACTION_READ = actions_set.define_new_action("READ");
const int ACTION_FIND = actions_set.define_new_action("FIND");
const int ACTION_LOAD_FROM_DISK = actions_set.define_new_action("LOAD FROM DISK");
const int ACTION_READ_FROM_DISK = actions_set.define_new_action("READ FROM DISK");
const int ACTION_PUT_INTO_CACHE = actions_set.define_new_action("PUT INTO CACHE");
const int ACTION_LOAD_FROM_CACHE = actions_set.define_new_action("LOAD FROM CACHE");

concurrent_time_stats_tree_t time_stats(actions_set); // Call tree for storing statistics.
time_stats_updater_t updater(time_stats); // Updater for gathering of statistics.

// Defining stub functions
bool find_record() {
	std::this_thread::sleep_for( std::chrono::microseconds(10) );
	return (rand() % 4) == 0;
}

std::string read_from_disk() {
	action_guard read_from_disk_guard(&updater, ACTION_READ_FROM_DISK);

	std::this_thread::sleep_for( std::chrono::microseconds(1000) );
	return "DISK";
}

void put_into_cache(std::string data) {
	action_guard put_into_cache_guard(&updater, ACTION_PUT_INTO_CACHE);

	std::this_thread::sleep_for( std::chrono::microseconds(50) );
}

std::string load_from_cache() {
	action_guard load_from_cache_guard(&updater, ACTION_LOAD_FROM_CACHE);

	std::this_thread::sleep_for( std::chrono::microseconds(25) );
	return "CACHE";
}

std::string cache_read() {
	action_guard read_guard(&updater, ACTION_READ); // Creates new guard and starts action which will be stopped on guard's destructor

	std::string data;

	updater.start(ACTION_FIND); // Starts new action which will be inner to ACTION_READ
	bool found = find_record();
	updater.stop(ACTION_FIND);

	if (!found) {
		action_guard load_from_disk_guard(&updater, ACTION_LOAD_FROM_DISK);

		data = read_from_disk();
		put_into_cache(data);
		return data; // Here all action guards are destructed and actions are correctly finished
	}
	data = load_from_cache();

	return data;
}

void print_json() {
	rapidjson::Document doc;
	doc.SetObject();
	auto &allocator = doc.GetAllocator();

	time_stats.copy_time_stats_tree().to_json(doc, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string result = buffer.GetString();
	std::cout << result << std::endl;
}

const int ITERATIONS_NUMBER = 1000;

void run_example() {
	std::cout << "Running cache read " << ITERATIONS_NUMBER << " times" << std::endl;
	for (int i = 0; i < ITERATIONS_NUMBER; ++i) {
		std::string data = cache_read();
	}
	print_json();
}

int main() {
	run_example();
	return 0;
}
