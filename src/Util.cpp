/*
 * Util.cpp
 *
 *  Created on: Sep 14, 2015
 *      Author: Zaeem
 */
#include "Util.h"

F32 Clamp(F32 value, F32 min, F32 max){
	if(value < min) return min;
	else if(value > max) return max;
	return value;
}


F32 Max(F32 a, F32 b){
	if(b > a) return b;
	return a;
}


F32 Sq(F32 x){
	return x * x;
}


F32 Qu(F32 x){
	return x * x * x * x;
}


I32 Sgn(F32 x){
	return (0 < x) - (x < 0);
}


F32 NormalizeAlpha(F32 alpha){
	return 0.5f * alpha + 0.5f;
}


F32 Lerp(F32 min, F32 max, F32 alpha){
	return (1.0f - alpha) * min + alpha * max;
}


F32 Coserp(F32 min, F32 max, F32 alpha){
	F32 alpha2 = (1.0f - cosf(alpha * PI)) * 0.5f;
	return (1.0f - alpha2) * min + alpha2 * max;
}


F32 SystemMagnitudeInterpolation(F32 min, F32 mid, F32 max, F32 alpha){
	if(alpha < 0.0f){
		alpha++;
		return Coserp(min, mid, alpha);
	}else{
		return Coserp(mid, max, alpha);
	}
}

F32 PrincipalAngleDeg(F32 deg){
	return deg - (I32)(deg/360) * 360; //TODO: Test
}

F32 PrincipalAngleRad(F32 rad){
	return rad - (I32)(rad/TAU) * TAU; //TODO: Test
}

F32 MinDistAngleDeg(F32 deg){
	return (deg - (I32)(deg/360) * 360) - 180.0f; //TODO: Test
}

F32 MinDistAngleRad(F32 rad){
	return (rad - (I32)(rad/TAU) * TAU) - PI; //TODO: Test
}

F32 AngularDistDeg(F32 from, F32 to){
	return MinDistAngleDeg(to - from);
}

F32 AngularDistRad(F32 from, F32 to){
	return MinDistAngleRad(to - from);
}

U64 Pow10(U32 exponent){
	U64 result = 1;
	for(U32 i = 0; i < exponent; i++){
		result *= 10;
	}
	return result;
}

U64 DecToBin(U32 dec){
	U64 bin = 0;
	for(int i = 0; dec != 0; i++){
		bin += Pow10(i) * (dec%2);
		dec /= 2;
	}
	return bin;
}
