#include <stdexcept>

#include "tests.hpp"

#include "react/actions_set.hpp"

BOOST_AUTO_TEST_SUITE( actions_set_suite )

using namespace react;

BOOST_AUTO_TEST_CASE( define_new_action_test )
{
	actions_set_t actions_set;

	for (int i = 0; i < 10; ++i)
	{
		int action_code = actions_set.define_new_action("ACTION" + std::to_string(static_cast<long long>(i)));
		BOOST_CHECK_EQUAL( action_code, i );
	}
}

BOOST_AUTO_TEST_CASE( define_new_action_duplicates_test )
{
	actions_set_t actions_set;

	for (int i = 0; i < 10; ++i)
	{
		int action_code = actions_set.define_new_action("ACTION" + std::to_string(static_cast<long long>(i)));
		BOOST_CHECK_EQUAL( action_code, i );
	}

	for (int i = 0; i < 10; ++i)
	{
		int action_code = actions_set.define_new_action("ACTION" + std::to_string(static_cast<long long>(i)));
		BOOST_CHECK_EQUAL( action_code, i );
	}
}

BOOST_AUTO_TEST_CASE( get_action_name_test )
{
	actions_set_t actions_set;

	for (int i = 0; i < 10; ++i)
	{
		actions_set.define_new_action("ACTION" + std::to_string(static_cast<long long>(i)));
	}

	for (int i = 0; i < 10; ++i)
	{
		BOOST_CHECK_EQUAL( actions_set.get_action_name(i), "ACTION" + std::to_string(static_cast<long long>(i)) );
	}
}

BOOST_AUTO_TEST_CASE( get_action_name_out_of_bounds_test )
{
	actions_set_t actions_set;

	for (int i = 0; i < 10; ++i)
	{
		BOOST_CHECK_THROW( actions_set.get_action_name(i), std::invalid_argument );
	}

	actions_set.define_new_action("ACTION");
	BOOST_CHECK_NO_THROW( actions_set.get_action_name(0) );

	for (int i = 1; i < 10; ++i)
	{
		BOOST_CHECK_THROW( actions_set.get_action_name(i), std::invalid_argument );
	}
}

BOOST_AUTO_TEST_CASE( get_action_name_no_action_test )
{
	actions_set_t actions_set;

	BOOST_CHECK_THROW( actions_set.get_action_name(actions_set_t::NO_ACTION),
					   std::invalid_argument )
}

BOOST_AUTO_TEST_SUITE_END()
