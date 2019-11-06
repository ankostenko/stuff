#pragma once

namespace my {

	template <typename F, typename S>
	class pair {
	public:
		F first;
		S second;

		pair() = default;

		~pair() = default;

		pair(F const& first, S const& second) : first(first), second(second) {};
	};

	template <typename F, typename S>
	pair<F, S> make_pair(F const& first, S const& second) {
		return pair<F, S>(first, second);
	}
}