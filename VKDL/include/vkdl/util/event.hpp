/*************************************************************************/
/* event.hpp                                                             */
/*************************************************************************/
/* https://www.dandevlog.com/all/programming/454/                        */
/* https://github.com/dandev0206/cpp-event                               */
/* https://github.com/dandev0206/cpp-delegate                            */
/*************************************************************************/
/* Copyright (c) 2024 www.dandevlog.com                                  */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#pragma once

#include <memory>
#include <vector>
#include "delegate.hpp"

template <class... Args>
class Event {
	using list_t      = std::vector<Delegate<void(Args...)>>;
	using list_iter_t = typename list_t::iterator;

public:
	inline void operator+=(const Delegate<void(Args...)>& handler) {
		if (!list_ptr) list_ptr = std::make_unique<list_t>();
		assert(!has(handler));
		list_ptr->emplace_back(handler);
	}

	inline void operator-=(const Delegate<void(Args...)>& handler) {
		auto iter = find(handler);
		assert(iter != list_ptr->end());
		list_ptr->erase(iter);
		if (list_ptr->empty()) list_ptr.reset();
	}

	inline void invoke(Args... args) const {
		if (!list_ptr) return;
		for (auto& handler : *list_ptr)
			handler(std::forward<Args>(args)...);
	}

	inline bool has(const Delegate<void(Args...)>& handler) const {
		if (!list_ptr) return false;
		if (list_ptr->empty()) return false;
		return find(handler) != list_ptr->end();
	}

	inline bool empty() const {
		return !(bool)list_ptr;
	}

	inline operator bool() {
		return (bool)list_ptr;
	}

private:
	inline list_iter_t find(const Delegate<void(Args...)>& handler) const {
		assert(list_ptr);
		return std::find(list_ptr->begin(), list_ptr->end(), handler);
	}

	std::unique_ptr<list_t> list_ptr;
};

// delete lines below if don't needed
class EventSender abstract {};

struct EventArgs {};

typedef Event<EventSender&, EventArgs&> EventHandler;