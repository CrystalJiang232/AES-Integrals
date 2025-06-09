#pragma once
#include <chrono>
#include <print>
#include <expected>

using std::chrono::steady_clock;
class Base_Timer
{
public:
	using tp_t = decltype(steady_clock::now());
	using underlying_t = std::expected<std::pair<steady_clock::time_point,steady_clock::time_point>,std::pair<steady_clock::duration,steady_clock::duration>>;
	Base_Timer();
	~Base_Timer() noexcept;
	Base_Timer(const Base_Timer&);

	steady_clock::duration current() const noexcept;
	std::string current_ms() const noexcept;
	steady_clock::time_point point() const noexcept;

	void pause() noexcept;
	void resume() noexcept;
	bool is_paused() const noexcept;
	void reset() noexcept;

	steady_clock::duration lap() noexcept;
	std::string lap_ms() noexcept;

private:
	underlying_t cp;
};

using Func_Timer = Base_Timer; //Tempeorary solution