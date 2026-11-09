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

// Each frame scrolls by a part of what is left, never to a fixed position, so a list moving itself is never fought.
constexpr auto kTimeConstant = 60.;
constexpr auto kMaxFrameTime = crl::time(50);

} // namespace

Scroller::Scroller(Config config)
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
	if (animating && ((_pending < 0.) != (delta < 0))) {
		_pending = _carry = 0.;
	}
	if (!animating || _pending == 0.) {
		// Nothing to scroll here: let the wheel reach the parent.
		const auto from = _config.getScroll();
		if (_config.getNewTarget(from + delta) == from) {
			stop();
			return false;
		}
	}
	_pending += delta;
	if (!animating) {
		_lastTime = crl::now();
		_animation.start();
	}
	return true;
}

bool Scroller::scrollBy(int delta) {
	const auto from = _config.getScroll();
	const auto to = _config.getNewTarget(from + delta);
	if (to != from) {
		_config.setScroll(to);
	}
	return (to - from) == delta;
}

bool Scroller::step(crl::time now) {
	const auto elapsed = std::clamp(now - _lastTime, crl::time(0), kMaxFrameTime);
	_lastTime = now;
	const auto part = 1. - std::exp(-float64(elapsed) / kTimeConstant);
	auto amount = _pending * part;
	const auto finish = (std::abs(_pending - amount) < 0.5);
	if (finish) {
		amount = _pending;
	}
	_pending -= amount;
	const auto total = amount + _carry;
	const auto move = int(finish ? std::round(total) : std::trunc(total));
	_carry = total - move;
	if (move != 0 && !scrollBy(move)) {
		// Reached an edge.
		_pending = _carry = 0.;
		return false;
	} else if (finish) {
		_pending = _carry = 0.;
		return false;
	}
	return true;
}

void Scroller::stop() {
	_animation.stop();
	_pending = _carry = 0.;
}

bool Scroller::isAnimating() const {
	return _animation.animating();
}

} // namespace SmoothScroll
