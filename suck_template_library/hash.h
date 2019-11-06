#pragma once

#include <cassert>
#include "vector.h"
#include "pair.h"

namespace my {

	class IHash {
	public:
		int virtual getHash() = 0;
	};


	template <typename Key, typename Val>
	class map {
	private:
		int const MOD = 3000;
		size_t _size{0};
		vector<vector<pair<Key, Val>>> buf{ vector<vector<pair<Key, Val>>>(3000, vector<pair<Key, Val>>()) };
		vector<short> AllHashes{ vector<short>(5) };

	public:

		map() = default;

		~map() = default;

		void add(Key key, Val val) {
			int hash = key.getHash() % MOD;
			for (int i = 0; i < buf[hash].size(); i++) {
				if (key == buf[hash][i].first) {
					buf[hash][i].second = val;
					return;
				}
			}
			_size += 1;
			buf[hash] << make_pair(key, val);
			AllHashes << hash;
		}

		Val get(Key const& key) {
			int hash = key.getHash % MOD;
			for (int i = 0; i < buf[hash].size(); i++) {
				if (key = buf[hash][i].first)
					return buf[hash][i].second;
			}
			assert(false);
		}

		bool has(Key const& key) {
			int hash = key.getHash % MOD;
			for (int i = 0; i < buf[hash].size(); i++) {
				if (key = buf[hash][i].first)
					return true;
			}
			return false;
		}

		Val operator[](Key const& key) {
			return get(key);
		}

		void remove(Key key) {
			int hash = key.getHash() % MOD;
			for (int i = 0; i < buf[hash].size(); i++) {
				if (key == buf[hash][i].first) {
					buf[hash].remove(i);
					_size -= 1;
				}
			}
		}

		size_t size() {
			return _size;
		}

		vector<Val>& getValues() {
			vector<bool> used(3000, false);
			vector<Val> res(AllHashes.size());
			for (int i = 0; i < AllHashes.size(); i++) {
				if (!used[AllHashes[i]]) {
					for (int j = 0; j < buf[AllHashes[i]].size(); j++) {
						res << buf[AllHashes[i]][j].second;
					}
					used[AllHashes[i]] = true;
				}
			}
			return res;
		}

		vector<Key> getKeys() {
			vector<bool> used(3000, false);
			vector<Key> res(AllHashes.size());
			for (int i = 0; i < AllHashes.size(); i++) {
				if (!used[AllHashes[i]]) {
					for (int j = 0; j < buf[AllHashes[i]].size(); j++) {
						res << buf[AllHashes[i]][j].first;
					}
					used[AllHashes[i]] = true;
				}
			}
			return res;
		}

		vector<pair<Key, Val>>& getEntres() {
			vector<bool> used(3000, false);
			vector<pair<Key, Val>> res(AllHashes.size());
			for (int i = 0; i < AllHashes.size(); i++) {
				if (!used[AllHashes[i]]) {
					for (int j = 0; j < buf[AllHashes[i]].size(); j++) {
						res << buf[AllHashes[i]][j];
					}
					used[AllHashes[i]] = true;
				}
			}
			return res;
		}

	};


	template <typename T>
	class set {
	private:
		int const MOD = 3000;
		vector<vector<T>> buf{ buf(3000, vector<T>()) };
		vector<short> Hashes{ vector<short>(5) };
		size_t _size{0};
	public:

		set() = default;

		~set() = default;

		void add(T val) {
			int hash = val.getHash() % MOD;
			for (int i = 0; i < buf[hash].size(); i++) {
				if (buf[hash][i] == val)
					return;
			}
			buf[hash] << val;
			Hashes << hash;
		}

		void operator << (T const& val) {
			add(val);
		}

		void remove(T val) {
			int hash = val.getHash() % MOD;
			for (int i = 0; i < buf[hash].size(); i++) {
				if (buf[hash][i] == val) {
					buf[hash].remove(i);
					break;
				}
			}
		}

		bool has(T val) {
			int hash = val.getHash() % MOD;
			for (int i = 0; i < buf[hash].size(); i++) {
				if (buf[hash][i] == val) {
					buf[hash].remove(i);
					return true;
				}
			}
			return false;
		}

		vector<T>&& getValues() {
			vector<bool> used(3000, false);
			vector<T> res(Hashes.size());
			for (int i = 0; i < Hashes.size(); i++) {
				if (!used[Hashes[i]]) {
					used[Hashes[i]] = true;
					for (int j = 0; j < buf[Hashes[i]].size(); j++) {
						res << buf[Hashes[i]][j];
					}
				}
			}
			return res;
		}

	};

}