#include <stdexcept>

#include "tests.hpp"

#include "react/aggregators/recent_trees_aggregator.hpp"

BOOST_AUTO_TEST_SUITE( aggregator_suite )

using namespace react;

BOOST_AUTO_TEST_CASE( recent_trees_aggregator_test )
{
	recent_trees_aggregator_t aggregator(2);
	actions_set_t actions_set;
	call_tree_t call_tree(actions_set);
	aggregator.aggregate(call_tree);
}

BOOST_AUTO_TEST_SUITE_END()

