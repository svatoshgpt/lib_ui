// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "smooth_scroll.h"

#include "ayu/ayu_ui_settings.h"
#include <utility>

namespace SmoothScroll {
namespace {

bool HandleScrollLogic(
	int delta,
	Ui::Animations::Simple &animation,
	float64 &targetValue,
	const Config &config,
	anim::transition easing,
	crl::time duration) {
	if (!AyuUiSettings::isSmoothScroll()) {
		// default scroll will be used instead
		return false;
	}

	if (delta == 0) {
		return true;
	}

	const auto from = config.getScroll();
	const auto currentTarget = animation.animating()
								   ? targetValue
								   : float64(from);

	const auto newTarget = config.getNewTarget(currentTarget + delta);

	if (newTarget != int(currentTarget)) {
		targetValue = newTarget;
		animation.start(
			[&, setScroll = config.setScroll](float64 value)
			{
				setScroll(base::SafeRound(value));
				if (!animation.animating()) {
					targetValue = -1.;
				}
			},
			from,
			newTarget,
			duration,
			easing);
	}
	return true;
}

}

Scroller::Scroller(
	Config config,
	anim::transition easing,
	crl::time duration)
	: _config(std::move(config))
	  , _easing(std::move(easing))
	  , _duration(duration) {
}

bool Scroller::handleScroll(int delta) {
	return HandleScrollLogic(
		delta,
		_animation,
		_targetValue,
		_config,
		_easing,
		_duration);
}

void Scroller::stop() {
	if (isAnimating()) {
		_animation.stop();
		_targetValue = -1.;
	}
}

bool Scroller::isAnimating() const {
	return _animation.animating();
}

// static methods
bool Scroller::handleScroll(
	int delta,
	Ui::Animations::Simple &animation,
	float64 &targetValue,
	const Config &config,
	anim::transition easing,
	crl::time duration) {
	return HandleScrollLogic(
		delta,
		animation,
		targetValue,
		config,
		std::move(easing),
		duration);
}

void Scroller::stop(
	Ui::Animations::Simple &animation,
	float64 &targetValue) {
	if (animation.animating()) {
		animation.stop();
		targetValue = -1.;
	}
}

} // namespace SmoothScroll
