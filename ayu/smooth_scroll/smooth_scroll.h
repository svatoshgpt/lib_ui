// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#pragma once

#include "ui/effects/animations.h"
#include <functional>

#include "base/basic_types.h"

namespace SmoothScroll {

struct Config
{
	std::function<int()> getScroll;
	std::function<void(int)> setScroll;
	std::function<int(int)> getNewTarget;
};

class Scroller
{
public:
	explicit Scroller(
		Config config,
		anim::transition easing = anim::easeOutCubic,
		crl::time duration = crl::time(300));

	bool handleScroll(int delta);
	void stop();
	[[nodiscard]] bool isAnimating() const;

	static bool handleScroll(
		int delta,
		Ui::Animations::Simple &animation,
		float64 &targetValue,
		const Config &config,
		anim::transition easing = anim::easeOutCubic,
		crl::time duration = crl::time(300));

	static void stop(
		Ui::Animations::Simple &animation,
		float64 &targetValue);

private:
	Config _config;
	anim::transition _easing;
	crl::time _duration;

	Ui::Animations::Simple _animation;
	float64 _targetValue = -1.;
};

} // namespace SmoothScroll
