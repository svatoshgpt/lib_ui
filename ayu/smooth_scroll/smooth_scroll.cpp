// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2025
#include "smooth_scroll.h"

#include "ayu/ayu_ui_settings.h"

#include <cmath>
#include <utility>

namespace SmoothScroll {
namespace {

// Wheel steps add to one target the position eases to each frame, so the speed never restarts.
constexpr auto kTimeConstant = 70.;
constexpr auto kMaxFrameTime = crl::time(50);

} // namespace

Scroller::Scroller(
	Config config,
	anim::transition,
	crl::time)
: _config(std::move(config))
, _animation([=](crl::time now) { return step(now); }) {
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
	} else {
		_position = _config.getScroll();
		_lastSet = int(base::SafeRound(_position));
	}
	const auto from = animating ? _target : _position;
	const auto newTarget = _config.getNewTarget(
		int(base::SafeRound(from + delta)));
	if (newTarget == int(base::SafeRound(from))) {
		return true;
	}
	_target = newTarget;
	if (!animating) {
		_lastTime = crl::now();
		_animation.start();
	}
	return true;
}

// A list loading more rows moves the scroll itself; the animation follows.
void Scroller::followExternalShift() {
	const auto shift = _config.getScroll() - _lastSet;
	_position += shift;
	_target += shift;
	_lastSet += shift;
}

bool Scroller::step(crl::time now) {
	followExternalShift();
	const auto elapsed = std::clamp(now - _lastTime, crl::time(0), kMaxFrameTime);
	_lastTime = now;
	const auto part = 1. - std::exp(-float64(elapsed) / kTimeConstant);
	_position += (_target - _position) * part;
	const auto done = (std::abs(_target - _position) < 0.5);
	if (done) {
		_position = _target;
	}
	const auto wanted = int(base::SafeRound(_position));
	_config.setScroll(wanted);
	// Rows loaded while scrolling shift the list inside setScroll already.
	const auto actual = _config.getScroll();
	const auto shift = actual - _config.getNewTarget(wanted);
	_position += shift;
	_target += shift;
	_lastSet = actual;
	// A list that got shorter can't reach the old target.
	_target = _config.getNewTarget(int(base::SafeRound(_target)));
	return !done;
}

void Scroller::stop() {
	_animation.stop();
}

bool Scroller::isAnimating() const {
	return _animation.animating();
}

} // namespace SmoothScroll
