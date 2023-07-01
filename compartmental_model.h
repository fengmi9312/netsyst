#pragma once
#include "elem.h"
#include "netsyst.h"
#include <random>


class probability
{
private:
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution;
public:
	probability();
	bool operator()(double prob);
};

class edge : public link
{
private:
	double infect_time;
	double infection;
	bool trans;
	edge();
	void init();
	std::string get_sender_type()const;
	std::string get_receiver_type()const;
	void set_infect_time(double _infect_time);
	void set_infection(double _infection);
	void set_trans();
	friend class sir_spreading;
	friend class sir_spreading_syn;
	friend class random_infection_seed;
	friend class special_infection_seed;
	friend class special_infection_seed_syn;
	friend class vertex;
	friend link* create_edge();
public:
	double get_infect_time() const;
	double get_infection() const;
	bool get_trans() const;
};
link* create_edge();


class vertex :public node
{
private:
	char state;
	double trans_time;
	double itrans_time;
	double s_prob;
	double i_prob;
	double r_prob;
	std::list<double> i_prob_v;
	vertex();
	lpl const& sir_infected(std::weibull_distribution<double>& _distribution_inf, 
		std::weibull_distribution<double>& _distribution_rem, 
		std::default_random_engine& _generator, double _time);
	void init();
	bool sir_infect_time_received(edge const* _edge);
	bool sir_removed();
	bool sir_infect_calc(double _alpha_inf, double _beta_inf, double _step);
	bool sir_remove_calc(double _alpha_rem, double _beta_rem, double _step);
	bool sir_infect_simu(double _alpha_inf, double _beta_inf, double _step, double _current_time, probability& _probability);
	bool sir_infected_simu(double _current_time);
	bool sir_remove_simu(double _alpha_rem, double _beta_rem, double _step, double _current_time, probability& _probability);
	friend class sir_spreading;
	friend class sir_spreading_syn;
	friend class sir_calculation;
	friend class random_infection_seed;
	friend class special_infection_seed;
	friend class special_infection_seed_syn;
	friend node* create_vertex();
public:
	char get_state()const;
	double get_trans_time()const;
	double get_s_prob()const;
	double get_i_prob()const;
	double get_r_prob()const;
};
node* create_vertex();


class random_infection_seed : public impl
{
private:
	size_t num;
	std::weibull_distribution<double> distribution_inf;
	std::weibull_distribution<double> distribution_rem;
	std::default_random_engine generator;
	random_infection_seed(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
	friend impl* create_random_infection_seed(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
public:
	void set_params(size_t _num, double _alpha_inf, double _beta_inf, double _alpha_rem, double _beta_rem);
	bool implement();
};
impl* create_random_infection_seed(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);


class special_infection_seed : public impl
{
private:
	std::set<size_t> node_indeces;
	std::weibull_distribution<double> distribution_inf;
	std::weibull_distribution<double> distribution_rem;
	std::default_random_engine generator;
	special_infection_seed(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
	friend impl* create_special_infection_seed(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
public:
	void set_params(std::set<size_t>& _node_indeces, double _alpha_inf, double _beta_inf, double _alpha_rem, double _beta_rem);
	bool implement();
};
impl* create_special_infection_seed(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);



class sir_spreading : public impl
{
private:
	bool init_able;
	double time;
	size_t s_amount;
	size_t i_amount;
	size_t r_amount;
	std::multimap<double, vertex*> trans_time_map;
	std::weibull_distribution<double> distribution_inf;
	std::weibull_distribution<double> distribution_rem;
	std::default_random_engine generator;
	sir_spreading(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
	friend impl* create_sir_spreading(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
public:
	void set_params(double _time, double _alpha_inf, double _beta_inf, double _alpha_rem, double _beta_rem);
	bool implement();
	size_t get_s_amount() const;
	size_t get_i_amount() const;
	size_t get_r_amount() const;
	double get_time() const;
	void check_trans_time_map();
	void present_trans_time_map();
	void init_time(double _time);
};
impl* create_sir_spreading(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);



class special_infection_seed_syn : public impl
{
private:
	std::set<size_t> node_indeces;
	special_infection_seed_syn(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
	friend impl* create_special_infection_seed_syn(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
public:
	void set_params(std::set<size_t>& _node_indeces);
	bool implement();
};
impl* create_special_infection_seed_syn(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);


class sir_spreading_syn : public impl
{
private:
	double time;
	double alpha_inf;
	double beta_inf;
	double alpha_rem;
	double beta_rem;
	double step;
	size_t s_amount;
	size_t i_amount;
	size_t r_amount;
	probability* prob_ptr;
	sir_spreading_syn(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
	friend impl* create_sir_spreading_syn(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
public:
	void set_params(double _step, double _alpha_inf, double _beta_inf, double _alpha_rem, double _beta_rem);
	bool implement();
	size_t get_s_amount() const;
	size_t get_i_amount() const;
	size_t get_r_amount() const;
	double get_time() const;
	void init_time(double _time);
};
impl* create_sir_spreading_syn(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);


class state_present : public impl
{
private:
public:
	state_present(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
	bool implement();
};



class sir_calculation : public impl
{
private:
	double time;
	double step;
	double alpha_inf;
	double beta_inf;
	double alpha_rem;
	double beta_rem;
	double s_amount;
	double i_amount;
	double r_amount;
	sir_calculation(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
	friend impl* create_sir_calculation(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
public:
	void set_params(double _time, double _step, double _alpha_inf, double _beta_inf, double _alpha_rem, double _beta_rem);
	double get_time() const;
	double get_s_amount() const;
	double get_i_amount() const;
	double get_r_amount() const;
	bool implement();
};
impl* create_sir_calculation(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);


class multi_interp
{
private:
	size_t amount;
	double step;
	size_t length;
	std::vector<double> data;
public:
	multi_interp(double _step, size_t _length);
	bool input_data(std::map<double, double>& _data);
	std::map<double, double>& output_data();
};
