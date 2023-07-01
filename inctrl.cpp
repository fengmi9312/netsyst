#include "elem.h"

/**************************************************************************************************************************************************************************/
// the implementation of class inlink controller's methods
// only the inctrl with receiver_ptr == nullptr, enabled inlink and enabled receiver can be attached
// only the inctrl with receiver_ptr != nullptr, enabled inlink and enabled receiver can be detached
/**************************************************************************************************************************************************************************/

/*constructor*/
inctrl::inctrl(link const* const _inlink_ptr) : inlink_ptr(_inlink_ptr), receiver_ptr(nullptr) {}


/*destructor*/
inctrl::~inctrl(){}


/*a receiver _receiver_ptr attaches the inlink*/
bool inctrl::receiver_attach(node* _receiver_ptr)
{
	if (_receiver_ptr == nullptr) return false;													// check whether the parameter(s) is/are available
	if (receiver_ptr != nullptr) return false;													// check whether receiver_ptr is not nullptr
	if ((!inlink_ptr->isable()) || (!_receiver_ptr->isable())) return false;					// check whether the inlink or receiver is enabled
	if (_receiver_ptr->get_type() != inlink_ptr->get_receiver_type()) return false;				// check whether the type of receiver is correct
	receiver_ptr = _receiver_ptr;																// attach
	return true;
}


/*the receiver detaches the inlink*/
bool inctrl::receiver_detach()
{
	if (receiver_ptr == nullptr) return false;													// check whether receiver_ptr is nullptr
	if (!inlink_ptr->isable())																	// check whether the inlink is enabled
	{
		std::cout << "Error (inctrl::receiver_detach): inlink (" << inlink_ptr <<") is disabled." << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}
	if (!receiver_ptr->isable())																// check whether the receiver is enabled
	{
		std::cout << "Error (inctrl::receiver_detach): receiver (" << receiver_ptr << ") is disabled." << std::endl;
		exit(EXIT_FAILURE);
		return false;
	}
	receiver_ptr = nullptr;																		// detach
	return true;
}


/*get the pointer of inlink*/
link const* inctrl::get_inlink_ptr() const
{
	return inlink_ptr;
}


/*get the pointer of receiver*/
node* inctrl::get_receiver_ptr() const
{
	return receiver_ptr;
}