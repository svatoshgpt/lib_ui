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

Scroller::Scroller(
	Config config,
	anim::transition easing,
	crl::time duration)
	: _config(std::move(config))
	  , _easing(std::move(easing))
	  , _duration(duration) {
}

bool Scroller::handleScroll(int delta) {
	if (!AyuUiSettings::isSmoothScroll()) {
		return false;
	} else if (delta == 0) {
		return true;
	}
	const auto animating = _animation.animating();
	if (animating) {
		followExternalShift();
	}
	const auto now = _config.getScroll();
	const auto current = animating ? _target : float64(now);
	const auto newTarget = _config.getNewTarget(
		int(base::SafeRound(current + delta)));
	if (newTarget == int(base::SafeRound(current))) {
		return true;
	}
	_from = now;
	_target = newTarget;
	_lastSet = now;
	_animation.start(
		[=](float64 progress) { step(progress); },
		0.,
		1.,
		_duration,
		_easing);
	return true;
}

// A list loading more rows moves the scroll itself; the animation follows.
void Scroller::followExternalShift() {
	const auto shift = _config.getScroll() - _lastSet;
	_from += shift;
	_target += shift;
	_lastSet += shift;
}

void Scroller::step(float64 progress) {
	followExternalShift();
	const auto value = _from + (_target - _from) * progress;
	_config.setScroll(int(base::SafeRound(value)));
	_lastSet = _config.getScroll();
}

void Scroller::stop() {
	_animation.stop();
}

bool Scroller::isAnimating() const {
	return _animation.animating();
}

} // namespace SmoothScroll
