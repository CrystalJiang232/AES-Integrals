#pragma once
#include <chrono>
#include <print>
#include <optional>

using std::chrono::steady_clock;
class Func_Timer
{
public:
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

	std::string elapsed_repr() const noexcept
	{
		return std::format("{} ms", count_nanos() / 1'000'000);
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
