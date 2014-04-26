#include "tests.hpp"

#include <array>

#include "react/updater.hpp"

BOOST_AUTO_TEST_SUITE( call_tree_updater_suite )

using namespace react;

const size_t DEFAULT_MAX_TRACE_DEPTH = +call_tree_updater_t::DEFAULT_MAX_TRACE_DEPTH;
const size_t NO_NODE = +call_tree_t::NO_NODE;
const int NO_ACTION = +actions_set_t::NO_ACTION;

BOOST_AUTO_TEST_CASE( call_tree_updater_constructors_test )
{
	// call_tree_updater_t()
	{
		call_tree_updater_t updater;
		BOOST_CHECK_EQUAL( updater.get_max_trace_depth(), DEFAULT_MAX_TRACE_DEPTH );
		BOOST_CHECK_EQUAL( updater.get_trace_depth(), 0 );
		BOOST_CHECK( !updater.has_call_tree() );
		BOOST_CHECK_THROW( updater.get_current_node_action_name(),
						   std::logic_error );
		BOOST_CHECK_THROW( updater.get_action_name(NO_ACTION),
						   std::logic_error );
		BOOST_CHECK_EQUAL( updater.get_current_node(), NO_NODE );
	}

	// call_tree_updater_t(size_t max_depth)
	{
		const size_t MAX_TRACE_DEPTH = 42;
		call_tree_updater_t updater(MAX_TRACE_DEPTH);
		BOOST_CHECK_EQUAL( updater.get_max_trace_depth(), MAX_TRACE_DEPTH );
		BOOST_CHECK_EQUAL( updater.get_trace_depth(), 0 );
		BOOST_CHECK( !updater.has_call_tree() );
		BOOST_CHECK_THROW( updater.get_current_node_action_name(),
						   std::logic_error );
		BOOST_CHECK_THROW( updater.get_action_name(NO_ACTION),
						   std::logic_error );
		BOOST_CHECK_EQUAL( updater.get_current_node(), NO_NODE );
	}

	// call_tree_updater_t(concurrent_call_tree_t &call_tree)
	{
		actions_set_t actions_set;
		concurrent_call_tree_t call_tree(actions_set);
		call_tree_updater_t updater(call_tree);
		BOOST_CHECK_EQUAL( updater.get_max_trace_depth(), DEFAULT_MAX_TRACE_DEPTH );
		BOOST_CHECK_EQUAL( updater.get_trace_depth(), 0 );
		BOOST_CHECK( updater.has_call_tree() );
		// Current node is root of the tree
		// Root node has no action code, so we throw logic_error
		BOOST_CHECK_THROW( updater.get_current_node_action_name(),
						   std::logic_error );
		BOOST_CHECK_THROW( updater.get_action_name(NO_ACTION),
						   std::invalid_argument );
		BOOST_CHECK_EQUAL( updater.get_current_node(),
						   call_tree.get_call_tree().root );
	}

	// call_tree_updater_t(concurrent_call_tree_t &call_tree,
	//                     size_t max_depth)
	{
		const size_t MAX_TRACE_DEPTH = 42;
		actions_set_t actions_set;
		concurrent_call_tree_t call_tree(actions_set);
		call_tree_updater_t updater(call_tree, MAX_TRACE_DEPTH);
		BOOST_CHECK_EQUAL( updater.get_max_trace_depth(), MAX_TRACE_DEPTH );
		BOOST_CHECK_EQUAL( updater.get_trace_depth(), 0 );
		BOOST_CHECK( updater.has_call_tree() );
		// Current node is root of the tree
		// Root node has no action code, so we throw logic_error
		BOOST_CHECK_THROW( updater.get_current_node_action_name(),
						   std::logic_error );
		BOOST_CHECK_THROW( updater.get_action_name(NO_ACTION),
						   std::invalid_argument );
		BOOST_CHECK_EQUAL( updater.get_current_node(),
						   call_tree.get_call_tree().root );
	}
}

BOOST_AUTO_TEST_CASE( call_tree_updater_set_max_trace_depth_test )
{
	call_tree_updater_t updater;
	const size_t MAX_TRACE_DEPTH = 42;
	updater.set_max_trace_depth(MAX_TRACE_DEPTH);
	BOOST_CHECK_EQUAL( updater.get_max_trace_depth(), MAX_TRACE_DEPTH );
}

BOOST_AUTO_TEST_CASE( call_tree_updater_set_call_tree_test )
{
	call_tree_updater_t updater;
	actions_set_t actions_set;
	concurrent_call_tree_t call_tree(actions_set);
	updater.set_call_tree(call_tree);

	BOOST_CHECK( updater.has_call_tree() );
	BOOST_CHECK_THROW( updater.get_current_node_action_name(),
					   std::logic_error );
	BOOST_CHECK_THROW( updater.get_action_name(NO_ACTION),
					   std::invalid_argument );
	BOOST_CHECK_EQUAL( updater.get_current_node(),
					   call_tree.get_call_tree().root );
}

BOOST_AUTO_TEST_CASE( call_tree_updater_reset_call_tree )
{
	actions_set_t actions_set;
	concurrent_call_tree_t call_tree(actions_set);
	call_tree_updater_t updater(call_tree);
	updater.reset_call_tree();

	BOOST_CHECK( !updater.has_call_tree() );
	BOOST_CHECK_THROW( updater.get_current_node_action_name(),
					   std::logic_error );
	BOOST_CHECK_THROW( updater.get_action_name(NO_ACTION),
					   std::logic_error );
	BOOST_CHECK_EQUAL( updater.get_current_node(), NO_NODE );
}

BOOST_AUTO_TEST_CASE( call_tree_updater_start_test )
{
	actions_set_t actions_set;
	int action_code = actions_set.define_new_action("ACTION");
	concurrent_call_tree_t call_tree(actions_set);
	call_tree_updater_t updater(call_tree);

	updater.start(action_code);
	BOOST_CHECK_EQUAL( updater.get_trace_depth(), 1 );
	BOOST_CHECK_NE( updater.get_current_node(), NO_NODE );
	BOOST_CHECK_EQUAL( updater.get_current_node_action_name(), "ACTION" );
	updater.stop(action_code);
}

BOOST_AUTO_TEST_CASE( call_tree_updater_start_invalid_action_test )
{
	actions_set_t actions_set;
	concurrent_call_tree_t call_tree(actions_set);
	call_tree_updater_t updater(call_tree);

	BOOST_CHECK_THROW( updater.start(NO_ACTION),
					   std::invalid_argument );

	BOOST_CHECK_EQUAL( updater.get_trace_depth(), 0 );
	BOOST_CHECK_THROW( updater.get_current_node_action_name(),
					   std::logic_error );
}

BOOST_AUTO_TEST_CASE( call_tree_updater_stop_test )
{
	actions_set_t actions_set;
	int action_code = actions_set.define_new_action("ACTION");
	concurrent_call_tree_t call_tree(actions_set);
	call_tree_updater_t updater(call_tree);

	updater.start(action_code);
	updater.stop(action_code);

	BOOST_CHECK_EQUAL( updater.get_trace_depth(), 0 );
	BOOST_CHECK_THROW( updater.get_current_node_action_name(),
					   std::logic_error );
	BOOST_CHECK_EQUAL( updater.get_current_node(),
					   call_tree.get_call_tree().root );
}

BOOST_AUTO_TEST_CASE( call_tree_updater_stop_invalid_action_test )
{
	actions_set_t actions_set;
	int action_code = actions_set.define_new_action("ACTION");
	int another_action_code = actions_set.define_new_action("ANOTHER_ACTION");
	concurrent_call_tree_t call_tree(actions_set);
	call_tree_updater_t updater(call_tree);

	updater.start(action_code);

	BOOST_CHECK_THROW( updater.stop(another_action_code),
					   std::logic_error );

	BOOST_CHECK_THROW( updater.stop(NO_ACTION),
					   std::invalid_argument );

	BOOST_CHECK_EQUAL( updater.get_trace_depth(), 1 );

	updater.stop(action_code);
}

BOOST_AUTO_TEST_CASE( call_tree_updater_start_stop_max_depth_test )
{
	actions_set_t actions_set;
	int action_code = actions_set.define_new_action("ACTION");
	concurrent_call_tree_t call_tree(actions_set);
	call_tree_updater_t updater(call_tree);

	updater.set_max_trace_depth(0);
	updater.start(action_code);
	BOOST_CHECK_EQUAL( updater.get_trace_depth(), 1 );
	BOOST_CHECK_EQUAL( updater.get_actual_trace_depth(), 0 );

	updater.stop(action_code);
	BOOST_CHECK_EQUAL( updater.get_trace_depth(), 0 );
	BOOST_CHECK_EQUAL( updater.get_actual_trace_depth(), 0 );

	updater.set_max_trace_depth(1);
	updater.start(action_code);
	BOOST_CHECK_EQUAL( updater.get_trace_depth(), 1 );
	BOOST_CHECK_EQUAL( updater.get_actual_trace_depth(), 1 );

	updater.stop(action_code);
	BOOST_CHECK_EQUAL( updater.get_trace_depth(), 0 );
	BOOST_CHECK_EQUAL( updater.get_actual_trace_depth(), 0 );
}

BOOST_AUTO_TEST_CASE( action_guard_constructors_test )
{
	{
		action_guard_t action_guard(NULL, NO_ACTION);
	}

	{
		actions_set_t actions_set;
		int action_code = actions_set.define_new_action("ACTION");
		concurrent_call_tree_t call_tree(actions_set);
		call_tree_updater_t updater(call_tree);

		{
			action_guard_t action_guard(&updater, action_code);
		}
		{
			BOOST_CHECK_THROW(
				action_guard_t action_guard(&updater, NO_ACTION),
				std::invalid_argument
			);
		}
	}
}

BOOST_AUTO_TEST_CASE( action_guard_stop_test )
{
	{
		action_guard_t action_guard(NULL, NO_ACTION);
		action_guard.stop();
		BOOST_CHECK_THROW( action_guard.stop(), std::logic_error )
	}

	{
		actions_set_t actions_set;
		int action_code = actions_set.define_new_action("ACTION");
		concurrent_call_tree_t call_tree(actions_set);
		call_tree_updater_t updater(call_tree);

		action_guard_t action_guard(&updater, action_code);
		action_guard.stop();
		BOOST_CHECK_THROW( action_guard.stop(), std::logic_error )
	}
}

BOOST_AUTO_TEST_SUITE_END()
