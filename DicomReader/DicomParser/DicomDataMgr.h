#pragma once
// Qt
/*#include <QReadWriteLock>*/
// Cpp
#include <vector>
#include <map>
// local
class DicomPatientData;
class DicomStudyData;
class DicomSeriesData;
class DcmFileFormat;
#include "DicomDataParser.h"
// meta
/*
#include <QObject>
#include "centralmanager_global.h"*/
using namespace std;

class /*CENTRALMANAGER_EXPORT*/ DicomDataMgr/* : public QObject*/
{
	/*Q_OBJECT*/
public:
	std::vector<DicomPatientData *> m_patients;
	std::map<std::string, int> m_patient_ID_to_idx_map;
	/*QReadWriteLock m_patient_lock;*/
	//多线程读取相关
	std::map<int, std::vector<short> *> m_dicom_data_map;
	std::map<int, std::vector<bool> *> m_dicom_data_ready_flag_map;
	/*std::map<int, QReadWriteLock *> m_dicom_data_lock_map;*/
	std::map<int, int> m_dicom_data_reference_map;
	/*std::map<int, QReadWriteLock *> m_dicom_data_reference_lock_map;
	QReadWriteLock m_dicom_data_container_lock;*/

	static inline DicomDataMgr *get_instance()
	{
		if (ms_instance == nullptr)
		{
			ms_instance = new DicomDataMgr();
		}
		return ms_instance;
	}

	void clear_data();  //数据管理清空数据
	bool check_patient_available(std::string &patient_ID, int &idx);
	bool check_study_available(int patient_idx, std::string &study_ID, int &idx);
	bool check_series_available(int patient_idx, int study_idx, std::string &series_number, int &idx);
	int append_patient(DicomPatientData *new_patient);
	int append_study(int patient_idx, DicomStudyData *new_study);
	int append_series(int patient_idx, int study_idx, DicomSeriesData *new_series);
	void append_slice(int patient_idx, int study_idx, int series_idx,
		unsigned int instance_num, float location, std::string &file_path, DcmFileFormat* file_format=NULL);
	void append_slice(std::string sop,int patient_idx, int study_idx, int series_idx,
		unsigned int instance_num, float location, std::string &file_path, DcmFileFormat* file_format = NULL);
	void append_slice(std::string sop, int patient_idx, int study_idx, int series_idx,
		unsigned int instance_num, float location, int width, int height, std::string sample, int bit_allocate,
		int planarConfiguration, std::string overlay, std::string &file_path, DcmFileFormat* file_format = NULL);   //前两个重载弃用
	void get_patient_study_series_data(int key, DicomPatientData *&patient_data, DicomStudyData *&study_data, DicomSeriesData *&series_data);
	void get_dicom_data_and_lock(int key, std::vector<short> *&dicom_data, std::vector<bool> *&dicom_data_ready_flags);
	unsigned char *get_texture_data2(int key, int slice_idx);
	unsigned char *get_slice_data(int key, int slice_idx, int type);
	unsigned char *get_slice_data(int key, int slice_idx, int type, float window_center, float window_width);  //获取显示数据 引入窗位窗宽计算 
	unsigned char *get_left_viewer_slice_data(int key, int slice_idx);
	unsigned char *get_left_viewer_slice_data(int key, int slice_idx, float window_center, float window_width);
	unsigned char *get_front_viewer_slice_data(int key, int slice_idx);
	unsigned char *get_front_viewer_slice_data(int key, int slice_idx, float window_center, float window_width);
	unsigned char *get_top_viewer_slice_data(int key, int slice_idx);
	unsigned char *get_top_viewer_slice_data(int key, int slice_idx, float window_center, float window_width);
	short *get_raw_data(int key, int slice_idx, int type);   //获取原始数据  
	short *get_left_viewer_raw_data(int key, int slice_idx);  //MPR重建相关
	short *get_front_viewer_raw_data(int key, int slice_idx);
	short *get_top_viewer_raw_data(int key, int slice_idx);

	bool get_locatioin(int key, int idx, float &location);
	void get_minmax_value(int key, short &min_value, short &max_value);

	unsigned char* shortToChar( short *data,int size,unsigned char* char_data);
protected:
	DicomDataMgr();
	~DicomDataMgr();

private:
	static DicomDataMgr *ms_instance;
};