#pragma once

#ifdef DLL_EXPORTS
#define SKELETONIZE_EXPORT __declspec(dllexport)
#else
#define SKELETONIZE_EXPORT __declspec(dllimport)
#endif

class SKELETONIZE_EXPORT Skeletonize {
public:
	Skeletonize(short* data, short* result, int* dims);
	~Skeletonize();

	void thinning();

private:
	bool isEulerInvariant(short* neighbors, int* LUT);
	bool isSimplePoint(short* neighbors);
	void fillEulerLUT(int* LUT);
	void Octree_labeling(int octant, int label, int* cube);
	inline short GetPixel(int src_x, int src_y, int src_z, int* offset);
	void GetPosition(int index, int& x, int& y, int& z);

private:
	bool* euler_invariant;
	bool* simple_point;
	short* data;
	int* dims;
	int vn;
	short resultTagData;

	short* xs;
	short* ys;
	short* zs;
	int object_number;
};
