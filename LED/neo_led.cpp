#include "NeoPixelBus.h"
#include "NeoPixelAnimator.h"

#include "neo_led.h"

const uint16_t pixelCount = 4;

RgbColor black(0);
RgbColor red(255, 0, 0);
RgbColor green(0, 255, 0);
RgbColor blue(0, 0, 255);
RgbColor yellow(255, 255, 0);
RgbColor purple(255, 0, 255);
RgbColor cyan(0, 255, 255);
RgbColor white(255);

RgbColor g_led_pixelsColor(0, 0, 255);

// RgbColor colorBuf[pixelCount];

LED_STATE g_led_state = LED_EVER_BURNING;
uint16_t g_led_animTime = 1000;
uint8_t g_led_fresh = 1;

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(pixelCount);
NeoPixelAnimator animations(11);

void EverAnimUpdate(const AnimationParam &param)
{
	if (param.state == AnimationState_Started)
	{
		for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
		{
			strip.SetPixelColor(indexPixel, g_led_pixelsColor);
		}
	}
}

void TwinkleAnimUpdate(const AnimationParam &param)
{
	static AnimEaseFunction moveEase =
		NeoEase::Linear;
	// NeoEase::QuadraticInOut;
	// NeoEase::CubicInOut;
	// NeoEase::QuarticInOut;
	// NeoEase::QuinticInOut;
	// NeoEase::SinusoidalInOut;
	// NeoEase::ExponentialInOut;
	// NeoEase::CircularInOut;
	float progress = moveEase(param.progress);

	if (progress < 0.5)
	{
		for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
		{
			strip.SetPixelColor(indexPixel, g_led_pixelsColor);
		}
	}
	else
	{
		for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
		{
			strip.SetPixelColor(indexPixel, black);
		}
	}

	if (param.state == AnimationState_Completed)
	{
		animations.RestartAnimation(param.index);
	}
}

void BreatheAnimUpdate(const AnimationParam &param)
{
	static AnimEaseFunction moveEase =
		NeoEase::Linear;
	// NeoEase::QuadraticInOut;
	// NeoEase::CubicInOut;
	// NeoEase::QuarticInOut;
	// NeoEase::QuinticInOut;
	// NeoEase::SinusoidalInOut;
	// NeoEase::ExponentialInOut;
	// NeoEase::CircularInOut;
	RgbColor color;
	float progress = moveEase(param.progress);
	int16_t ratio = 0;

	if (progress <= 0.5)
	{
		ratio = progress * 2.0F * 255;
	}
	else
	{
		ratio = (1.0F - progress) * 2.0F * 255;
	}

	if (ratio >= 255)
	{
		ratio = 255;
	}
	if (ratio <= 0)
	{
		ratio = 0;
	}

	for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
	{
		color = g_led_pixelsColor.Dim((uint8_t)ratio);
		strip.SetPixelColor(indexPixel, color);
	}

	if (param.state == AnimationState_Completed)
	{
		animations.RestartAnimation(param.index);
	}
}

void MeteorAnimUpdate(const AnimationParam &param)
{
	static float ratio[pixelCount] = {0};
	static AnimEaseFunction moveEase =
		NeoEase::Linear;
	// NeoEase::QuadraticInOut;
	// NeoEase::CubicInOut;
	// NeoEase::QuarticInOut;
	// NeoEase::QuinticInOut;
	// NeoEase::SinusoidalInOut;
	// NeoEase::ExponentialInOut;
	// NeoEase::CircularInOut;

	RgbColor color;
	float progress = moveEase(param.progress);
	float temp = 0.0;

	if (ratio[pixelCount / 2] < 0.1) // not initialize
	{
		if (pixelCount > 2)
		{
			float one_piece = 1.0F / ((pixelCount / 2) + (pixelCount % 2) - 1);
			for (uint16_t i = 0; i < (uint16_t)(pixelCount / 2); i++)
			{
				ratio[i] = i * one_piece;
				if (ratio[i] > 1.0F)
				{
					ratio[i] = 1.0F;
				}
				if (ratio[i] < 0.0F)
				{
					ratio[i] = 0.0F;
				}
				ratio[pixelCount - 1 - i] = ratio[i];
			}
			ratio[pixelCount / 2] = 1.0F;
		}
		else
		{
			for (uint16_t i = 0; i < pixelCount; i++)
			{
				ratio[i] = 1.0F;
			}
		}
		for (uint16_t i = 0; i < pixelCount; i++)
		{
			ratio[i] = ratio[i] - 1.0F;
		}
	}

	if (progress <= 0.5)
	{
		temp = progress * 4;
	}
	else
	{
		temp = (1.0F - progress) * 4;
	}
	for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
	{
		int16_t temp_ratio = (ratio[indexPixel] + temp) * 255;
		if (temp_ratio > 255)
		{
			temp_ratio = 255;
		}
		if (temp_ratio < 0)
		{
			temp_ratio = 0;
		}
		color = g_led_pixelsColor.Dim((uint8_t)temp_ratio);
		strip.SetPixelColor(indexPixel, color);
	}

	if (param.state == AnimationState_Completed)
	{
		animations.RestartAnimation(param.index);
	}
}

void TwinkleOnceAnimUpdate(const AnimationParam &param)
{
	static AnimEaseFunction moveEase =
		NeoEase::Linear;
	// NeoEase::QuadraticInOut;
	// NeoEase::CubicInOut;
	// NeoEase::QuarticInOut;
	// NeoEase::QuinticInOut;
	// NeoEase::SinusoidalInOut;
	// NeoEase::ExponentialInOut;
	// NeoEase::CircularInOut;
	float progress = moveEase(param.progress);

	if (progress < 0.5)
	{
		for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
		{
			strip.SetPixelColor(indexPixel, g_led_pixelsColor);
		}
	}
	else
	{
		for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
		{
			strip.SetPixelColor(indexPixel, black);
		}
	}
}

void CometAnimUpdate(const AnimationParam &param)
{
	static uint16_t lastPixel = 0xFFFF; // track the eye position
	static int8_t moveDir = 1;			// track the direction of movement

	// uncomment one of the lines below to see the effects of
	// changing the ease function on the movement animation
	static AnimEaseFunction moveEase =
		// NeoEase::Linear;
		NeoEase::QuadraticInOut;
	// NeoEase::CubicInOut;
	// NeoEase::QuarticInOut;
	// NeoEase::QuinticInOut;
	// NeoEase::SinusoidalInOut;
	// NeoEase::ExponentialInOut;
	// NeoEase::CircularInOut;

	// apply the movement animation curve
	float progress = moveEase(param.progress);

	// use the curved progress to calculate the pixel to effect
	uint16_t nextPixel;
	if (moveDir > 0)
	{
		nextPixel = progress * pixelCount;
	}
	else
	{
		nextPixel = (1.0f - progress) * pixelCount;
	}

	// if progress moves fast enough, we may move more than
	// one pixel, so we update all between the calculated and
	// the last
	if (lastPixel != nextPixel)
	{
		for (uint16_t i = lastPixel + moveDir; i != nextPixel; i += moveDir)
		{
			strip.SetPixelColor(i, g_led_pixelsColor);
		}
		lastPixel = nextPixel;
	}
	strip.SetPixelColor(nextPixel, g_led_pixelsColor);

	if (param.state == AnimationState_Completed)
	{
		// reverse direction of movement
		moveDir *= -1;

		lastPixel = 0xFFFF;

		// done, time to restart this position tracking animation/timer
		animations.RestartAnimation(param.index);
	}
}

void FadeAnimUpdate(const AnimationParam &param)
{
	RgbColor color;

	if (param.state == AnimationState_Completed)
	{
		for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
		{
			color = strip.GetPixelColor(indexPixel);
			color.Darken(10);
			strip.SetPixelColor(indexPixel, color);
		}
		animations.RestartAnimation(param.index);
	}
}

void LightenL2RAnimUpdate(const AnimationParam &param)
{
	static uint16_t lastPixel = 0xFFFF;
	static AnimEaseFunction moveEase =
		NeoEase::Linear;
	// NeoEase::QuadraticInOut;
	// NeoEase::CubicInOut;
	// NeoEase::QuarticInOut;
	// NeoEase::QuinticInOut;
	// NeoEase::SinusoidalInOut;
	// NeoEase::ExponentialInOut;
	// NeoEase::CircularInOut;
	if (param.state == AnimationState_Started)
	{
		for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
		{
			strip.SetPixelColor(indexPixel, black);
		}
	}
	else if (param.state != AnimationState_Completed)
	{
		// apply the movement animation curve
		float progress = moveEase(param.progress);
		uint16_t nextPixel = (uint16_t)(progress * pixelCount) - 1;
		if (lastPixel != nextPixel)
		{
			strip.SetPixelColor(nextPixel, g_led_pixelsColor);
			lastPixel = nextPixel;
		}
	}
	else
	{
		strip.SetPixelColor(lastPixel + 1, g_led_pixelsColor);
	}
}

void ShutR2LAnimUpdate(const AnimationParam &param)
{
	static uint16_t lastPixel = pixelCount;
	static AnimEaseFunction moveEase =
		NeoEase::Linear;
	// NeoEase::QuadraticInOut;
	// NeoEase::CubicInOut;
	// NeoEase::QuarticInOut;
	// NeoEase::QuinticInOut;
	// NeoEase::SinusoidalInOut;
	// NeoEase::ExponentialInOut;
	// NeoEase::CircularInOut;
	if (param.state == AnimationState_Started)
	{
		for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
		{
			strip.SetPixelColor(indexPixel, g_led_pixelsColor);
		}
	}
	else if (param.state != AnimationState_Completed)
	{
		// apply the movement animation curve
		float progress = moveEase(param.progress);
		uint16_t nextPixel = (uint16_t)((1.0 - progress) * pixelCount) + 1;
		if (lastPixel != nextPixel)
		{
			strip.SetPixelColor(nextPixel, black);
			lastPixel = nextPixel;
		}
	}
	else
	{
		strip.SetPixelColor(lastPixel - 1, black);
	}
}

void RaiseAnimUpdate(const AnimationParam &param)
{
	static AnimEaseFunction moveEase =
		// NeoEase::Linear;
		// NeoEase::QuadraticInOut;
		// NeoEase::CubicInOut;
		// NeoEase::QuarticInOut;
		// NeoEase::QuinticInOut;
		// NeoEase::SinusoidalInOut;
		// NeoEase::ExponentialInOut;
		NeoEase::CircularInOut;
	RgbColor color;
	float progress = moveEase(param.progress);
	int16_t ratio = 0;

	ratio = progress * 255;

	if (ratio >= 255)
	{
		ratio = 255;
	}
	if (ratio <= 0)
	{
		ratio = 0;
	}

	for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
	{
		color = g_led_pixelsColor.Dim((uint8_t)ratio);
		strip.SetPixelColor(indexPixel, color);
	}
}

void DeclineAnimUpdate(const AnimationParam &param)
{
	static AnimEaseFunction moveEase =
		NeoEase::Linear;
	// NeoEase::QuadraticInOut;
	// NeoEase::CubicInOut;
	// NeoEase::QuarticInOut;
	// NeoEase::QuinticInOut;
	// NeoEase::SinusoidalInOut;
	// NeoEase::ExponentialInOut;
	// NeoEase::CircularInOut;
	RgbColor color;
	float progress = moveEase(param.progress);
	int16_t ratio = 0;

	ratio = 255 - progress * 255;

	if (ratio >= 255)
	{
		ratio = 255;
	}
	if (ratio <= 0)
	{
		ratio = 0;
	}

	for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
	{
		color = g_led_pixelsColor.Dim((uint8_t)ratio);
		strip.SetPixelColor(indexPixel, color);
	}
}

void NeoPixelsSetup(void)
{
	strip.Begin();
	strip.Show();
}

void LedsUpdate(void)
{
	static LED_STATE state_last = LED_EVER_BURNING;
	static uint16_t time_last = 0;
	static RgbColor color_last = black;

	if (g_led_state != state_last || g_led_animTime != time_last || g_led_pixelsColor != color_last || g_led_fresh)
	{
		animations.StopAll(); // stop all animation

		switch (g_led_state)
		{
		case LED_EVER_BURNING:
			animations.StartAnimation(0, g_led_animTime, EverAnimUpdate);
			break;
		case LED_TWINKLE:
			animations.StartAnimation(1, g_led_animTime, TwinkleAnimUpdate);
			break;
		case LED_BREATHE:
			animations.StartAnimation(2, g_led_animTime, BreatheAnimUpdate);
			break;
		case LED_METEOR:
			animations.StartAnimation(3, g_led_animTime, MeteorAnimUpdate);
			break;
		case LED_TWINKLE_ONCE:
			animations.StartAnimation(4, g_led_animTime, TwinkleOnceAnimUpdate);
			break;
		case LED_COMET:
			// fade all pixels providing a tail that is longer the faster
			// the pixel moves.
			animations.StartAnimation(10, g_led_animTime / 40, FadeAnimUpdate);
			// take several seconds to move pot fron one side to the other
			animations.StartAnimation(5, g_led_animTime, CometAnimUpdate);
			break;
		case LED_LIGHTEN_L2R:
			animations.StartAnimation(6, g_led_animTime, LightenL2RAnimUpdate);
			break;
		case LED_SHUTDOWN_R2L:
			animations.StartAnimation(7, g_led_animTime, ShutR2LAnimUpdate);
			break;
		case LED_RAISE:
			animations.StartAnimation(8, g_led_animTime, RaiseAnimUpdate);
			break;
		case LED_DECLINE:
			animations.StartAnimation(9, g_led_animTime, DeclineAnimUpdate);
			break;

		default:
			animations.StartAnimation(0, 2000, EverAnimUpdate);
			break;
		}

		state_last = g_led_state;
		time_last = g_led_animTime;
		color_last = g_led_pixelsColor;
		g_led_fresh = 0;
	}
	else
	{
		animations.UpdateAnimations();
		strip.Show();
	}
}

void LedsStateSet(LED_STATE state)
{
	g_led_state = state;
}

LED_STATE LedsStateGet(void)
{
	return g_led_state;
}

void LedsColorSet(uint8_t colorCode)
{
	switch (colorCode)
	{
	case 0:
		g_led_pixelsColor = black;
		break;
	case 1:
		g_led_pixelsColor = red;
		break;
	case 2:
		g_led_pixelsColor = green;
		break;
	case 3:
		g_led_pixelsColor = blue;
		break;
	case 4:
		g_led_pixelsColor = yellow;
		break;
	case 5:
		g_led_pixelsColor = purple;
		break;
	case 6:
		g_led_pixelsColor = cyan;
		break;
	case 7:
		g_led_pixelsColor = white;
		break;

	default:
		break;
	}
}

void LedsAnimTimeSet(uint16_t time)
{
	g_led_animTime = time;
}

void LedsForceFresh(void)
{
	g_led_fresh = 1;
}
