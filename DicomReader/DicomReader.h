#pragma once

#ifdef DLL_EXPORTS
#define DICOM_READER_EXPORT __declspec(dllexport)
#else
#define DICOM_READER_EXPORT __declspec(dllimport)
#endif

#include <iostream>
#include <vector>
#include <map>

class DICOM_READER_EXPORT DcmData {
public:
	DcmData(std::string dcm_path, bool dcm_multiFrame = false);
	~DcmData();

	void LoadSingleFrameData(std::string file_path);
	void LoadMultiFrameData(std::string file_path);

public:
	std::string folder_path;
	std::vector<std::string> file_names;

	std::map<unsigned int, int> instance_number_to_idx_map;

	bool is_img_inverse;
	float img_pixel_spacing[2];
	float img_slice_thickness;
	unsigned short img_width;
	unsigned short img_height;
	unsigned short img_bit_num;
	std::string img_sample_num;
	std::string img_modality;
	float img_rescale_slope;
	float img_rescale_intercept;
	int img_planar_configuration;
	int slice_num;
	float distance_source_detector;
	float distance_source_patient;

	short *volume_buf;
};
