// dcmtk
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
// local
#include "DicomDataMgr.h"
#include "DicomPatientData.h"
#include "DicomStudyData.h"
#include "DicomSeriesData.h"
/*#include "DicomPatientPlateImage.h"*/
// meta
#include "DicomHeaderParser.h"

/*#include "../DICOMReader/DcmHandler.h"*/
// public
DicomHeaderParser::DicomHeaderParser()
{
	
}

DicomHeaderParser::~DicomHeaderParser()
{

}
//工具类 用于字符串转换
std::vector<std::string> split(const  std::string& s, const std::string& delim)
{
	std::vector<std::string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}

//bool DicomHeaderParser::parse_header_info(std::string &file_path, QByteArray* data_array)
bool DicomHeaderParser::parse_header_info(std::string file_path, DcmFileFormat* file_format)
{
    DicomDataMgr *l_data_mgr = DicomDataMgr::get_instance();
    DcmFileFormat l_file_format;
    OFCondition l_status;
    /*DcmHandler handler(&l_file_format);*/
    //if (!data_array) {
    //    l_status = handler.loadDicomFile(file_path.c_str());
    //} else {
    //    l_status = handler.loadDicomFile(*data_array);
    //}
    //if (l_status.bad())
    //    return false;
    if (!file_format) {
        /*l_status = handler.loadDicomFile(file_path.c_str());*/
		l_status = l_file_format.loadFile(file_path.c_str());
        if (l_status.bad())
            return false;
    } else {
        
    }

    //DcmDataset *l_dataset = l_file_format.getDataset();
    DcmDataset *l_dataset;
    if (!file_format) {
        l_dataset = l_file_format.getDataset();
    } else {
        l_dataset = file_format->getDataset();
    }

	// 2、读取patient信息
	OFString l_tmp_str;
	std::string l_patient_ID;
	l_status = l_dataset->findAndGetOFString(DCM_PatientID, l_tmp_str);
	if (l_status.good())
		l_patient_ID = l_tmp_str.data();
	else
	{
		//return false;
		l_patient_ID = "nameNotRecord";
	}
	l_status = l_dataset->findAndGetOFStringArray(DCM_StudyInstanceUID, l_tmp_str);
	if (!l_status.good())
		return false;

	int l_patient_idx;
	bool l_new_patient_flag = !l_data_mgr->check_patient_available(l_patient_ID, l_patient_idx);

	if (l_new_patient_flag)
	{
		DicomPatientData *l_new_patient = new DicomPatientData();
		l_new_patient->m_patient_ID = l_patient_ID;

		l_status = l_dataset->findAndGetOFStringArray(DCM_PatientName, l_tmp_str);
		if (l_status.good())
			l_new_patient->m_patient_name = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFString(DCM_PatientSex, l_tmp_str);
		if (l_status.good())
			l_new_patient->m_patient_gender = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_PatientBirthDate, l_tmp_str);
		if (l_status.good())
			l_new_patient->m_patient_birthday = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_PatientAge, l_tmp_str);
		if (l_status.good())
			l_new_patient->m_patient_age = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_PatientPosition, l_tmp_str);
		if (l_status.good())
			l_new_patient->m_patient_position = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_PatientOrientation, l_tmp_str);
		if (l_status.good())
			l_new_patient->m_patient_orientation = l_tmp_str.data();

		/*DicomPatientPlateImage *l_patient_plate_image = new DicomPatientPlateImage(l_new_patient->m_patient_name.c_str(), l_new_patient->m_patient_birthday.c_str());
		l_new_patient->m_patient_plate_image = l_patient_plate_image;*/

		l_patient_idx = l_data_mgr->append_patient(l_new_patient);
	}

	// 3、读取study信息
	std::string l_study_ID;
	l_status = l_dataset->findAndGetOFString(DCM_StudyID, l_tmp_str);
	if (l_status.good())
		l_study_ID = l_tmp_str.data();
	int l_study_idx;
	bool l_new_study_flag = !l_data_mgr->check_study_available(l_patient_idx, l_study_ID, l_study_idx);

	if (l_new_study_flag)
	{
		DicomStudyData *l_new_study = new DicomStudyData();
		l_new_study->m_study_ID = l_study_ID;

		l_status = l_dataset->findAndGetOFStringArray(DCM_StudyDate, l_tmp_str);
		if (l_status.good())
			l_new_study->m_study_date = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_StudyTime, l_tmp_str);
		if (l_status.good())
			l_new_study->m_study_time = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_StudyDescription, l_tmp_str);
		if (l_status.good())
			l_new_study->m_study_description = l_tmp_str.data();

		l_study_idx = l_data_mgr->append_study(l_patient_idx, l_new_study);
	}

	// 4、读取series信息
	std::string l_series_number;
	l_status = l_dataset->findAndGetOFString(DCM_SeriesNumber, l_tmp_str);
	if (l_status.good())
		l_series_number = l_tmp_str.data();
	int l_series_idx;
	bool l_new_series_flag = !l_data_mgr->check_series_available(l_patient_idx, l_study_idx, l_series_number, l_series_idx);

	
	if (l_new_series_flag)
	{
		DicomSeriesData *l_new_series = new DicomSeriesData();

		l_new_series->m_series_number = l_series_number;

		// 基本信息
		l_status = l_dataset->findAndGetOFString(DCM_ContentTime, l_tmp_str);
		if (l_status.good())
			l_new_series->m_content_time = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_SeriesTime, l_tmp_str);
		if (l_status.good())
			l_new_series->m_series_time = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_SeriesDate, l_tmp_str);
		if (l_status.good())
			l_new_series->m_series_date = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_Modality, l_tmp_str);
		if (l_status.good())
			l_new_series->m_series_modality = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_SeriesDescription, l_tmp_str);
		if (l_status.good())
			l_new_series->m_series_description = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_BodyPartExamined, l_tmp_str);
		if (l_status.good())
			l_new_series->m_body_part_examined = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_InstitutionName, l_tmp_str);
		if (l_status.good())
			l_new_series->m_institution_name = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_Manufacturer, l_tmp_str);
		if (l_status.good())
			l_new_series->m_manufactureer_name = l_tmp_str.data();

		// 数据格式信息
		l_status = l_dataset->findAndGetOFString(DCM_Rows, l_tmp_str);
		if (l_status.good())
			l_new_series->m_resolution[1] = ofstr_to_uint16(l_tmp_str);
		l_status = l_dataset->findAndGetOFString(DCM_Columns, l_tmp_str);
		if (l_status.good())
			l_new_series->m_resolution[0] = ofstr_to_uint16(l_tmp_str);
		l_status = l_dataset->findAndGetOFString(DCM_BitsAllocated, l_tmp_str);
		if (l_status.good())
			l_new_series->m_bits_allocated = ofstr_to_uint16(l_tmp_str);
		l_status = l_dataset->findAndGetOFString(DCM_HighBit, l_tmp_str);
		if (l_status.good())
			l_new_series->m_high_bit = ofstr_to_uint16(l_tmp_str);
		l_status = l_dataset->findAndGetOFString(DCM_BitsStored, l_tmp_str);
		if (l_status.good())
			l_new_series->m_bits_stored = ofstr_to_uint16(l_tmp_str);

		l_status = l_dataset->findAndGetOFString(DCM_RescaleIntercept, l_tmp_str);
		if (l_status.good())
			l_new_series->m_rescale_intercept = ofstr_to_float(l_tmp_str);
		else
			l_new_series->m_rescale_intercept = 0.0f;
		l_status = l_dataset->findAndGetOFString(DCM_RescaleSlope, l_tmp_str);
		if (l_status.good())
			l_new_series->m_rescale_slope = ofstr_to_float(l_tmp_str);
		else
			l_new_series->m_rescale_slope = 1.0f;

		//最大最小CT值
		l_status = l_dataset->findAndGetOFString(DCM_SmallestImagePixelValue, l_tmp_str);
		if (l_status.good())
			l_new_series->m_smallest_image_pixel_value = ofstr_to_float(l_tmp_str);
		else
			l_new_series->m_smallest_image_pixel_value = -1;
		l_status = l_dataset->findAndGetOFString(DCM_LargestImagePixelValue, l_tmp_str);
		if (l_status.good())
			l_new_series->m_largest_image_pixel_value = ofstr_to_float(l_tmp_str);
		else
			l_new_series->m_largest_image_pixel_value = -1;

		// 窗宽窗位
		l_status = l_dataset->findAndGetOFString(DCM_WindowCenter, l_tmp_str);
		if (l_status.good())
		{
			l_new_series->m_window_center = ofstr_to_float(l_tmp_str);
			l_new_series->m_window_center_good = true;
		}
		else
		{
			l_new_series->m_window_center = 128;
			l_new_series->m_window_center_good = false;
		}
		l_status = l_dataset->findAndGetOFString(DCM_WindowWidth, l_tmp_str);
		if (l_status.good())
		{
			l_new_series->m_window_width = ofstr_to_float(l_tmp_str);
			l_new_series->m_window_width_good = true;
		}
		else
		{
			l_new_series->m_window_width = 256;
			l_new_series->m_window_width_good = false;
		}

		// 额外信息
		l_status = l_dataset->findAndGetOFStringArray(DCM_MediaStorageSOPInstanceUID, l_tmp_str);
		if (l_status.good())
			l_new_series->m_media_storage_SOP_instance_ID = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_ImagePositionPatient, l_tmp_str);
		if (l_status.good())
			l_new_series->m_image_position_patient = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_ImageOrientationPatient, l_tmp_str);
		if (l_status.good())
			l_new_series->m_image_orientation_patient = l_tmp_str.data();
		//added by zym
		l_status = l_dataset->findAndGetOFStringArray(DCM_PatientPosition, l_tmp_str);
		if (l_status.good())
			l_new_series->m_patient_position = l_tmp_str.data();

		l_status = l_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, l_tmp_str);
		if (l_status.good())
			l_new_series->m_series_instance_ID = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_MediaStorageSOPClassUID, l_tmp_str);
		if (l_status.good())
			l_new_series->m_media_storage_SOP_class_UID = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_AcquisitionTime, l_tmp_str);
		if (l_status.good())
			l_new_series->m_acquisition_time = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_SamplesPerPixel, l_tmp_str);
		if (l_status.good())
			l_new_series->m_samples_per_pixel = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_PhotometricInterpretation, l_tmp_str);
		if (l_status.good())
			l_new_series->m_photo_metric_interpretation = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_StudyInstanceUID, l_tmp_str);
		if (l_status.good())
			l_new_series->m_study_instance_UID = l_tmp_str.data();
		else
		{
			l_new_series->m_study_instance_UID = "bad";
			return false;
		}
		l_status = l_dataset->findAndGetOFStringArray(DCM_ImageType, l_tmp_str);
		if (l_status.good())
			l_new_series->m_image_type = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_PixelRepresentation, l_tmp_str);
		if (l_status.good())
			l_new_series->m_pixel_representation = l_tmp_str.data();
		if (l_new_series->m_pixel_representation == "0")
			l_new_series->m_pixel_representation = "0.1";
		if (abs(l_new_series->m_rescale_intercept - (int)l_new_series->m_rescale_intercept) != 0
			|| abs(l_new_series->m_rescale_slope - (int)l_new_series->m_rescale_slope) != 0)
		{
			l_new_series->m_data_type = GL_FLOAT;
		}
		else if (l_new_series->m_bits_allocated == 8)
			l_new_series->m_data_type = GL_UNSIGNED_BYTE;
		else if (l_new_series->m_bits_allocated == 16)
			l_new_series->m_data_type = GL_SHORT;
		l_status = l_dataset->findAndGetOFStringArray(DCM_PixelSpacing, l_tmp_str, true);
		if (l_status.good())
		{
			Float32 *l_spacing = ofstr_to_float_array(l_tmp_str, 2);
			l_new_series->m_pixel_spacing[0] = l_spacing[0];
			l_new_series->m_pixel_spacing[1] = l_spacing[1];
			delete[] l_spacing;
		}
		else
		{
			l_status = l_dataset->findAndGetOFStringArray(DCM_ImagerPixelSpacing, l_tmp_str, true);
			if (l_status.good())
			{
				Float32 *l_spacing = ofstr_to_float_array(l_tmp_str, 2);
				l_new_series->m_pixel_spacing[0] = l_spacing[0];
				l_new_series->m_pixel_spacing[1] = l_spacing[1];
				delete[] l_spacing;
			}
			else
			{
				l_new_series->m_pixel_spacing[0] = 0.3;
				l_new_series->m_pixel_spacing[1] = 0.3;
			}
		}

		l_status = l_dataset->findAndGetOFString(DCM_SliceThickness, l_tmp_str);
		if (l_status.good())
		{
			l_new_series->m_series_slice_thickness_good = true;
			l_new_series->m_series_slice_thickness = ofstr_to_float(l_tmp_str);
		}

		l_status = l_dataset->findAndGetOFString(DCM_KVP, l_tmp_str);
		if (l_status.good())
			l_new_series->m_KVP = ofstr_to_float(l_tmp_str);

		l_status = l_dataset->findAndGetOFString(DCM_XRayTubeCurrent, l_tmp_str);
		if (l_status.good())
			l_new_series->m_KVI = ofstr_to_float(l_tmp_str);
		else
			l_new_series->m_KVI = 0;

        l_status = l_dataset->findAndGetOFStringArray(DCM_RETIRED_ModifiedImageDate, l_tmp_str);
		//l_status = l_dataset->findAndGetOFStringArray(DCM_ACR_NEMA_ModifiedImageDate, l_tmp_str);
		if (l_status.good())
			l_new_series->m_image_data = l_tmp_str.data();
        l_status = l_dataset->findAndGetOFStringArray(DCM_RETIRED_ModifiedImageTime, l_tmp_str);
		//l_status = l_dataset->findAndGetOFStringArray(DCM_ACR_NEMA_ModifiedImageTime, l_tmp_str);
		if (l_status.good())
			l_new_series->m_image_time = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_InstitutionAddress, l_tmp_str);
		if (l_status.good())
			l_new_series->m_institution_address = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_StationName, l_tmp_str);
		if (l_status.good())
			l_new_series->m_station_name = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_InstitutionalDepartmentName, l_tmp_str);
		if (l_status.good())
			l_new_series->m_institutional_department_name = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_ManufacturerModelName, l_tmp_str);
		if (l_status.good())
			l_new_series->m_manufactureer_model_name = l_tmp_str.data();

		l_status = l_dataset->findAndGetOFStringArray(DCM_ReferencedImageSequence, l_tmp_str);
		if (l_status.good())
			l_new_series->m_referenced_image_sequence = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_ContrastBolusAgent, l_tmp_str);
		if (l_status.good())
			l_new_series->m_contrast_bolus_agent = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_ScanOptions, l_tmp_str);
		if (l_status.good())
			l_new_series->m_scan_options = l_tmp_str.data();

		l_status = l_dataset->findAndGetOFStringArray(DCM_RepetitionTime, l_tmp_str);
		if (l_status.good())
			l_new_series->m_repetition_time = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_EchoTime, l_tmp_str);
		if (l_status.good())
			l_new_series->m_echo_time = l_tmp_str.data();

		l_status = l_dataset->findAndGetOFStringArray(DCM_SpacingBetweenSlices, l_tmp_str);
		if (l_status.good())
			l_new_series->m_spacing_between_slices = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_DataCollectionDiameter, l_tmp_str);
		if (l_status.good())
			l_new_series->m_data_collection_diameter = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_SoftwareVersions, l_tmp_str);
		if (l_status.good())
			l_new_series->m_software_versions = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_ProtocolName, l_tmp_str);
		if (l_status.good())
			l_new_series->m_protocol_name = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_ReconstructionDiameter, l_tmp_str);
		if (l_status.good())
			l_new_series->m_reconstruction_diameter = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_GantryDetectorTilt, l_tmp_str);
		if (l_status.good())
			l_new_series->m_gantry_detector_tilt = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_TableHeight, l_tmp_str);
		if (l_status.good())
			l_new_series->m_table_height = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_RotationDirection, l_tmp_str);
		if (l_status.good())
			l_new_series->m_rotation_direction = l_tmp_str.data();

		l_status = l_dataset->findAndGetOFStringArray(DCM_ExposureTime, l_tmp_str);
		if (l_status.good())
			l_new_series->m_exposure_time = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_XRayTubeCurrent, l_tmp_str);
		if (l_status.good())
			l_new_series->m_XRay_tube_current = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_PlanarConfiguration, l_tmp_str);
		if (l_status.good())
			l_new_series->m_planar_configuration = l_tmp_str.data();
		//l_status = l_dataset->findAndGetOFString(DCM_NumberOfFrames, l_tmp_str);
		//if (l_status.good())
		//	{l_new_series->m_number_Of_frames = l_tmp_str.data();
		//std::cout<<l_new_series->m_number_Of_frames <<std::endl;}
		l_status = l_dataset->findAndGetOFStringArray(DCM_OverlayRows, l_tmp_str);
		if (l_status.good())
			l_new_series->m_overlay_row = l_tmp_str.data();
		else
			l_new_series->m_overlay_row = "notoverlay";
		l_status = l_dataset->findAndGetOFStringArray(DCM_OverlayColumns, l_tmp_str);
		if (l_status.good())
			l_new_series->m_overlay_column = l_tmp_str.data();
		else
			l_new_series->m_overlay_column = "notoverlay";
		l_status = l_dataset->findAndGetOFStringArray(DCM_OverlayBitsAllocated, l_tmp_str);
		if (l_status.good())
			l_new_series->m_overlay_bit_allocated = l_tmp_str.data();
		else
			l_new_series->m_overlay_bit_allocated = "notoverlay";
		l_status = l_dataset->findAndGetOFStringArray(DCM_OverlayOrigin, l_tmp_str);
		if (l_status.good())
		{
			l_new_series->m_overlay_origin = l_tmp_str.data();
			std::vector<std::string> v = split(l_new_series->m_overlay_origin, "\\");
			l_new_series->m_overlay_row_ori = atoi(v.at(0).c_str());
			l_new_series->m_overlay_column_ori = atoi(v.at(1).c_str());
		}
		else
			l_new_series->m_overlay_origin = "notoverlay";

		l_status = l_dataset->findAndGetOFStringArray(DCM_Exposure, l_tmp_str);
		if (l_status.good())
			l_new_series->m_exposure = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_ConvolutionKernel, l_tmp_str);
		if (l_status.good())
			l_new_series->m_convolution_kernel = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFStringArray(DCM_AccessionNumber, l_tmp_str);
		if (l_status.good())
			l_new_series->m_accession_number = l_tmp_str.data();

		l_series_idx = l_data_mgr->append_series(l_patient_idx, l_study_idx, l_new_series);

		/*emit new_series(l_patient_idx, l_study_idx, l_series_idx);*/

	}

    // 5、确定数据位置	
	//解决有时候部分series分辨率不一致
	int width = 0;
	int height = 0;
	int winCenter = 0;
	int winWidth = 0;
	std::string sample;
	int bitallocate;
	std::string overlay;
	int planarConfiguration = 0;
	l_status = l_dataset->findAndGetOFString(DCM_Rows, l_tmp_str);
	if (l_status.good())
		width = ofstr_to_uint16(l_tmp_str);
	l_status = l_dataset->findAndGetOFString(DCM_Columns, l_tmp_str);
	if (l_status.good())
		height = ofstr_to_uint16(l_tmp_str);
	//l_status = l_dataset->findAndGetOFString(DCM_WindowWidth, l_tmp_str);
	//if (l_status.good())
	//	winWidth = ofstr_to_uint16(l_tmp_str);
	//else
	//	winWidth = 256;
	//l_status = l_dataset->findAndGetOFString(DCM_WindowCenter, l_tmp_str);
	//if (l_status.good())
	//	winCenter = ofstr_to_uint16(l_tmp_str);
	//else
	//	winCenter = 128;
	l_status = l_dataset->findAndGetOFStringArray(DCM_PlanarConfiguration, l_tmp_str);
	if (l_status.good())
		planarConfiguration = atoi(l_tmp_str.data());
	else
		planarConfiguration = 0;
	l_status = l_dataset->findAndGetOFStringArray(DCM_SamplesPerPixel, l_tmp_str);
	if (l_status.good())
		sample = l_tmp_str.data();
	else
		sample = 1;
	l_status = l_dataset->findAndGetOFString(DCM_BitsAllocated, l_tmp_str);
	if (l_status.good())
		bitallocate = ofstr_to_uint16(l_tmp_str);
	l_status = l_dataset->findAndGetOFStringArray(DCM_OverlayRows, l_tmp_str);
	if (l_status.good())
		overlay = l_tmp_str.data();
	else
		overlay = "notoverlay";

	std::string SOP_id;
	l_status = l_dataset->findAndGetOFString(DCM_SOPInstanceUID, l_tmp_str);
	if (l_status.good())
	{
		SOP_id = l_tmp_str.data();
		l_status = l_dataset->findAndGetOFString(DCM_InstanceNumber, l_tmp_str);
		if (l_status.good())
		{
			unsigned int l_instance_num = ofstr_to_uint16(l_tmp_str);
			l_status = l_dataset->findAndGetOFString(DCM_SliceLocation, l_tmp_str);
			if (l_status.good())
			{
				float l_slice_location = ofstr_to_float(l_tmp_str);
				
				l_data_mgr->append_slice(SOP_id, l_patient_idx, l_study_idx, l_series_idx,
					l_instance_num, l_slice_location, width, height, sample, bitallocate, planarConfiguration, overlay, file_path, file_format);
			}
			else
			{
				l_data_mgr->append_slice(SOP_id, l_patient_idx, l_study_idx, l_series_idx,
					l_instance_num, std::numeric_limits<float>::quiet_NaN(), width, height, sample, bitallocate, planarConfiguration, overlay, file_path, file_format);
			}
		}
		else
		{

			l_data_mgr->append_slice(SOP_id, l_patient_idx, l_study_idx, l_series_idx,
				0, std::numeric_limits<float>::quiet_NaN(), width, height, sample, bitallocate, planarConfiguration, overlay, file_path, file_format);

			std::cout << "instance number bad!" << std::endl;
		}
	}
	else
		std::cout << "sop_id bad!" << std::endl;

    return true;
	
}

// protected


// private
