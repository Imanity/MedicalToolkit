#pragma once
// Qt
/*#include <QReadWriteLock>*/
// Cpp
#include <string>
#include <vector>
#include <map>
// local
class DicomSeriesData;
class DicomStudyPlateImage;

class DicomStudyData
{
public:
	std::string m_study_ID;
	std::string m_study_date;
	std::string m_study_time;
	std::string m_study_description;

	std::vector<DicomSeriesData *> m_series;
	std::map<std::string, int> m_series_ID_to_idx_map;
	/*QReadWriteLock m_series_lock;*/

	DicomStudyPlateImage *m_study_plate_image;
	/*QReadWriteLock m_study_plate_lock;*/
	bool m_study_plate_dirty;

	DicomStudyData();

	void clear_data();
	bool check_series_available(std::string &series_ID, int &idx);
};