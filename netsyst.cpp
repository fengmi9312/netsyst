// netsyst.cpp : Defines the entry point for the application.
//

#include"netsyst.h"

netsyst::netsyst() : end_impl_ptr(new clear_all_nodes(node_ptr_set_map, link_ptr_set_map)) {}

netsyst::~netsyst()
{
	end_impl_ptr->implement();
}


bool netsyst::add_impl_creator(std::string const& _impl_type, impc _imp_creator)
{
	if(impl_creator_map.find(_impl_type) != impl_creator_map.end()) return false;
	impl_creator_map.insert(std::pair<std::string, impc>(_impl_type, _imp_creator));
	return true;
}


bool netsyst::del_impl_creator(std::string const& _impl_type)
{
	impcm::const_iterator _impc_it = impl_creator_map.find(_impl_type);
	if (_impc_it == impl_creator_map.end()) return false;
	impl_creator_map.erase(_impc_it);
	return true;
}


bool netsyst::add_exec(std::string const& _impl_type)
{
	impcm::const_iterator _impc_it = impl_creator_map.find(_impl_type);
	if (_impc_it == impl_creator_map.end()) return false;
	impl* _impl_ptr = _impc_it->second(node_ptr_set_map, link_ptr_set_map);
	if (_impl_ptr == nullptr) return false;
	if (_impl_ptr->get_type() != _impl_type)
	{
		std::cout << "Error (netsyst::add_impl): unequal impl type " << _impl_type << "." << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}
	if (exec_ptr_map.find(_impl_ptr->get_type()) != exec_ptr_map.end()) return false;
	if (exec_ptr_map.insert(impm_pair{ _impl_ptr->get_type() , _impl_ptr }).second) return true;
	else
	{
		std::cout << "Error (netsyst::add_impl): failing inserting impl " << _impl_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}
}


bool netsyst::add_init(std::string const& _init_type)
{
	impcm::const_iterator _impc_it = impl_creator_map.find(_init_type);
	if (_impc_it == impl_creator_map.end()) return false;
	impl* _init_ptr = _impc_it->second(node_ptr_set_map, link_ptr_set_map);
	if (_init_ptr == nullptr) return false;
	if (_init_ptr->get_type() != _init_type)
	{
		std::cout << "Error (netsyst::add_impl): unequal impl type " << _init_type << "." << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}
	if (init_ptr_map.find(_init_ptr->get_type()) != init_ptr_map.end()) return false;
	if (init_ptr_map.insert(impm_pair{ _init_ptr->get_type() , _init_ptr }).second) return true;
	else
	{
		std::cout << "Error (netsyst::add_init): failing inserting impl " << _init_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}
}


bool netsyst::del_exec(std::string const& _impl_type)
{
	impm::iterator _impl_ptr_it{ exec_ptr_map.find(_impl_type) };
	if (_impl_ptr_it == exec_ptr_map.end()) return false;
	else
	{
		delete _impl_ptr_it->second;
		exec_ptr_map.erase(_impl_ptr_it);
		return true;
	}
}


bool netsyst::del_init(std::string const& _init_type)
{
	impm::iterator _init_ptr_it{ init_ptr_map.find(_init_type) };
	if (_init_ptr_it == init_ptr_map.end()) return false;
	else
	{
		delete _init_ptr_it->second;
		init_ptr_map.erase(_init_ptr_it);
		return true;
	}
}


bool netsyst::del_exec(impl* _impl_ptr)
{
	if (_impl_ptr == nullptr) return false;
	impm::iterator _impl_ptr_it{ exec_ptr_map.find(_impl_ptr->get_type()) };
	if (_impl_ptr_it == exec_ptr_map.end()) return false;
	else
	{
		delete _impl_ptr_it->second;
		exec_ptr_map.erase(_impl_ptr_it);
		return true;
	}
}

bool netsyst::del_init(impl* _init_ptr)
{
	if (_init_ptr == nullptr) return false;
	impm::iterator _init_ptr_it{ init_ptr_map.find(_init_ptr->get_type()) };
	if (_init_ptr_it == init_ptr_map.end()) return false;
	else
	{
		delete _init_ptr_it->second;
		init_ptr_map.erase(_init_ptr_it);
		return true;
	}
}


bool netsyst::del_all_exec()
{
	for (impm::iterator it{ exec_ptr_map.begin() }; it != exec_ptr_map.end(); ++it)
		delete it->second;
	exec_ptr_map.clear();
	return true;
}


bool netsyst::del_all_init()
{
	for (impm::iterator it{ init_ptr_map.begin() }; it != init_ptr_map.end(); ++it)
		delete it->second;
	init_ptr_map.clear();
	return true;
}


impl* netsyst::get_init(std::string const& _init_type)
{
	return init_ptr_map[_init_type];
}

impl* netsyst::get_exec(std::string const& _impl_type)
{
	return exec_ptr_map[_impl_type];
}

void netsyst::init()
{
	for (impm::iterator it{ init_ptr_map.begin() }; it != init_ptr_map.end(); ++it)
		it->second->implement();
}


void netsyst::run(int times)
{
	for (int i{ 0 }; i < times; ++i)
		for (impm::iterator it{ exec_ptr_map.begin() }; it != exec_ptr_map.end(); ++it)
			it->second->implement();
}
