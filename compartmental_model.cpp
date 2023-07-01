#include "compartmental_model.h"
#include <vector>
#include <random>


edge::edge() :link(), infect_time(-1), infection(0), trans(false) {}

void edge::init()
{
	infect_time = -1;
	infection = 0;
	trans = false;
}

void edge::set_infect_time(double _infect_time)
{
	infect_time = _infect_time;
}

void edge::set_infection(double _infection)
{
	infection = _infection;
}

void edge::set_trans()
{
	trans = true;
}

double edge::get_infect_time() const
{
	return infect_time;
}

double edge::get_infection() const
{
	return infection;
}

bool edge::get_trans() const
{
	return trans;
}

std::string edge::get_sender_type() const 
{ 
	return "vertex"; 
}

std::string edge::get_receiver_type() const 
{ 
	return "vertex"; 
}

link* create_edge()
{
	return new edge{};
}


vertex::vertex() : state('s'), trans_time(-1), s_prob(1), i_prob(0), r_prob(0), i_prob_v({ 0 }), itrans_time(-1) {}

void vertex::init()
{
	state = 's';
	trans_time = -1;
	s_prob = 1;
	i_prob = 0;
	r_prob = 0;
	i_prob_v.clear();
	i_prob_v.push_back(0);
	itrans_time = -1;
}

char vertex::get_state() const 
{ 
	return state; 
}

double vertex::get_trans_time() const 
{ 
	return trans_time;
}

lpl const& vertex::sir_infected(std::weibull_distribution<double>& _distribution_inf, 
								std::weibull_distribution<double>& _distribution_rem, 
								std::default_random_engine& _generator, double _time)
{
	if (state != 's') return *(new lpl{});
	double _trans_time_tmp;
	lpl const* _outlinks_ptr{ &(get_outlinks("vertex"))};
	lpl* _trans_outlinks_ptr{ new lpl };
	state = 'i';
	trans_time = _distribution_rem(_generator) + _time;
	_distribution_rem.reset();
	if (_outlinks_ptr->size() > 0)
	{
		for (lpl::const_iterator it{ _outlinks_ptr->begin() }; it != _outlinks_ptr->end(); ++it)
		{
			_trans_time_tmp = _distribution_inf(_generator) + _time;
			_distribution_inf.reset();
			if (_trans_time_tmp <= trans_time)
			{
				((edge*)(*it))->set_infect_time(_trans_time_tmp);
				_trans_outlinks_ptr->push_back(*it);
			}
		}
	}
	delete _outlinks_ptr;
	return *_trans_outlinks_ptr;
}

bool vertex::sir_infect_time_received(edge const* _edge)
{
	if (state == 's' && check_inlink(_edge) && _edge->get_infect_time() > -0.5 && (_edge->get_infect_time() < trans_time || trans_time < -0.5))
	{
		trans_time = _edge->get_infect_time();
		return true;
	}
	else return false;
}

bool vertex::sir_removed()
{
	if (state != 'i') return false;
	state = 'r';
	trans_time = -1;
	return true;
}


bool vertex::sir_infect_calc(double _alpha_inf, double _beta_inf, double _step)
{
	lpl const* _outlinks_ptr{ &(get_outlinks("vertex")) };
	double _infection{ 0 };
	size_t i{ i_prob_v.size() };
	for (std::list<double>::const_iterator _it(i_prob_v.begin()); _it != i_prob_v.end(); ++_it)
	{
		_infection += (exp(-pow((i - 1) * _step / _beta_inf, _alpha_inf)) - exp(-pow(i *_step / _beta_inf, _alpha_inf)))
			/ exp(-pow(i *_step / _beta_inf, _alpha_inf)) * (*_it);
		--i;
	}
	for (lpl::const_iterator _it{ _outlinks_ptr->begin() }; _it != _outlinks_ptr->end(); ++_it)
		((edge*)(*_it))->set_infection(_infection);
	return true;
}

bool vertex::sir_remove_calc(double _alpha_rem, double _beta_rem, double _step)
{
	lpl_const const* _inlinks_ptr{ &(find_inlinks("vertex")) };
	size_t i{ i_prob_v.size() };
	double tmp;
	double _infection{ 1 };
	double _removal{ 0 };
	for (std::list<double>::iterator _it(i_prob_v.begin()); _it != i_prob_v.end(); ++_it)
	{
		tmp = (exp(-pow((i - 1) * _step / _beta_rem, _alpha_rem)) - exp(-pow(i *_step / _beta_rem, _alpha_rem)))
			/ exp(-pow(i *_step / _beta_rem, _alpha_rem)) * (*_it);
		(*_it) -= tmp;
		_removal += tmp;
		--i;
	}
	for (lpl_const::const_iterator _it{ _inlinks_ptr->begin() }; _it != _inlinks_ptr->end(); ++_it)
		_infection *= (1 - ((edge*)(*_it))->get_infection());
	_infection = s_prob * (1 - _infection);
	s_prob -= _infection;
	i_prob -= _removal;
	i_prob += _infection;
	i_prob_v.push_back(_infection);
	r_prob += _removal;
	return true;
}

bool vertex::sir_infect_simu(double _alpha_inf, double _beta_inf, double _step, double _current_time, probability& _probability)
{
	if (state != 'i') return false;
	double state_age = _current_time - itrans_time;
	lpl const* _outlinks_ptr{ &(get_outlinks("vertex")) };
	for (lpl::const_iterator _it = _outlinks_ptr->begin(); _it != _outlinks_ptr->end(); ++_it)
	{
		if (_probability((exp(-pow(state_age / _beta_inf, _alpha_inf)) - exp(-pow((state_age + _step) / _beta_inf, _alpha_inf)))
			/ exp(-pow(state_age / _beta_inf, _alpha_inf))))
		{
			((edge*)(*_it))->set_trans();
		}
	}
	return true;
}

bool vertex::sir_infected_simu(double _current_time)
{
	if (state != 's') return false;
	lpl_const const* _inlinks_ptr{ &(find_inlinks("vertex")) };
	for (lpl_const::const_iterator _it = _inlinks_ptr->begin(); _it != _inlinks_ptr->end(); ++_it)
	{
		if (((edge*)(*_it))->get_trans())
		{
			state = 'i';
			itrans_time = _current_time;
			return true;
		}
	}
	return false;
}

bool vertex::sir_remove_simu(double _alpha_rem, double _beta_rem, double _step, double _current_time, probability& _probability)
{
	if (state != 'i') return false;
	double state_age = _current_time - itrans_time;
	if (_probability((exp(-pow(state_age / _beta_rem, _alpha_rem)) - exp(-pow((state_age + _step) / _beta_rem, _alpha_rem)))
		/ exp(-pow(state_age / _beta_rem, _alpha_rem))))
	{
		state = 'r';
		itrans_time = -1;
		return true;
	}
	return false;
}

double vertex::get_s_prob()const
{
	return s_prob;
}

double vertex::get_i_prob()const
{
	return i_prob;
}

double vertex::get_r_prob()const
{
	return r_prob;
}

node* create_vertex()
{
	return new vertex{};
}


random_infection_seed::random_infection_seed(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map)
	:impl(_node_ptr_set_map, _link_ptr_set_map), num(0)
{
	distribution_inf.param(std::weibull_distribution<double>::param_type(1, 1));
	distribution_rem.param(std::weibull_distribution<double>::param_type(1, 1));
}

void random_infection_seed::set_params(size_t _num, double _alpha_inf, double _beta_inf, double _alpha_rem, double _beta_rem)
{
	num = _num;
	distribution_inf.param(std::weibull_distribution<double>::param_type(_alpha_inf, _beta_inf));
	distribution_rem.param(std::weibull_distribution<double>::param_type(_alpha_rem, _beta_rem));
}

bool random_infection_seed::implement()
{
	if (distribution_inf.param().a() == 0 || distribution_inf.param().b() == 0) return false;
	std::default_random_engine _generator;
	std::uniform_int_distribution<int> _distribution;
	lpl const* _outlinks_ptr;
	size_t _num{ num };
	size_t _amount{ node_ptr_set_map["vertex"].size() };
	if (num < 0) _num = 0;
	else if (num >= _amount) _num = _amount;
	else;
	std::vector<vertex*> _vertex_ptr_v(_amount, nullptr);
	std::set<int> _num_set;
	std::set<int>::iterator it;
	int _count{ 0 };
	for (nps::iterator it{ node_ptr_set_map["vertex"].begin() }; it != node_ptr_set_map["vertex"].end(); ++it)
	{
		_vertex_ptr_v[_count] = (vertex*)(*it);
		++_count;
	}
	for (int i{ 0 }; i < _amount; ++i) _num_set.insert(i);
	int _tmp;
	for (int i{ 0 }; i < _num; ++i)
	{
		_distribution.reset();
		_distribution.param(std::uniform_int_distribution<int>::param_type{ 0,(int)_num_set.size() - 1 });
		_tmp = _distribution(_generator);
		it = _num_set.begin();
		for (int j{ 0 }; j < _tmp; ++j) ++it;
		_outlinks_ptr = &(_vertex_ptr_v[*it]->sir_infected(distribution_inf, distribution_rem, generator, 0));
		for (lpl::const_iterator _it{ _outlinks_ptr->begin() }; _it != _outlinks_ptr->end(); ++_it)
			 ((vertex*)get_link_receiver_ptr(*_it))->sir_infect_time_received((edge*)(*_it));
		delete _outlinks_ptr;
		_num_set.erase(it);
	}
	return true;
}

impl* create_random_infection_seed(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map)
{
	return new random_infection_seed(_node_ptr_set_map, _link_ptr_set_map);
}


special_infection_seed::special_infection_seed(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map)
	:impl(_node_ptr_set_map, _link_ptr_set_map)
{
	distribution_inf.param(std::weibull_distribution<double>::param_type(1, 1));
	distribution_rem.param(std::weibull_distribution<double>::param_type(1, 1));
}

void special_infection_seed::set_params(std::set<size_t>& _node_indeces, double _alpha_inf, double _beta_inf, double _alpha_rem, double _beta_rem)
{
	node_indeces = _node_indeces; 
	distribution_inf.param(std::weibull_distribution<double>::param_type(_alpha_inf, _beta_inf));
	distribution_rem.param(std::weibull_distribution<double>::param_type(_alpha_rem, _beta_rem));
}

bool special_infection_seed::implement()
{
	lpl const* _outlinks_ptr;
	for (nps::const_iterator _it{ node_ptr_set_map["vertex"].begin() }; _it != node_ptr_set_map["vertex"].end(); ++_it) ((vertex*)(*_it))->init();
	for (lps::const_iterator _it{ link_ptr_set_map["edge"].begin() }; _it != link_ptr_set_map["edge"].end(); ++_it) ((edge*)(*_it))->init();
	std::set<size_t>::const_iterator it{node_indeces.begin()};
	size_t i{ 0 };
	for (nps::const_iterator _it{ node_ptr_set_map["vertex"].begin() }; _it != node_ptr_set_map["vertex"].end(); ++_it)
	{
		if (it != node_indeces.end() && i == *it)
		{
			_outlinks_ptr = &((vertex*)(*_it))->sir_infected(distribution_inf, distribution_rem, generator, 0);
			for (lpl::const_iterator __it{ _outlinks_ptr->begin() }; __it != _outlinks_ptr->end(); ++__it)
				((vertex*)get_link_receiver_ptr(*__it))->sir_infect_time_received((edge*)(*__it));
			((vertex*)(*_it))->s_prob = 0;
			((vertex*)(*_it))->i_prob = 1;
			((vertex*)(*_it))->r_prob = 0;
			(*((vertex*)(*_it))->i_prob_v.begin()) = 1;
			++it;
		}
		++i;
	}
	return true;
}

impl* create_special_infection_seed(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map)
{
	return new special_infection_seed(_node_ptr_set_map, _link_ptr_set_map);
}


sir_spreading::sir_spreading(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map) :
	impl(_node_ptr_set_map, _link_ptr_set_map), time(0), s_amount(0), i_amount(0), r_amount(0)
{
	distribution_inf.param(std::weibull_distribution<double>::param_type(1, 1));
	distribution_rem.param(std::weibull_distribution<double>::param_type(1, 1));
	init_able = true;
}

void sir_spreading::set_params(double _time, double _alpha_inf, double _beta_inf, double _alpha_rem, double _beta_rem)
{
	time = _time;
	s_amount = 0;
	i_amount = 0;
	r_amount = 0;
	distribution_inf.param(std::weibull_distribution<double>::param_type(_alpha_inf, _beta_inf));
	distribution_rem.param(std::weibull_distribution<double>::param_type(_alpha_rem, _beta_rem));
}

bool sir_spreading::implement()
{
	if (node_ptr_set_map["vertex"].size() == 0 || link_ptr_set_map["edge"].size() == 0) return false;
	if (trans_time_map.size() == 0 && init_able)
	{
		for (nps::iterator it{ node_ptr_set_map["vertex"].begin() }; it != node_ptr_set_map["vertex"].end(); ++it)
		{
			if (((vertex*)(*it))->get_trans_time() > -0.5)
			{
				trans_time_map.insert(std::pair<double, vertex*>(((vertex*)(*it))->get_trans_time(), ((vertex*)(*it))));
			}
		}
		init_able = false;
	}
	if (i_amount == 0) return false;
	vertex* _vertex = (vertex*)(trans_time_map.begin()->second);
	time = _vertex->get_trans_time();
	lpl const* _outlinks_ptr;
	vertex* _vertex_tmp;
	std::multimap<double, vertex*>::node_type nh;
	std::multimap<double, vertex*>::iterator _it;
	if (_vertex->get_state() == 's')
	{
		_outlinks_ptr = &(_vertex->sir_infected(distribution_inf, distribution_rem, generator, time));
		nh = trans_time_map.extract(trans_time_map.begin());
		nh.key() = _vertex->get_trans_time();
		trans_time_map.insert(move(nh));
		--s_amount;
		++i_amount;
		if (_outlinks_ptr->size() > 0)
		{
			for (lpl::const_iterator it{ _outlinks_ptr->begin() }; it != _outlinks_ptr->end(); ++it)
			{
				_vertex_tmp = (vertex*)get_link_receiver_ptr(((edge*)(*it)));
				if (_vertex_tmp->get_trans_time() < -0.5)
				{
					if (_vertex_tmp->sir_infect_time_received((edge*)(*it)))
					{
						trans_time_map.insert(std::pair<double, vertex*>(_vertex_tmp->get_trans_time(), _vertex_tmp));
					}
				}
				else
				{
					_it = trans_time_map.find(_vertex_tmp->get_trans_time());
					while (_it->second != _vertex_tmp) ++_it;
					if (_vertex_tmp->sir_infect_time_received((edge*)(*it)))
					{
						nh = trans_time_map.extract(_it);
						nh.key() = _vertex_tmp->get_trans_time();
						trans_time_map.insert(move(nh));
					}
				}
			}
		}
		delete _outlinks_ptr;
	}
	else if (_vertex->get_state() == 'i')
	{
		_vertex->sir_removed();
		--i_amount;
		++r_amount;
		trans_time_map.erase(trans_time_map.begin());
	}
	else system("pause");
	return true;
}

size_t sir_spreading::get_s_amount() const
{
	return s_amount;
}

size_t sir_spreading::get_i_amount() const
{
	return i_amount;
}

size_t sir_spreading::get_r_amount() const
{
	return r_amount;
}

double sir_spreading::get_time() const
{
	return time;
}

void sir_spreading::present_trans_time_map()
{
	for (std::multimap<double, vertex*>::const_iterator it{ trans_time_map.begin() }; it != trans_time_map.end(); ++it)
	{
		std::cout << it->first << " " << it->second->get_type() << " " << it->second->get_index() << " " << it->second->get_state() << " " << it->second->get_trans_time() << std::endl;
	}
}

void sir_spreading::check_trans_time_map()
{
	size_t _amount = 0;
	size_t _s_amount = 0;
	size_t _i_amount = 0;
	size_t _r_amount = 0;
	for (nps::const_iterator _it = node_ptr_set_map["vertex"].begin(); _it != node_ptr_set_map["vertex"].end(); ++_it) 
	{
		if (((vertex*)(*_it))->get_trans_time() >= -0.5)
			++_amount;
		if (((vertex*)(*_it))->get_state() == 'r' && ((vertex*)(*_it))->get_trans_time() >= -0.5)
		{
			std::cout << "wrong r." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (((vertex*)(*_it))->get_state() == 's') ++_s_amount;
		if (((vertex*)(*_it))->get_state() == 'i') ++_i_amount;
		if (((vertex*)(*_it))->get_state() == 'r') ++_r_amount;

	}
	for (std::multimap<double, vertex*>::const_iterator _it = trans_time_map.begin(); _it != trans_time_map.end(); ++_it)
	{
		if (_it->first != ((vertex*)(_it->second))->get_trans_time())
		{
			std::cout << "unequal trans_time." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (_amount != trans_time_map.size())
	{
		std::cout << "unequal size." << std::endl;
		exit(EXIT_FAILURE);
	}
	if (_s_amount != s_amount || _i_amount != i_amount || _r_amount != r_amount)
	{
		std::cout << "unequal amount." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void sir_spreading::init_time(double _time)
{
	init_able = true;
	time = _time;
	s_amount = 0;
	i_amount = 0;
	r_amount = 0;
	trans_time_map.clear();
	for (nps::iterator it{ node_ptr_set_map["vertex"].begin() }; it != node_ptr_set_map["vertex"].end(); ++it)
	{
		if (((vertex*)(*it))->get_state() == 's') ++s_amount;
		if (((vertex*)(*it))->get_state() == 'i') ++i_amount;
		if (((vertex*)(*it))->get_state() == 'r') ++r_amount;
	}
}

impl* create_sir_spreading(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map)
{
	return new sir_spreading(_node_ptr_set_map, _link_ptr_set_map);
}


special_infection_seed_syn::special_infection_seed_syn(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map) :
	impl(_node_ptr_set_map, _link_ptr_set_map) {};

void special_infection_seed_syn::set_params(std::set<size_t>& _node_indeces)
{
	node_indeces = _node_indeces;
}

bool special_infection_seed_syn::implement()
{
	lpl const* _outlinks_ptr;
	for (nps::const_iterator _it{ node_ptr_set_map["vertex"].begin() }; _it != node_ptr_set_map["vertex"].end(); ++_it) ((vertex*)(*_it))->init();
	for (lps::const_iterator _it{ link_ptr_set_map["edge"].begin() }; _it != link_ptr_set_map["edge"].end(); ++_it) ((edge*)(*_it))->init();
	std::set<size_t>::const_iterator it{ node_indeces.begin() };
	size_t i{ 0 };
	for (nps::const_iterator _it{ node_ptr_set_map["vertex"].begin() }; _it != node_ptr_set_map["vertex"].end(); ++_it)
	{
		if (it != node_indeces.end() && i == *it)
		{
			((vertex*)(*_it))->state = 'i';
			((vertex*)(*_it))->s_prob = 0;
			((vertex*)(*_it))->i_prob = 1;
			((vertex*)(*_it))->r_prob = 0;
			(*((vertex*)(*_it))->i_prob_v.begin()) = 1;
			++it;
		}
		++i;
	}
	return true;
}


impl* create_special_infection_seed_syn(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map)
{
	return new special_infection_seed_syn(_node_ptr_set_map, _link_ptr_set_map);
}




sir_spreading_syn::sir_spreading_syn(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map) :
	impl(_node_ptr_set_map, _link_ptr_set_map), time(0), step(0.01), alpha_inf(1), beta_inf(1), alpha_rem(1), beta_rem(1), s_amount(0), i_amount(0), r_amount(0), prob_ptr(new probability) {}

void sir_spreading_syn::set_params(double _step, double _alpha_inf, double _beta_inf, double _alpha_rem, double _beta_rem)
{
	step = _step;
	alpha_inf =_alpha_inf;
	beta_inf = _beta_inf;
	alpha_rem = _alpha_rem;
	beta_rem = _beta_rem;
}

bool sir_spreading_syn::implement()
{
	if (node_ptr_set_map["vertex"].size() == 0 || link_ptr_set_map["edge"].size() == 0) return false;
	for (nps::iterator _it(node_ptr_set_map["vertex"].begin()); _it != node_ptr_set_map["vertex"].end(); ++_it)
		((vertex*)(*_it))->sir_infect_simu(alpha_inf, beta_inf, step, time, *prob_ptr);
	for (nps::iterator _it(node_ptr_set_map["vertex"].begin()); _it != node_ptr_set_map["vertex"].end(); ++_it)
	{
		if (((vertex*)(*_it))->sir_infected_simu(time))
		{
			--s_amount;
			++i_amount;
		}
		if (((vertex*)(*_it))->sir_remove_simu(alpha_rem, beta_rem, step, time, *prob_ptr))
		{
			--i_amount;
			++r_amount;
		}
	}
	time += step;
	return true;
}

size_t sir_spreading_syn::get_s_amount() const
{
	return s_amount;
}

size_t sir_spreading_syn::get_i_amount() const
{
	return i_amount;
}

size_t sir_spreading_syn::get_r_amount() const
{
	return r_amount;
}

double sir_spreading_syn::get_time() const
{
	return time;
}

void sir_spreading_syn::init_time(double _time)
{
	time = _time;
	s_amount = 0;
	i_amount = 0;
	r_amount = 0;
	for (nps::iterator it{ node_ptr_set_map["vertex"].begin() }; it != node_ptr_set_map["vertex"].end(); ++it)
	{
		if (((vertex*)(*it))->get_state() == 's') ++s_amount;
		if (((vertex*)(*it))->get_state() == 'i') ++i_amount;
		if (((vertex*)(*it))->get_state() == 'r') ++r_amount;
	}
}

impl* create_sir_spreading_syn(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map)
{
	return new sir_spreading_syn(_node_ptr_set_map, _link_ptr_set_map);
}



state_present::state_present(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map) :impl(_node_ptr_set_map, _link_ptr_set_map) {}


sir_calculation::sir_calculation(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map):
	impl(_node_ptr_set_map, _link_ptr_set_map), time(0), step(0.01), alpha_inf(1), beta_inf(1), alpha_rem(1), beta_rem(1), s_amount(0), i_amount(0), r_amount(0) {}

void sir_calculation::set_params(double _time, double _step, double _alpha_inf, double _beta_inf, double _alpha_rem, double _beta_rem)
{
	time = _time;
	step = _step;
	alpha_inf = _alpha_inf;
	beta_inf = _beta_inf;
	alpha_rem = _alpha_rem;
	beta_rem = _beta_rem;
}

bool sir_calculation::implement()
{
	s_amount = 0;
	i_amount = 0;
	r_amount = 0;
	for (nps::iterator _it(node_ptr_set_map["vertex"].begin()); _it != node_ptr_set_map["vertex"].end(); ++_it)
		((vertex*)(*_it))->sir_infect_calc(alpha_inf, beta_inf, step);
	for (nps::iterator _it(node_ptr_set_map["vertex"].begin()); _it != node_ptr_set_map["vertex"].end(); ++_it)
	{
		((vertex*)(*_it))->sir_remove_calc(alpha_rem, beta_rem, step);
		s_amount += ((vertex*)(*_it))->get_s_prob();
		i_amount += ((vertex*)(*_it))->get_i_prob();
		r_amount += ((vertex*)(*_it))->get_r_prob();
	}
	time += step;
	return true;
}


double sir_calculation::get_time() const
{
	return time;
}

double sir_calculation::get_s_amount() const
{
	return s_amount;
}

double sir_calculation::get_i_amount() const
{
	return i_amount;
}

double sir_calculation::get_r_amount() const
{
	return r_amount;
}

impl* create_sir_calculation(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map)
{
	return new sir_calculation(_node_ptr_set_map, _link_ptr_set_map);
}

bool state_present::implement()
{
	lpl_const const* _lpl_const_ptr;
	strl _strl;
	for (npsm::iterator it{ node_ptr_set_map.begin() }; it != node_ptr_set_map.end(); ++it) _strl.push_back(it->first);
	for (npsm::iterator it{ node_ptr_set_map.begin() }; it != node_ptr_set_map.end(); ++it)
	{
		std::cout << "node_type: " << it->first << std::endl;
		std::cout << std::endl;
		for (nps::iterator _it{ it->second.begin() }; _it != it->second.end(); ++_it)
		{
			std::cout << "		";
			std::cout << "node: " << (*_it)->get_index() << "   (" << *_it << "):" << std::endl;
			std::cout << "		out" << std::endl;
			for (strl::iterator _it0{ _strl.begin() }; _it0 != _strl.end(); ++_it0)
			{
				std::cout << "			" << *_it0 << ":" << std::endl;
				_lpl_const_ptr = &(*_it)->find_outlinks(*_it0);
				for (lpl_const::const_iterator _it1{ _lpl_const_ptr->begin() }; _it1 != _lpl_const_ptr->end(); ++_it1)
				{
					std::cout << "				";
					std::cout << (*_it1)->get_sender_ptr()->get_type() << "	" << (*_it1)->get_sender_ptr()->get_index() << " ";
					std::cout << ((vertex*)(*_it1)->get_sender_ptr())->get_state() << "	" << ((vertex*)(*_it1)->get_sender_ptr())->get_trans_time();
					std::cout << " ---" << (*_it1)->get_type() << " " << (*_it1)->get_index() << " " << ((edge*)(*_it1))->get_infect_time() << "--->	";
					std::cout << (*_it1)->get_receiver_ptr()->get_type() << "	" << (*_it1)->get_receiver_ptr()->get_index() << " ";
					std::cout << ((vertex*)(*_it1)->get_receiver_ptr())->get_state() << "	" << ((vertex*)(*_it1)->get_receiver_ptr())->get_trans_time();
					std::cout << std::endl;
				}
			}
			std::cout << "		in" << std::endl;
			for (strl::iterator _it0{ _strl.begin() }; _it0 != _strl.end(); ++_it0)
			{
				std::cout << "			" << *_it0 << ":" << std::endl;
				_lpl_const_ptr = &(*_it)->find_inlinks(*_it0);
				for (lpl_const::const_iterator _it1{ _lpl_const_ptr->begin() }; _it1 != _lpl_const_ptr->end(); ++_it1)
				{
					std::cout << "				";
					std::cout << (*_it1)->get_receiver_ptr()->get_type() << "	" << (*_it1)->get_receiver_ptr()->get_index() << " ";
					std::cout << ((vertex*)(*_it1)->get_receiver_ptr())->get_state() << "	" << ((vertex*)(*_it1)->get_receiver_ptr())->get_trans_time();
					std::cout << " <---" << (*_it1)->get_type() << " " << (*_it1)->get_index() << " " << ((edge*)(*_it1))->get_infect_time() << "---	";
					std::cout << (*_it1)->get_sender_ptr()->get_type() << "	" << (*_it1)->get_sender_ptr()->get_index() << " ";
					std::cout << ((vertex*)(*_it1)->get_sender_ptr())->get_state() << "	" << ((vertex*)(*_it1)->get_sender_ptr())->get_trans_time();
					std::cout << std::endl;
				}
				std::cout << std::endl;
			}
		}
	}
	return true;
}


multi_interp::multi_interp(double _step, size_t _length):amount(0), step(_step), length(_length), data((0, _length)){}


bool multi_interp::input_data(std::map<double, double>& _data)
{
	if (_data.size() <= 1 || _data.begin()->first < 0) return false; 
	std::map<double, double>::const_iterator _it_head = _data.begin();
	std::map<double, double>::const_iterator _it_tail = _data.begin();
	++_it_tail;
	size_t _head;
	size_t _tail;
	size_t _mark;
	double _mark_data;
	++amount;
	while ( _it_tail != _data.end())
	{
		_head = _it_head->first / step ;
		if (_it_head->first / step != (double)_head) ++_head;
		_tail = _it_tail->first / step;
		if (_it_tail->first / step != (double)_tail) ++_tail;
		for (size_t _num = _head; _num != _tail; ++_num)
		{
			data[_num] += ((_it_tail->second - _it_head->second) / (_it_tail->first - _it_head->first)) * (_num * step - _it_head->first) + _it_head->second;
			_mark = _num;
			_mark_data = _it_tail->second;
			if (_num == length - 1) return true;
		}
		++_it_head;
		++_it_tail;
	}
	for (size_t _num = _mark + 1; _num < length; ++_num) data[_num] += _mark_data;
	return true;
}


std::map<double, double>& multi_interp::output_data()
{
	std::map<double, double>* _map_ptr = new std::map<double, double>;
	size_t _num = 0;
	for (std::vector<double>::const_iterator _it = data.begin(); _it != data.end(); ++_it)
	{
		_map_ptr->insert(std::pair<double, double>(_num * step, (*_it) / amount));
		++_num;
	}
	return *_map_ptr;
}


probability::probability()
{
	distribution.param(std::uniform_real_distribution<double>::param_type(0.0, 1.0));
}


bool probability::operator()(double _prob)
{
	if (_prob <= 0 ) return false;
	if (_prob >= 1) return true;
	while (_prob < 0.5)
	{
		_prob *= 2;
		distribution.reset();
		if (distribution(generator) < 0.5) return false;
	}
	distribution.reset();
	if (distribution(generator) < _prob) return true;
	return false;
}