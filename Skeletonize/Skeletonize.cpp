#define DLL_EXPORTS

#include "Skeletonize.h"
#include <fstream>
#include <time.h>

Skeletonize::Skeletonize(short* data, short* result, int* dims) {
	this->dims = dims;

	vn = dims[0] * dims[1] * dims[2];

	object_number = 0;
	for (int i = 0; i < vn; i++) {
		if (data[i] != 0) {
			result[i] = 1;
			object_number++;
		}
		else {
			result[i] = 0;
		}
	}

	this->data = result;

	xs = new short[object_number]();
	ys = new short[object_number]();
	zs = new short[object_number]();

	int count = 0;
	for (int i = 0; i < vn; i++)
	{
		if (result[i] > 0)
		{
			int x, y, z;
			GetPosition(i, x, y, z);
			xs[count] = x;
			ys[count] = y;
			zs[count] = z;
			count++;
		}
	}
}

Skeletonize::~Skeletonize() {
	delete[] xs;
	delete[] ys;
	delete[] zs;
}

inline short  Skeletonize::GetPixel(int src_x, int src_y, int src_z, int* offset) {

	int x = src_x + offset[0];
	int y = src_y + offset[1];
	int z = src_z + offset[2];

	if (x < 0 || x >= dims[0] || y < 0 || y >= dims[1] || z < 0 || z >= dims[2])
		return -1;

	int index = x + y * dims[0] + z * dims[0] * dims[1];
	return data[index];
}

void  Skeletonize::GetPosition(int index, int& x, int& y, int& z) {
	z = index / (dims[0] * dims[1]);
	int left = index % (dims[0] * dims[1]);
	y = left / dims[0];
	x = left % dims[0];
}

int getFrontNumber(short* data, int nv, short resultTagData) {
	int ans = 0;
	for (int i = 0; i < nv; i++)
		if (data[i] == resultTagData)
			ans++;
	return ans;
}

void Skeletonize::thinning() {

	// Define offsets
	int N[] = { 0,-1, 0 };  // north
	int S[] = { 0, 1, 0 };  // south
	int E[] = { 1, 0, 0 };  // east
	int W[] = { -1, 0, 0 };  // west
	int U[] = { 0, 0, 1 };  // up
	int B[] = { 0, 0,-1 };  // bottom

	// prepare Euler LUT [Lee94]
	int eulerLUT[256];
	fillEulerLUT(eulerLUT);

	// Loop through the image several times until there is no change.
	int unchangedBorders = 0;
	int step = 0;
	while (unchangedBorders < 6)  // loop until no change for all the six border types
	{

		clock_t startTime, endTime;
		startTime = clock();
		unchangedBorders = 0;

		for (int currentBorder = 1; currentBorder <= 6; currentBorder++)
		{

			clock_t start, end, start_in, end_in;
			start = clock();

			bool* simpleBorderPoints = new bool[object_number]();
			int border_number = 0;

			start_in = clock();

			// Loop through the image.
			for (int index = 0; index < object_number; index++)
			{
				int x = xs[index];
				int y = ys[index];
				int z = zs[index];

				simpleBorderPoints[index] = false;

				int vi = xs[index] + ys[index] * dims[0] + zs[index] * dims[0] * dims[1];

				// check if point is foreground
				if (!data[vi])
				{
					continue;         // current point is already background 
				}
				// check 6-neighbors if point is a border point of type currentBorder
				int p[3] = { x,y,z };

				bool isBorderPoint = false;
				short tmp = GetPixel(p[0], p[1], p[2], N);
				if (currentBorder == 1 && tmp <= 0)
					isBorderPoint = true;
				if (currentBorder == 2 && GetPixel(p[0], p[1], p[2], S) <= 0)
					isBorderPoint = true;
				if (currentBorder == 3 && GetPixel(p[0], p[1], p[2], E) <= 0)
					isBorderPoint = true;
				if (currentBorder == 4 && GetPixel(p[0], p[1], p[2], W) <= 0)
					isBorderPoint = true;
				if (currentBorder == 5 && GetPixel(p[0], p[1], p[2], U) <= 0)
					isBorderPoint = true;
				if (currentBorder == 6 && GetPixel(p[0], p[1], p[2], B) <= 0)
					isBorderPoint = true;
				if (!isBorderPoint)
				{
					continue;         // current point is not deletable
				}


				//--------------------------------------------------------------------------------------------------------
				// check if point is the end of an arc
				int numberOfNeighbors = -1;   // -1 and not 0 because the center pixel will be counted as well  

				short neighborhood[27];
				int tmp_index = 0;
				for (int j = -1; j <= 1; ++j) for (int k = -1; k <= 1; ++k) for (int l = -1; l <= 1; ++l)
				{
					int offset[] = { l,k,j };

					int value = GetPixel(p[0], p[1], p[2], offset);

					neighborhood[tmp_index++] = value;

					//neighborhood.push_back(value);
					if (value > 0) {
						numberOfNeighbors++;
					}

				}
				if (numberOfNeighbors == 1)
				{
					continue;         // current point is not deletable
				}

				//if (!euler_invariant[neighborhood])
				if (!isEulerInvariant(neighborhood, eulerLUT))
				{
					continue;         // current point is not deletable
				}

				// check if point is simple (deletion does not change connectivity in the 3x3x3 neighborhood)
				//if (!simple_point[neighborhood])
				if (!isSimplePoint(neighborhood))
				{
					continue;         // current point is not deletable
				}

				// add all simple border points to a list for sequential re-checking
				//simpleBorderPoints.push_back(vi);
				simpleBorderPoints[index] = true;
			} // end image iteration loop

			end_in = clock();
			//cout << "		image Time : " << (double)(end_in - start_in) / CLOCKS_PER_SEC << "s" << endl;



			//--------------------------------------------------------------------------------------------------------
			// sequential re-checking to preserve connectivity when
			// deleting in a parallel way
			bool noChange = true;
			for (int p = 0; p < object_number; p++)//simpleBorderPoints.size()
			{
				if (simpleBorderPoints[p] == false)
					continue;

				int x = xs[p];
				int y = ys[p];
				int z = zs[p];
				int index = x + y * dims[0] + z * dims[0] * dims[1];

				// 1. Set simple border point to 0
				data[index] = 0;

				// 2. Check if neighborhood is still connected
				short neighborhood[27];
				int t_index = 0;
				for (int z = -1; z <= 1; ++z) for (int y = -1; y <= 1; ++y) for (int x = -1; x <= 1; ++x) {
					int offset[] = { x,y,z };
					int p[3];
					GetPosition(index, p[0], p[1], p[2]);
					int value = GetPixel(p[0], p[1], p[2], offset);//{p[0],p[1],p[2]}
					neighborhood[t_index++] = value;
				}


				if (!isSimplePoint(neighborhood))
				{
					// we cannot delete current point, so reset
					data[index] = 1;
				}
				else
				{
					//deleted.push_back(p);
					noChange = false;
				}
			}// end for p

			if (noChange)
				unchangedBorders++;

			delete[]simpleBorderPoints;

			end = clock();
			//cout << "   for Time : " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;

		} // end currentBorder for loop

		  //int pixels = getFrontNumber( data,this->vn, 1);

		  //cout << "step:" << step<<","<<pixels << endl;
		step++;

		endTime = clock();
		//cout << step << " Time : " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;

	} // end unchangedBorders while loop
}

void Skeletonize::fillEulerLUT(int* LUT) {
	LUT[1] = 1;
	LUT[3] = -1;
	LUT[5] = -1;
	LUT[7] = 1;
	LUT[9] = -3;
	LUT[11] = -1;
	LUT[13] = -1;
	LUT[15] = 1;
	LUT[17] = -1;
	LUT[19] = 1;
	LUT[21] = 1;
	LUT[23] = -1;
	LUT[25] = 3;
	LUT[27] = 1;
	LUT[29] = 1;
	LUT[31] = -1;
	LUT[33] = -3;
	LUT[35] = -1;
	LUT[37] = 3;
	LUT[39] = 1;
	LUT[41] = 1;
	LUT[43] = -1;
	LUT[45] = 3;
	LUT[47] = 1;
	LUT[49] = -1;
	LUT[51] = 1;

	LUT[53] = 1;
	LUT[55] = -1;
	LUT[57] = 3;
	LUT[59] = 1;
	LUT[61] = 1;
	LUT[63] = -1;
	LUT[65] = -3;
	LUT[67] = 3;
	LUT[69] = -1;
	LUT[71] = 1;
	LUT[73] = 1;
	LUT[75] = 3;
	LUT[77] = -1;
	LUT[79] = 1;
	LUT[81] = -1;
	LUT[83] = 1;
	LUT[85] = 1;
	LUT[87] = -1;
	LUT[89] = 3;
	LUT[91] = 1;
	LUT[93] = 1;
	LUT[95] = -1;
	LUT[97] = 1;
	LUT[99] = 3;
	LUT[101] = 3;
	LUT[103] = 1;

	LUT[105] = 5;
	LUT[107] = 3;
	LUT[109] = 3;
	LUT[111] = 1;
	LUT[113] = -1;
	LUT[115] = 1;
	LUT[117] = 1;
	LUT[119] = -1;
	LUT[121] = 3;
	LUT[123] = 1;
	LUT[125] = 1;
	LUT[127] = -1;
	LUT[129] = -7;
	LUT[131] = -1;
	LUT[133] = -1;
	LUT[135] = 1;
	LUT[137] = -3;
	LUT[139] = -1;
	LUT[141] = -1;
	LUT[143] = 1;
	LUT[145] = -1;
	LUT[147] = 1;
	LUT[149] = 1;
	LUT[151] = -1;
	LUT[153] = 3;
	LUT[155] = 1;

	LUT[157] = 1;
	LUT[159] = -1;
	LUT[161] = -3;
	LUT[163] = -1;
	LUT[165] = 3;
	LUT[167] = 1;
	LUT[169] = 1;
	LUT[171] = -1;
	LUT[173] = 3;
	LUT[175] = 1;
	LUT[177] = -1;
	LUT[179] = 1;
	LUT[181] = 1;
	LUT[183] = -1;
	LUT[185] = 3;
	LUT[187] = 1;
	LUT[189] = 1;
	LUT[191] = -1;
	LUT[193] = -3;
	LUT[195] = 3;
	LUT[197] = -1;
	LUT[199] = 1;
	LUT[201] = 1;
	LUT[203] = 3;
	LUT[205] = -1;
	LUT[207] = 1;

	LUT[209] = -1;
	LUT[211] = 1;
	LUT[213] = 1;
	LUT[215] = -1;
	LUT[217] = 3;
	LUT[219] = 1;
	LUT[221] = 1;
	LUT[223] = -1;
	LUT[225] = 1;
	LUT[227] = 3;
	LUT[229] = 3;
	LUT[231] = 1;
	LUT[233] = 5;
	LUT[235] = 3;
	LUT[237] = 3;
	LUT[239] = 1;
	LUT[241] = -1;
	LUT[243] = 1;
	LUT[245] = 1;
	LUT[247] = -1;
	LUT[249] = 3;
	LUT[251] = 1;
	LUT[253] = 1;
	LUT[255] = -1;
}

bool Skeletonize::isEulerInvariant(short* neighbors, int* LUT)
{
	// calculate Euler characteristic for each octant and sum up
	int EulerChar = 0;
	unsigned char n;
	// Octant SWU
	n = 1;
	if (neighbors[24] == 1)
		n |= 128;
	if (neighbors[25] == 1)
		n |= 64;
	if (neighbors[15] == 1)
		n |= 32;
	if (neighbors[16] == 1)
		n |= 16;
	if (neighbors[21] == 1)
		n |= 8;
	if (neighbors[22] == 1)
		n |= 4;
	if (neighbors[12] == 1)
		n |= 2;
	EulerChar += LUT[n];
	// Octant SEU
	n = 1;
	if (neighbors[26] == 1)
		n |= 128;
	if (neighbors[23] == 1)
		n |= 64;
	if (neighbors[17] == 1)
		n |= 32;
	if (neighbors[14] == 1)
		n |= 16;
	if (neighbors[25] == 1)
		n |= 8;
	if (neighbors[22] == 1)
		n |= 4;
	if (neighbors[16] == 1)
		n |= 2;
	EulerChar += LUT[n];
	// Octant NWU
	n = 1;
	if (neighbors[18] == 1)
		n |= 128;
	if (neighbors[21] == 1)
		n |= 64;
	if (neighbors[9] == 1)
		n |= 32;
	if (neighbors[12] == 1)
		n |= 16;
	if (neighbors[19] == 1)
		n |= 8;
	if (neighbors[22] == 1)
		n |= 4;
	if (neighbors[10] == 1)
		n |= 2;
	EulerChar += LUT[n];
	// Octant NEU
	n = 1;
	if (neighbors[20] == 1)
		n |= 128;
	if (neighbors[23] == 1)
		n |= 64;
	if (neighbors[19] == 1)
		n |= 32;
	if (neighbors[22] == 1)
		n |= 16;
	if (neighbors[11] == 1)
		n |= 8;
	if (neighbors[14] == 1)
		n |= 4;
	if (neighbors[10] == 1)
		n |= 2;
	EulerChar += LUT[n];
	// Octant SWB
	n = 1;
	if (neighbors[6] == 1)
		n |= 128;
	if (neighbors[15] == 1)
		n |= 64;
	if (neighbors[7] == 1)
		n |= 32;
	if (neighbors[16] == 1)
		n |= 16;
	if (neighbors[3] == 1)
		n |= 8;
	if (neighbors[12] == 1)
		n |= 4;
	if (neighbors[4] == 1)
		n |= 2;
	EulerChar += LUT[n];
	// Octant SEB
	n = 1;
	if (neighbors[8] == 1)
		n |= 128;
	if (neighbors[7] == 1)
		n |= 64;
	if (neighbors[17] == 1)
		n |= 32;
	if (neighbors[16] == 1)
		n |= 16;
	if (neighbors[5] == 1)
		n |= 8;
	if (neighbors[4] == 1)
		n |= 4;
	if (neighbors[14] == 1)
		n |= 2;
	EulerChar += LUT[n];
	// Octant NWB
	n = 1;
	if (neighbors[0] == 1)
		n |= 128;
	if (neighbors[9] == 1)
		n |= 64;
	if (neighbors[3] == 1)
		n |= 32;
	if (neighbors[12] == 1)
		n |= 16;
	if (neighbors[1] == 1)
		n |= 8;
	if (neighbors[10] == 1)
		n |= 4;
	if (neighbors[4] == 1)
		n |= 2;
	EulerChar += LUT[n];
	// Octant NEB
	n = 1;
	if (neighbors[2] == 1)
		n |= 128;
	if (neighbors[1] == 1)
		n |= 64;
	if (neighbors[11] == 1)
		n |= 32;
	if (neighbors[10] == 1)
		n |= 16;
	if (neighbors[5] == 1)
		n |= 8;
	if (neighbors[4] == 1)
		n |= 4;
	if (neighbors[14] == 1)
		n |= 2;
	EulerChar += LUT[n];
	if (EulerChar == 0)
		return true;
	else
		return false;
}

bool Skeletonize::isSimplePoint(short* neighbors) {
	// copy neighbors for labeling
	int cube[26];
	int i;
	for (i = 0; i < 13; i++)  // i =  0..12 -> cube[0..12]
		cube[i] = neighbors[i];
	// i != 13 : ignore center pixel when counting (see [Lee94])
	for (i = 14; i < 27; i++) // i = 14..26 -> cube[13..25]
		cube[i - 1] = neighbors[i];
	// set initial label
	int label = 2;
	// for all points in the neighborhood
	for (int i = 0; i < 26; i++) {
		if (cube[i] == 1) {
			switch (i)
			{
			case 0:
			case 1:
			case 3:
			case 4:
			case 9:
			case 10:
			case 12:
				Octree_labeling(1, label, cube);
				break;
			case 2:
			case 5:
			case 11:
			case 13:
				Octree_labeling(2, label, cube);
				break;
			case 6:
			case 7:
			case 14:
			case 15:
				Octree_labeling(3, label, cube);
				break;
			case 8:
			case 16:
				Octree_labeling(4, label, cube);
				break;
			case 17:
			case 18:
			case 20:
			case 21:
				Octree_labeling(5, label, cube);
				break;
			case 19:
			case 22:
				Octree_labeling(6, label, cube);
				break;
			case 23:
			case 24:
				Octree_labeling(7, label, cube);
				break;
			case 25:
				Octree_labeling(8, label, cube);
				break;
			}
			label++;
			if (label - 2 >= 2)
			{
				return false;
			}
		}
	}

	return true;
}


void Skeletonize::Octree_labeling(int octant, int label, int* cube) {
	// check if there are points in the octant with value 1
	if (octant == 1) {
		if (cube[0] == 1)
			cube[0] = label;
		if (cube[1] == 1)
		{
			cube[1] = label;
			Octree_labeling(2, label, cube);
		}
		if (cube[3] == 1)
		{
			cube[3] = label;
			Octree_labeling(3, label, cube);
		}
		if (cube[4] == 1)
		{
			cube[4] = label;
			Octree_labeling(2, label, cube);
			Octree_labeling(3, label, cube);
			Octree_labeling(4, label, cube);
		}
		if (cube[9] == 1)
		{
			cube[9] = label;
			Octree_labeling(5, label, cube);
		}
		if (cube[10] == 1)
		{
			cube[10] = label;
			Octree_labeling(2, label, cube);
			Octree_labeling(5, label, cube);
			Octree_labeling(6, label, cube);
		}
		if (cube[12] == 1)
		{
			cube[12] = label;
			Octree_labeling(3, label, cube);
			Octree_labeling(5, label, cube);
			Octree_labeling(7, label, cube);
		}
	}
	if (octant == 2)
	{
		if (cube[1] == 1)
		{
			cube[1] = label;
			Octree_labeling(1, label, cube);
		}
		if (cube[4] == 1)
		{
			cube[4] = label;
			Octree_labeling(1, label, cube);
			Octree_labeling(3, label, cube);
			Octree_labeling(4, label, cube);
		}
		if (cube[10] == 1)
		{
			cube[10] = label;
			Octree_labeling(1, label, cube);
			Octree_labeling(5, label, cube);
			Octree_labeling(6, label, cube);
		}
		if (cube[2] == 1)
			cube[2] = label;
		if (cube[5] == 1)
		{
			cube[5] = label;
			Octree_labeling(4, label, cube);
		}
		if (cube[11] == 1)
		{
			cube[11] = label;
			Octree_labeling(6, label, cube);
		}
		if (cube[13] == 1)
		{
			cube[13] = label;
			Octree_labeling(4, label, cube);
			Octree_labeling(6, label, cube);
			Octree_labeling(8, label, cube);
		}
	}
	if (octant == 3)
	{
		if (cube[3] == 1)
		{
			cube[3] = label;
			Octree_labeling(1, label, cube);
		}
		if (cube[4] == 1)
		{
			cube[4] = label;
			Octree_labeling(1, label, cube);
			Octree_labeling(2, label, cube);
			Octree_labeling(4, label, cube);
		}
		if (cube[12] == 1)
		{
			cube[12] = label;
			Octree_labeling(1, label, cube);
			Octree_labeling(5, label, cube);
			Octree_labeling(7, label, cube);
		}
		if (cube[6] == 1)
			cube[6] = label;
		if (cube[7] == 1)
		{
			cube[7] = label;
			Octree_labeling(4, label, cube);
		}
		if (cube[14] == 1)
		{
			cube[14] = label;
			Octree_labeling(7, label, cube);
		}
		if (cube[15] == 1)
		{
			cube[15] = label;
			Octree_labeling(4, label, cube);
			Octree_labeling(7, label, cube);
			Octree_labeling(8, label, cube);
		}
	}
	if (octant == 4)
	{
		if (cube[4] == 1)
		{
			cube[4] = label;
			Octree_labeling(1, label, cube);
			Octree_labeling(2, label, cube);
			Octree_labeling(3, label, cube);
		}
		if (cube[5] == 1)
		{
			cube[5] = label;
			Octree_labeling(2, label, cube);
		}
		if (cube[13] == 1)
		{
			cube[13] = label;
			Octree_labeling(2, label, cube);
			Octree_labeling(6, label, cube);
			Octree_labeling(8, label, cube);
		}
		if (cube[7] == 1)
		{
			cube[7] = label;
			Octree_labeling(3, label, cube);
		}
		if (cube[15] == 1)
		{
			cube[15] = label;
			Octree_labeling(3, label, cube);
			Octree_labeling(7, label, cube);
			Octree_labeling(8, label, cube);
		}
		if (cube[8] == 1)
			cube[8] = label;
		if (cube[16] == 1)
		{
			cube[16] = label;
			Octree_labeling(8, label, cube);
		}
	}
	if (octant == 5)
	{
		if (cube[9] == 1)
		{
			cube[9] = label;
			Octree_labeling(1, label, cube);
		}
		if (cube[10] == 1)
		{
			cube[10] = label;
			Octree_labeling(1, label, cube);
			Octree_labeling(2, label, cube);
			Octree_labeling(6, label, cube);
		}
		if (cube[12] == 1)
		{
			cube[12] = label;
			Octree_labeling(1, label, cube);
			Octree_labeling(3, label, cube);
			Octree_labeling(7, label, cube);
		}
		if (cube[17] == 1)
			cube[17] = label;
		if (cube[18] == 1)
		{
			cube[18] = label;
			Octree_labeling(6, label, cube);
		}
		if (cube[20] == 1)
		{
			cube[20] = label;
			Octree_labeling(7, label, cube);
		}
		if (cube[21] == 1)
		{
			cube[21] = label;
			Octree_labeling(6, label, cube);
			Octree_labeling(7, label, cube);
			Octree_labeling(8, label, cube);
		}
	}
	if (octant == 6)
	{
		if (cube[10] == 1)
		{
			cube[10] = label;
			Octree_labeling(1, label, cube);
			Octree_labeling(2, label, cube);
			Octree_labeling(5, label, cube);
		}
		if (cube[11] == 1)
		{
			cube[11] = label;
			Octree_labeling(2, label, cube);
		}
		if (cube[13] == 1)
		{
			cube[13] = label;
			Octree_labeling(2, label, cube);
			Octree_labeling(4, label, cube);
			Octree_labeling(8, label, cube);
		}
		if (cube[18] == 1)
		{
			cube[18] = label;
			Octree_labeling(5, label, cube);
		}
		if (cube[21] == 1)
		{
			cube[21] = label;
			Octree_labeling(5, label, cube);
			Octree_labeling(7, label, cube);
			Octree_labeling(8, label, cube);
		}
		if (cube[19] == 1)
			cube[19] = label;
		if (cube[22] == 1)
		{
			cube[22] = label;
			Octree_labeling(8, label, cube);
		}
	}
	if (octant == 7)
	{
		if (cube[12] == 1)
		{
			cube[12] = label;
			Octree_labeling(1, label, cube);
			Octree_labeling(3, label, cube);
			Octree_labeling(5, label, cube);
		}
		if (cube[14] == 1)
		{
			cube[14] = label;
			Octree_labeling(3, label, cube);
		}
		if (cube[15] == 1)
		{
			cube[15] = label;
			Octree_labeling(3, label, cube);
			Octree_labeling(4, label, cube);
			Octree_labeling(8, label, cube);
		}
		if (cube[20] == 1)
		{
			cube[20] = label;
			Octree_labeling(5, label, cube);
		}
		if (cube[21] == 1)
		{
			cube[21] = label;
			Octree_labeling(5, label, cube);
			Octree_labeling(6, label, cube);
			Octree_labeling(8, label, cube);
		}
		if (cube[23] == 1)
			cube[23] = label;
		if (cube[24] == 1)
		{
			cube[24] = label;
			Octree_labeling(8, label, cube);
		}
	}
	if (octant == 8)
	{
		if (cube[13] == 1)
		{
			cube[13] = label;
			Octree_labeling(2, label, cube);
			Octree_labeling(4, label, cube);
			Octree_labeling(6, label, cube);
		}
		if (cube[15] == 1)
		{
			cube[15] = label;
			Octree_labeling(3, label, cube);
			Octree_labeling(4, label, cube);
			Octree_labeling(7, label, cube);
		}
		if (cube[16] == 1)
		{
			cube[16] = label;
			Octree_labeling(4, label, cube);
		}
		if (cube[21] == 1)
		{
			cube[21] = label;
			Octree_labeling(5, label, cube);
			Octree_labeling(6, label, cube);
			Octree_labeling(7, label, cube);
		}
		if (cube[22] == 1)
		{
			cube[22] = label;
			Octree_labeling(6, label, cube);
		}
		if (cube[24] == 1)
		{
			cube[24] = label;
			Octree_labeling(7, label, cube);
		}
		if (cube[25] == 1)
			cube[25] = label;
	}
}
