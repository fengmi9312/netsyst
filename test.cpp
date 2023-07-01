#include <iostream>
#include <fstream>
#include <deque>
#include "network_generating.h"
#include "compartmental_model.h"

int main()
{

	std::set<size_t> seeds{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	std::ofstream fout_calc("output_file\\output_calc.csv");
	std::ofstream fout("output_file\\output.csv");
	fout << "time,s,i,r" << std::endl;
	fout_calc << "time,s,i,r" << std::endl;
	netsyst* net_t = new netsyst;
	net_t->add_impl_creator("er_network_generating", create_er_network_genrating);
	net_t->add_impl_creator("sir_spreading", create_sir_spreading);
	net_t->add_impl_creator("special_infection_seed", create_special_infection_seed);
	net_t->add_impl_creator("sir_calculation", create_sir_calculation);

	net_t->add_init("er_network_generating");
	net_t->add_init("special_infection_seed");
	net_t->add_exec("sir_spreading");
	net_t->add_exec("sir_calculation");

	((er_network_generating*)net_t->get_init("er_network_generating"))->set_params(100, 500, 0, create_vertex, create_edge);
	((special_infection_seed*)net_t->get_init("special_infection_seed"))->set_params(seeds, 2, 1.3, 2, 0.5);
	((sir_spreading*)net_t->get_exec("sir_spreading"))->set_params(0.01, 2, 1.3, 2, 0.5);
	((sir_calculation*)net_t->get_exec("sir_calculation"))->set_params(0, 0.01, 2, 1.3, 2, 0.5);

	net_t->get_init("er_network_generating")->implement();
	net_t->get_init("special_infection_seed")->implement();
	for (int i = 0; i < 800; i++)
	{
		net_t->get_exec("sir_calculation")->implement();
		std::cout << ((sir_calculation*)net_t->get_exec("sir_calculation"))->get_time() << ",";
		std::cout << ((sir_calculation*)net_t->get_exec("sir_calculation"))->get_s_amount() << ",";
		std::cout << ((sir_calculation*)net_t->get_exec("sir_calculation"))->get_i_amount() << ",";
		std::cout << ((sir_calculation*)net_t->get_exec("sir_calculation"))->get_r_amount() << ",";
		std::cout << std::endl;

		fout_calc << ((sir_calculation*)net_t->get_exec("sir_calculation"))->get_time() << ",";
		fout_calc << ((sir_calculation*)net_t->get_exec("sir_calculation"))->get_s_amount() << ",";
		fout_calc << ((sir_calculation*)net_t->get_exec("sir_calculation"))->get_i_amount() << ",";
		fout_calc << ((sir_calculation*)net_t->get_exec("sir_calculation"))->get_r_amount() << ",";
		fout_calc << std::endl;
	}
	multi_interp x(0.01, 8000);
	multi_interp y(0.01, 8000);
	multi_interp z(0.01, 8000);
	std::map<double, double> xm;
	std::map<double, double> ym;
	std::map<double, double> zm;
	for (int j = 0; j < 100; j++)
	{
		std::cout << j << std::endl;
		double time_t0 = 0;
		double time_t1 = 0;
		xm.clear();
		ym.clear();
		zm.clear();
		net_t->get_init("special_infection_seed")->implement();
		((sir_spreading*)net_t->get_exec("sir_spreading"))->init_time(0);
		xm[((sir_spreading*)net_t->get_exec("sir_spreading"))->get_time()] = ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_s_amount();
		ym[((sir_spreading*)net_t->get_exec("sir_spreading"))->get_time()] = ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_i_amount();
		zm[((sir_spreading*)net_t->get_exec("sir_spreading"))->get_time()] = ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_r_amount();
		for (int i = 0; i < 2000; i++)
		{
			net_t->get_exec("sir_spreading")->implement();
			//((sir_spreading*)net_t->get_exec("sir_spreading"))->check_trans_time_map();
			/*
			std::cout << ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_time() << " ";
			std::cout << ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_s_amount() << " ";
			std::cout << ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_i_amount() << " ";
			std::cout << ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_r_amount() << " ";
			std::cout << std::endl;*/
			time_t1 = ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_time();
			xm[((sir_spreading*)net_t->get_exec("sir_spreading"))->get_time()] = ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_s_amount();
			ym[((sir_spreading*)net_t->get_exec("sir_spreading"))->get_time()] = ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_i_amount();
			zm[((sir_spreading*)net_t->get_exec("sir_spreading"))->get_time()] = ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_r_amount();
			if (time_t0 > time_t1) system("pause");
			time_t0 = time_t1;
			/*
			fout << ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_time() << ",";
			fout << ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_s_amount() << ",";
			fout << ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_i_amount() << ",";
			fout << ((sir_spreading*)net_t->get_exec("sir_spreading"))->get_r_amount() << ",";
			fout << std::endl;
			*/
		}
		x.input_data(xm);
		y.input_data(ym);
		z.input_data(zm);
	}
	std::map<double, double>& xd = x.output_data();
	std::map<double, double>& yd = y.output_data();
	std::map<double, double>& zd = z.output_data();
	std::map<double, double>::const_iterator x_it = xd.begin();
	std::map<double, double>::const_iterator y_it = yd.begin();
	std::map<double, double>::const_iterator z_it = zd.begin();
	while (x_it != xd.end() && y_it != yd.end() && z_it != zd.end())
	{
		fout << x_it->first << ",";
		fout << x_it->second << ",";
		fout << y_it->second << ",";
		fout << z_it->second << ",";
		fout << std::endl;
		++x_it;
		++y_it;
		++z_it;
	}

	fout.close();
	return 0;
}