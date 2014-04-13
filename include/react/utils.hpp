#ifndef UTILS_HPP
#define UTILS_HPP

namespace react {

template<typename T>
void print_json(const T &object) {
	rapidjson::Document doc;
	doc.SetObject();
	auto &allocator = doc.GetAllocator();

	object.to_json(doc, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string result = buffer.GetString();
	std::cout << result << std::endl;
}

} // namespace react

#endif // UTILS_HPP
