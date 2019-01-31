#pragma once
// glew
/*#include <GL/glew.h>*/
// Qt
/*#include <QReadWriteLock>*/
// Cpp
#include <string>
#include <vector>
#include <map>
// local
class DicomSeriesPlateImage;
// meta
/*#include "centralmanager_global.h"*/

class DcmFileFormat;

class /*CENTRALMANAGER_EXPORT*/ DicomSeriesData
{
public:
	std::string m_series_number;

	std::string m_content_time;
	std::string m_series_time;
	std::string m_series_date;
	std::string m_series_modality;
	std::string m_series_description;
	std::string m_body_part_examined;
	std::string m_institution_name;
	std::string m_manufactureer_name;

	unsigned short m_resolution[2];

	unsigned short m_bits_allocated;
	unsigned short m_high_bit;
	unsigned short m_bits_stored;

	float m_rescale_intercept;
	float m_rescale_slope;

	float m_window_center;
	float m_window_width;
	bool m_window_center_good;
	bool m_window_width_good;

	std::string m_media_storage_SOP_instance_ID;
	std::string m_image_position_patient;
	std::string m_image_orientation_patient;
	std::string m_series_instance_ID;
	std::string m_media_storage_SOP_class_UID;
	std::string m_acquisition_time;
	std::string m_samples_per_pixel;
	std::string m_photo_metric_interpretation;
	std::string m_study_instance_UID;
	std::string m_image_type;
	std::string m_pixel_representation;
	std::string m_repetition_time;
	std::string m_echo_time;
	//std::string m_referencedm_file_ID;

	std::string m_patient_position;
	float m_oriention_thickness;       //用于记录一个带有方向的层位置差

	/*GLenum m_data_type;*/
	int m_data_type;

	float m_pixel_spacing[2];
	float m_series_slice_thickness;
	bool m_series_slice_thickness_good;
	bool m_series_slice_thickness_validated;
	float m_KVP;
	float m_KVI;

	std::string m_image_data;
	std::string m_image_time;
	std::string m_institution_address;
	std::string m_station_name;
	std::string m_institutional_department_name;
	std::string m_manufactureer_model_name;

	std::string m_referenced_image_sequence;
	std::string m_contrast_bolus_agent;
	std::string m_scan_options;

	std::string m_spacing_between_slices;
	std::string m_data_collection_diameter;
	std::string m_software_versions;
	std::string m_protocol_name;
	std::string m_reconstruction_diameter;
	std::string m_gantry_detector_tilt;
	std::string m_table_height;
	std::string m_rotation_direction;

	std::string m_exposure_time;
	std::string m_XRay_tube_current;

	std::string m_exposure;
	std::string m_convolution_kernel;
	std::string m_accession_number;

	float m_largest_image_pixel_value;
	float m_smallest_image_pixel_value;
	std::string m_planar_configuration;
	std::string m_number_Of_frames;
	std::string m_SOP_instance_UID;

	//覆盖层
	std::string m_overlay_row;
	std::string m_overlay_column;
	std::string m_overlay_bit_allocated;
	std::string m_overlay_origin;
	int m_overlay_row_ori;      //用于记录附加层的其实行列
	int m_overlay_column_ori;

	std::vector<std::string> m_image_files;
	std::vector<float> m_locations;
    //std::vector<QByteArray*> m_image_byte_arrays;
    std::vector<DcmFileFormat*> m_image_file_formats;
	std::map<unsigned int, int> m_instance_number_to_idx_map;
	std::map<unsigned int, int> m_idx_to_instance_number_map;
	std::map<unsigned int, int> m_idx_to_data_pos_map;
	std::map<unsigned int, std::string> m_instance_number_to_overlay_map;
	std::map<std::string, unsigned int> m_sop_to_instance_number_map;
	std::map<unsigned int, int> m_instance_number_to_width_map;
	std::map<unsigned int, int> m_instance_number_to_height_map;
	std::map<unsigned int, int> m_instance_number_to_planarConfiguration_map;
	std::map<unsigned int, std::string> m_instance_number_to_sample_map;
	std::map<unsigned int, int> m_instance_number_to_bit_allocate_map;
	//std::map<unsigned int, int> m_instance_number_to_winwidth_map;
	//std::map<unsigned int, int> m_instance_number_to_wincenter_map;
	/*QReadWriteLock m_image_file_lock;*/

	DicomSeriesPlateImage *m_series_plate_image;
	/*QReadWriteLock m_series_plate_lock;*/
	bool m_series_plate_dirty;

	DicomSeriesData();
	~DicomSeriesData();

	bool update_slice_thickness();
};