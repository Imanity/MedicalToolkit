// local"
#include "DicomStudyData.h"
/*#include "DicomPatientPlateImage.h"*/
// meta
#include "DicomPatientData.h"

// public
DicomPatientData::DicomPatientData() :
	/*m_study_lock(QReadWriteLock::Recursive),*/
	m_patient_plate_image(nullptr)/*,
	m_patient_plate_lock(QReadWriteLock::Recursive)*/
{

}

void DicomPatientData::clear_data()
{
	/*m_study_lock.lockForWrite();*/
	int l_study_num = m_studies.size();
	for (int i = 0; i < l_study_num; ++i)
	{
		DicomStudyData *l_cur_study_data = m_studies[i];
		l_cur_study_data->clear_data();
		delete l_cur_study_data;
	}
	m_studies.clear();
	m_study_ID_to_idx_map.clear();
	/*m_study_lock.unlock();*/

	/*m_patient_plate_lock.lockForWrite();*/
	if (m_patient_plate_image != nullptr)
	{
		delete m_patient_plate_image;
	}
	/*m_patient_plate_lock.unlock();*/
}


bool DicomPatientData::check_study_available(std::string &study_ID, int &idx)
{
	bool l_ret = false;
	/*m_study_lock.lockForRead();*/
	auto l_iter = m_study_ID_to_idx_map.find(study_ID);
	l_ret = l_iter != m_study_ID_to_idx_map.end();
	if (l_ret)
		idx = l_iter->second;
	/*m_study_lock.unlock();*/
	return l_ret;
}

bool DicomPatientData::check_series_available(int study_idx, std::string &series_number, int &idx)
{
	return m_studies[study_idx]->check_series_available(series_number, idx);
}

// protected

// private
