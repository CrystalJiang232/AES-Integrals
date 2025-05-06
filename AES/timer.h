#pragma once
#include <chrono>
#include <print>
#include <optional>

using std::chrono::system_clock;
class Func_Timer
{
public:
	using tp_t = decltype(system_clock::now());
	Func_Timer() noexcept : t0{system_clock::now()}
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

	system_clock::duration elapsed_time() const noexcept
	{
		if (!t0)
		{
			return {};
		}
		return system_clock::now() - *t0;
	}

	std::string elapsed_repr() const noexcept
	{
		return std::format("{} ms", size_t(elapsed_time().count()) / 10000);
	}

	void reset() noexcept
	{
		t0 = std::nullopt;
	}

	void retime() noexcept
	{
		t0 = system_clock::now();
	}

private:
	std::optional<tp_t> t0;
};
