// local
#include "DicomSeriesData.h"
// meta
#include "DicomStudyData.h"

// public
DicomStudyData::DicomStudyData() :
	/*m_series_lock(QReadWriteLock::Recursive),*/
	m_study_plate_image(nullptr),
	/*m_study_plate_lock(QReadWriteLock::Recursive),*/
	m_study_plate_dirty(false)
{

}

void DicomStudyData::clear_data()
{
	/*m_series_lock.lockForWrite();*/
	int l_series_num = m_series.size();
	for (int i = 0; i < l_series_num; ++i)
	{
		delete m_series[i];
	}
	m_series.clear();
	m_series_ID_to_idx_map.clear();
	/*m_series_lock.unlock();*/

	/*m_study_plate_lock.lockForWrite();*/
	if (m_study_plate_image != nullptr)
	{
		delete m_study_plate_image;
	}
	/*m_study_plate_lock.unlock();*/
}


bool DicomStudyData::check_series_available(std::string &series_number, int &idx)
{
	bool l_ret = false;
	/*m_series_lock.lockForRead();*/
	auto l_iter = m_series_ID_to_idx_map.find(series_number);
	l_ret = l_iter != m_series_ID_to_idx_map.end();
	if (l_ret)
		idx = l_iter->second;
	/*m_series_lock.unlock();*/
	return l_ret;
}

// protected

// private