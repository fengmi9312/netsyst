#include "elem.h"
/**************************************************************************************************************************************************************************/
// the implementation of class link's methods
/**************************************************************************************************************************************************************************/

size_t link::link_created_amount = 0;

/*constructor*/
link::link() : inctrl_ptr(new inctrl(this)), index(link_created_amount)
{
	able = false;
	sender_ptr = nullptr;
	++link_created_amount;
}


/*destructor*/
link::~link()
{
	delete inctrl_ptr;
}


/*enable this link (can only be called by bool impl::insert_link(link* _link_ptr))*/
void link::enable()
{
	able = true;
}


/*check whether this link is enabled*/
bool link::isable() const
{
	return able;
}


/*get the index of this link*/
size_t link::get_index() const
{
	return index;
}


/*a sender _sender_ptr attaches this link*/
bool link::sender_attach(node* _sender_ptr)
{
	if (_sender_ptr == nullptr) return false;													// check whether the parameter(s) is/are available
	if (sender_ptr != nullptr) return false;													// check whether sender_ptr is not nullptr
	if ((!isable()) || (!_sender_ptr->isable())) return false;									// check whether the outlink or sender is enabled
	if (_sender_ptr->get_type() != get_sender_type()) return false;								// check whether the type of sender is correct
	sender_ptr = _sender_ptr;																	// attach
	return true;
}


/*a receiver _receiver_ptr attaches this link*/
bool link::receiver_attach(node* _receiver_ptr)
{
	return inctrl_ptr->receiver_attach(_receiver_ptr);
}


/*the sender detaches this link*/
bool link::sender_detach()
{
	if (sender_ptr == nullptr) return false;													// check whether sender_ptr is nullptr
	if (!isable())																				// check whether the outlink is enabled
	{
		std::cout << "Error (link::sender_detach): link (" << this << ") is disabled." << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}
	if(!sender_ptr->isable())																	// check whether the sender is enabled
	{
		std::cout << "Error (link::sender_detach): sender (" << sender_ptr << ") is disabled." << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}
	sender_ptr = nullptr;																		// detach
	return true;
}


/*the receiver detaches this link*/
bool link::receiver_detach()
{
	return inctrl_ptr->receiver_detach();
}


/*get the pointer of sender*/
node const* link::get_sender_ptr() const
{
	return sender_ptr;
}


/*get the pointer of receiver*/
node const* link::get_receiver_ptr() const
{
	return inctrl_ptr->get_receiver_ptr();
}


/*get the pointer of sender (object can be modified)*/
node* link::get_sender_ptr_p() const
{
	return sender_ptr;
}


/*get the pointer of receiver (object can be modified)*/
node* link::get_receiver_ptr_p() const
{
	return inctrl_ptr->get_receiver_ptr();
}


/*get the pointer of inctrl*/
inctrl* link::get_inctrl_ptr() const
{
	return inctrl_ptr;
}


/*get the type of this link*/
std::string link::get_type() const
{
	return typeid(*this).name() + 6;	
}