#ifndef AGGREGATOR_HPP
#define AGGREGATOR_HPP

#include <list>

#include "../call_tree.hpp"

namespace react {

class aggregator_t {
public:
	aggregator_t() {}
	virtual ~aggregator_t() {}
	virtual void aggregate(const call_tree_t &call_tree) = 0;
	virtual void to_json(rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator) const = 0;
};

} // namespace react

#endif // AGGREGATOR_HPP
