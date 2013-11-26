/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_REGISTRYHELPER_H
#define UTIL_REGISTRYHELPER_H

#include <functional>
#include <memory>
#include <utility>

namespace Util {

/**
 * @brief Safe storage for a RegistryHandle
 * 
 * By storing a handle that is obtained by registering an element at a Registry
 * in an instance of this holder class, the registration of the element is
 * automatically canceled when the holder is destroyed. This class can be used 
 * to apply the Resource Acquisition Is Initialization (RAII) idiom for an
 * registered element.
 * 
 * @tparam handle_t Type of the handle stored inside the holder
 * @author Benjamin Eikel
 * @date 2013-11-26
 */
template<typename handle_t>
class RegistryHandleHolder {
	private:
		typedef std::function<handle_t (void)> register_fn_t;
		typedef std::function<void (handle_t)> unregister_fn_t;
		handle_t handle;
		unregister_fn_t callOnDestruction;

	public:
		/**
		 * Register an element at a registry by calling the given function. The
		 * handle will be stored inside this holder.
		 * 
		 * @param registerFunction A function without parameters that registers
		 * an element and returns a handle for the registered element
		 * @param unregisterFunction A function that receives a handle for an
		 * registered element and cancels the registration of that element
		 */
		RegistryHandleHolder(register_fn_t registerFunction,
							 unregister_fn_t unregisterFunction) :
			handle(registerFunction()),
			callOnDestruction(std::move(unregisterFunction)) {
		}
		/**
		 * Cancel the registration of an element by calling the stored
		 * function with the stored handle as parameter.
		 */
		~RegistryHandleHolder() {
			callOnDestruction(std::move(handle));
		}
		RegistryHandleHolder(RegistryHandleHolder &&) = default;
		RegistryHandleHolder(const RegistryHandleHolder &) = delete;
		RegistryHandleHolder & operator=(RegistryHandleHolder &&) = default;
		RegistryHandleHolder & operator=(const RegistryHandleHolder &) = delete;
};

/**
 * @brief Safe storage for an optional RegistryHandle
 * 
 * By storing a handle that is obtained by registering an element at a Registry
 * in an instance of this holder class, the registration of the element is
 * automatically canceled when the holder is destroyed. This class can be used 
 * to apply the Resource Acquisition Is Initialization (RAII) idiom for an
 * registered element. In contrast to the RegistryHandleHolder class, the
 * handle inside this class is optional. This means that an instance of this
 * class is allowed to hold nothing.
 * 
 * @tparam handle_t Type of the handle optionally stored inside the holder
 * @author Benjamin Eikel
 * @date 2013-11-26
 */
template<typename handle_t>
class OptionalRegistryHandleHolder {
	private:
		typedef std::function<handle_t (void)> register_fn_t;
		typedef std::function<void (handle_t)> unregister_fn_t;
		std::unique_ptr<handle_t> optionalHandle;
		register_fn_t registerFn;
		unregister_fn_t unregisterFn;

	public:
		/**
		 * Store the given functions. The handle is not created by calling the 
		 * @p registerFunction. The new instance will hold nothing. Call
		 * enable() to obtain a valid handle.
		 * 
		 * @param registerFunction A function without parameters that registers
		 * an element and returns a handle for the registered element
		 * @param unregisterFunction A function that receives a handle for an
		 * registered element and cancels the registration of that element
		 */
		OptionalRegistryHandleHolder(register_fn_t registerFunction,
									 unregister_fn_t unregisterFunction) :
			optionalHandle(),
			registerFn(std::move(registerFunction)),
			unregisterFn(std::move(unregisterFunction)) {
		}
		/**
		 * If a valid handle is hold, cancel the registration of an element by
		 * calling the stored function with the stored handle as parameter.
		 */
		~OptionalRegistryHandleHolder() {
			disable();
		}
		OptionalRegistryHandleHolder(OptionalRegistryHandleHolder &&) = default;
		OptionalRegistryHandleHolder(const OptionalRegistryHandleHolder &) = delete;
		OptionalRegistryHandleHolder & operator=(OptionalRegistryHandleHolder &&) = default;
		OptionalRegistryHandleHolder & operator=(const OptionalRegistryHandleHolder &) = delete;
		/**
		 * Register an element at a registry by calling the stored function.
		 * The handle will be stored inside this holder. If a valid handle is
		 * already stored, nothing will be done.
		 * 
		 * @retval true A new handle was created by calling the registration
		 * function
		 * @retval false A valid handle has already been stored before and
		 * nothing was done
		 */
		bool enable() {
			if(optionalHandle) {
				return false;
			}
			optionalHandle.reset(new handle_t(registerFn()));
			return true;
		}
		/**
		 * Cancel the registration of an element at a registry by calling the
		 * stored function. For this, the handle stored inside this holder will
		 * be used. If no valid handle is stored, nothing will be done.
		 * 
		 * @retval true The registration was canceled by using the stored
		 * handle. The handle was removed and the holder holds nothing now.
		 * @retval false No valid handle has been stored before and nothing was
		 * done
		 */
		bool disable() {
			if(!optionalHandle) {
				return false;
			}
			unregisterFn(std::move(*optionalHandle.get()));
			optionalHandle.reset();
			return true;
		}
};

}

#endif /* UTIL_REGISTRYHELPER_H */
