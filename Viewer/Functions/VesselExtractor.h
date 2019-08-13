#pragma once

#include "../VolumeData/VolumeData.h"

class VesselExtractor {
public:
	VesselExtractor();
	~VesselExtractor();

	VolumeData<short> getOutput(VolumeData<short> &v1, VolumeData<short> &v2);

private:
	void computeCoefs3(double sigma, double & B, std::array<double, 4>& b);
	VolumeData<short> IIRGaussianBlur(VolumeData<short> &v, double sigma);
	void removeSmallCluster(VolumeData<short> &v, int threshold);
};
