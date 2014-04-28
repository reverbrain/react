#include <stdexcept>

#include "../tests.hpp"

#include "react/aggregators/recent_trees_aggregator.hpp"
#include "react/utils.hpp"

BOOST_AUTO_TEST_SUITE( aggregators_suite )

using namespace react;

BOOST_AUTO_TEST_CASE( recent_trees_aggregator_test )
{
	const size_t RECENT_LIST_SIZE = 5;

	actions_set_t actions_set;
	recent_trees_aggregator_t aggregator(actions_set, RECENT_LIST_SIZE);

	boost::test_tools::output_test_stream output;

	size_t previous_length = 0;
	{
		cout_redirect guard(output.rdbuf());

		// Checking whether RECENT_LIST_SIZE limit is considered
		for (size_t i = 0; i < RECENT_LIST_SIZE * 2; ++i) {
			print_json(aggregator);

			call_tree_t call_tree(actions_set);
			aggregator.aggregate(call_tree);

			size_t length = output.str().size();

			if (i > RECENT_LIST_SIZE) {
				BOOST_CHECK_EQUAL( length, previous_length );
			} else {
				BOOST_CHECK_NE( length, previous_length );
			}

			previous_length = length;
			output.flush();
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
