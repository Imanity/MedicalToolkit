#pragma once

#ifdef DLL_EXPORTS
#define VESSEL_EXTRACT_EXPORT __declspec(dllexport)
#else
#define VESSEL_EXTRACT_EXPORT __declspec(dllimport)
#endif

#include "../VolumeData/VolumeData.h"

class VESSEL_EXTRACT_EXPORT VesselExtract {
public:
	VesselExtract();
	~VesselExtract();

	VolumeData<short> getOutput(VolumeData<short> &v1, VolumeData<short> &v2);

private:
	void computeCoefs3(double sigma, double & B, std::array<double, 4>& b);
	VolumeData<short> IIRGaussianBlur(VolumeData<short> &v, double sigma);
	void removeSmallCluster(VolumeData<short> &v, int threshold);
};
