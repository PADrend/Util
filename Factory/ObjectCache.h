/*
	This file is part of the Util library.
	Copyright (C) 2020 Sascha Brandt <myeti@mail.upb.de>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_FACTORY_OBJECTCACHE_H
#define UTIL_FACTORY_OBJECTCACHE_H

#include "LambdaFactory.h"
#include "../Utils.h"

#include <unordered_map>

namespace Util {
	
//! @addtogroup factory
//! @{

/**
 * @brief Generic object cache for objects allowing lambdas with variable number of arguments as cache functions.
 *
 * The cache template can be instaniated to generate different kinds of caches.
 * The generated cache generates and caches objects of type @a ObjectType.
 * It hashes the parameters used to create an object and stores it in a map.
 * If an object with the same hash already exists, it is returned instead of creating a new one.
 *
 * @tparam ObjectType Base type for all objects that are generated by the factory
 * @tparam IdentifierType Type of the identifier that specifies which object cache to call
 * @tparam FallbackPolicy Template with a function @a onUnknownType() that handles the case that the requested object type was not found
 */
template < class ObjectType,
		 typename IdentifierType,
		 template<class, typename> class FallbackPolicy = FallbackPolicies::ExceptionFallback >
class ObjectCache {
	private:
		typedef std::unordered_map<size_t, ObjectType> cache_t;
		struct Cache final {
			void* creator;
			const std::type_info* signature;
			cache_t cache;
		};
		typedef std::unordered_map<IdentifierType, Cache> registrations_t;
		registrations_t registrations;
	public:
		typedef FallbackPolicy<ObjectType, IdentifierType> fallbackPolicy_t;
		fallbackPolicy_t fallbackPolicy;
		ObjectCache() : fallbackPolicy() {
		}
		ObjectCache(fallbackPolicy_t policy) : fallbackPolicy(policy) {
		}
		~ObjectCache() {
			for(auto entry : registrations)
				delete static_cast<std::function<ObjectType()>*>(entry.second.creator);
		}

		template<typename ObjectCreator>
		void registerType(const IdentifierType & id, ObjectCreator creator) {
			if(registrations.find(id) != registrations.end())
				throw std::invalid_argument("the cache already exists");

			auto function = new decltype(to_function(creator))(to_function(creator));

			registrations[id].creator = static_cast<void*>(function);
			registrations[id].signature = &typeid(function);
		}

		void unregisterType(const IdentifierType & id) {
			if(registrations.find(id) == registrations.end())
				return;
			delete static_cast<std::function<ObjectType()>*>(registrations[id].function);
			registrations.erase(id);
		}

		bool hasType(const IdentifierType& id) {
			return registrations.find(id) != registrations.end();
		}

		template<typename ...Args>
		ObjectType create(const IdentifierType & id, Args... args) {
			if(registrations.find(id) == registrations.end())
				return fallbackPolicy.onUnknownType(std::bind(&ObjectCache::create<>, this, std::placeholders::_1), id);

			auto& registration = registrations.at(id);
			auto creator = static_cast<std::function<ObjectType(Args...)>*>(registration.creator);

			if(typeid(creator) != *(registration.signature))
				throw std::bad_typeid();
			
			size_t hash = 0;
			hash_param(hash, args...);
			auto it = registration.cache.find(hash);
			if(it != registration.cache.end())
				return it->second;
			
			ObjectType obj = (*creator)(args...);
			registration.cache.emplace(hash, obj);
			return obj;
		}

		void release(const IdentifierType & id, size_t hash) {
			if(registrations.find(id) == registrations.end())
				return fallbackPolicy.onUnknownType(std::bind(&ObjectCache::create<>, this, std::placeholders::_1), id);
			auto& registration = registrations.at(id);
			registration.cache.erase(hash);
		}

		void reset() {
			for(auto entry : registrations)
				entry.cache.clear();
		}
};

//! @}

}

#endif /* UTIL_FACTORY_OBJECTCACHE_H */
