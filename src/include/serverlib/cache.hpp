#ifndef WEBSERVER_CACHE_HPP
#define WEBSERVER_CACHE_HPP

#include <cstdio>
#include <fstream>
#include <optional>
#include <string>
#include <unordered_map>
#include <queue>

template<>
class ContentParser<std::string> {
public:
	constexpr std::string encode(const std::string& content) {
		return content;
	}

	constexpr std::optional<std::string> decode(const std::string& content) {
		return content;
	}
};

template <typename T>
class ContentParser {
	static_assert(false, "No parser for type T ");
};

template <typename K, typename V>
class FileStore {
	int id;
	std::unordered_map<K, std::string> index;
	ContentParser<V> parser;
	std::string path;
public:
	std::optional<V> get(K key) {
		std::fstream file(path + index[key]);
		if (!file.is_open()) {
			return std::nullopt;
		}
		
		std::string content((std::istreambuf_iterator<char>(file)),
												std::istreambuf_iterator<char>());
		return parser.decode(content);
	};
	
	void put(K key, V value) {
		index[key] = std::to_string(id++);

		parser.encode(value);

		std::fstream file(path + index[key], std::ios::out);
		file << value;
		file.close();
	};
	
	void remove(K key) {
		file.remove(path + index[key]);	
		index.erase(key);
	};

	void clear() {
		for (const auto &[key, value] : index) {
			file.remove(path + value);
		}

		index.clear();
	};
};

template <typename K, typename V>
class InMemoryStore {
	std::unordered_map<K, V> store;

public:
	std::optional<V> get(K key) {
		auto it = store.find(key);
		if (it == store.end()) {
			return std::nullopt;
		}
		return it->second;
	};
	void put(K key, V value) {
		store[key] = value;
	};
	void remove(K key) {
		store.erase(key);
	};
	void clear() {
		store.clear();
	};
};

// TODO: LRU cache
// TODO: TTL cache + background thread
// TODO: capacity cache
// TODO: replace queue with more complex/effective data structure
// TODO: correctly handle eviction of duplicate keys
template <typename K, typename V, typename Storage>
class Cache {
	std::queue<const K&> queue;
	Storage<K, V> storage;
	int capacity;
	int defaultLifetime;

public:
	virtual std::optional<V> get(K key) {
		return storage.get(key);
	};
	
	virtual void put(K key, V value) {
		if(queue.size() >= capacity) {
			evict();
		}
		storage.put(key, value);
	};
	
	// Currently does not handle removal correctly,
	// as it does not remove the key from the queue
	virtual void remove(K key) {
		storage.remove(key);
	};

	virtual void clear() {
		storage.clear();
		queue = {};
	};

private:
	// Currently does not handle eviction correctly,
	// as it removes the value even if the key would still be in the queue
	virtual void evict() {
		K key = queue.front();
		queue.pop();
		remove(key);
	};
};

#endif // WEBSERVER_CACHE_HPP