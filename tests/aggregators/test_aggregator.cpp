#include <stdexcept>

#include "../tests.hpp"

#include "react/aggregators/recent_trees_aggregator.hpp"
#include "react/aggregators/unordered_call_tree_aggregator.hpp"
#include "react/aggregators/histogram_aggregator.hpp"
#include "react/utils.hpp"

BOOST_AUTO_TEST_SUITE( aggregator_suite )

using namespace react;

BOOST_AUTO_TEST_CASE( recent_trees_aggregator_test )
{
	actions_set_t actions_set;
	recent_trees_aggregator_t aggregator(actions_set, 2);
	call_tree_t call_tree(actions_set);
	aggregator.aggregate(call_tree);
}

BOOST_AUTO_TEST_CASE( unordered_call_tree_aggregator_test )
{
	actions_set_t actions_set;
	unordered_call_tree_aggregator_t aggregator(actions_set);
	call_tree_t call_tree(actions_set);
	aggregator.aggregate(call_tree);
}

BOOST_AUTO_TEST_CASE( histogram1D_test )
{
	std::vector<int> ticks = {1, 2, 4, 8, 16};
	histogram1D_t histogram1D(ticks);
	histogram1D.update(0);
	histogram1D.update(1);

	BOOST_REQUIRE_EQUAL( histogram1D.get(0), 1 );
}

BOOST_AUTO_TEST_CASE( histogram2D_test )
{
	std::vector<int> ticks = {1, 2, 4, 8, 16};
	histogram2D_t histogram2D(ticks, ticks);
	histogram2D.update(0, 3);
	histogram2D.update(1, 3);
}

BOOST_AUTO_TEST_SUITE_END()
