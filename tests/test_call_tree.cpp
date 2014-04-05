#include "tests.hpp"

#include "react/call_tree.hpp"

BOOST_AUTO_TEST_SUITE( call_tree_suite )

using namespace react;

typedef void* dummy_type;

BOOST_AUTO_TEST_CASE( node_constructors_test )
{
	node_t<dummy_type> node(42);
	BOOST_CHECK_EQUAL( node.action_code, 42 );
}

BOOST_AUTO_TEST_CASE( unordered_node_constructors_test )
{
	unordered_node_t node(42);
	BOOST_CHECK_EQUAL( node.action_code, 42 );
	BOOST_CHECK_EQUAL( node.calls_number, 0 );
	BOOST_CHECK_EQUAL( node.time, 0 );
	BOOST_CHECK( node.links.empty() );
}

BOOST_AUTO_TEST_CASE( ordered_node_constructors_test )
{
	ordered_node_t node(42);
	BOOST_CHECK_EQUAL( node.action_code, 42 );
	BOOST_CHECK_EQUAL( node.start_time, 0 );
	BOOST_CHECK_EQUAL( node.stop_time, 0 );
	BOOST_CHECK( node.links.empty() );
}

BOOST_AUTO_TEST_CASE( call_tree_constructors_test )
{
	actions_set_t actions_set;
	int action_code = actions_set.define_new_action("ACTION");
	call_tree_t call_tree(actions_set);
	BOOST_CHECK_EQUAL( call_tree.get_node_action_code(call_tree.root),
					   +actions_set_t::NO_ACTION );
	BOOST_CHECK_EQUAL( call_tree.get_actions_set().get_action_name(action_code),
					   "ACTION" );
}

BOOST_AUTO_TEST_CASE( call_tree_node_time_test )
{
	actions_set_t actions_set;
	call_tree_t call_tree(actions_set);
	call_tree_t::p_node_t node = call_tree.root;

	BOOST_CHECK_EQUAL( call_tree.get_node_start_time(node), 0 );
	BOOST_CHECK_EQUAL( call_tree.get_node_stop_time(node), 0 );

	call_tree.set_node_start_time(node, 42);
	BOOST_CHECK_EQUAL( call_tree.get_node_start_time(node), 42 );
	BOOST_CHECK_EQUAL( call_tree.get_node_stop_time(node), 0 );

	call_tree.set_node_stop_time(node, 43);
	BOOST_CHECK_EQUAL( call_tree.get_node_start_time(node), 42 );
	BOOST_CHECK_EQUAL( call_tree.get_node_stop_time(node), 43 );
}

BOOST_AUTO_TEST_CASE( call_tree_add_new_link_test )
{
	actions_set_t actions_set;
	int action_code = actions_set.define_new_action("ACTION");
	call_tree_t call_tree(actions_set);
	call_tree_t::p_node_t node = call_tree.root;

	for (int i = 0; i < 10; ++i) {
		call_tree_t::p_node_t new_node = call_tree.add_new_link(node, action_code);
		BOOST_CHECK_NE( node, new_node );
		BOOST_CHECK_EQUAL( call_tree.get_node_action_code(new_node), action_code );
		BOOST_CHECK_EQUAL( call_tree.get_node_start_time(new_node), 0 );
		BOOST_CHECK_EQUAL( call_tree.get_node_stop_time(new_node), 0 );
		node = new_node;
	}
}

BOOST_AUTO_TEST_CASE( concurrent_call_tree_inner_tree_test )
{
	actions_set_t actions_set;
	int action_code = actions_set.define_new_action("ACTION");
	concurrent_call_tree_t concurrent_call_tree(actions_set);
	call_tree_t::p_node_t root =
			concurrent_call_tree.get_call_tree().root;
	call_tree_t::p_node_t node =
			concurrent_call_tree.get_call_tree().add_new_link(root, action_code);

	call_tree_t tree_copy = concurrent_call_tree.copy_call_tree();
	BOOST_CHECK_EQUAL( tree_copy.get_node_action_code(node), action_code );
}

BOOST_AUTO_TEST_SUITE_END()
