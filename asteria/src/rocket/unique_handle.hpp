// This file is part of Asteria.
// Copyleft 2018, LH_Mouse. All wrongs reserved.

#ifndef ROCKET_UNIQUE_HANDLE_HPP_
#define ROCKET_UNIQUE_HANDLE_HPP_

#include <type_traits> // so many...
#include <utility> // std::move(), std::swap(), std::declval()
#include "compatibility.hpp"
#include "assert.hpp"
#include "final_allocator_wrapper.hpp"

/* Requirements:
 * 1. Handles must be trivial types other than arrays.
 * 2. Closers shall be copy-constructible and move-constructible.
 *    The following operations are required, all of which, as well as copy/move construction/assignment and swap, shall not throw exceptions.
 *    1) `Closer().null()` returns a handle value called the 'null handle value'.
 *    2) `Closer().is_null()` returns `true` if the argument is a 'null handle value' and `false` otherwise.
 *       N.B. There could more than one null handle values. It is required that `Closer().is_null(Closer().null())` is always `true`.
 *    3) `Closer().close(h)` closes the handle `h`. Null handle values will not be passed to this function.
 */

namespace rocket {

using ::std::is_array;
using ::std::is_trivial;
using ::std::decay;
using ::std::conditional;

template<typename handleT, typename closerT>
class unique_handle;

namespace details_unique_handle {
	template<typename valueT>
	inline valueT xchg(valueT &dst, typename decay<valueT>::type src){
		auto old = ::std::move(dst);
		dst = ::std::move(src);
		return old;
	}

	template<typename handleT, typename closerT>
	class stored_handle : private allocator_wrapper_base_for<closerT> {
	public:
		using handle_type  = handleT;
		using closer_type  = closerT;

	private:
		using closer_base = allocator_wrapper_base_for<closerT>;

	private:
		handle_type m_h;

	public:
		explicit stored_handle(const closer_type &close) noexcept
			: closer_base(close)
			, m_h(this->as_closer().null())
		{ }
		explicit stored_handle(closer_type &&close) noexcept
			: closer_base(::std::move(close))
			, m_h(this->as_closer().null())
		{ }
		~stored_handle(){
			this->reset(this->as_closer().null());
		}

		stored_handle(const stored_handle &) = delete;
		stored_handle & operator=(const stored_handle &) = delete;

	public:
		const closer_type & as_closer() const noexcept {
			return static_cast<const closer_base &>(*this);
		}
		closer_type & as_closer() noexcept {
			return static_cast<closer_base &>(*this);
		}

		bool test() const noexcept {
			return this->as_closer().is_null(this->m_h) == false;
		}
		handle_type get() const noexcept {
			return this->m_h;
		}
		handle_type release() noexcept {
			return ((xchg))(this->m_h, this->as_closer().null());
		}
		void reset(handle_type h_new) noexcept {
			const auto h_old = ((xchg))(this->m_h, h_new);
			if(this->as_closer().is_null(h_old)){
				return;
			}
			this->as_closer().close(h_old);
		}
		void swap(stored_handle &other) noexcept {
			::std::swap(this->m_h, other.m_h);
		}
	};
}

template<typename handleT, typename closerT>
class unique_handle {
	static_assert(is_array<handleT>::value == false, "`handleT` must not be an array type.");
	static_assert(is_trivial<handleT>::value != false, "`handleT` must be a trivial type.");

public:
	using handle_type  = handleT;
	using closer_type  = closerT;

private:
	details_unique_handle::stored_handle<handleT, closerT> m_sth;

public:
	// 23.11.1.2.1, constructors
	unique_handle() noexcept(noexcept(closer_type()))
		: m_sth(closer_type())
	{ }
	explicit unique_handle(const closer_type &close) noexcept
		: m_sth(close)
	{ }
	explicit unique_handle(handle_type h) noexcept(noexcept(closer_type()))
		: m_sth(closer_type())
	{
		this->reset(h);
	}
	unique_handle(handle_type h, const closer_type &close) noexcept
		: m_sth(close)
	{
		this->reset(h);
	}
	unique_handle(unique_handle &&other) noexcept
		: m_sth(::std::move(other.m_sth.as_closer()))
	{
		this->reset(other.release());
	}
	unique_handle(unique_handle &&other, const closer_type &close) noexcept
		: m_sth(close)
	{
		this->reset(other.release());
	}
	// 23.11.1.2.3, assignment
	unique_handle & operator=(unique_handle &&other) noexcept {
		return this->reset(other.release());
	}

public:
	// 23.11.1.2.4, observers
	handle_type get() const noexcept {
		return this->m_sth.get();
	}
	const closer_type & get_closer() const noexcept {
		return this->m_sth.as_closer();
	}
	closer_type & get_closer() noexcept {
		return this->m_sth.as_closer();
	}
	explicit operator bool () const noexcept {
		return this->m_sth.test();
	}

	// 23.11.1.2.5, modifiers
	handle_type release() noexcept {
		return this->m_sth.release();
	}
	// N.B. The return type differs from `std::unique_ptr`.
	unique_handle & reset() noexcept {
		this->m_sth.reset(this->m_sth.as_closer().null());
		return *this;
	}
	unique_handle & reset(handle_type h_new) noexcept {
		this->m_sth.reset(h_new);
		return *this;
	}

	void swap(unique_handle &other) noexcept {
		using ::std::swap;
		swap(this->m_sth.as_closer(), other.m_sth.as_closer());
		this->m_sth.swap(other.m_sth);
	}
};

template<typename handleT, typename closerT>
inline bool operator==(const unique_handle<handleT, closerT> &lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs.get() == rhs.get();
}
template<typename handleT, typename closerT>
inline bool operator!=(const unique_handle<handleT, closerT> &lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs.get() != rhs.get();
}
template<typename handleT, typename closerT>
inline bool operator<(const unique_handle<handleT, closerT> &lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs.get() < rhs.get();
}
template<typename handleT, typename closerT>
inline bool operator>(const unique_handle<handleT, closerT> &lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs.get() > rhs.get();
}
template<typename handleT, typename closerT>
inline bool operator<=(const unique_handle<handleT, closerT> &lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs.get() <= rhs.get();
}
template<typename handleT, typename closerT>
inline bool operator>=(const unique_handle<handleT, closerT> &lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs.get() >= rhs.get();
}

template<typename handleT, typename closerT>
inline bool operator==(handleT lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs == rhs.get();
}
template<typename handleT, typename closerT>
inline bool operator!=(handleT lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs != rhs.get();
}
template<typename handleT, typename closerT>
inline bool operator<(handleT lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs < rhs.get();
}
template<typename handleT, typename closerT>
inline bool operator>(handleT lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs > rhs.get();
}
template<typename handleT, typename closerT>
inline bool operator<=(handleT lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs <= rhs.get();
}
template<typename handleT, typename closerT>
inline bool operator>=(handleT lhs, const unique_handle<handleT, closerT> &rhs) noexcept {
	return lhs >= rhs.get();
}

template<typename handleT, typename closerT>
inline bool operator==(const unique_handle<handleT, closerT> &lhs, handleT rhs) noexcept {
	return lhs.get() == rhs;
}
template<typename handleT, typename closerT>
inline bool operator!=(const unique_handle<handleT, closerT> &lhs, handleT rhs) noexcept {
	return lhs.get() != rhs;
}
template<typename handleT, typename closerT>
inline bool operator<(const unique_handle<handleT, closerT> &lhs, handleT rhs) noexcept {
	return lhs.get() < rhs;
}
template<typename handleT, typename closerT>
inline bool operator>(const unique_handle<handleT, closerT> &lhs, handleT rhs) noexcept {
	return lhs.get() > rhs;
}
template<typename handleT, typename closerT>
inline bool operator<=(const unique_handle<handleT, closerT> &lhs, handleT rhs) noexcept {
	return lhs.get() <= rhs;
}
template<typename handleT, typename closerT>
inline bool operator>=(const unique_handle<handleT, closerT> &lhs, handleT rhs) noexcept {
	return lhs.get() >= rhs;
}

template<typename handleT, typename closerT>
inline void swap(unique_handle<handleT, closerT> &lhs, unique_handle<handleT, closerT> &rhs) noexcept {
	lhs.swap(rhs);
}

}

#endif
