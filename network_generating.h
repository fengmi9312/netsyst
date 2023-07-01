#pragma once
#include "elem.h"


class er_network_generating : public impl
{
private:
	size_t node_amount;
	size_t link_amount;
	double link_probability;
	node* (*node_generating)();
	link* (*link_generating)();
	bool connecting_type;
	bool direct;
	er_network_generating(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
	friend impl* create_er_network_genrating(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
public:
	void set_params(size_t _node_amount, size_t _link_amount, double _link_probability, node* (*_node_generating)(), link* (*_link_generating)(), 
		bool _connecting_type = true, bool _direct = false);
	bool implement();
	size_t get_node_amount() const;
	size_t get_link_amount() const;
	double get_link_probability() const;
	bool get_connecting_type() const;
	bool get_direct() const;
};
impl* create_er_network_genrating(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);

class ws_network_generating : public impl
{
private:
	size_t node_amount;
	size_t k;
	double beta;
	node* (*node_generating)();
	link* (*link_generating)();
	ws_network_generating(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
	friend impl* create_ws_network_genrating(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
public:
	void set_params(size_t _node_amount, size_t _k,  double _beta, node* (*_node_generating)(), link* (*_link_generating)());
	bool implement();

};
impl* create_ws_network_genrating(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);