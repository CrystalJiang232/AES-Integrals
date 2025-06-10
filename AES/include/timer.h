#pragma once
#include <chrono>
#include <print>
#include <expected>
#include <utility>

using std::chrono::steady_clock;
class Base_Timer
{
public:
	enum class time_units : unsigned char
	{
		automatic,
		ns,
		us,
		ms,
		s_p3,
		s_int
	};

	using tp_t = decltype(steady_clock::now());
	using underlying_t = std::expected<std::pair<steady_clock::time_point,steady_clock::time_point>,std::pair<steady_clock::duration,steady_clock::duration>>;
	Base_Timer();
	~Base_Timer() noexcept;
	Base_Timer(const Base_Timer&);

	steady_clock::duration current() const noexcept;
	std::string current_str(time_units = time_units::automatic) const noexcept;
	steady_clock::time_point point() const noexcept;

	void pause() noexcept;
	void resume() noexcept;
	void reset() noexcept;
	
	bool is_paused() const noexcept;
	operator bool() const noexcept;

	steady_clock::duration lap() noexcept;
	std::string lap_str(time_units = time_units::automatic) noexcept;

private:
	underlying_t cp;

	static constexpr std::string duration_conv(steady_clock::duration du,time_units tu) noexcept
	{
		if(tu == time_units::automatic)
		{
			auto val = std::chrono::duration_cast<std::chrono::nanoseconds>(du).count();
			if(val >= 1'000'000'000'000)
			{
				tu = time_units::s_int;
			}
			else if(val >= 1'000'000'000)
			{
				tu = time_units::s_p3;
			}
			else if(val >= 1'000'000)
			{
				tu = time_units::ms;
			}
			else if(val >= 1'000)
			{
				tu = time_units::us;
			}
			else
			{
				tu = time_units::ns;
			}
		}

		switch(tu)
		{
		case time_units::automatic:
			std::unreachable();
		case time_units::ns:
			return std::format("{}",std::chrono::duration_cast<std::chrono::nanoseconds>(du));
		case time_units::us:
			return std::format("{}",std::chrono::duration_cast<std::chrono::microseconds>(du));
		case time_units::ms:
			return std::format("{}",std::chrono::duration_cast<std::chrono::milliseconds>(du));
		case time_units::s_int:
			return std::format("{}",std::chrono::duration_cast<std::chrono::seconds>(du));
		case time_units::s_p3:
			return std::format("{:.3f}s",std::chrono::duration_cast<std::chrono::milliseconds>(du).count() / 1000.0);
		}

		std::unreachable(); //Reference from g++lib: prevent the warning of reaching the end of non-void function without returning(XD)
	}
};

using Func_Timer = Base_Timer; //Tempeorary solution