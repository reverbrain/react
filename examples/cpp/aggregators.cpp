#include "react/react.hpp"
#include "react/aggregators/recent_trees_aggregator.hpp"
#include "react/utils.hpp"

int action_code = react_define_new_action("ACTION");

int main() {
	react_context_t *context = react_activate();
	react::recent_trees_aggregator_t aggregator(3);

	aggregator.aggregate(react::get_react_context_call_tree(context));

	react_start_action(action_code);
	react_stop_action(action_code);
	aggregator.aggregate(react::get_react_context_call_tree(context));

	react_start_action(action_code);
	react_stop_action(action_code);
	react_start_action(action_code);
	react_stop_action(action_code);
	aggregator.aggregate(react::get_react_context_call_tree(context));

	react_start_action(action_code);
	react_stop_action(action_code);
	aggregator.aggregate(react::get_react_context_call_tree(context));

	print_json(aggregator);

	react_deactivate(context);
	return 0;
}
