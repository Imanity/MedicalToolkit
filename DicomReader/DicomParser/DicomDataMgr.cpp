// windows
#include <windows.h>
// dcmtk
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
// local
#include "DicomPatientData.h"
#include "DicomStudyData.h"
#include "DicomSeriesData.h"
/*
#include "DicomPatientPlateImage.h"
#include "DicomSeriesPlateImage.h"*/
// meta
#include "DicomDataMgr.h"
/*#include "../DICOMReader/DcmHandler.h"*/

// public
void DicomDataMgr::clear_data()
{
	/*m_patient_lock.lockForWrite();*/
	int l_patient_num = m_patients.size();
	for (int i = 0; i < l_patient_num; ++i)
	{
		DicomPatientData *l_cur_patient_data = m_patients[i];
		l_cur_patient_data->clear_data();
		delete l_cur_patient_data;
	}
	m_patients.clear();
	m_patient_ID_to_idx_map.clear();
	/*m_patient_lock.unlock();*/
}

bool DicomDataMgr::check_patient_available(std::string &patient_ID, int &idx)
{
	bool l_ret = false;
	/*m_patient_lock.lockForRead();*/
	auto l_iter = m_patient_ID_to_idx_map.find(patient_ID);
	l_ret = l_iter != m_patient_ID_to_idx_map.end();
	if (l_ret)
		idx = l_iter->second;
	/*m_patient_lock.unlock();*/
	return l_ret;
}

bool DicomDataMgr::check_study_available(int patient_idx, std::string &study_ID, int &idx)
{
	return m_patients[patient_idx]->check_study_available(study_ID, idx);
}

bool DicomDataMgr::check_series_available(int patient_idx, int study_idx, std::string &series_number, int &idx)
{
	return m_patients[patient_idx]->check_series_available(study_idx, series_number, idx);
}

int DicomDataMgr::append_patient(DicomPatientData *new_patient)
{
	int l_ret;
	/*m_patient_lock.lockForWrite();*/
	l_ret = m_patients.size();
	m_patient_ID_to_idx_map.insert(std::make_pair(new_patient->m_patient_ID, l_ret));
	m_patients.push_back(new_patient);
	/*m_patient_lock.unlock();*/
	return l_ret;
}

int DicomDataMgr::append_study(int patient_idx, DicomStudyData *new_study)
{
	DicomPatientData *l_target_patient_data = m_patients[patient_idx];

	int l_ret;
	/*l_target_patient_data->m_study_lock.lockForWrite();*/
	l_ret = l_target_patient_data->m_studies.size();
	l_target_patient_data->m_study_ID_to_idx_map.insert(std::make_pair(new_study->m_study_ID, l_ret));
	l_target_patient_data->m_studies.push_back(new_study);
	/*l_target_patient_data->m_study_lock.unlock();*/

	return l_ret;
}

int DicomDataMgr::append_series(int patient_idx, int study_idx, DicomSeriesData *new_series)
{
	DicomPatientData *l_target_patient_data = m_patients[patient_idx];
	DicomStudyData *l_target_study_data = l_target_patient_data->m_studies[study_idx];

	int l_ret;
	/*l_target_study_data->m_series_lock.lockForWrite();*/
	l_ret = l_target_study_data->m_series.size();
	l_target_study_data->m_series_ID_to_idx_map.insert(std::make_pair(new_series->m_series_number, l_ret));
	l_target_study_data->m_series.push_back(new_series);

	/*l_target_study_data->m_study_plate_lock.lockForWrite();*/
	l_target_study_data->m_study_plate_dirty = true;
	/*l_target_study_data->m_study_plate_lock.unlock();*/

	/*l_target_study_data->m_series_lock.unlock();*/

	return l_ret;
}

void DicomDataMgr::append_slice(int patient_idx, int study_idx, int series_idx,
	unsigned int instance_num, float location, std::string &file_path, DcmFileFormat* file_format)
{
	DicomPatientData *l_target_patient_data = m_patients[patient_idx];
	DicomStudyData *l_target_study_data = l_target_patient_data->m_studies[study_idx];
	DicomSeriesData *l_target_series_data = l_target_study_data->m_series[series_idx];

	/*l_target_series_data->m_image_file_lock.lockForWrite();*/
	auto l_iter = l_target_series_data->m_instance_number_to_idx_map.find(instance_num);
	//while (l_iter != l_target_series_data->m_instance_number_to_idx_map.end())
	//{
	//	++instance_num;
	//	l_iter = l_target_series_data->m_instance_number_to_idx_map.find(instance_num);
	//}
	if (l_iter != l_target_series_data->m_instance_number_to_idx_map.end())
	{
		/*l_target_series_data->m_image_file_lock.unlock();*/
		return;
	}
	if (location != location)       //???
		location = instance_num;
	int l_slice_num = l_target_series_data->m_image_files.size();
	l_target_series_data->m_instance_number_to_idx_map.insert(
		std::make_pair(instance_num, l_slice_num));
	l_target_series_data->m_locations.push_back(location);
    l_target_series_data->m_image_files.push_back(file_path);
	l_target_series_data->m_image_file_formats.push_back(file_format);

	/*l_target_series_data->m_series_plate_lock.lockForWrite();*/
	l_target_series_data->m_series_plate_dirty = true;
	/*l_target_series_data->m_series_plate_lock.unlock();*/

	/*l_target_series_data->m_image_file_lock.unlock();*/
}

void DicomDataMgr::append_slice(std::string sop,int patient_idx, int study_idx, int series_idx,
	unsigned int instance_num, float location, std::string &file_path, DcmFileFormat* file_format)
{
	DicomPatientData *l_target_patient_data = m_patients[patient_idx];
	DicomStudyData *l_target_study_data = l_target_patient_data->m_studies[study_idx];
	DicomSeriesData *l_target_series_data = l_target_study_data->m_series[series_idx];

	/*l_target_series_data->m_image_file_lock.lockForWrite();*/
	auto l_iter = l_target_series_data->m_sop_to_instance_number_map.find(sop);
	if (l_iter != l_target_series_data->m_sop_to_instance_number_map.end())
	{
		/*l_target_series_data->m_image_file_lock.unlock();*/
		return;
	}
	if (location != location)       //???
		location = instance_num;
	int l_slice_num = l_target_series_data->m_image_files.size();
	int out = 1;
	while (out){	
		auto l_iter_id = l_target_series_data->m_instance_number_to_idx_map.find(instance_num);
		if (l_iter_id != l_target_series_data->m_instance_number_to_idx_map.end())
		{
			++instance_num;
		}
		else
		{
			out = 0;
		}
	}

	l_target_series_data->m_instance_number_to_idx_map.insert(
		std::make_pair(instance_num, l_slice_num));
	l_target_series_data->m_sop_to_instance_number_map.insert(
		std::make_pair(sop, instance_num));
	l_target_series_data->m_locations.push_back(location);
	l_target_series_data->m_image_files.push_back(file_path);
	l_target_series_data->m_image_file_formats.push_back(file_format);

	/*l_target_series_data->m_series_plate_lock.lockForWrite();*/
	l_target_series_data->m_series_plate_dirty = true;
	/*l_target_series_data->m_series_plate_lock.unlock();*/

	/*l_target_series_data->m_image_file_lock.unlock();*/
}

void DicomDataMgr::append_slice(std::string sop, int patient_idx, int study_idx, int series_idx,
	unsigned int instance_num, float location, int width, int height, std::string sample, int bitallocate, int planarConfiguration,
	std::string overlay, std::string &file_path, DcmFileFormat* file_format){
	DicomPatientData *l_target_patient_data = m_patients[patient_idx];
	DicomStudyData *l_target_study_data = l_target_patient_data->m_studies[study_idx];
	DicomSeriesData *l_target_series_data = l_target_study_data->m_series[series_idx];
	/*l_target_series_data->m_image_file_lock.lockForWrite();*/
	auto l_iter = l_target_series_data->m_sop_to_instance_number_map.find(sop);
	if (l_iter != l_target_series_data->m_sop_to_instance_number_map.end())
	{
		/*l_target_series_data->m_image_file_lock.unlock();*/
		return;
	}
	if (location != location)       //???
		location = instance_num;
	int l_slice_num = l_target_series_data->m_image_files.size();
	int out = 1;
	while (out){
		auto l_iter_id = l_target_series_data->m_instance_number_to_idx_map.find(instance_num);
		if (l_iter_id != l_target_series_data->m_instance_number_to_idx_map.end())
		{
			++instance_num;
		}
		else
		{
			out = 0;
		}
	}

	if (sop == "1.2.840.113619.2.55.3.1689032244.711.1508311576.316.1" ||
		sop == "1.2.840.113619.2.55.3.1689032244.711.1508311576.318.2")
	{
		if (instance_num == 1)
			instance_num = 2;
		else
			instance_num = 1;
	}

	l_target_series_data->m_instance_number_to_idx_map.insert(
		std::make_pair(instance_num, l_slice_num));
	l_target_series_data->m_idx_to_instance_number_map.insert(
		std::make_pair(l_slice_num,instance_num));
	l_target_series_data->m_sop_to_instance_number_map.insert(
		std::make_pair(sop, instance_num));
	l_target_series_data->m_instance_number_to_width_map.insert(
		std::make_pair(instance_num, width));
	l_target_series_data->m_instance_number_to_height_map.insert(
		std::make_pair(instance_num , height));
	//l_target_series_data->m_instance_number_to_winwidth_map.insert(
		//std::make_pair(instance_num, winWidth));
	//l_target_series_data->m_instance_number_to_wincenter_map.insert(
		//std::make_pair(instance_num, winCenter));
	l_target_series_data->m_instance_number_to_planarConfiguration_map.insert(
		std::make_pair(instance_num, planarConfiguration));
	l_target_series_data->m_instance_number_to_sample_map.insert(
		std::make_pair(instance_num , sample));
	l_target_series_data->m_instance_number_to_bit_allocate_map.insert(
		std::make_pair(instance_num , bitallocate));
	l_target_series_data->m_instance_number_to_overlay_map.insert(
		std::make_pair(instance_num, overlay));

	l_target_series_data->m_locations.push_back(location);
	l_target_series_data->m_image_files.push_back(file_path); 
	//l_target_series_data->m_image_byte_arrays.push_back(data_array);
	l_target_series_data->m_image_file_formats.push_back(file_format);

	/*l_target_series_data->m_series_plate_lock.lockForWrite();*/
	l_target_series_data->m_series_plate_dirty = true;
	/*l_target_series_data->m_series_plate_lock.unlock();*/

	/*l_target_series_data->m_image_file_lock.unlock();*/
}

/*
void DicomDataMgr::add_reference(int key)
{
	while (true) // 等待数据读取线程穿件对应key的数据缓存
	{
		/ *m_dicom_data_container_lock.lockForRead();* /
		auto l_iter = m_dicom_data_reference_map.find(key);
		if (l_iter != m_dicom_data_reference_map.end())
		{
			auto l_lock_iter = m_dicom_data_reference_lock_map.find(key);
			l_lock_iter->second->lockForWrite();
			++l_iter->second;
			std::cout << "Key: " << key << " Num: " << l_iter->second << std::endl;
			l_lock_iter->second->unlock();
			m_dicom_data_container_lock.unlock();
			break;
		}
		else
		{
			m_dicom_data_container_lock.unlock();
			Sleep(100);
		}
	}
}

void DicomDataMgr::remove_reference(int key)
{
	m_dicom_data_container_lock.lockForWrite();
	auto l_iter = m_dicom_data_reference_map.find(key);
	if (l_iter != m_dicom_data_reference_map.end())
	{
		auto l_reference_lock_iter = m_dicom_data_reference_lock_map.find(key);
		QReadWriteLock *l_reference_lock = l_reference_lock_iter->second;
		l_reference_lock->lockForWrite();
		--(l_iter->second);
		std::cout << "Key: " << key << " Num: " << l_iter->second << std::endl;
		if (l_iter->second <= 0)
		{
			delete m_dicom_data_map.find(key)->second;
			m_dicom_data_map.erase(key);
			delete m_dicom_data_ready_flag_map.find(key)->second;
			m_dicom_data_ready_flag_map.erase(key);
			delete m_dicom_data_lock_map.find(key)->second;
			m_dicom_data_lock_map.erase(key);
			m_dicom_data_reference_map.erase(l_iter);
			m_dicom_data_reference_lock_map.erase(l_reference_lock_iter);

			l_reference_lock->unlock();
			delete l_reference_lock;
		}
		else
		{
			l_reference_lock->unlock();
		}
	}
	m_dicom_data_container_lock.unlock();
}*/

void DicomDataMgr::get_patient_study_series_data(int key, DicomPatientData *&patient_data, DicomStudyData *&study_data, DicomSeriesData *&series_data)
{
	int l_patient_key = key >> 24;
	int l_study_key = (key & 0xFF0000) >> 16;
	int l_series_key = key & 0xFFFF;

	/*m_patient_lock.lockForRead();*/
	patient_data = m_patients[l_patient_key];
	/*m_patient_lock.unlock();*/

	/*patient_data->m_study_lock.lockForRead();*/
	study_data = patient_data->m_studies[l_study_key];
	/*patient_data->m_study_lock.unlock();*/

	/*study_data->m_series_lock.lockForRead();*/
	series_data = study_data->m_series[l_series_key];
	/*study_data->m_series_lock.unlock();*/
}

void DicomDataMgr::get_dicom_data_and_lock(int key, std::vector<short> *&dicom_data, std::vector<bool> *&dicom_data_ready_flags/*, QReadWriteLock *&dicom_data_lock*/)
{
	/*m_dicom_data_container_lock.lockForRead();*/
	auto l_dicom_data_iter = m_dicom_data_map.find(key);
	if (l_dicom_data_iter != m_dicom_data_map.end())
	{
		dicom_data = l_dicom_data_iter->second;
		dicom_data_ready_flags = m_dicom_data_ready_flag_map.find(key)->second;
		/*dicom_data_lock = m_dicom_data_lock_map.find(key)->second;*/
	}
	else
	{
		dicom_data = nullptr;
		dicom_data_ready_flags = nullptr;
		/*dicom_data_lock = nullptr;*/
	}
	/*m_dicom_data_container_lock.unlock();*/
}

unsigned char *DicomDataMgr::get_texture_data2(int key, int slice_idx)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_data_ready_flags;
	/*QReadWriteLock *l_dicom_data_lock;*/
	get_patient_study_series_data(key, l_patient_data, l_study_data, l_series_data);
	get_dicom_data_and_lock(key, l_dicom_data, l_dicom_data_ready_flags/*, l_dicom_data_lock*/);

	int l_slice_pixel_num = l_series_data->m_resolution[0] * l_series_data->m_resolution[1];
	float l_window_width = l_series_data->m_window_width;
	if (l_window_width == 0.0f)
		l_window_width = 1.0f;
	float l_window_center = l_series_data->m_window_center;
	float l_window_bottom = l_window_center - l_window_width * 0.5f;
	float l_window_width_reciprocal = 1.0f / l_window_width;

	unsigned char *l_texture_data = new unsigned char[l_slice_pixel_num * 4];
	/*l_dicom_data_lock->lockForRead();*/
	short *l_dicom_data_ptr = l_dicom_data->data() + slice_idx * l_slice_pixel_num;

	int l_min_value = 32767;
	int l_max_value = -32768;
	for (int i = 0; i < l_slice_pixel_num; ++i)
	{
		int l_value = l_dicom_data_ptr[i];
		if (l_value < l_min_value)
			l_min_value = l_value;
		if (l_value > l_max_value)
			l_max_value = l_value;
	}
	l_window_width_reciprocal = 1.0f / (l_max_value - l_min_value + 1);

	for (int i = 0; i < l_slice_pixel_num; ++i)
	{
		int l_base_idx = i * 4;
		int l_value = l_dicom_data_ptr[i];

		int l_pixel = (l_value - l_min_value) * l_window_width_reciprocal * 255.0f;
		//int l_pixel = (l_value - l_window_bottom) * l_window_width_reciprocal * 255.0f;
		if (l_pixel < 0)
			l_pixel = 0;
		else if (l_pixel > 255)
			l_pixel = 255;

		l_texture_data[l_base_idx] = l_pixel;
		l_texture_data[l_base_idx + 1] = l_pixel;
		l_texture_data[l_base_idx + 2] = l_pixel;
		l_texture_data[l_base_idx + 3] = 255;
	}
	/*l_dicom_data_lock->unlock();*/

	return l_texture_data;
}

unsigned char *DicomDataMgr::get_slice_data(int key, int slice_idx, int type)
{
	switch (type)
	{
	case 0:
		return get_left_viewer_slice_data(key, slice_idx);
	case 1:
		return get_front_viewer_slice_data(key, slice_idx);
	case 2:
		return get_top_viewer_slice_data(key, slice_idx);
	}
	return nullptr;
}

unsigned char *DicomDataMgr::get_slice_data(int key, int slice_idx, int type, float window_center, float window_width)
{
	switch (type)
	{
	case 0:
		return get_left_viewer_slice_data(key, slice_idx, window_center, window_width);
	case 1:
		return get_front_viewer_slice_data(key, slice_idx, window_center, window_width);
	case 2:
		return get_top_viewer_slice_data(key, slice_idx, window_center, window_width);
	}
	return nullptr;
}

unsigned char *DicomDataMgr::get_left_viewer_slice_data(int key, int slice_idx)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_data_ready_flags;
	/*QReadWriteLock *l_dicom_data_lock;*/
	get_patient_study_series_data(key, l_patient_data, l_study_data, l_series_data);
	get_dicom_data_and_lock(key, l_dicom_data, l_dicom_data_ready_flags/*, l_dicom_data_lock*/);

	int l_width = l_series_data->m_resolution[0];
	int l_height = l_series_data->m_resolution[1];
	/*l_dicom_data_lock->lockForRead();*/
	int l_image_num = l_dicom_data_ready_flags->size();
	/*l_dicom_data_lock->unlock();*/
	int l_slice_pixel_num = l_image_num * l_height;

	float l_window_width = l_series_data->m_window_width;
	if (l_window_width == 0.0f)
		l_window_width = 1.0f;
	float l_window_center = l_series_data->m_window_center;
	float l_window_bottom = l_window_center - l_window_width * 0.5f;
	float l_window_width_reciprocal = 1.0f / l_window_width;

	unsigned char *l_texture_data = new unsigned char[l_slice_pixel_num * 4];
	/*l_dicom_data_lock->lockForRead();*/
	int l_data_base_idx = slice_idx;
	int l_texture_base_idx = 0;
	for (int i = 0; i < l_image_num; ++i)
	{
		int l_data_base_idx2 = l_data_base_idx;
		for (int j = 0; j < l_height; ++j)
		{
			int l_value = (*l_dicom_data)[l_data_base_idx2];

			int l_pixel = (l_value - l_window_bottom) * l_window_width_reciprocal * 255.0f;
			if (l_pixel < 0)
				l_pixel = 0;
			else if (l_pixel > 255)
				l_pixel = 255;
			if(l_series_data->m_photo_metric_interpretation.compare("MONOCHROME1") == 0)
					l_pixel = 255- l_pixel;
			l_texture_data[l_texture_base_idx] = l_pixel;
			l_texture_data[l_texture_base_idx + 1] = l_pixel;
			l_texture_data[l_texture_base_idx + 2] = l_pixel;
			l_texture_data[l_texture_base_idx + 3] = 255;

			l_data_base_idx2 += l_width;
			l_texture_base_idx += 4;
		}
		l_data_base_idx2 += l_width * l_height;
	}
	/*l_dicom_data_lock->unlock();*/

	return l_texture_data;
}

unsigned char *DicomDataMgr::get_left_viewer_slice_data(int key, int slice_idx, float window_center, float window_width)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_data_ready_flags;
	/*QReadWriteLock *l_dicom_data_lock;*/
	get_patient_study_series_data(key, l_patient_data, l_study_data, l_series_data);
	get_dicom_data_and_lock(key, l_dicom_data, l_dicom_data_ready_flags/*, l_dicom_data_lock*/);

	int l_width = l_series_data->m_resolution[0];
	int l_height = l_series_data->m_resolution[1];
	/*l_dicom_data_lock->lockForRead();*/
	int l_image_num = l_dicom_data_ready_flags->size();
	/*l_dicom_data_lock->unlock();*/
	int l_slice_pixel_num = l_image_num * l_height;

	float l_window_width = window_width;
	if (l_window_width == 0.0f)
		l_window_width = 1.0f;
	float l_window_center = window_center;
	float l_window_bottom = l_window_center - l_window_width * 0.5f;
	float l_window_width_reciprocal = 1.0f / l_window_width;

	unsigned char *l_texture_data = new unsigned char[l_slice_pixel_num * 4];
	/*l_dicom_data_lock->lockForRead();*/

	int l_data_base_idx = slice_idx;
	int l_texture_base_idx = 0;
	for (int i = 0; i < l_image_num; ++i)
	{
		int l_data_base_idx2 = l_data_base_idx;
		for (int j = 0; j < l_height; ++j)
		{
			int l_value = (*l_dicom_data)[l_data_base_idx2];

			int l_pixel = (l_value - l_window_bottom) * l_window_width_reciprocal * 255.0f;
			if (l_pixel < 0)
				l_pixel = 0;
			else if (l_pixel > 255)
				l_pixel = 255;
			if(l_series_data->m_photo_metric_interpretation.compare("MONOCHROME1") == 0)
					l_pixel = 255- l_pixel;
			l_texture_data[l_texture_base_idx] = l_pixel;
			l_texture_data[l_texture_base_idx + 1] = l_pixel;
			l_texture_data[l_texture_base_idx + 2] = l_pixel;
			l_texture_data[l_texture_base_idx + 3] = 255;

			l_data_base_idx2 += l_width;
			l_texture_base_idx += 4;
		}
		l_data_base_idx += l_width * l_height;
	}

	/*l_dicom_data_lock->unlock();*/

	return l_texture_data;
}

unsigned char *DicomDataMgr::get_front_viewer_slice_data(int key, int slice_idx)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_data_ready_flags;
	/*QReadWriteLock *l_dicom_data_lock;*/
	get_patient_study_series_data(key, l_patient_data, l_study_data, l_series_data);
	get_dicom_data_and_lock(key, l_dicom_data, l_dicom_data_ready_flags/*, l_dicom_data_lock*/);

	int l_width = l_series_data->m_resolution[0];
	int l_height = l_series_data->m_resolution[1];
	/*l_dicom_data_lock->lockForRead();*/
	int l_image_num = l_dicom_data_ready_flags->size();
	/*l_dicom_data_lock->unlock();*/
	int l_slice_pixel_num = l_image_num * l_width;

	float l_window_width = l_series_data->m_window_width;
	if (l_window_width == 0.0f)
		l_window_width = 1.0f;
	float l_window_center = l_series_data->m_window_center;
	float l_window_bottom = l_window_center - l_window_width * 0.5f;
	float l_window_width_reciprocal = 1.0f / l_window_width;

	unsigned char *l_texture_data = new unsigned char[l_slice_pixel_num * 4];
	/*l_dicom_data_lock->lockForRead();*/

	int l_data_base_idx = slice_idx * l_width;
	int l_texture_base_idx = 0;
	for (int i = 0; i < l_image_num; ++i)
	{
		int l_data_base_idx2 = l_data_base_idx ;
		for (int j = 0; j < l_width; ++j)
		{
			int l_value = (*l_dicom_data)[l_data_base_idx2];

			int l_pixel = (l_value - l_window_bottom) * l_window_width_reciprocal * 255.0f;
			if (l_pixel < 0)
				l_pixel = 0;
			else if (l_pixel > 255)
				l_pixel = 255;
			if(l_series_data->m_photo_metric_interpretation.compare("MONOCHROME1") == 0)
					l_pixel = 255- l_pixel;
			l_texture_data[l_texture_base_idx] = l_pixel;
			l_texture_data[l_texture_base_idx + 1] = l_pixel;
			l_texture_data[l_texture_base_idx + 2] = l_pixel;
			l_texture_data[l_texture_base_idx + 3] = 255;

			++l_data_base_idx2;
			l_texture_base_idx += 4;
		}

		l_data_base_idx += l_width * l_height;
	}		
	
	/*l_dicom_data_lock->unlock();*/

	return l_texture_data;
}

unsigned char *DicomDataMgr::get_front_viewer_slice_data(int key, int slice_idx, float window_center, float window_width)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_data_ready_flags;
	/*QReadWriteLock *l_dicom_data_lock;*/
	get_patient_study_series_data(key, l_patient_data, l_study_data, l_series_data);
	get_dicom_data_and_lock(key, l_dicom_data, l_dicom_data_ready_flags/*, l_dicom_data_lock*/);

	int l_width = l_series_data->m_resolution[0];
	int l_height = l_series_data->m_resolution[1];
	/*l_dicom_data_lock->lockForRead();*/
	int l_image_num = l_dicom_data_ready_flags->size();
	/*l_dicom_data_lock->unlock();*/
	int l_slice_pixel_num = l_image_num * l_width;

	float l_window_width = window_width;
	if (l_window_width == 0.0f)
		l_window_width = 1.0f;
	float l_window_center = window_center;
	float l_window_bottom = l_window_center - l_window_width * 0.5f;
	float l_window_width_reciprocal = 1.0f / l_window_width;

	unsigned char *l_texture_data = new unsigned char[l_slice_pixel_num * 4];
	/*l_dicom_data_lock->lockForRead();*/


	int l_data_base_idx = slice_idx * l_width;
	int l_texture_base_idx = 0;
	for (int i = 0; i < l_image_num; ++i)
	{
		int l_data_base_idx2 = l_data_base_idx ;
		for (int j = 0; j < l_width; ++j)
		{
			int l_value = (*l_dicom_data)[l_data_base_idx2];

			int l_pixel = (l_value - l_window_bottom) * l_window_width_reciprocal * 255.0f;
			if (l_pixel < 0)
				l_pixel = 0;
			else if (l_pixel > 255)
				l_pixel = 255;
			if(l_series_data->m_photo_metric_interpretation.compare("MONOCHROME1") == 0)
					l_pixel = 255- l_pixel;
			l_texture_data[l_texture_base_idx] = l_pixel;
			l_texture_data[l_texture_base_idx + 1] = l_pixel;
			l_texture_data[l_texture_base_idx + 2] = l_pixel;
			l_texture_data[l_texture_base_idx + 3] = 255;

			++l_data_base_idx2;
			l_texture_base_idx += 4;
		}

		l_data_base_idx += l_width * l_height;
	}		
	

	/*l_dicom_data_lock->unlock();*/

	return l_texture_data;
}

unsigned char *DicomDataMgr::get_top_viewer_slice_data(int key, int slice_idx)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_data_ready_flags;
	/*QReadWriteLock *l_dicom_data_lock;*/
	get_patient_study_series_data(key, l_patient_data, l_study_data, l_series_data);
	get_dicom_data_and_lock(key, l_dicom_data, l_dicom_data_ready_flags/*, l_dicom_data_lock*/);

	//modified by zym
	int l_width = l_series_data->m_resolution[0];
	int l_height = l_series_data->m_resolution[1];
	/*l_dicom_data_lock->lockForRead();*/
	int l_image_num = l_dicom_data_ready_flags->size();
	/*l_dicom_data_lock->unlock();*/
	int l_slice_pixel_num = l_width * l_height;

	float l_window_width = l_series_data->m_window_width;
	if (l_window_width == 0.0f)
		l_window_width = 1.0f;
	float l_window_center = l_series_data->m_window_center;
	float l_window_bottom = l_window_center - l_window_width * 0.5f;
	float l_window_width_reciprocal = 1.0f / l_window_width;

	unsigned char *l_texture_data = new unsigned char[l_slice_pixel_num * 4];
	/*l_dicom_data_lock->lockForRead();*/

	int l_data_base_idx = slice_idx * l_height * l_width;
	int l_texture_base_idx = 0;
	for (int i = 0; i < l_height; ++i)
	{
		int l_data_base_idx2 = l_data_base_idx;
		for (int j = 0; j < l_width; ++j)
		{
			int l_value = (*l_dicom_data)[l_data_base_idx2];

			int l_pixel = (l_value - l_window_bottom) * l_window_width_reciprocal * 255.0f;
			if (l_pixel < 0)
				l_pixel = 0;
			else if (l_pixel > 255)
				l_pixel = 255;
			if(l_series_data->m_photo_metric_interpretation.compare("MONOCHROME1") == 0)
					l_pixel = 255- l_pixel;

			l_texture_data[l_texture_base_idx] = l_pixel;
			l_texture_data[l_texture_base_idx + 1] = l_pixel;
			l_texture_data[l_texture_base_idx + 2] = l_pixel;
			l_texture_data[l_texture_base_idx + 3] = 255;

			++l_data_base_idx2;
			l_texture_base_idx += 4;
		}
		l_data_base_idx += l_width;
	}
	/*l_dicom_data_lock->unlock();*/

	return l_texture_data;
}

unsigned char *DicomDataMgr::get_top_viewer_slice_data(int key, int slice_idx, float window_center, float window_width)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_data_ready_flags;
	/*QReadWriteLock *l_dicom_data_lock;*/
	get_patient_study_series_data(key, l_patient_data, l_study_data, l_series_data);
	get_dicom_data_and_lock(key, l_dicom_data, l_dicom_data_ready_flags/*, l_dicom_data_lock*/);
	int l_width = l_series_data->m_resolution[0];
	int l_height = l_series_data->m_resolution[1];
	std::string sample = "1";		
	auto iter3 = l_series_data->m_instance_number_to_sample_map.find(slice_idx + 1);
		if (iter3 != l_series_data->m_instance_number_to_sample_map.end())
			sample = iter3->second;
		else
			sample = l_series_data->m_samples_per_pixel;
	if (l_series_data->m_series_modality != "DX")
	{
		auto iter1 = l_series_data->m_instance_number_to_width_map.find(slice_idx + 1);
		if (iter1 != l_series_data->m_instance_number_to_width_map.end())
			l_height = iter1->second;
		auto iter = l_series_data->m_instance_number_to_height_map.find(slice_idx + 1);
		if (iter != l_series_data->m_instance_number_to_height_map.end())
			l_width = iter->second;
	}
	else
	{
		int instance_num;
		auto it = l_series_data->m_idx_to_instance_number_map.find(slice_idx);
		if (it != l_series_data->m_idx_to_instance_number_map.end())
		{
			instance_num = it->second;
			auto iter4 = l_series_data->m_instance_number_to_width_map.find(instance_num);
			if (iter4 != l_series_data->m_instance_number_to_width_map.end())
				l_height = iter4->second;
			auto ite = l_series_data->m_instance_number_to_height_map.find(instance_num);
			if (ite != l_series_data->m_instance_number_to_height_map.end())
				l_width = ite->second;
		}
		else
		{
			l_width = l_series_data->m_resolution[0];
			l_height = l_series_data->m_resolution[1];
		}
	}
	
	/*l_dicom_data_lock->lockForRead();*/
	int l_image_num = l_dicom_data_ready_flags->size();
	/*l_dicom_data_lock->unlock();*/
	int l_slice_pixel_num = l_width * l_height;

	float l_window_width = window_width;
	if (l_window_width == 0.0f)
		l_window_width = 1.0f;
	float l_window_center = window_center;
	float l_window_bottom = l_window_center - l_window_width * 0.5f;
	float l_window_width_reciprocal = 1.0f / l_window_width;
	
	int total_pixel_num = 0;
	auto it = l_series_data->m_idx_to_data_pos_map.find(slice_idx);
	if (it != l_series_data->m_idx_to_data_pos_map.end())
		total_pixel_num = it->second;
	else
	{
		total_pixel_num = slice_idx * l_slice_pixel_num;
	}

	unsigned char *l_texture_data = new unsigned char[l_slice_pixel_num * 4];
	/*l_dicom_data_lock->lockForRead();*/
	//short *l_dicom_data_ptr = l_dicom_data->data() + slice_idx * l_slice_pixel_num;
	//short *l_dicom_data_ptr = l_dicom_data->data() + total_pixel_num;

	if(l_series_data->m_series_modality.compare("DR")!=0)
	{
		int l_data_base_idx = total_pixel_num;
		int l_texture_base_idx = 0;			
		for (int i = 0; i < l_height; ++i)
		{
			int l_data_base_idx2 = l_data_base_idx;

			for (int j = 0; j < l_width; ++j)
			{
				if (l_data_base_idx2 > l_dicom_data->size())
				{
					std::cout  << "size error" << std::endl;
					return l_texture_data;
				}
				int l_value = (*l_dicom_data)[l_data_base_idx2];

				int l_pixel = (l_value - l_window_bottom) * l_window_width_reciprocal * 255.0f;
				if (l_pixel < 0)
					l_pixel = 0;
				else if (l_pixel > 255)
					l_pixel = 255;
				if(l_series_data->m_photo_metric_interpretation.compare("MONOCHROME1") == 0)
					l_pixel = 255- l_pixel;
				//if(l_series_data->m_samples_per_pixel == "1")
				if (sample == "1")
				{
					if (l_value != -32767)
					{
						l_texture_data[l_texture_base_idx] = l_pixel;
						l_texture_data[l_texture_base_idx + 1] = l_pixel;
						l_texture_data[l_texture_base_idx + 2] = l_pixel;
						l_texture_data[l_texture_base_idx + 3] = 255;
					}
					else
					{
						
						l_texture_data[l_texture_base_idx] = 255;     //黄色
						l_texture_data[l_texture_base_idx + 1] = 255;
						l_texture_data[l_texture_base_idx + 2] = 0;
						l_texture_data[l_texture_base_idx + 3] = 255;
					}
				}
				//else if(l_series_data->m_samples_per_pixel == "3")
				else if (sample == "3")
				{
					l_texture_data[l_texture_base_idx] = l_pixel;
					
					l_data_base_idx2++;
					l_value = (*l_dicom_data)[l_data_base_idx2];
					l_pixel = (l_value - l_window_bottom) * l_window_width_reciprocal * 255.0f;
					if (l_pixel < 0)
						l_pixel = 0;
					else if (l_pixel > 255)
						l_pixel = 255;
					if(l_series_data->m_photo_metric_interpretation.compare("MONOCHROME1") == 0)
						l_pixel = 255- l_pixel;
					l_texture_data[l_texture_base_idx + 1] = l_pixel;
					l_data_base_idx2++;
					
					l_value = (*l_dicom_data)[l_data_base_idx2];
					l_pixel = (l_value - l_window_bottom) * l_window_width_reciprocal * 255.0f;
					if (l_pixel < 0)
						l_pixel = 0;
					else if (l_pixel > 255)
						l_pixel = 255;
					if(l_series_data->m_photo_metric_interpretation.compare("MONOCHROME1") == 0)
						l_pixel = 255- l_pixel;
					l_texture_data[l_texture_base_idx + 2] = l_pixel;
					l_texture_data[l_texture_base_idx + 3] = 255;
				}

				++l_data_base_idx2;
				l_texture_base_idx += 4;
			}
			if (sample == "1")
				l_data_base_idx += l_width;
			else
				l_data_base_idx += l_width * 3;
		}
	}
	else
	{
		int l_data_base_idx = total_pixel_num;//slice_idx * l_height * l_width;
		int l_texture_base_idx = 0;
		for (int i = 0; i < l_height; ++i)
		{
			int l_data_base_idx2 = l_data_base_idx;
			for (int j = 0; j < l_width; ++j)
			{
				unsigned short l_value = (unsigned short)(*l_dicom_data)[l_data_base_idx2];
				int l_pixel = (l_value - l_window_bottom) * l_window_width_reciprocal * 255.0f;
				if (l_pixel < 0)
					l_pixel = 0;
				else if (l_pixel > 255)
					l_pixel = 255;

				l_texture_data[l_texture_base_idx] = l_pixel;
				l_texture_data[l_texture_base_idx + 1] = l_pixel;
				l_texture_data[l_texture_base_idx + 2] = l_pixel;
				l_texture_data[l_texture_base_idx + 3] = 255;

				++l_data_base_idx2;
				l_texture_base_idx += 4;
			}
			if (sample == "1")
				l_data_base_idx += l_width;
			else
				l_data_base_idx += l_width * 3;
		}
	}

	/*l_dicom_data_lock->unlock();*/

	return l_texture_data;
}

short *DicomDataMgr::get_raw_data(int key, int slice_idx, int type)
{
	switch (type)
	{
	case 0:
		return get_left_viewer_raw_data(key, slice_idx);
	case 1:
		return get_front_viewer_raw_data(key, slice_idx);
	case 2:
		return get_top_viewer_raw_data(key, slice_idx);
	}
	return nullptr;
}

short *DicomDataMgr::get_left_viewer_raw_data(int key, int slice_idx)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_data_ready_flags;
	/*QReadWriteLock *l_dicom_data_lock;*/
	get_patient_study_series_data(key, l_patient_data, l_study_data, l_series_data);
	get_dicom_data_and_lock(key, l_dicom_data, l_dicom_data_ready_flags/*, l_dicom_data_lock*/);

	int l_width = l_series_data->m_resolution[0];
	int l_height = l_series_data->m_resolution[1];
	/*l_dicom_data_lock->lockForRead();*/
	int l_image_num = l_dicom_data_ready_flags->size();
	/*l_dicom_data_lock->unlock();*/
	int l_slice_pixel_num = l_image_num * l_height;

	short *l_raw_data = new short[l_slice_pixel_num];
	/*l_dicom_data_lock->lockForRead();*/
	int l_data_base_idx = slice_idx;
	int l_raw_base_idx = 0;
	for (int i = 0; i < l_image_num; ++i)
	{
		int l_data_base_idx2 = l_data_base_idx;
		for (int j = 0; j < l_height; ++j)
		{
			l_raw_data[l_raw_base_idx] = (*l_dicom_data)[l_data_base_idx2] - l_series_data->m_rescale_intercept;

			l_data_base_idx2 += l_width;
			++l_raw_base_idx;
		}
		l_data_base_idx += l_width * l_height;
	}
	/*l_dicom_data_lock->unlock();*/

	return l_raw_data;
}

short *DicomDataMgr::get_front_viewer_raw_data(int key, int slice_idx)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_data_ready_flags;
	/*QReadWriteLock *l_dicom_data_lock;*/
	get_patient_study_series_data(key, l_patient_data, l_study_data, l_series_data);
	get_dicom_data_and_lock(key, l_dicom_data, l_dicom_data_ready_flags/*, l_dicom_data_lock*/);

	int l_width = l_series_data->m_resolution[0];
	int l_height = l_series_data->m_resolution[1];
	/*l_dicom_data_lock->lockForRead();*/
	int l_image_num = l_dicom_data_ready_flags->size();
	/*l_dicom_data_lock->unlock();*/
	int l_slice_pixel_num = l_image_num * l_width;


	short *l_raw_data = new short[l_slice_pixel_num];
	/*l_dicom_data_lock->lockForRead();*/
	int l_data_base_idx = slice_idx * l_width;
	int l_raw_base_idx = 0;
	for (int i = 0; i < l_image_num; ++i)
	{
		int l_data_base_idx2 = l_data_base_idx;
		for (int j = 0; j < l_width; ++j)
		{
			l_raw_data[l_raw_base_idx] = (*l_dicom_data)[l_data_base_idx2] - l_series_data->m_rescale_intercept;

			++l_data_base_idx2;
			++l_raw_base_idx;
		}

		l_data_base_idx += l_width * l_height;
	}
	/*l_dicom_data_lock->unlock();*/

	return l_raw_data;
}

short *DicomDataMgr::get_top_viewer_raw_data(int key, int slice_idx)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_data_ready_flags;
	/*QReadWriteLock *l_dicom_data_lock;*/
	get_patient_study_series_data(key, l_patient_data, l_study_data, l_series_data);
	get_dicom_data_and_lock(key, l_dicom_data, l_dicom_data_ready_flags/*, l_dicom_data_lock*/);

	int l_width = l_series_data->m_resolution[0];
	int l_height = l_series_data->m_resolution[1];
	/*l_dicom_data_lock->lockForRead();*/
	int l_image_num = l_dicom_data_ready_flags->size();
	/*l_dicom_data_lock->unlock();*/
	int l_slice_pixel_num = l_width * l_height;

	short *l_raw_data = new short[l_slice_pixel_num];
	/*l_dicom_data_lock->lockForRead();*/
	int l_data_base_idx = slice_idx * l_height * l_width;
	int l_raw_base_idx = 0;
	for (int i = 0; i < l_height; ++i)
	{
		int l_data_base_idx2 = l_data_base_idx;
		for (int j = 0; j < l_width; ++j)
		{
			l_raw_data[l_raw_base_idx] = (*l_dicom_data)[l_data_base_idx2] - l_series_data->m_rescale_intercept;

			++l_data_base_idx2;
			++l_raw_base_idx;
		}

		l_data_base_idx += l_width;
	}
	/*l_dicom_data_lock->unlock();*/

	return l_raw_data;
}

bool DicomDataMgr::get_locatioin(int key, int idx, float &location)
{
	DicomPatientData *l_patient_data;
	DicomStudyData *l_study_data;
	DicomSeriesData *l_series_data;
	get_patient_study_series_data(key,
		l_patient_data, l_study_data, l_series_data);
	if (l_series_data->update_slice_thickness())
	{
		/*l_series_data->m_image_file_lock.lockForRead();*/
		int l_available_slice_num = l_series_data->m_instance_number_to_idx_map.size();
		if (l_available_slice_num < 2)
		{
			if (idx == 0)
			{
				location = l_series_data->m_locations[0];
				/*l_series_data->m_image_file_lock.unlock();*/
				return true;
			}
			else
			{
				/*l_series_data->m_image_file_lock.unlock();*/
				return false;
			}
		}

		auto l_iter = l_series_data->m_instance_number_to_idx_map.begin();
		int l_base_instance_num = l_iter->first;
		int l_base_idx = l_iter->second;
		float l_base_location = l_series_data->m_locations[l_base_idx];
		++l_iter;
		int l_pivot_idx = l_iter->second;
		float l_pivot_location = l_series_data->m_locations[l_pivot_idx];
		/*l_series_data->m_image_file_lock.unlock();*/

		int l_step = idx - l_base_instance_num + 1;
		if (l_pivot_location < l_base_location)
			l_step = -l_step;
		location = l_base_location + l_step * l_series_data->m_series_slice_thickness;

		return true;
	}
	else
	{
		return false;
	}
}

void DicomDataMgr::get_minmax_value(int key, short &min_value, short &max_value)
{
	std::vector<short> *l_dicom_data;
	std::vector<bool> *l_dicom_ready_flag;
	/*QReadWriteLock *l_dicom_lock;*/
	get_dicom_data_and_lock(key,
		l_dicom_data, l_dicom_ready_flag/*, l_dicom_lock*/);

	/*l_dicom_lock->lockForRead();*/
	auto l_minmax_elem = std::minmax_element(l_dicom_data->begin(), l_dicom_data->end());
	min_value = *l_minmax_elem.first;
	max_value = *l_minmax_elem.second;
	/*l_dicom_lock->unlock();*/
}

unsigned char* DicomDataMgr::shortToChar(short *data,int size,unsigned char* str){

	int j = 0;
	for (int i = 0; i < size; i++) {
		str[j++] = data[i] & 0x00ff;
	//	str[j++] = ushort_arr[i];
	}
	return str;
}

// protected
DicomDataMgr::DicomDataMgr()/* :
	m_patient_lock(QReadWriteLock::Recursive)*/
{
}

DicomDataMgr::~DicomDataMgr()
{

}

// private
DicomDataMgr *DicomDataMgr::ms_instance = nullptr;
