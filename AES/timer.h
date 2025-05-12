#pragma once
#include <chrono>
#include <print>
#include <optional>

using std::chrono::steady_clock;
class Func_Timer
{
public:
	enum class options
	{
		preserve, //Continue timing
		retime, //Set timer to zero
		reset //Disable timer
	};


	using tp_t = decltype(steady_clock::now());
	Func_Timer() noexcept : t0{steady_clock::now()}
	{

	}

	~Func_Timer()
	{
		if (*this)
		{
			std::println("{}", elapsed_repr());
		}
	}

	operator bool() const noexcept
	{
		return bool{ t0 };
	}

	steady_clock::duration elapsed_time() const noexcept
	{
		if (!t0)
		{
			return {};
		}
		return steady_clock::now() - *t0;
	}

	std::string elapsed_repr(options opt = options::preserve) noexcept
	{
		auto ret = std::format("{} ms", count_nanos() / 1'000'000);
		
		switch (opt)
		{
		case options::retime:
			t0 = steady_clock::now();
			break;
		case options::reset:
			t0 = std::nullopt;
			break;
		}

		return ret;

	}

	void reset() noexcept
	{
		t0 = std::nullopt;
	}

	void retime() noexcept
	{
		t0 = steady_clock::now();
	}

	long long count_nanos() const noexcept
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time()).count();
	}

private:
	std::optional<tp_t> t0;
};
