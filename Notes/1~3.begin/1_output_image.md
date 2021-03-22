# 简单图片处理

## 1.About

春招找到实习啦，悬着的心总算是放下来了，可以继续完成蓄谋(期待)已久的事业——从0开始制作一整套渲染管线。

唔，任务繁重，希望能在3月里完成它。

这篇博客和Pipeline关系不是很大...教程原文用的是linux系统，可以直接用`>`从管道输出成为ppm文件，但是我在window10下用visualStudio开发，需要另寻图像输出的方式。

我选择的是`stb_image.h`库，这个库功能可以说是非常强大了，但是...第一次用，看到的资料也不多，自己也不太明白这东西怎么运行。
对照着原作者的注释，磕磕绊绊的阅读，外加一点尝试和阅读源码，总算实现了png格式的图片输出，虽然成果很小，但是这是我克服源码恐惧症的一步。

## 2. 输出一张png图

```c++
#define STB_IMAGE_WRITE_IMPLEMENTATION 

#include <iostream>
#include <stdio.h>
#include "stb_image_write.h"


int main()
{
	const int image_weight(256);
	const int image_height(256);
	const int cmp(3);
	int dataIndex(0);
	std::auto_ptr<unsigned char> data(new unsigned char[image_height * image_weight * cmp]);

	//Render
	std::cout << "weight:" << image_weight << ",height:" << image_height << std::endl;

	for (int i = image_height-1; i >= 0; --i)
	{
		for (int j = 0; j < image_weight; ++j)
		{
			auto r = double(i) / (image_weight - 1);
			auto g = double(j) / (image_height - 1);
			auto b = 0.25;

			data.get()[dataIndex+0] = static_cast<int>(r * 255.999);
			data.get()[dataIndex+1] = static_cast<int>(g * 255.999);
			data.get()[dataIndex+2] = static_cast<int>(b * 255.999);
			//printf("r:%d,g:%d,b:%d", data[dataIndex + 0], data.get()[dataIndex + 1], data.get()[dataIndex + 2]);
			dataIndex += cmp;
		}
	}

	stbi_write_png("out.png", image_weight, image_height, cmp, data.get(), image_weight*cmp);
	return 0;
}
```

首先分析几个重要参数：

1. `cmp`，原作者写道

   > Each pixel contains 'comp' channels of data stored interleaved with 8-bits
   >    per channel, in the following order: 1=Y, 2=YA, 3=RGB, 4=RGBA. (Y is
   >    monochrome color.) 

   不难理解，就是通道数。例如RGB就是三个通道，因此我设置cmp为3。

2. data，存储所有颜色的数据结构。如果是灰度图(cmp为1)，那么data的长度就是weight\*weight，如果是RGB，那么每个像素都需要三倍的字节空间，也就需要额外\*3
   于是data的真正长度的weight\*height\*cmp。我用new的方式去为其配置空间，以免浪费栈的空间，使用`auto_ptr`确保它离开作用域时能被释放。

写入颜色的部分没什么好说的，依次顺序写入三种颜色。重点是最后一项，`image_weight*cmp`，该参数指明图片的一行有多少个字节长度。

## 3.Vector3类

Eigen库很好用，但我希望能够手写一个简单的Vector3类去代替它。

```c++
class vec3;

typedef vec3 point3;
typedef vec3 color;

inline void output_color(std::ostream& out, color cor)
{
	out << static_cast<int>(255.99 * cor.x()) << ' '
		<< static_cast<int>(255.99 * cor.y()) << ' '
		<< static_cast<int>(255.99 * cor.z()) << '\n';
}

class vec3
{
public:
	double d[3];

	vec3() : d{ 0,0,0 } {}
	vec3(double _d) : d{ _d, _d, _d } {}
	vec3(double x, double y, double z) : d{ x,y,z } {}
	
	double x() { return d[0]; }
	double y() { return d[1]; }
	double z() { return d[2]; }

	vec3 operator-() { return vec3(-d[0], -d[1], -d[2]); }
	double operator[](int i) const { return d[i]; }
	double& operator[](int i) { return d[i]; }
	
	vec3& operator+=(const vec3& _v) {
		d[0] += _v.d[0];
		d[1] += _v.d[1];
		d[2] += _v.d[2];
		return *this;
	}

	vec3& operator*=(double t){
		d[0] *= t;
		d[1] *= t;
		d[2] *= t;
		return *this;
	}

	vec3& operator/=(double t){
		//return *this *= 1 / t; //这个优先级绝了我还是加括号吧
		return ((*this) *= (1 / t)); 
	}

	double length_squared() const
	{
		return d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
	}

	double length() const {
		return std::sqrt(length_squared());
	}
};

inline std::ostream& operator<<(std::ostream& out, const vec3& v)
{
	return out << v.d[0] << ' ' << v.d[1] << ' ' << v.d[2];
}

inline vec3 operator+(const vec3& u, const vec3& v)
{
	return vec3(u.d[0] + v.d[0], u.d[1] + v.d[1], u.d[2] + v.d[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v)
{
	return vec3(u.d[0] - v.d[0], u.d[1] - v.d[1], u.d[2] - v.d[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v)
{
	return vec3(u.d[0] * v.d[0], u.d[1] * v.d[1], u.d[2] * v.d[2]);
}

inline vec3 operator*(const vec3& u, double t)
{
	return vec3(u.d[0] * t, u.d[1] * t, u.d[2] * t);
}

inline vec3 operator*(double t, const vec3& u)
{
	return u * t;
}

inline vec3 operator/(const vec3& u, double t)
{
	return u * (1 / t);
}

inline double dot(const vec3& u, const vec3& v)
{
	return u.d[0] * v.d[0] + u.d[1] * v.d[1] + u.d[2] * v.d[2];
}

inline vec3 cross(const vec3& u, const vec3& v)
{
	return vec3(
		u.d[1] * v.d[2] - u.d[2] * v.d[1],
		u.d[2] * v.d[0] - u.d[0] * v.d[2],
		u.d[0] * v.d[1] - u.d[1] * v.d[0]
	);
}

inline vec3 normalized(vec3 v)
{
	return v / v.length();
}
```

比较有趣的是叉积的求法，为了方便记忆，直接给出矩阵表达式
$$
a*b = 
\begin{vmatrix}
i,j,k
\\
a_{x},a_{y}, a_{z}
\\
b_{x}, b_{y}, b_{z}
\end{vmatrix}
$$
于是可由代数余子式求得
$$
c_{x} = a_{y}*b_{z} - a_{z}*b_{y};其余同理
$$
