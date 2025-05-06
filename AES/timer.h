#pragma once
#include <chrono>
#include <print>
#include <optional>

using std::chrono::system_clock;
class Func_Timer
{
public:
	using tp_t = decltype(system_clock::now());
	constexpr Func_Timer() noexcept : t0{system_clock::now()}
	{

	}

	constexpr ~Func_Timer()
	{
		if (*this)
		{
			std::println("{}", elapsed_repr());
		}
	}

	constexpr operator bool() const noexcept
	{
		return bool{ t0 };
	}

	constexpr system_clock::duration elapsed_time() const noexcept
	{
		if (!t0)
		{
			return {};
		}
		return system_clock::now() - *t0;
	}

	std::string elapsed_repr() const noexcept
	{
		return std::format("{} ms", int(elapsed_time().count()) / 10000);
	}

	constexpr void reset() noexcept
	{
		t0 = std::nullopt;
	}

	constexpr void retime() noexcept
	{
		t0 = system_clock::now();
	}

private:
	std::optional<tp_t> t0;
};
