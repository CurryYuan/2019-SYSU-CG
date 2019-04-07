#pragma once
#include <vector>
#include <algorithm>

using namespace std;

struct Point {
	float x, y;

	Point(float _x, float _y) {
		x = _x;
		y = _y;
	}
};

void flipY(Point& a, Point& b) {
	float dy = a.y - b.y;
	b.y += 2 * dy;
}

void flipXY(Point& p) {
	float temp = p.x;
	p.x = p.y;
	p.y = temp;
}

vector<Point> getLineData(Point p1, Point p2) {

	bool isFlipXY = abs(p1.x - p2.x) < abs(p1.y - p2.y);
	//扩展3,反转xy轴
	if (isFlipXY) {
		flipXY(p1);
		flipXY(p2);
	}

	//扩展1,保证x是递增
	if (p1.x > p2.x) {
		swap(p1, p2);
	}

	//扩展2,保证y是递增
	bool isFlipY = p1.y > p2.y;
	if (isFlipXY) {
		flipY(p1, p2);
	}

	vector<Point> data;
	data.push_back(p1);

	int dx = p2.x - p1.x;
	int dy = abs(p2.y - p1.y);
	int p = 2 * dy - dx;

	for (int m = p1.x, n = p1.y; m < p2.x; ++m) {

		if (p <= 0) {
			data.push_back(Point(m + 1, n));
			p += 2 * dy;
		}
		else {
			n += 1;
			data.push_back(Point(m + 1, n));
			p += 2 * dy - 2 * dx;
		}
	}

	if (isFlipY) {
		Point base = data.front();
		for (auto &p : data) {
			flipY(base, p);
		}
	}

	if (isFlipXY) {
		for_each(data.begin(), data.end(), flipXY);
	}

	return data;
}

int min3(const int a, const int b, const int c) {
	int temp = min(a, b);
	return min(temp, c);
}

int max3(const int a, const int b, const int c) {
	int temp = max(a, b);
	return max(temp, c);
}

// 计算直线方程
std::vector<float> getLine(const Point& p0, const Point& p1) {
	// For a line Ax + By + C = 0;
	// v[0] is A, v[1] is B, v[2] is C
	std::vector<float> v;
	v.resize(3);

	v[0] = p0.y - p1.y;
	v[1] = p1.x - p0.x;
	v[2] = p0.x * p1.y - p1.x * p0.y;
	return v;
}

//填充三角形
vector<Point> getFilledTri(Point p0, Point p1, Point p2) {
	vector<Point> res;
	//最小矩形
	int xmin = min3(p0.x, p1.x, p2.x);
	int xmax = max3(p0.x, p1.x, p2.x);
	int ymin = min3(p0.y, p1.y, p2.y);
	int ymax = max3(p0.y, p1.y, p2.y);

	// Cal 3 lines paras
	vector<vector<float>> lines;
	lines.resize(3);
	lines[0] = getLine(p0, p1);
	lines[1] = getLine(p0, p2);
	lines[2] = getLine(p1, p2);

	auto f = [](const vector<float>& line, const Point& p) {
		return line[0] * p.x + line[1] * p.y + line[2];
	};

	// use flag to determine halfspaces
	int flag[3];
	flag[0] = (f(lines[0], p2) > 0) ? 1 : -1;
	flag[1] = (f(lines[1], p1) > 0) ? 1 : -1;
	flag[2] = (f(lines[2], p0) > 0) ? 1 : -1;

	for (int i = xmin; i <= xmax; ++i) {
		for (int j = ymin; j <= ymax; ++j) {
			bool inside = true;
			for (int k = 0; k < lines.size(); ++k) {
				if ((lines[k][0] * i + lines[k][1] * j + lines[k][2]) * flag[k] < 0) {
					inside = false;
					break;
				}
			}
			if (inside) {
				res.push_back(Point(i, j));
			}
		}
	}

	return res;
}

vector<Point> getTriData(Point p0, Point p1, Point p2, bool isFill = false) {
	auto data = getLineData(p1, p2);
	auto data1 = getLineData(p0, p2);
	auto data2 = getLineData(p0, p1);

	data.insert(data.end(), data1.begin(), data1.end());
	data.insert(data.end(), data2.begin(), data2.end());

	if (isFill) {
		auto insideData = getFilledTri(p0, p1, p2);
		data.insert(data.end(), insideData.begin(), insideData.end());
	}

	return data;
}

vector<float> getGlPoint(vector<Point> data, unsigned int width, unsigned int height) {
	vector<float> res;
	res.resize(data.size() * 3);
	for (int i = 0; i < data.size(); i++) {
		int j = i * 3;
		res[j] = data[i].x / width;
		res[j + 1] = data[i].y / height;
		res[j + 2] = 0;
	}

	return res;
}

void addCirclePlot(vector<Point>& pv, const Point& origin, const float x, const float y) {
	vector<Point> eightPoints = {
		Point(x + origin.x, y + origin.y), Point(y + origin.x, x + origin.y),
		Point(y + origin.x, -x + origin.y), Point(x + origin.x, -y + origin.y),
		Point(-x + origin.x, -y + origin.y), Point(-y + origin.x, -x + origin.y),
		Point(-y + origin.x, x + origin.y), Point(-x + origin.x, y + origin.y)
	};
	pv.insert(pv.end(), eightPoints.begin(), eightPoints.end());
}

vector<Point> getCircleData(Point orign, int r) {
	vector<Point> res;
	if (r < 2) {
		return res;
	}

	int x = 0;
	int y = r;
	int d = 3 - 2 * r;
	addCirclePlot(res, orign, x, y);
	while (x < y) {
		if (d < 0) {
			d = d + 4 * x + 6;
		}
		else {
			d = d + 4 * (x - y) + 10;
			--y;
		}
		++x;
		addCirclePlot(res, orign, x, y);
	}
	return res;
}