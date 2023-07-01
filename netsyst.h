#pragma once
#include "elem.h"

typedef std::map<std::string, impl*> impm;
typedef std::pair<std::string, impl*> impm_pair;
typedef impl* (*impc)(npsm&, lpsm&);
typedef std::map<std::string, impc> impcm;

class netsyst
{
protected:
	npsm node_ptr_set_map;
	lpsm link_ptr_set_map;
	impm exec_ptr_map;
	impm init_ptr_map;
	impl* end_impl_ptr;
	impcm impl_creator_map;
public:
	bool add_impl_creator(std::string const& _impl_type, impc _imp_creator);
	bool del_impl_creator(std::string const& _impl_type);
	bool add_exec(std::string const& _impl_type);
	bool add_init(std::string const& _init_type);
	bool del_exec(std::string const& _impl_type);
	bool del_init(std::string const& _init_type);
	bool del_exec(impl* _impl_ptr);
	bool del_init(impl* _init_ptr);
	bool del_all_exec();
	bool del_all_init();
	impl* get_init(std::string const& _init_type);
	impl* get_exec(std::string const& _impl_type);
	netsyst();
	~netsyst();
	void init();
	void run(int times);
};