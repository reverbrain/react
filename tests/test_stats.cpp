#include <stdexcept>

#include "tests.hpp"

#include "react/react.hpp"

BOOST_AUTO_TEST_SUITE( stats_suite )

using namespace react;

BOOST_AUTO_TEST_CASE( add_stat_test )
{
	actions_set_t actions_set;
	call_tree_t call_tree(actions_set);

	call_tree.add_stat("bool", bool());
	call_tree.add_stat("int", int());
	call_tree.add_stat("double", double());
	call_tree.add_stat("string", std::string());
	call_tree.add_stat("char*", "");
}

BOOST_AUTO_TEST_CASE( get_stat_test )
{
	actions_set_t actions_set;
	call_tree_t call_tree(actions_set);

	call_tree.add_stat("bool", bool());
	BOOST_REQUIRE_EQUAL( call_tree.get_stat<bool>("bool"), bool() );

	call_tree.add_stat("int", int());
	BOOST_REQUIRE_EQUAL( call_tree.get_stat<int>("int"), int() );

	call_tree.add_stat("double", double());
	BOOST_REQUIRE_EQUAL( call_tree.get_stat<double>("double"), double() );

	call_tree.add_stat("string", std::string());
	BOOST_REQUIRE_EQUAL( call_tree.get_stat<std::string>("string"), std::string() );

	call_tree.add_stat("char*", "");
	BOOST_REQUIRE_EQUAL( call_tree.get_stat<std::string>("char*"), "" );
}

BOOST_AUTO_TEST_SUITE_END()


