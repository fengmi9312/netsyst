#include "elem.h"

/**************************************************************************************************************************************************************************/
// the implementation of class node's methods
/**************************************************************************************************************************************************************************/

size_t node::node_created_amount = 0;


/*constructor*/
node::node() : index(node_created_amount)
{
	able = false;
	++node_created_amount;
}


/*destructor*/
node::~node() {}


/*enable this node (can only be called by bool impl::insert_node(node* _node_ptr))*/
void node::enable()
{
	able = true;
}


/*check whether this node is enabled*/
bool node::isable() const
{
	return able;
}


/*get the index of this node*/
size_t node::get_index() const
{
	return index;
}


/*get the type of the node*/
std::string node::get_type() const
{
	return typeid(*this).name() + 6;															// return the name of this class
}


/*attach an outlink*/
bool node::attach_outlink(link* const _outlink_ptr)
{
	if (_outlink_ptr == nullptr) return false;													// check whether the parameter(s) is/are available
	if (!_outlink_ptr->sender_attach(this)) return false;										// check whether the outlink can be attached successfully
	std::string const& _receiver_type{ _outlink_ptr->get_receiver_type() };
	lpmm::iterator _outlink_map_it{ outlinks.find(_receiver_type) };							// search for the outlink map by receiver type
	if (_outlink_map_it == outlinks.end())
	{
		if (!outlinks.insert(lpmm_pair{ _receiver_type, lpm{lpm_pair{ _outlink_ptr->get_receiver_ptr(), _outlink_ptr }} }).second)
		{
			std::cout << "Error (node::attach_outlink): failing attaching outlink " << _outlink_ptr << "." << std::endl;
			exit(EXIT_FAILURE);
			return false;
		}																						
	}																							// if the map is not found, create a map containing the lpm_pair (if failing, exit)
	else _outlink_map_it->second.insert(lpm_pair{ _outlink_ptr->get_receiver_ptr(), _outlink_ptr });		// else, insert the lpm_pair
	return true;
}


/*attach an inlink*/
bool node::attach_inlink(link* const _inlink_ptr)
{
	if (_inlink_ptr == nullptr) return false;													// check whether the parameter(s) is/are available
	if (!_inlink_ptr->receiver_attach(this)) return false;										// check whether the inlink can be attaced successfully 
	std::string const& _sender_type{ _inlink_ptr->get_sender_type() };
	ipmm::iterator _inctrl_map_it{ inctrls.find(_sender_type) };								// search for the inlink map by receiver type
	if (_inctrl_map_it == inctrls.end())
	{
		if (!inctrls.insert(ipmm_pair{ _sender_type, ipm{ipm_pair{ _inlink_ptr->get_sender_ptr(), _inlink_ptr->get_inctrl_ptr() }} }).second)
		{
			std::cout << "Error (node::attach_inlink): failing attaching inlink " << _inlink_ptr << "." << std::endl;
			exit(EXIT_FAILURE);
			return false;
		}
	}																							// if the map is not found, create a map containing the lpm_const_pair (if failing, exit)
	else _inctrl_map_it->second.insert(ipm_pair{ _inlink_ptr->get_sender_ptr(), _inlink_ptr->get_inctrl_ptr() });		// else, insert the lpm_const_pair
	return true;
}


/*detach an outlink*/
bool node::detach_outlink(link* const _outlink_ptr)
{
	if (_outlink_ptr == nullptr) return false;													// check whether the parameter(s) is/are available
	if (_outlink_ptr->get_sender_ptr() != this) return false;									// check whether the sender pointer of outlink is correct 
	std::string const& _receiver_type{ _outlink_ptr->get_receiver_type() };
	lpmm::iterator _outlink_map_it{ outlinks.find(_receiver_type) };							// search for the outlink map by receiver type
	if (_outlink_map_it == outlinks.end())														// if not found, exit
	{
		std::cout << "Error (node::detach_outlink): failing finding receiver type of outlink " << _outlink_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
		return false;																			
	}																							
	std::pair<lpm::iterator, lpm::iterator> const& _outlink_it_range{ _outlink_map_it->second.equal_range(_outlink_ptr->get_receiver_ptr()) };
	if (_outlink_it_range.first == _outlink_it_range.second)									// if not found, exit
	{
		std::cout << "Error (node::detach_outlink): failing finding outlink " << _outlink_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}																							
	else
	{
		for (lpm::iterator _outlink_it{ _outlink_it_range.first }; _outlink_it != _outlink_it_range.second; ++_outlink_it)
		{
			if (_outlink_it->second == _outlink_ptr)											// if found
			{
				if (_outlink_ptr->get_sender_type() != get_type())								// check whether the types are consistent
				{
					std::cout << "Error (node::detach_outlink): unequal sender type." << std::endl;
					exit(EXIT_FAILURE);
					return false;
				}
				if (!_outlink_it->second->sender_detach())										// if failing detaching, exit
				{
					std::cout << "Error (node::detach_outlink): failing detaching outlink " << _outlink_ptr << "." << std::endl;
					exit(EXIT_FAILURE);
					return false;
				}																				// if outlink fails detaching, exit
				_outlink_map_it->second.erase(_outlink_it);										// erase the pointer in outlink map of node
				return true;
			}
		}
		return false;
	}
}


/*detach an inlink*/
bool node::detach_inlink(link* const _inlink_ptr)
{
	if (_inlink_ptr == nullptr) return false;													// check whether the parameter(s) is/are available
	if (_inlink_ptr->get_receiver_ptr() != this) return false;									// check whether the receiver pointer is correct
	std::string const& _sender_type{ _inlink_ptr->get_sender_type() };
	ipmm::iterator _inctrl_map_it{ inctrls.find(_sender_type) };								// search for the inctrl map by sender type
	if (_inctrl_map_it == inctrls.end())														// if not found, exit
	{
		std::cout << "Error (node::detach_inlink): failing finding sender type of inlink " << _inlink_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
		return false;																			
	}
	std::pair<ipm::iterator, ipm::iterator> const& _inctrl_it_range{ _inctrl_map_it->second.equal_range(_inlink_ptr->get_sender_ptr()) };	// search for the inctrl map by sender pointer
	if (_inctrl_it_range.first == _inctrl_it_range.second)										// if not found, exit
	{
		std::cout << "Error (node::detach_inlink): failing finding inlink " << _inlink_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}
	else
	{
		for (ipm::iterator _inctrl_it{ _inctrl_it_range.first }; _inctrl_it != _inctrl_it_range.second; ++_inctrl_it)
		{
			if (_inctrl_it->second->get_inlink_ptr() == _inlink_ptr)							// if found
			{
				if (_inlink_ptr->get_receiver_type() != get_type())								// check whether the receiver type is correct
				{
					std::cout << "Error(node::detach_inlink): unequal receiver type." << std::endl;
					exit(EXIT_FAILURE);
					return false;
				}
				if (!_inctrl_it->second->receiver_detach())										// if failing detaching, exit
				{
					std::cout << "Error(node::detach_inlink): failing detaching inlink " << _inlink_ptr << "." << std::endl;
					exit(EXIT_FAILURE);
					return false;
				}
				_inctrl_map_it->second.erase(_inctrl_it);										// erase the point in inctrl map of node
				return true;
			}
		}
		return false;
	}
}


/*detach all the outlinks connected to a certain receiver*/
lpl const& node::detach_outlinks(node const* const _receiver_ptr)
{
	lpl* _lpl_ptr = new lpl{};
	if (_receiver_ptr == nullptr) return *_lpl_ptr;												// check whether the parameter(s) is/are available
	std::string const& _receiver_type{ _receiver_ptr->get_type() };
	lpmm::iterator _outlink_map_it{ outlinks.find(_receiver_type) };							// search for the outlink map by receiver type
	if (_outlink_map_it == outlinks.end()) return *_lpl_ptr;									// if not found, return empty lpl
	std::pair<lpm::iterator, lpm::iterator> const& _outlink_it_range{ _outlink_map_it->second.equal_range(_receiver_ptr) };
	if (_outlink_it_range.first == _outlink_it_range.second) return *_lpl_ptr;
	else
	{
		for (lpm::iterator _outlink_it{ _outlink_it_range.first }; _outlink_it != _outlink_it_range.second; ++_outlink_it)
		{
			_lpl_ptr->push_back(_outlink_it->second);
			if (!_outlink_it->second->sender_detach())
			{
				std::cout << "Error (node::detach_outlinks): failing detaching outlinks connected to " << _receiver_ptr << "." << std::endl;
				exit(EXIT_FAILURE);
			}
			_outlink_map_it->second.erase(_outlink_it);											// and erase the pointer in outlink map
		}
		return *_lpl_ptr;
	}
}


/*detach all the inlinks connected to a certain sender*/
lpl_const const& node::detach_inlinks(node const* const _sender_ptr)
{
	lpl_const* _lpl_const_ptr = new lpl_const{};
	if (_sender_ptr == nullptr) return *_lpl_const_ptr;											// check whether the parameter(s) is/are available
	std::string const& _sender_type{ _sender_ptr->get_type() };
	ipmm::iterator _inctrl_map_it{ inctrls.find(_sender_type) };								// search for the inctrl map by sender type
	if (_inctrl_map_it == inctrls.end()) return *_lpl_const_ptr;								// if not found, return empty lpl_const
	std::pair<ipm::iterator, ipm::iterator> const& _inctrl_it_range{ _inctrl_map_it->second.equal_range(_sender_ptr) };
	if (_inctrl_it_range.first == _inctrl_it_range.second) return *_lpl_const_ptr;
	else
	{
		for (ipm::iterator _inctrl_it{ _inctrl_it_range.first }; _inctrl_it != _inctrl_it_range.second; ++_inctrl_it)
		{
			_lpl_const_ptr->push_back(_inctrl_it->second->get_inlink_ptr());
			if(!_inctrl_it->second->receiver_detach())
			{
				std::cout << "Error (node::detach_inlinks): failing detaching inlinks connected to " << _sender_ptr << "." << std::endl;
				exit(EXIT_FAILURE);
			}
			_inctrl_map_it->second.erase(_inctrl_it);
		}
		return *_lpl_const_ptr;
	}
}


/*detach all the outlinks connected to the receiver type*/
lpl const& node::detach_outlink_type(std::string const& _receiver_type)
{
	lpl* _lpl_ptr = new lpl{};
	lpmm::iterator _outlink_map_it{ outlinks.find(_receiver_type) };							// check whether the parameter(s) is/are available
	if (_outlink_map_it == outlinks.end()) return *_lpl_ptr;									// if not found, return empty lpl
	for (lpm::iterator _outlink_it{ _outlink_map_it->second.begin() }; _outlink_it != _outlink_map_it->second.end(); ++_outlink_it)
	{
		_lpl_ptr->push_back(_outlink_it->second);	
		if (!_outlink_it->second->sender_detach())
		{
			std::cout << "Error (node::detach_outlink_type): failing detaching outlink type connected to " << _receiver_type << "." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	_outlink_map_it->second.clear();
	return *_lpl_ptr;
}


/*detach all the inlinks connected to the sender type*/
lpl_const const& node::detach_inlink_type(std::string const& _sender_type)
{
	lpl_const* _lpl_const_ptr = new lpl_const{};
	ipmm::iterator _inctrl_map_it{ inctrls.find(_sender_type) };								// check whether the parameter(s) is/are available
	if (_inctrl_map_it == inctrls.end()) return *_lpl_const_ptr;								// if not found, return emply lpl_const
	for (ipm::iterator _inctrl_it{ _inctrl_map_it->second.begin() }; _inctrl_it != _inctrl_map_it->second.end(); ++_inctrl_it)
	{
		_lpl_const_ptr->push_back(_inctrl_it->second->get_inlink_ptr());
		if (!_inctrl_it->second->receiver_detach())
		{
			std::cout << "Error (node::detach_inlink_type): failing detaching inlink type connected to " << _sender_type << "." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	_inctrl_map_it->second.clear();
	return *_lpl_const_ptr;
}


/*detach all the outlinks*/
lpl const& node::detach_all_outlinks()
{
	lpl* _lpl_ptr = new lpl{};
	for (lpmm::iterator _outlink_map_it{ outlinks.begin() }; _outlink_map_it != outlinks.end(); ++_outlink_map_it)
	{
		for (lpm::iterator _outlink_it(_outlink_map_it->second.begin()); _outlink_it != _outlink_map_it->second.end(); ++_outlink_it)
		{
			_lpl_ptr->push_back(_outlink_it->second);
			if (!_outlink_it->second->sender_detach())
			{
				std::cout << "Error (node::detach_all_outlinks): failing detaching all outlinks." << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		_outlink_map_it->second.clear();
	}
	outlinks.clear();
	return *_lpl_ptr;
}


/*detach all the inlinks*/
lpl_const const& node::detach_all_inlinks()
{
	lpl_const* _lpl_const_ptr = new lpl_const{};
	for (ipmm::iterator _inctrl_map_it{ inctrls.begin() }; _inctrl_map_it != inctrls.end(); ++_inctrl_map_it)
	{
		for (ipm::iterator _inctrl_it(_inctrl_map_it->second.begin()); _inctrl_it != _inctrl_map_it->second.end(); ++_inctrl_it)
		{
			_lpl_const_ptr->push_back(_inctrl_it->second->get_inlink_ptr());
			if (!_inctrl_it->second->receiver_detach())
			{
				std::cout << "Error (node::detach_all_inlinks): failing detaching all inlinks." << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		_inctrl_map_it->second.clear();
	}
	inctrls.clear();
	return *_lpl_const_ptr;
}


/*get all the outlinks connected to a certain receiver*/
lpl const& node::get_outlinks(node const* const _receiver_ptr) const
{
	lpl* _outlink_ptr_list_ptr = new lpl{};														// create an emplty outlink pointer list
	if (_receiver_ptr == nullptr) return *_outlink_ptr_list_ptr;								// check whether the parameter(s) is/are available
	std::string const& _receiver_type{ _receiver_ptr->get_type() };
	lpmm::const_iterator _outlink_map_it{ outlinks.find(_receiver_type) };						// search for the outlink map by receiver type
	if (_outlink_map_it == outlinks.end()) return *_outlink_ptr_list_ptr;						// if not found, return the empty list
	std::pair<lpm::const_iterator, lpm::const_iterator> const& _outlink_it_range{ _outlink_map_it->second.equal_range(_receiver_ptr) };
	for (lpm::const_iterator it{ _outlink_it_range.first }; it != _outlink_it_range.second; ++it) _outlink_ptr_list_ptr->push_back(it->second);	// add outlink pointer to the list
	return *_outlink_ptr_list_ptr;
}


/*find all the outlinks connected to a certain receiver*/
lpl_const const& node::find_outlinks(node const* const _receiver_ptr) const
{
	lpl_const* _outlink_ptr_list_ptr = new lpl_const{};											// create an emplty outlink pointer list
	if (_receiver_ptr == nullptr) return *_outlink_ptr_list_ptr;								// check whether the parameter(s) is/are available
	std::string const& _receiver_type{ _receiver_ptr->get_type() };
	lpmm::const_iterator _outlink_map_it{ outlinks.find(_receiver_type) };						// search for the outlink map by receiver type
	if (_outlink_map_it == outlinks.end()) return *_outlink_ptr_list_ptr;						// if not found, return the empty list
	std::pair<lpm::const_iterator, lpm::const_iterator> const& _outlink_it_range{ _outlink_map_it->second.equal_range(_receiver_ptr) };
	for (lpm::const_iterator it{ _outlink_it_range.first }; it != _outlink_it_range.second; ++it) _outlink_ptr_list_ptr->push_back(it->second);	// add outlink pointer to the list
	return *_outlink_ptr_list_ptr;
}


/*find all the inlinks connected to a certain sender*/
lpl_const const& node::find_inlinks(node const* const _sender_ptr) const
{
	lpl_const* _inlink_ptr_list_ptr = new lpl_const{};											// create an emplty inlink pointer list
	if (_sender_ptr == nullptr) return *_inlink_ptr_list_ptr;									// check whether the parameter(s) is/are available
	std::string const& _sender_type{ _sender_ptr->get_type() };
	ipmm::const_iterator _inctrl_map_it{ inctrls.find(_sender_type) };							// search for the inctrl map by receiver type
	if (_inctrl_map_it == inctrls.end()) return *_inlink_ptr_list_ptr;							// if not found, return the empty list
	std::pair<ipm::const_iterator, ipm::const_iterator> const& _inlink_it_range{ _inctrl_map_it->second.equal_range(_sender_ptr) };
	for (ipm::const_iterator it{ _inlink_it_range.first }; it != _inlink_it_range.second; ++it) _inlink_ptr_list_ptr->push_back(it->second->get_inlink_ptr());	// add inlink pointer to the list
	return *_inlink_ptr_list_ptr;
}


/*get all the outlinks connected to a certain receiver type*/
lpl const& node::get_outlinks(std::string const& _receiver_type) const				
{
	lpl* _outlink_ptr_list_ptr = new lpl{};														// create an emplty outlink pointer list
	lpmm::const_iterator _outlink_map_it{ outlinks.find(_receiver_type) };						// search for the outlink map by receiver type
	if (_outlink_map_it == outlinks.end()) return *_outlink_ptr_list_ptr;
	for (lpm::const_iterator it{ _outlink_map_it->second.begin() }; it != _outlink_map_it->second.end(); ++it) _outlink_ptr_list_ptr->push_back(it->second);	// add outlink pointer to the list
	return *_outlink_ptr_list_ptr;
}


/*find all the outlinks connected to a certain receiver type*/
lpl_const const& node::find_outlinks(std::string const& _receiver_type) const
{
	lpl_const* _outlink_ptr_list_ptr = new lpl_const{};											// create an emplty outlink pointer list
	lpmm::const_iterator _outlink_map_it{ outlinks.find(_receiver_type) };						// search for the outlink map by receiver type
	if (_outlink_map_it == outlinks.end()) return *_outlink_ptr_list_ptr;
	for (lpm::const_iterator it{ _outlink_map_it->second.begin() }; it != _outlink_map_it->second.end(); ++it) _outlink_ptr_list_ptr->push_back(it->second);	// add outlink pointer to the list
	return *_outlink_ptr_list_ptr;
}


/*find all the inlinks connected to a certain sender type*/
lpl_const const& node::find_inlinks(std::string const& _sender_type) const			
{
	lpl_const* _inlink_ptr_list_ptr = new lpl_const{};											// create an emplty inlink pointer list
	ipmm::const_iterator _inctrl_map_it{ inctrls.find(_sender_type) };							// search for the inlink map by sender type
	if (_inctrl_map_it == inctrls.end()) return *_inlink_ptr_list_ptr;
	for (ipm::const_iterator it{ _inctrl_map_it->second.begin() }; it != _inctrl_map_it->second.end(); ++it) _inlink_ptr_list_ptr->push_back(it->second->get_inlink_ptr());	// add inlink pointer to the list
	return *_inlink_ptr_list_ptr;
}


/*get all the outlinks*/
lpl const& node::get_outlinks() const													
{
	lpl* _outlink_ptr_list_ptr = new lpl{};														// create an emplty outlink pointer list
	for (lpmm::const_iterator _outlink_map_it{ outlinks.begin() }; _outlink_map_it != outlinks.end(); ++_outlink_map_it)
		for (lpm::const_iterator it{ _outlink_map_it->second.begin() }; it != _outlink_map_it->second.end(); ++it)
			_outlink_ptr_list_ptr->push_back(it->second);										// add outlink pointer to the list
	return *_outlink_ptr_list_ptr;
}


/*find all the outlinks*/
lpl_const const& node::find_outlinks() const											
{
	lpl_const* _outlink_ptr_list_ptr = new lpl_const{};											// create an emplty outlink pointer list
	for (lpmm::const_iterator _outlink_map_it{ outlinks.begin() }; _outlink_map_it != outlinks.end(); ++_outlink_map_it)
		for (lpm::const_iterator it{ _outlink_map_it->second.begin() }; it != _outlink_map_it->second.end(); ++it)
			_outlink_ptr_list_ptr->push_back(it->second);										// add outlink pointer to the list
	return *_outlink_ptr_list_ptr;
}


/*find all the inlinks*/
lpl_const const& node::find_inlinks() const
{
	lpl_const* _inlink_ptr_list_ptr = new lpl_const{};											// create an emplty inlink pointer list
	for (ipmm::const_iterator _inctrl_map_it{ inctrls.begin() }; _inctrl_map_it != inctrls.end(); ++_inctrl_map_it)
		for (ipm::const_iterator it{ _inctrl_map_it->second.begin() }; it != _inctrl_map_it->second.end(); ++it)
			_inlink_ptr_list_ptr->push_back(it->second->get_inlink_ptr());						// add inlink pointer to the list
	return *_inlink_ptr_list_ptr;
}


/*find all the receivers of _receiver_type*/
npl_const const& node::find_receivers(std::string const& _receiver_type) const
{
	npl_const* _receiver_ptr_list_ptr = new npl_const{};
	lpmm::const_iterator _outlink_map_it{ outlinks.find(_receiver_type) };						// search for the outlink map by receiver type
	if (_outlink_map_it == outlinks.end()) return *_receiver_ptr_list_ptr;
	node const* node_ptr_tmp{ nullptr };
	for (lpm::const_iterator _outlink_it{ _outlink_map_it->second.begin()}; _outlink_it != _outlink_map_it->second.end(); ++_outlink_it )
	{
		if (_outlink_it->first != node_ptr_tmp) _receiver_ptr_list_ptr->push_back(_outlink_it->first);
		node_ptr_tmp = _outlink_it->first;
	}
	return *_receiver_ptr_list_ptr;
}


/*find all the senders of _sender_type*/
npl_const const& node::find_senders(std::string const& _sender_type) const
{
	npl_const* _sender_ptr_list_ptr = new npl_const{};
	ipmm::const_iterator _inctrl_map_it{ inctrls.find(_sender_type) };							// search for the inctrl map by sender type
	if (_inctrl_map_it == inctrls.end()) return *_sender_ptr_list_ptr;
	node const* node_ptr_tmp{ nullptr };
	for (ipm::const_iterator _inctrl_it{ _inctrl_map_it->second.begin() }; _inctrl_it != _inctrl_map_it->second.end(); ++_inctrl_it)
	{
		if (_inctrl_it->first != node_ptr_tmp) _sender_ptr_list_ptr->push_back(_inctrl_it->first);
		node_ptr_tmp = _inctrl_it->first;
	}
	return *_sender_ptr_list_ptr;
}


/*find all the receiver types*/
strl const& node::find_receiver_types() const
{
	strl* _str_list_ptr = new strl{};
	for (lpmm::const_iterator _outlink_map_it{ outlinks.begin() }; _outlink_map_it != outlinks.end(); ++_outlink_map_it)
		_str_list_ptr->push_back(_outlink_map_it->first);
	return *_str_list_ptr;
}


/*find all the sender types*/
strl const& node::find_sender_types() const
{
	strl* _str_list_ptr = new strl{};
	for (ipmm::const_iterator _inctrl_map_it{ inctrls.begin() }; _inctrl_map_it != inctrls.end(); ++_inctrl_map_it)
		_str_list_ptr->push_back(_inctrl_map_it->first);
	return *_str_list_ptr;
}


/*count all the outlinks connected to a certain receiver*/
size_t node::count_outlinks(node const* const _receiver_ptr) const
{
	if (_receiver_ptr == nullptr) return 0;														// check whether the parameter(s) is/are available
	std::string const& _receiver_type{ _receiver_ptr->get_type() };
	lpmm::const_iterator _outlink_map_it{ outlinks.find(_receiver_type) };						// search for the outlink map
	if (_outlink_map_it == outlinks.end()) return 0;											// if the map is not found, return 0
	return _outlink_map_it->second.count(_receiver_ptr);										// else, return the count of the outlinks connected to receiver
}


/*count all the inlinks connected to a certain sender*/
size_t node::count_inlinks(node const* const _sender_ptr) const
{
	if (_sender_ptr == nullptr) return 0;														// check whether the parameter(s) is/are available
	std::string const& _sender_type{ _sender_ptr->get_type() };
	ipmm::const_iterator _inctrl_map_it{ inctrls.find(_sender_type) };							// search for the inctrl map
	if (_inctrl_map_it == inctrls.end()) return 0;												// if the map is not found, return 0
	return _inctrl_map_it->second.count(_sender_ptr);											// else, return the count of the inlinks connected to sender (equalling to the counts of corresponding inctrls)
}


/*count all the outlinks*/
size_t node::count_all_outlinks() const
{
	size_t counts{ 0 };
	for (lpmm::const_iterator it{ outlinks.begin() }; it != outlinks.end(); ++it)
		counts += it->second.size();
	return counts;
}


/*count all the inlinks*/
size_t node::count_all_inlinks() const
{
	size_t counts{ 0 };
	for (ipmm::const_iterator it{ inctrls.begin() }; it != inctrls.end(); ++it)
		counts += it->second.size();
	return counts;
}


/*count all the links*/
size_t node::count_all_links() const
{
	return count_all_outlinks() + count_all_inlinks();
}


/*check whether the outlink is existing*/
bool node::check_outlink(link* const _outlink_ptr) const
{
	if (_outlink_ptr == nullptr) return false;													// check whether the parameter(s) is/are available
	if (_outlink_ptr->get_sender_ptr() != this) return false;									// check whether the sender pointer is correct
	std::string const& _receiver_type{ _outlink_ptr->get_receiver_type() };
	lpmm::const_iterator _outlink_map_it{ outlinks.find(_receiver_type) };						// search for the outlink map by receiver type
	if (_outlink_map_it == outlinks.end()) return false;										// if not found, return false
	std::pair<lpm::const_iterator, lpm::const_iterator> const& _outlink_it_range{ _outlink_map_it->second.equal_range(_outlink_ptr->get_receiver_ptr()) };
	if (_outlink_it_range.first == _outlink_it_range.second) return false;
	else
	{
		for (lpm::const_iterator it{ _outlink_it_range.first }; it != _outlink_it_range.second; ++it)
			if (it->second == _outlink_ptr)														// if found
			{
				if (_outlink_ptr->get_sender_type() != get_type())								// check whether the types are consistent
				{
					std::cout << "Error (node::check_outlink): unequal sender type." << std::endl;
					exit(EXIT_FAILURE);
					return false;
				}
				return true;																	
			}
		return false;																			// not found, return false
	}
}


/*check whether the inlink is existing*/
bool node::check_inlink(link const* const _inlink_ptr) const
{
	if (_inlink_ptr == nullptr) return false;													// check whether the parameter(s) is/are available
	if (_inlink_ptr->get_receiver_ptr() != this) return false;
	std::string const& _sender_type{ _inlink_ptr->get_sender_type() };
	ipmm::const_iterator _inctrl_map_it{ inctrls.find(_sender_type) };							// search for the inctrl map by sender type
	if (_inctrl_map_it == inctrls.end()) return false;											// if not found, return false
	std::pair<ipm::const_iterator, ipm::const_iterator> const& _inctrl_it_range{ _inctrl_map_it->second.equal_range(_inlink_ptr->get_sender_ptr()) };
	if (_inctrl_it_range.first == _inctrl_it_range.second) return false;
	else
	{
		for (ipm::const_iterator it{ _inctrl_it_range.first }; it != _inctrl_it_range.second; ++it)
			if (it->second->get_inlink_ptr() == _inlink_ptr)									// if found
			{
				if (_inlink_ptr->get_receiver_type() != get_type())								// check whether types are consistent
				{
					std::cout << "Error (node::check_outlink): unequal receiver type." << std::endl;
					exit(EXIT_FAILURE);
					return false;	
				}
				return true;																	
			}
		return false;																			// not found, return false
	}
}
