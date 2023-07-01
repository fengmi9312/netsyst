#pragma once
#include <map>
#include <string>
#include <typeinfo>
#include <iostream>
#include <list>
#include <set>

class node;
class link;
class inctrl;
class impl;

typedef std::multimap <node const*, inctrl*> ipm;
typedef std::map <std::string, ipm> ipmm;
typedef std::pair <node const*, inctrl*> ipm_pair;
typedef std::pair <std::string, ipm> ipmm_pair;

typedef std::multimap <node const*, link*> lpm;
typedef std::map <std::string, lpm> lpmm;
typedef std::pair <node const*, link*> lpm_pair;
typedef std::pair <std::string, lpm> lpmm_pair;

typedef std::list <link*> lpl;
typedef std::list <link const*> lpl_const;

typedef std::list <node const*> npl_const;

typedef std::list <std::string> strl;

typedef std::set<link*> lps;
typedef std::map<std::string, lps> lpsm;
typedef std::pair<std::string, lps> lpsm_pair;

typedef std::set<node*> nps;
typedef std::map<std::string, nps> npsm;
typedef std::pair<std::string, nps> npsm_pair;


/**************************************************************************************************************************************************************************/
//class inlink controller
//this class is reponsible for transmitting data to receiver from link and mark whether the link has been detached.
//only class node and link are accessible to its private elements, 
//it is not a virtual class
/**************************************************************************************************************************************************************************/
class inctrl
{
private:
	link const* const inlink_ptr;																				// the inlink pointer (cannot be modified)
	node* receiver_ptr;																							// the receiver pointer
	inctrl(link const* const _inlink_ptr);																		// constructor
	~inctrl();																									// destructor
	bool receiver_attach(node* _receiver_ptr);																	// a receiver _receiver_ptr attaches the inlink
	bool receiver_detach();																						// the receiver detaches the inlink
	link const* get_inlink_ptr() const;																			// get the pointer of inlink
	node* get_receiver_ptr() const;																				// get the pointer of receiver
	friend class node;
	friend class link;
};


/**************************************************************************************************************************************************************************/
//class link
//this class is reponsible for linking two nodes directly
//only class node is accessible to its protected and private elements
//it is a virtual class
/**************************************************************************************************************************************************************************/
class link
{
private:
	static size_t link_created_amount;																			// record how many links have been created
	size_t const index;																							// record the link index
	bool able;																									// record whether the link has been put in the container
	node* sender_ptr;																							// the sender pointer
	inctrl* const inctrl_ptr;																					// the inlink controller pointer
	void enable();																								// enable this link (can only be called by bool impl::insert_link(link* _link_ptr))
	bool sender_attach(node* _sender_ptr);																		// a sender _sender_ptr attaches this link
	bool receiver_attach(node* _receiver_ptr);																	// a receiver _receiver_ptr attaches this link
	bool sender_detach();																						// the sender detaches this link
	bool receiver_detach();																						// the receiver detaches this link
	node* get_sender_ptr_p() const;																				// get the pointer of sender (object can be modified)
	node* get_receiver_ptr_p() const;																			// get the pointer of receiver (object can be modified)
	inctrl* get_inctrl_ptr() const;																				// get the pointer of inctrl
	friend class node;
	friend class impl;

protected:
	link();																										// constructor
	~link();																									// destructor

public:
	virtual std::string get_sender_type() const = 0;															// get the type of sender
	virtual std::string get_receiver_type() const = 0;															// get the type of receiver
	virtual std::string get_type() const;																		// get the type of this link
	node const* get_sender_ptr() const;																			// get the pointer of sender
	node const* get_receiver_ptr() const;																		// get the pointer of receiver
	bool isable() const;																						// check whether this link is enabled
	size_t get_index() const;																					// get the index of this link
};


/**************************************************************************************************************************************************************************/
//class node
//only class implement is accessible to its private elements, 
//it is a virtual class
/**************************************************************************************************************************************************************************/
class node
{
private:
	static size_t node_created_amount;																			// record how many nodes have been created
	size_t const index;																							// record the node index
	bool able;																									// record whether the node has been put in the container
	lpmm outlinks;																								// the map of map of outlink pointer
	ipmm inctrls;																								// the map of map of inlink controller pointer

	bool attach_outlink(link* const _outlink_ptr);																// attach an outlink
	bool attach_inlink(link* const _inlink_ptr);																// attach an inlink

	bool detach_outlink(link* const _outlink_ptr);																// detach an outlink
	bool detach_inlink(link* const _inlink_ptr);																// detach an inlink
	lpl const& detach_outlinks(node const* const _receiver_ptr);												// detach all the outlinks connected to a certain receiver
	lpl_const const& detach_inlinks(node const* const _sender_ptr);												// detach all the inlinks connected to a certain sender
	lpl const& detach_outlink_type(std::string const& _receiver_type);											// detach all the outlinks connected to the receiver type
	lpl_const const& detach_inlink_type(std::string const& _sender_type);										// detach all the inlinks connected to the sender type
	lpl const& detach_all_outlinks();																			// detach all the outlinks
	lpl_const const& detach_all_inlinks();																		// detach all the inlinks

	void enable();																								// enable this node (can only be called by bool impl::insert_node(node* _node_ptr))
	friend class impl;

protected:
	node();																										// constructor
	~node();																									// destructor
	lpl const& get_outlinks(node const* const _receiver_ptr) const;												// get all the outlinks connected to a certain receiver
	lpl const& get_outlinks(std::string const& _receiver_type) const;											// get all the outlinks connected to a certain receiver type
	lpl const& get_outlinks() const;																			// get all the outlinks
	friend class impl;

public:
	lpl_const const& find_outlinks(node const* const _receiver_ptr) const;										// find all the outlinks connected to a certain receiver
	lpl_const const& find_inlinks(node const* const _sender_ptr) const;											// find all the inlinks connected to a certain sender
	lpl_const const& find_outlinks(std::string const& _receiver_type) const;									// find all the outlinks connected to a certain receiver type
	lpl_const const& find_inlinks(std::string const& _sender_type) const;										// find all the inlinks connected to a certain sender type
	lpl_const const& find_outlinks() const;																		// find all the outlinks
	lpl_const const& find_inlinks() const;																		// find all the inlinks
	npl_const const& find_receivers(std::string const& _receiver_type) const;									// find all the receivers of _receiver_type
	npl_const const& find_senders(std::string const& _sender_type) const;										// find all the senders of _sender_type
	strl const& find_receiver_types() const;																	// find all the receiver types
	strl const& find_sender_types() const;																		// find all the sender types
	size_t count_outlinks(node const* const _receiver_ptr) const;												// count all the outlinks connected to a certain receiver
	size_t count_inlinks(node const* const _sender_ptr) const;													// count all the inlinks connected to a certain sender
	size_t count_all_outlinks() const;																			// count all the outlinks
	size_t count_all_inlinks() const;																			// count all the inlinks
	size_t count_all_links() const;																				// count all the links
	bool check_outlink(link* const _outlink_ptr) const;															// check whether the outlink is existing
	bool check_inlink(link const* const _inlink_ptr) const;														// check whether the inlink is existing
	virtual std::string get_type() const;																		// get the type of the node
	bool isable() const;																						// check whether this node is enabled
	size_t get_index() const;																					// get the index of this node
};




/**************************************************************************************************************************************************************************/
//class implement
//only class netsyst is accessible to its private and protected elements, 
//it is a virtual class
/**************************************************************************************************************************************************************************/
class impl
{
private:
	bool insert_node(node* _node_ptr);																			// insert _node_ptr into node_ptr_set_map
	bool insert_link(link* _link_ptr);																			// insert _link_ptr into link_ptr_set_map
	bool erase_node(node* _node_ptr);																			// erase _node_ptr from node_ptr_set_map
	bool erase_node_type(std::string const& _node_type);														// erase _node_type from node_ptr_set_map
	bool erase_all_nodes();																						// erase all nodes from node_ptr_set_map
	bool erase_link(link* _link_ptr);																			// erase _link_ptr from link_ptr_set_map
	bool erase_link_type(std::string const& _link_type);														// erase _link_type from link_ptr_set_map
	bool erase_all_links(link* _link_ptr);																		// erase all links from link_ptr_set_map
	friend class netsyst;

protected:
/*attach link*/
	npsm& node_ptr_set_map;
	lpsm& link_ptr_set_map;
	bool attach_outlink(node* const _sender_ptr, link* const _outlink_ptr);										// attach an outlink to _sender_ptr
	bool attach_inlink(node* const _receiver_ptr, link* const _inlink_ptr);										// attach an inlink to _receiver_ptr
	bool attach_link(node* const _sender_ptr, node* const _receiver_ptr, link* const _link_ptr);				// attach a link to _sender_ptr and _receiver_ptr

/*detach link(s)*/
	bool detach_outlink(link* const _outlink_ptr);																// detach the outlink from its sender
	bool detach_inlink(link* const _inlink_ptr);																// detach the inlink from its receiver
	bool detach_link(link* const _link_ptr);																	// detach the link _link_ptr from its sender and receiver
	lpl const& detach_outlinks(node* const _sender_ptr, node* const _receiver_ptr);								// detach all the outlinks (from _sender_ptr to _receiver_ptr)
	lpl_const const& detach_inlinks(node* const _sender_ptr, node* const _receiver_ptr);						// detach all the inlinks (from _sender_ptr to _receiver_ptr)
	lpl const& detach_links(node* const _sender_ptr, node* const _receiver_ptr);								// detach all the links (from _sender_ptr to _receiver_ptr)
	lpl const& detach_all_outlinks(node* const _sender_ptr);													// detach all the outlinks of _sender_ptr
	lpl_const const& detach_all_inlinks(node* const _receiver_ptr);												// detach all the inlinks of _receiver_ptr
	lpl const& detach_outlink_type(node* const _sender_ptr, std::string const& _receiver_type);					// detach all the outlink type (from _sender_ptr to _receiver_ptr)
	lpl_const const& detach_inlink_type(std::string const& _sender_type, node* const _receiver_ptr);			// detach all the inlink type (from _sender_ptr to _receiver_ptr)

	impl(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);														// constructor
	~impl();																									// destructor
	virtual node* create_node(std::string const& _node_type);													// create a node (virtual method)
	virtual link* create_link(std::string const& _link_type);													// create a link (virtual method)

/*add node*/
	node* add_node(std::string const& _node_type);																// add a node (type: _node_type)
	bool add_node(node* _node_ptr);

/*delete node(s)*/
	bool del_node(node* const _node_ptr);																		// delete a node _node_ptr
	size_t del_node_type(std::string const& _node_type);														// delete the node type _node_type
	size_t del_all_nodes();																						// delete all the nodes

/*add link*/
	link* add_link(node* const _sender_ptr, node* const _receiver_ptr, std::string const& _link_type);			// add a link (type: _link_type; from _sender_ptr to _receiver_ptr)
	bool add_link(node* const _sender_ptr, node* const _receiver_ptr, link* _link_ptr);

/*delete link(s)*/
	bool del_link(link* const _link_ptr);																		// delete the link _link_ptr
	size_t del_links(node* const _sender_ptr, node* const _receiver_ptr);										// delete all the links from _sender_ptr to _receiver_ptr
	size_t del_all_outlinks(node* const _sender_ptr);															// delete all the outlinks of _sender_ptr
	size_t del_all_inlinks(node* const _receiver_ptr);															// delete all the inlinks of _receiver_ptr
	size_t del_all_links(node* const _node_ptr);																// delete all the links of _node_ptr
	size_t del_outlink_type(node* const _sender_ptr, std::string const& _receiver_type);						// delete the outlinks (from _sender_ptr to _receiver_type)
	size_t del_inlink_type(std::string const& _sender_type, node* const _receiver_ptr);							// delete the inlinks (from _sender_type to _receiver_ptr)

/*reattach link*/
	bool reattach_link(link* const _link_ptr, node* const _new_sender_ptr, node* const _new_receiver_ptr);		// reattach _link_ptr to (_new_sender_ptr to _new_receiver_ptr)
	bool reattach_outlink(link* const _outlink_ptr, node* const _new_sender_ptr);								// reattach _outlink_ptr to _new_sender_ptr
	bool reattach_inlink(link* const _inlink_ptr, node* const _new_receiver_ptr);								// reattach _inlink_ptr to _new_receiver_ptr

/*get node pointer from link pointer*/
	node* get_link_sender_ptr(link const* _link_ptr);															// get the sender pointer of _link_ptr
	node* get_link_receiver_ptr(link const* _link_ptr);															// get the receiver pointer of _link_ptr

public:
	virtual bool implement() = 0;																				// implement
	virtual std::string get_type() const;																		// get the type of this class
	void present();																								// present the structure of the network
};

class clear_all_nodes :public impl
{
public:
	clear_all_nodes(npsm& _node_ptr_set_map, lpsm& _link_ptr_set_map);
	bool implement();
};