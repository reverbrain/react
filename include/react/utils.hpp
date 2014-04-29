#ifndef UTILS_HPP
#define UTILS_HPP

namespace react {

template<typename T>
std::string print_json_to_string(const T &object) {
	rapidjson::Document doc;
	doc.SetObject();
	auto &allocator = doc.GetAllocator();

	object.to_json(doc, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	return buffer.GetString();
}

template<typename T>
void print_json(const T &object) {
	std::cout << print_json_to_string(object);
}

} // namespace react

#endif // UTILS_HPP
