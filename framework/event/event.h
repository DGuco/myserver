#ifndef _SRC_EVENT_EVENT_H_
#define _SRC_EVENT_EVENT_H_

#include "../base/object.h"

#include <typeindex>
#include <typeinfo>
#include <vector>
#include <stdexcept>

/**
 * \brief The base event class, all events inherit from this class
 */
class Event : public CObj
{
public:
	/**
	 * \brief Default constructor
	 *
	 * @param typeIndex The type ID of the inherited class
	 * @param sender The sender of the event
	 */
	Event(CObj & sender) :
		sender(sender),
		canceled(false) {
	}


	/**
	 * \brief Empty virtual destructor
	 */
	virtual ~Event() { }


	int Initialize(){
		return 0;
	}

	int Resume(){
		return 0;
	}


	/**
	 * \brief Gets the source object for this event
	 *
	 * @return The event sender
	 */
	CObj & getSender() {
		return sender;
	}


	/**
	 * \brief Gets whether the event has been canceled
	 *
	 * @return true if the event is canceled
	 */
	bool getCanceled() {
		return canceled;
	}


	/**
	 * \brief Sets the canceled status for the event
	 *
	 * @param canceled Whether the even is canceled or not
	 */
	void setCanceled(bool canceled) {
		this->canceled = canceled;
	}

private:
	CObj & sender;
	bool canceled;

};

#endif /* _SRC_EVENT_EVENT_HPP_ */
