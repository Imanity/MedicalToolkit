// meta
#include "DicomSeriesData.h"
#include <iostream>
using namespace std;

// public
DicomSeriesData::DicomSeriesData() :
	m_series_slice_thickness_good(false),
	m_series_slice_thickness_validated(false),
	/*m_image_file_lock(QReadWriteLock::Recursive),*/
	m_series_plate_image(nullptr),
	/*m_series_plate_lock(QReadWriteLock::Recursive),*/
	m_series_plate_dirty(false)
{

}

DicomSeriesData::~DicomSeriesData()
{
	/*m_series_plate_lock.lockForWrite();*/
	if (m_series_plate_image != nullptr);
		delete m_series_plate_image;
	/*m_series_plate_lock.unlock();*/
}

bool DicomSeriesData::update_slice_thickness()
{
	if (!m_series_slice_thickness_validated)
	{
		/*m_image_file_lock.lockForRead();*/
		for (auto iter = m_instance_number_to_idx_map.begin();
			iter != m_instance_number_to_idx_map.end(); ++iter)
		{
			int l_next_instance_num = iter->first + 1;
			auto l_tmp_iter = m_instance_number_to_idx_map.find(l_next_instance_num);
			if (l_tmp_iter != m_instance_number_to_idx_map.end())
			{
				int l_idx_1 = iter->second;
				int l_idx_2 = l_tmp_iter->second;
				float l_location_1 = m_locations[l_idx_1];
				float l_location_2 = m_locations[l_idx_2];
				m_oriention_thickness = l_location_1 - l_location_2;
				float l_derived_thickness = abs(l_location_2 - l_location_1);

				if (l_derived_thickness == l_derived_thickness &&
					l_derived_thickness > 0.0f)
				{
					m_series_slice_thickness = l_derived_thickness;
					m_series_slice_thickness_good = true;
					m_series_slice_thickness_validated = true;
				}
				break;
			}
		}
		/*m_image_file_lock.unlock();*/
	}
	return m_series_slice_thickness_good;
}
