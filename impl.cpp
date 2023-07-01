#include "elem.h"

/*constructor*/
impl::impl(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map) :node_ptr_set_map(_node_ptr_set_map), link_ptr_set_map(_link_ptr_set_map) {}


/*destructor*/
impl::~impl() {}


/*get the type of this class*/
std::string impl::get_type() const
{
	return typeid(*this).name() + 6;
}


/*insert _node_ptr into node_ptr_set_map*/
bool impl::insert_node(node* _node_ptr)
{
	if (_node_ptr == nullptr) return false;														// check whether the parameter(s) is/are available
	if (_node_ptr->isable() || _node_ptr->count_all_links() != 0) return false;					// node should be enabled and with no connected links
	npsm::iterator _node_set_it{ node_ptr_set_map.find(_node_ptr->get_type()) };				// search for the node set by node type
	if (_node_set_it == node_ptr_set_map.end())													// if the set is not found, create a set containing the node pointer
	{
		if (!node_ptr_set_map.insert(npsm_pair{ _node_ptr->get_type(), nps{_node_ptr} }).second)
		{
			std::cout << "Error (impl::insert_node): failing inserting node " << _node_ptr << "." << std::endl;
			exit(EXIT_FAILURE);
			return false;
		}
		_node_ptr->enable();
		return true;
	}
	else
	{
		if (_node_set_it->second.find(_node_ptr) == _node_set_it->second.end())
		{
			_node_set_it->second.insert(_node_ptr);
			_node_ptr->enable();
			return true;
		}
		else return false;																		// the node cannot be inserted twice
	}
}


/*insert _link_ptr into link_ptr_set_map*/
bool impl::insert_link(link* _link_ptr)
{
	if (_link_ptr == nullptr) return false;														// check whether the parameter(s) is/are available
	if (_link_ptr->isable() || _link_ptr->get_sender_ptr() != nullptr || _link_ptr->get_receiver_ptr() != nullptr) return false;//link should not be attached to any node
	lpsm::iterator _link_set_it{ link_ptr_set_map.find(_link_ptr->get_type()) };				// search for the link set by link type
	if (_link_set_it == link_ptr_set_map.end())													// if the set is not found, create a set containing the node pointer
	{
		if (!link_ptr_set_map.insert(lpsm_pair{ _link_ptr->get_type(), lps{_link_ptr} }).second)
		{
			std::cout << "Error (impl::insert_link): failing inserting link " << _link_ptr << "." << std::endl;
			exit(EXIT_FAILURE);		
			return false;
		}
		_link_ptr->enable();
		return true;
	}
	else
	{
		if (_link_set_it->second.find(_link_ptr) == _link_set_it->second.end())
		{
			_link_set_it->second.insert(_link_ptr);
			_link_ptr->enable();
			return true;
		}
		else return false;																		// the link cannot be inserted twice
	}
}


/*erase _node_ptr from node_ptr_set_map*/
bool impl::erase_node(node* _node_ptr)
{
	if (_node_ptr == nullptr) return false;														// check whether the parameter(s) is/are available
	if (_node_ptr->count_all_links() != 0) return false;										// node should not be connected to any link
	npsm::iterator _node_ptr_set_it{ node_ptr_set_map.find(_node_ptr->get_type()) };			// search for the link set by link type
	if (_node_ptr_set_it == node_ptr_set_map.end()) return false;								// if the set is not found, return false
	nps::iterator _node_ptr_it{ _node_ptr_set_it->second.find(_node_ptr) };						// search for the node by node pointer
	if (_node_ptr_it == _node_ptr_set_it->second.end()) return false;							// if the node is not found, return false
	else																						// if found, erase and delete
	{
		_node_ptr_set_it->second.erase(_node_ptr_it);
		delete _node_ptr;
		return true;
	}
}


/*erase _node_type from node_ptr_set_map*/
bool impl::erase_node_type(std::string const& _node_type)
{
	npsm::iterator _node_ptr_set_it{ node_ptr_set_map.find(_node_type) };
	if (_node_ptr_set_it == node_ptr_set_map.end()) return false;
	for (nps::iterator _node_ptr_it{ _node_ptr_set_it->second.begin() }; _node_ptr_it != _node_ptr_set_it->second.begin(); ++_node_ptr_it)
		if ((*_node_ptr_it)->count_all_links() != 0) return false;								// if there exists node with connected links, return false
	for (nps::iterator _node_ptr_it{ _node_ptr_set_it->second.begin() }; _node_ptr_it != _node_ptr_set_it->second.begin(); ++_node_ptr_it)
		delete *_node_ptr_it;
	_node_ptr_set_it->second.clear();
	return true;
}


/*erase all nodes from node_ptr_set_map*/
bool impl::erase_all_nodes()
{
	for (npsm::iterator _node_ptr_set_it{ node_ptr_set_map.begin() }; _node_ptr_set_it != node_ptr_set_map.begin(); ++_node_ptr_set_it)
		for (nps::iterator _node_ptr_it{ _node_ptr_set_it->second.begin() }; _node_ptr_it != _node_ptr_set_it->second.begin(); ++_node_ptr_it)
			if ((*_node_ptr_it)->count_all_links() != 0) return false;
	for (npsm::iterator _node_ptr_set_it{ node_ptr_set_map.begin() }; _node_ptr_set_it != node_ptr_set_map.begin(); ++_node_ptr_set_it)
	{
		for (nps::iterator _node_ptr_it{ _node_ptr_set_it->second.begin() }; _node_ptr_it != _node_ptr_set_it->second.begin(); ++_node_ptr_it)
			delete* _node_ptr_it;
		_node_ptr_set_it->second.clear();
	}
	return true;
}


/*erase _link_ptr from link_ptr_set_map*/
bool impl::erase_link(link* _link_ptr)
{
	if (_link_ptr == nullptr) return false;														// check whether the parameter(s) is/are available
	if (_link_ptr->get_sender_ptr() != nullptr || _link_ptr->get_receiver_ptr() != nullptr) return false;	// link should not be attacted to any node
	lpsm::iterator _link_ptr_set_it{ link_ptr_set_map.find(_link_ptr->get_type()) };			// search for the link set by link type
	if (_link_ptr_set_it == link_ptr_set_map.end()) return false;								// if the set is not found, return false
	lps::iterator _link_ptr_it{ _link_ptr_set_it->second.find(_link_ptr) };						// search for the link by link pointer
	if (_link_ptr_it == _link_ptr_set_it->second.end()) return false;							// if the link is not found, return false
	else																						// if found, erase and delete
	{
		_link_ptr_set_it->second.erase(_link_ptr_it);
		delete _link_ptr;
		return true;
	}
}


/*erase _link_type from link_ptr_set_map*/
bool impl::erase_link_type(std::string const& _link_type)
{
	lpsm::iterator _link_ptr_set_it{ link_ptr_set_map.find(_link_type) };
	if (_link_ptr_set_it == link_ptr_set_map.end()) return false;
	for (lps::iterator _link_ptr_it{ _link_ptr_set_it->second.begin() }; _link_ptr_it != _link_ptr_set_it->second.begin(); ++_link_ptr_it)
		if ((*_link_ptr_it)->get_sender_ptr() != nullptr || (*_link_ptr_it)->get_receiver_ptr() != nullptr) return false;
	for (lps::iterator _link_ptr_it{ _link_ptr_set_it->second.begin() }; _link_ptr_it != _link_ptr_set_it->second.begin(); ++_link_ptr_it)
		delete *_link_ptr_it;
	_link_ptr_set_it->second.clear();
	return true;
}


/*erase all links from link_ptr_set_map*/
bool impl::erase_all_links(link* _link_ptr)
{
	for (lpsm::iterator _link_ptr_set_it{ link_ptr_set_map.begin() }; _link_ptr_set_it != link_ptr_set_map.begin(); ++_link_ptr_set_it)
		for (lps::iterator _link_ptr_it{ _link_ptr_set_it->second.begin() }; _link_ptr_it != _link_ptr_set_it->second.begin(); ++_link_ptr_it)
			if ((*_link_ptr_it)->get_sender_ptr() != nullptr || (*_link_ptr_it)->get_receiver_ptr() != nullptr) return false;
	for (lpsm::iterator _link_ptr_set_it{ link_ptr_set_map.begin() }; _link_ptr_set_it != link_ptr_set_map.begin(); ++_link_ptr_set_it)
	{
		for (lps::iterator _link_ptr_it{ _link_ptr_set_it->second.begin() }; _link_ptr_it != _link_ptr_set_it->second.begin(); ++_link_ptr_it)
			delete* _link_ptr_it;
		_link_ptr_set_it->second.clear();
	}
	return true;
}


/*create a node(virtual method)*/
node* impl::create_node(std::string const& _node_type)
{
	return nullptr;
}


/*create a link (virtual method)*/
link* impl::create_link(std::string const& _link_type)
{
	return nullptr;
}


/*attach an outlink to _sender_ptr*/
bool impl::attach_outlink(node* const _sender_ptr, link* const _outlink_ptr)
{
	if (_sender_ptr == nullptr || _outlink_ptr == nullptr) return false;						// check whether the parameter(s) is/are available
	return _sender_ptr->attach_outlink(_outlink_ptr);
}


/*attach an inlink to _receiver_ptr*/
bool impl::attach_inlink(node* const _receiver_ptr, link* const _inlink_ptr)
{
	if (_receiver_ptr == nullptr || _inlink_ptr == nullptr) return false;						// check whether the parameter(s) is/are available
	return _receiver_ptr->attach_inlink(_inlink_ptr);
}


/*attach a link to _sender_ptr and _receiver_ptr*/
bool impl::attach_link(node* const _sender_ptr, node* const _receiver_ptr, link* const _link_ptr)
{
	if (_sender_ptr == nullptr || _receiver_ptr == nullptr || _link_ptr == nullptr) return false;	// check whether the parameter(s) is/are available
	if (_sender_ptr->get_type() != _link_ptr->get_sender_type() || _receiver_ptr->get_type() != _link_ptr->get_receiver_type()) return false;
	if (_sender_ptr->attach_outlink(_link_ptr))
	{
		if (!_receiver_ptr->attach_inlink(_link_ptr))
		{
			std::cout << "Error (impl::attach_link): failing attaching link " << _link_ptr << " between " << _sender_ptr << " and " << _receiver_ptr << "." << std::endl;
			exit(EXIT_FAILURE);
			return false;
		}
		else return true;
	}
	else return false;
}


/*detach the outlink from its sender*/
bool impl::detach_outlink(link* const _outlink_ptr)
{
	if (_outlink_ptr == nullptr || _outlink_ptr->get_sender_ptr_p() == nullptr) return false;	// check whether the parameter(s) is/are available
	return _outlink_ptr->get_sender_ptr_p()->detach_outlink(_outlink_ptr);
}


/*detach the inlink from its receiver*/
bool impl::detach_inlink(link* const _inlink_ptr)
{
	if (_inlink_ptr == nullptr || _inlink_ptr->get_receiver_ptr_p() == nullptr) return false;	// check whether the parameter(s) is/are available
	return _inlink_ptr->get_receiver_ptr_p()->detach_inlink(_inlink_ptr);
}


/*detach the link _link_ptr from its sender and receiver*/
bool impl::detach_link(link* const _link_ptr)
{
	if (_link_ptr == nullptr) return false;														// check whether the parameter(s) is/are available
	if (_link_ptr->get_sender_ptr_p() == nullptr || _link_ptr->get_receiver_ptr_p() == nullptr) return false;
	if (_link_ptr->get_receiver_ptr_p()->detach_inlink(_link_ptr))
	{
		if (!_link_ptr->get_sender_ptr_p()->detach_outlink(_link_ptr))
		{
			std::cout << "Error (impl::detach_link): failing detaching link " << _link_ptr << "." << std::endl;
			exit(EXIT_FAILURE);
			return false;
		}
		else return true;
	}
	else return false;
}


/*detach all the outlinks (from _sender_ptr to _receiver_ptr)*/
lpl const& impl::detach_outlinks(node* const _sender_ptr, node* const _receiver_ptr)
{
	if (_sender_ptr == nullptr || _receiver_ptr == nullptr) return *new lpl{};
	return _sender_ptr->detach_outlinks(_receiver_ptr);
}


/*detach all the inlinks (from _sender_ptr to _receiver_ptr)*/
lpl_const const& impl::detach_inlinks(node* const _sender_ptr, node* const _receiver_ptr)
{
	if (_receiver_ptr == nullptr || _sender_ptr == nullptr) return *new lpl_const{};
	return _receiver_ptr->detach_inlinks(_sender_ptr);
}


/*detach all the links (from _sender_ptr to _receiver_ptr)*/
lpl const& impl::detach_links(node* const _sender_ptr, node* const _receiver_ptr)
{
	lpl const& _outlinks_tmp{ detach_outlinks(_sender_ptr, _receiver_ptr) };
	lpl_const const& _inlinks_tmp{ detach_inlinks(_sender_ptr, _receiver_ptr) };
	if (_outlinks_tmp.size() != _inlinks_tmp.size())
	{
		std::cout << "Error (impl::detach_links): failing detaching links (unequal size of outlink and inlink lists) " << " between " << _sender_ptr << " and " << _receiver_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::set<link*> _outlink_ptr_set;
	std::set<link const*> _inlink_ptr_set;
	for (lpl::const_iterator it{ _outlinks_tmp.begin() }; it != _outlinks_tmp.begin(); ++it)  _outlink_ptr_set.insert(*it);
	for (lpl_const::const_iterator it_c{ _inlinks_tmp.begin() }; it_c != _inlinks_tmp.begin(); ++it_c)  _inlink_ptr_set.insert(*it_c);
	if (_outlinks_tmp.size() != _outlink_ptr_set.size() || _inlinks_tmp.size() != _inlink_ptr_set.size())
	{
		std::cout << "Error (impl::detach_links): failing detaching links (unequal size of link list and set) " << " between " << _sender_ptr << " and " << _receiver_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::set<link*>::const_iterator _it1{ _outlink_ptr_set.begin() };
	std::set<link const*>::const_iterator _it2{ _inlink_ptr_set.begin() };
	while (_it1 != _outlink_ptr_set.end() && _it2 != _inlink_ptr_set.end())
	{
		if (*_it1 != *_it2)
		{
			std::cout << "Error (impl::detach_links): failing detaching links (unequal contents of link sets) " << " between " << _sender_ptr << " and " << _receiver_ptr << "." << std::endl;
			exit(EXIT_FAILURE);
		}
		++_it1;
		++_it2;
	}
	return _outlinks_tmp;
}


/*detach all the outlinks of _sender_ptr*/
lpl const& impl::detach_all_outlinks(node* const _sender_ptr)
{
	if (_sender_ptr == nullptr) return *new lpl{};
	return _sender_ptr->detach_all_outlinks();
}


/*detach all the inlinks of _receiver_ptr*/
lpl_const const& impl::detach_all_inlinks(node* const _receiver_ptr)
{
	if (_receiver_ptr == nullptr) return *new lpl_const{};
	return _receiver_ptr->detach_all_inlinks();
}


/*detach all the outlink type (from _sender_ptr to _receiver_ptr)*/
lpl const& impl::detach_outlink_type(node* const _sender_ptr, std::string const& _receiver_type)
{
	if (_sender_ptr == nullptr) return *new lpl{};
	return _sender_ptr->detach_outlink_type(_receiver_type);
}


/*detach all the inlink type (from _sender_ptr to _receiver_ptr)*/
lpl_const const& impl::detach_inlink_type(std::string const& _sender_type, node* const _receiver_ptr)
{
	if (_receiver_ptr == nullptr) return *new lpl_const{};
	return _receiver_ptr->detach_inlink_type(_sender_type);
}


/*reattach _link_ptr to (_new_sender_ptr to _new_receiver_ptr)*/
bool impl::reattach_link(link* const _link_ptr, node* const _new_sender_ptr, node* const _new_receiver_ptr)
{
	if (_link_ptr == nullptr || _new_sender_ptr == nullptr || _new_receiver_ptr == nullptr) return false;
	if (!reattach_outlink(_link_ptr, _new_sender_ptr)) return false;
	if (!reattach_inlink(_link_ptr, _new_receiver_ptr)) return false;
	return true;
}


/*reattach _outlink_ptr to _new_sender_ptr*/
bool impl::reattach_outlink(link* const _outlink_ptr, node* const _new_sender_ptr)
{
	if (_outlink_ptr == nullptr || _new_sender_ptr == nullptr) return false;
	if (_new_sender_ptr->get_type() != _outlink_ptr->get_sender_type()) return false;
	if (_outlink_ptr->get_sender_ptr_p() == _new_sender_ptr) return true;
	if (detach_outlink(_outlink_ptr))
	{
		if (!attach_outlink(_new_sender_ptr, _outlink_ptr))
		{
			std::cout << "Error (impl::reattach_outlink): failing reattaching outlink " << _outlink_ptr << " to " << _new_sender_ptr << "." << std::endl;
			exit(EXIT_FAILURE);
			return false;
		}
		return true;
	}
	else return false;
}


/*reattach _inlink_ptr to _new_receiver_ptr*/
bool impl::reattach_inlink(link* const _inlink_ptr, node* const _new_receiver_ptr)
{
	if (_inlink_ptr == nullptr || _new_receiver_ptr == nullptr) return false;
	if (_new_receiver_ptr->get_type() != _inlink_ptr->get_receiver_type()) return false;
	if (_inlink_ptr->get_receiver_ptr_p() == _new_receiver_ptr) return true;
	if (!detach_inlink(_inlink_ptr))
	{
		if (!attach_inlink(_new_receiver_ptr, _inlink_ptr))
		{
			std::cout << "Error (impl::reattach_inlink): failing reattaching outlink " << _inlink_ptr << " to " << _new_receiver_ptr << "." << std::endl;
			exit(EXIT_FAILURE);
			return false;
		}
		return true;
	}
	else return true;
}


/*add a link (type: _link_type; from _sender_ptr to _receiver_ptr)*/
link* impl::add_link(node* const _sender_ptr, node* const _receiver_ptr, std::string const& _link_type)
{
	if (_sender_ptr == nullptr || _receiver_ptr == nullptr) return nullptr;
	link* _link_ptr{ create_link(_link_type) };
	if (_link_ptr == nullptr)
	{
		std::cout << "Error (impl::add_link): failing creating link from " << _sender_ptr << " to " << _receiver_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
		return nullptr;
	}
	if (!insert_link(_link_ptr))
	{
		std::cout << "Error (impl::add_link): failing inserting link" << _link_ptr << "." << std::endl;
		delete _link_ptr; 
		exit(EXIT_FAILURE);
		return nullptr;
	}
	if (!_sender_ptr->attach_outlink(_link_ptr))
	{
		delete _link_ptr;
		std::cout << "Error (impl::add_link): failing attaching outlink " << _link_type << " to " << _sender_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
		return nullptr;
	}
	else
	{
		if (!_receiver_ptr->attach_inlink(_link_ptr))
		{
			delete _link_ptr;
			std::cout << "Error (impl::add_link): failing adding inlink " << _link_type << " to " << _receiver_ptr << "." << std::endl;
			exit(EXIT_FAILURE);
			return nullptr;
		}
		else return _link_ptr;
	}
}


bool impl::add_link(node* const _sender_ptr, node* const _receiver_ptr, link* _link_ptr)
{
	if (_sender_ptr == nullptr || _receiver_ptr == nullptr || _link_ptr == nullptr) return false;
	if (!insert_link(_link_ptr))
	{
		std::cout << "Error (impl::add_link): failing inserting link" << _link_ptr << "." << std::endl;
		delete _link_ptr;
		exit(EXIT_FAILURE);
		return false;
	}
	if (!_sender_ptr->attach_outlink(_link_ptr))
	{
		std::cout << "Error (impl::add_link): failing attaching outlink " << _link_ptr << " to " << _sender_ptr << "." << std::endl;
		delete _link_ptr;
		exit(EXIT_FAILURE);
		return false;
	}
	else
	{
		if (!_receiver_ptr->attach_inlink(_link_ptr))
		{
			std::cout << "Error (impl::add_link): failing adding inlink " << _link_ptr << " to " << _receiver_ptr << "." << std::endl;
			delete _link_ptr;
			exit(EXIT_FAILURE);
			return false;
		}
		else return true;
	}
}


/*delete the link _link_ptr*/
bool impl::del_link(link* const _link_ptr)
{
	if (!detach_link(_link_ptr)) return false;
	if (!erase_link(_link_ptr))
	{
		std::cout << "Error (impl::del_link): failing erasing link (erasing) " << _link_ptr << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}
	return true;
}


/*delete all the links from _sender_ptr to _receiver_ptr*/
size_t impl::del_links(node* const _sender_ptr, node* const _receiver_ptr)
{
	if (_sender_ptr == nullptr || _receiver_ptr == nullptr) return 0;
	lpl const& _link_ptr_list{detach_links(_sender_ptr, _receiver_ptr)};
	size_t _count{ 0 };
	for (lpl::const_iterator it{ _link_ptr_list.begin() }; it != _link_ptr_list.end(); ++it)
	{
		if (!erase_link(*it))
		{
			std::cout << "Error (impl::del_links): failing erasing links between " << _sender_ptr << " and " << _receiver_ptr << "." << std::endl;
			exit(EXIT_FAILURE);
		}
		++_count;
	}
	return _count;
}


/*delete all the outlinks of _sender_ptr*/
size_t impl::del_all_outlinks(node* const _sender_ptr)
{
	if (_sender_ptr == nullptr) return 0;
	lpl const& _outlink_ptr_list{ detach_all_outlinks(_sender_ptr) };
	size_t _count{ 0 };
	for (lpl::const_iterator it{ _outlink_ptr_list.begin() }; it != _outlink_ptr_list.end(); ++it)
	{
		if (!detach_inlink(*it))
		{
			std::cout << "Error (impl::del_all_outlinks): failing deleting all outlinks of " << _sender_ptr << "(detaching inlink)." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (!erase_link(*it))
		{
			std::cout << "Error (impl::del_all_outlinks): failing deleting all outlinks of " << _sender_ptr << "(erasing outlink)." << std::endl;
			exit(EXIT_FAILURE);
		}
		++_count;
	}
	return _count;
}


/*delete all the inlinks of _receiver_ptr*/
size_t impl::del_all_inlinks(node* const _receiver_ptr)
{
	if (_receiver_ptr == nullptr) return 0;
	lpl_const const& _inlink_ptr_list{ detach_all_inlinks(_receiver_ptr) };
	size_t _count{ 0 };
	for (lpl_const::const_iterator it{ _inlink_ptr_list.begin() }; it != _inlink_ptr_list.end(); ++it)
	{
		if (!detach_inlink((link*)(*it)))
		{
			std::cout << "Error (impl::del_all_inlinks): failing deleting all inlinks of " << _receiver_ptr << "(detaching outlink)." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (!erase_link((link*)(*it)))
		{
			std::cout << "Error (impl::del_all_inlinks): failing deleting all inlinks of " << _receiver_ptr << "(erasing inlink)." << std::endl;
			exit(EXIT_FAILURE);
		}
		++_count;
	}
	return _count;
}


/*delete all the links of _node_ptr*/
size_t impl::del_all_links(node* const _node_ptr)
{
	return del_all_outlinks(_node_ptr) + del_all_inlinks(_node_ptr);
}


/*delete the outlinks (from _sender_ptr to _receiver_type)*/
size_t impl::del_outlink_type(node* const _sender_ptr, std::string const& _receiver_type)
{
	if (_sender_ptr == nullptr) return 0;
	lpl const& _outlink_ptr_list{ detach_outlink_type(_sender_ptr, _receiver_type) };
	size_t _count{ 0 };
	for (lpl::const_iterator it{ _outlink_ptr_list.begin() }; it != _outlink_ptr_list.end(); ++it)
	{
		if (!detach_inlink(*it))
		{
			std::cout << "Error (impl::del_outlink_type): failing deleting all outlinks of " << _sender_ptr << "(detaching inlink)." << std::endl;
			exit(EXIT_FAILURE);
			return 0;
		}
		if (!erase_link(*it))
		{
			std::cout << "Error (impl::del_outlink_type): failing deleting all outlinks of " << _sender_ptr << "(erasing outlink)." << std::endl;
			exit(EXIT_FAILURE);
			return 0;
		}
		++_count;
	}
	return _count;
}


/*delete the inlinks (from _sender_type to _receiver_ptr)*/
size_t impl::del_inlink_type(std::string const& _sender_type, node* const _receiver_ptr)
{
	if (_receiver_ptr == nullptr) return 0;
	lpl_const const& _inlink_ptr_list{ detach_inlink_type(_sender_type, _receiver_ptr) };
	size_t _count{ 0 };
	for (lpl_const::const_iterator it{ _inlink_ptr_list.begin() }; it != _inlink_ptr_list.end(); ++it)
	{
		if (!detach_inlink((link*)(*it)))
		{
			std::cout << "Error (impl::del_inlink_type): failing deleting all inlinks of " << _receiver_ptr << "(detaching outlink)." << std::endl;
			exit(EXIT_FAILURE);
			return 0;
		}
		if (!erase_link((link*)(*it)))
		{
			std::cout << "Error (impl::del_inlink_type): failing deleting all inlinks of " << _receiver_ptr << "(erasing inlink)." << std::endl;
			exit(EXIT_FAILURE);
			return 0;
		}
		++_count;
	}
	return _count;
}


/*add a node (type: _node_type)*/
node* impl::add_node(std::string const& _node_type)/*add a node(including creating a node)*/
{
	node* _node_ptr{ create_node(_node_type) };
	if (_node_ptr == nullptr)
	{
		delete _node_ptr;
		std::cout << "Error (impl::add_node): failing creating node type" << _node_type << "." << std::endl;
		exit(EXIT_FAILURE);
		return nullptr;
	}
	if (_node_ptr->get_type() != _node_type)
	{
		delete _node_ptr;
		std::cout << "Error (impl::add_node): unequal node types." << std::endl;
		exit(EXIT_FAILURE);
		return nullptr;
	}
	if (!insert_node(_node_ptr))
	{
		delete _node_ptr;
		std::cout << "Error (impl::add_node): failing inserting node " << _node_ptr << "." << std::endl;
		exit(EXIT_FAILURE);
		return nullptr;
	}
	return _node_ptr;
}


bool impl::add_node(node* _node_ptr)
{
	if (_node_ptr == nullptr) return false;
	if (!insert_node(_node_ptr))
	{
		std::cout << "Error (impl::add_node): failing inserting node " << _node_ptr << "." << std::endl;
		delete _node_ptr;
		exit(EXIT_FAILURE);
		return false;
	}
	return true;
}

/*delete a node _node_ptr*/
bool impl::del_node(node* const _node_ptr)
{
	if (_node_ptr == nullptr) return false;
	npsm::iterator _node_ptr_set_it{ node_ptr_set_map.find(_node_ptr->get_type()) };
	if (_node_ptr_set_it == node_ptr_set_map.end()) return false;
	nps::iterator _node_ptr_it{ _node_ptr_set_it->second.find(_node_ptr) };
	if (_node_ptr_it == _node_ptr_set_it->second.end()) return false;
	else
	{
		del_all_links(*_node_ptr_it);
		_node_ptr_set_it->second.erase(_node_ptr_it);
		return true;
	}
}


/*delete the node type _node_type*/
size_t impl::del_node_type(std::string const& _node_type)
{
	npsm::iterator _node_ptr_set_it{ node_ptr_set_map.find(_node_type) };
	if (_node_ptr_set_it == node_ptr_set_map.end()) return 0;
	size_t _counts{ _node_ptr_set_it->second.size() };
	for (nps::iterator _node_ptr_it{ _node_ptr_set_it->second.begin() }; _node_ptr_it != _node_ptr_set_it->second.begin(); ++_node_ptr_it)
		if (!del_all_links(*_node_ptr_it))
		{
			std::cout << "Error (impl::del_node_type): failing deleting all links of " << *_node_ptr_it << "." << std::endl;
			exit(EXIT_FAILURE);
			return 0;
		}
	erase_node_type(_node_type);
	return _counts;
}


/*delete all the nodes*/
size_t impl::del_all_nodes()
{
	size_t _counts{ 0 };
	for (npsm::iterator _node_ptr_set_it{ node_ptr_set_map.begin() }; _node_ptr_set_it != node_ptr_set_map.end(); ++_node_ptr_set_it)
	{
		_counts += _node_ptr_set_it->second.size();
		for (nps::iterator _node_ptr_it{ _node_ptr_set_it->second.begin() }; _node_ptr_it != _node_ptr_set_it->second.end(); ++_node_ptr_it)
			if (!del_all_links(*_node_ptr_it))
			{
				std::cout << "Error (impl::del_all_nodes): failing deleting all links of " << *_node_ptr_it << "." << std::endl;
				exit(EXIT_FAILURE);
				return 0;
			}
	}
	erase_all_nodes();
	return _counts;
}


/*get the sender pointer of _link_ptr*/
node* impl::get_link_sender_ptr(link const* _link_ptr)
{
	return _link_ptr->get_sender_ptr_p();
}


/*get the receiver pointer of _link_ptr*/
node* impl::get_link_receiver_ptr(link const* _link_ptr)
{
	return _link_ptr->get_receiver_ptr_p();
}


/*present the structure of the network*/
void impl::present()
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
					std::cout << (*_it1)->get_sender_ptr()->get_type() << "	" << (*_it1)->get_sender_ptr()->get_index();
					std::cout << "	 ---" << (*_it1)->get_type() << " " << (*_it1)->get_index() << "--->		";
					std::cout << (*_it1)->get_receiver_ptr()->get_type() << "	" << (*_it1)->get_receiver_ptr()->get_index();
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
					std::cout << (*_it1)->get_receiver_ptr()->get_type() << "	" << (*_it1)->get_receiver_ptr()->get_index();
					std::cout << "	 <---" << (*_it1)->get_type() << " " << (*_it1)->get_index() << "---		";
					std::cout << (*_it1)->get_sender_ptr()->get_type() << "	" << (*_it1)->get_sender_ptr()->get_index();
					std::cout << std::endl;
				}
				std::cout << std::endl;
			}
		}
	}
}




clear_all_nodes::clear_all_nodes(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map) : impl(_node_ptr_set_map, _link_ptr_set_map) {}



bool clear_all_nodes::implement() {
	del_all_nodes();
	return true;
};