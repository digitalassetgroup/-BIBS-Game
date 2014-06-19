#pragma once
#ifndef IntPoint_h
#define IntPoint_h

struct IntPoint
{
	IntPoint() : x(), y() {}
	IntPoint(int x_, int y_) : x(x_), y(y_) {}

	int x;
	int y;

	bool operator==(const IntPoint& p) const { return p.x == x && p.y == y; }
};


struct IntSize
{
	IntSize() : width(), height() {}
	IntSize(int w, int h) : width(w), height(h) {}

	int width;
	int height;
};


struct FloatPoint
{
	FloatPoint() : x(), y() {}
	FloatPoint(float x_, float y_) : x(x_), y(y_) {}

	float x;
	float y;
};

#endif
