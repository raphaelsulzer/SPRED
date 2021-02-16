#ifndef MAP_WITH_POINTER_H_
#define MAP_WITH_POINTER_H_



#include <memory>
#include <map>
#include <boost/optional.hpp>

/**
 * Class that stores a map from key to value (unique) pointers.
 */
template<typename Key, typename Value> class Map_with_pointers{

private:
	typedef typename std::unique_ptr<Value> Value_ptr;
	std::map<Key,Value_ptr> key_to_value_;



public:
	typedef typename std::map<Key,Value_ptr>::iterator Pair_iterator;
	typedef typename std::map<Key,Value_ptr>::const_iterator Pair_const_iterator;


	void clear(){
		key_to_value_.clear();
	}


	/**
	 * if key is present value is initialized to the value stored in key
	 * otherwise not initialized
	 */
	boost::optional<Value&> get(const Key& key){
		boost::optional<Value&> res;
		auto it=key_to_value_.find(key);
		if(it != key_to_value_.cend()) res = *(it->second);
		return res;
	}


	/**
	 * if key is present value is initialized to the value stored in key
	 * otherwise not initialized
	 */
	boost::optional<const Value&> get(const Key& key) const{
		boost::optional<const Value&> res;
		auto it=key_to_value_.find(key);
		if(it != key_to_value_.cend()) res = *(it->second);
		return res;
	}

	/**
	 * test if key is stored
	 */
	bool contains(const Key& key) const{
		boost::optional<Value&> res;
		return(key_to_value_.find(key) != key_to_value_.cend());
	}

	/**
	 * number of pair<Key,Value> stored
	 */
	size_t size() const{
		return key_to_value_.size();
	}


	/**
	 * true iff key was not already there
	 * otherwise returns false and do nothing
	 */
	std::pair<Pair_iterator,bool> add(const Key& key, const Value& value){
		auto ret = key_to_value_.insert(std::make_pair(key,Value_ptr(new Value(value))));
		return ret;
	}

	/**
	 * true iff key was there
	 * otherwise returns false and do nothing
	 */
	bool remove(const Key& key){
		return(key_to_value_.erase(key) == 1);
	}


	Pair_iterator begin(){
		return(key_to_value_.begin());
	}
	Pair_const_iterator begin() const{
		return(key_to_value_.cbegin());
	}

	Pair_iterator end(){
		return(key_to_value_.end());
	}

	Pair_const_iterator end() const{
		return(key_to_value_.cend());
	}


	// todo begin/end for value/key

};

#endif //MAP_WITH_POINTER_H_
