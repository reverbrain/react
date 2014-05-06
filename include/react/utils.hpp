/*
* 2014+ Copyright (c) Andrey Kashin <kashin.andrej@gmail.com>
* All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*/

#ifndef REACT_UTILS_HPP
#define REACT_UTILS_HPP

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

#endif // REACT_UTILS_HPP
