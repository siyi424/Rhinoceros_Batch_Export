#include "Windows.h"
#include "tlhelp32.h"
#include <sys/stat.h>
#include <Psapi.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <valarray>
#include <iomanip>
#define _CRT_NONSTDC_NO_DEPRECATE
using namespace std;
#pragma comment(lib,"psapi.lib")

int GetProcessCount(const TCHAR* szExeName)
{
	TCHAR sztarget[MAX_PATH];
	lstrcpy(sztarget, szExeName);
	CharLowerBuff(sztarget, MAX_PATH);

	int count = 0;
	PROCESSENTRY32 my;
	HANDLE l = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	long long l1 = reinterpret_cast<long long>(l);
	if (l1 != -1)
	{
		my.dwSize = sizeof(my);
		if (Process32First(l, &my))
		{
			do
			{
				CharLowerBuff(my.szExeFile, MAX_PATH);
				if (lstrcmp(sztarget, my.szExeFile) == 0)
				{
					count++;
				}
			} while (Process32Next(l, &my));
		}
		CloseHandle(l);
	}
	return count;
}

/**********************************************************************/
string VoxelizerProg = "binvox.exe";
//string path = "D:\\Pheobe\\0_pregnent\\output\\ARADILLA-VELEZ_CLAUDIA_97352272_A\\wrl\\";
string path = "C:\\Users\\crthx\\Desktop\\Fudan\\Rhinoceros_Batch_Export_wrl\\";

const unsigned char BodyID = 10;
const unsigned char SkinID = 11;

int MergeOrgans(string BaseFilename, string MergedPhantomName, string InfoFileName, int FileSize_x, int FileSize_y, int FileSize_z)
{
	const int MergedOrganNumb = 6;

	string MergedOrganFiles[MergedOrganNumb] = {
		"15",
		"16",
		"18",
		"19_0",
		"19_1",
		"21"
	};

	float MergedOrgan[MergedOrganNumb][7] = { 
		{ 15, 27, 12, 0, 94, 66, 62 },
		{ 16, 32, 17, 4, 88, 63, 56 },
		{ 18, 9, 0, 341, 130, 150, 454 },
		{ 19, 0, 71, 184, 13, 81, 194 },
		{ 19, 116, 79, 178, 128, 88, 188 },
		{ 21, 50, 19, 172, 69, 75, 281 }
	}; //OrganID,min_pos,max_pos

	int MergedOrgan_posdeta[MergedOrganNumb][4] = { 
		{ 15,0,0,0},
		{ 16,0,0,0},
		{ 18,0,0,0},
		{ 19,0,0,0},
		{ 19,0,0,0},
		{ 21,0,0,0},
	}; //OrganID,movement

	const int ErasedOrganNumb = 1;
	const int ErasedOrganID[ErasedOrganNumb] = { -1 };

	const unsigned long int MaxNum = FileSize_x * FileSize_y * FileSize_z;

	int VoxelN[1000] = { 0 };
	int VoxelN2[1000] = { 0 };

	unsigned long int VNum = FileSize_x * FileSize_y * FileSize_z;
	unsigned long int VFlag = 0;

	unsigned char* Old_matrix = new unsigned char[MaxNum];
	unsigned char* New_matrix = new unsigned char[MaxNum];

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "r");
	cout << BaseFilename.c_str() << endl;
	fread(Old_matrix, sizeof(unsigned char), MaxNum, fp);

	cout << "MergeOrgans..." << endl;

	for (unsigned long int i = 0; i < MaxNum; i++)
	{
		New_matrix[i] = 0;
		if (Old_matrix[i] > 0 && Old_matrix[i] < 100)
		{
			New_matrix[i] = Old_matrix[i];
			for (int id = 0; id < ErasedOrganNumb; id++)
			{
				if (ErasedOrganID[id] == Old_matrix[i])
				{
					New_matrix[i] = BodyID;
					break;
				}
			}
			VoxelN[Old_matrix[i]]++;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < MergedOrganNumb; i++)
	{
		string MergedOrganName = path;
		MergedOrganName.append(MergedOrganFiles[i]);
		MergedOrganName.append(".wrl");

		string VoxelizedOrganName = path;
		VoxelizedOrganName.append(MergedOrganFiles[i]);
		VoxelizedOrganName.append(".raw");

		string AABBOrganName = path;
		AABBOrganName.append(MergedOrganFiles[i]);
		AABBOrganName.append(".AABB.bin");

		string InfoOrganName = path;
		InfoOrganName.append(MergedOrganFiles[i]);
		InfoOrganName.append(".txt");

		int MergedOrganSize_x = abs(MergedOrgan[i][1] - MergedOrgan[i][4]);
		int MergedOrganSize_y = abs(MergedOrgan[i][2] - MergedOrgan[i][5]);
		int MergedOrganSize_z = abs(MergedOrgan[i][3] - MergedOrgan[i][6]);

		int VoxelizerSize = MergedOrganSize_x;
		if (VoxelizerSize < MergedOrganSize_y)
			VoxelizerSize = MergedOrganSize_y;
		if (VoxelizerSize < MergedOrganSize_z)
			VoxelizerSize = MergedOrganSize_z;
		cout << "OrganFile" << MergedOrganName.c_str() << "\n VoxelizerSize: Organ" << MergedOrgan[i][0] << " = " << VoxelizerSize << endl;
			


		// �����У�path\\binvox.exe -d sizeNum -t raw MergedOrganName
		string Voxelizer = path;
		Voxelizer.append(VoxelizerProg.c_str());
		string watcher = VoxelizerProg;
		Voxelizer.append(" -d ");
		char* sizeNum = new char[4];
		// VoxelizerSize change to char[4] put into sizeNum (decimalism)
		_itoa_s(VoxelizerSize, sizeNum, 4, 10);
		Voxelizer.append(sizeNum);
		Voxelizer.append(" -t raw ");
		Voxelizer.append(MergedOrganName);

		DeleteFile(VoxelizedOrganName.c_str());
		cout << "voxelizer execut: " << Voxelizer.c_str() << endl;
		
		STARTUPINFO si = {0};
		PROCESS_INFORMATION proi = { 0 };
		si.cb = sizeof(si);
		DWORD StartTime = GetTickCount();//��ʼʱ��
		DWORD EndTime = 0;
		CreateProcess(NULL, &(Voxelizer[0]), NULL, NULL, FALSE, 0, NULL, NULL, &si, &proi);//����mcnpx.exe,�������ͬһĿ¼����ָ��·��
		
		while (1)
		{
			Sleep(100);
			if (GetProcessCount(&(watcher[0])) == 0)
			{
				break;
			} 
		}

		const unsigned long int voxelized_MaxNum = VoxelizerSize * VoxelizerSize * VoxelizerSize;
		unsigned char* voxelized_matrix = new unsigned char[voxelized_MaxNum];
		unsigned char* rot_voxel_matrix = new unsigned char[voxelized_MaxNum];
		//		unsigned char* mir_voxel_matrix = new unsigned char[voxelized_MaxNum];
		cout << VoxelizedOrganName.c_str() << endl;
		FILE* fov;
		fopen_s(&fov, VoxelizedOrganName.c_str(), "r");
		fread(voxelized_matrix, sizeof(unsigned char), voxelized_MaxNum, fov);
		::fclose(fov);

		for (int z = 0; z < VoxelizerSize; z++)
		{
			for (int y = 0; y < VoxelizerSize; y++)
			{
				for (int x = 0; x < VoxelizerSize; x++)
				{
					rot_voxel_matrix[z * VoxelizerSize * VoxelizerSize + y * VoxelizerSize + x] = voxelized_matrix[x * VoxelizerSize * VoxelizerSize + z * VoxelizerSize + y];
				}
			}
		}

		int Z_start_body = VoxelizerSize;
		int Z_end_body = 0;
		int X_start_body = VoxelizerSize;
		int X_end_body = 0;
		int Y_start_body = VoxelizerSize;
		int Y_end_body = 0;
		int pos_x = 0;
		int pos_y = 0;
		int pos_z = 0;

		for (unsigned long int i = 0; i < voxelized_MaxNum; i++)
		{
			if (rot_voxel_matrix[i] > 0)
			{
				pos_z = i / (VoxelizerSize * VoxelizerSize);
				if (pos_z > Z_end_body)
					Z_end_body = pos_z;
				if (pos_z < Z_start_body)
					Z_start_body = pos_z;

				pos_y = (i % (VoxelizerSize * VoxelizerSize)) / VoxelizerSize;
				if (pos_y > Y_end_body)
					Y_end_body = pos_y;
				if (pos_y < Y_start_body)
					Y_start_body = pos_y;

				pos_x = (i % (VoxelizerSize * VoxelizerSize)) % VoxelizerSize;
				if (pos_x > X_end_body)
					X_end_body = pos_x;
				if (pos_x < X_start_body)
					X_start_body = pos_x;
			}
		}
		cout << Z_start_body << " - " << Z_end_body << ";" << Y_start_body << " - " << Y_end_body << ";" << X_start_body << " - " << X_end_body << endl;
		int Vox_OrganOBB_x = X_end_body - X_start_body + 1;
		int Vox_OrganOBB_y = Y_end_body - Y_start_body + 1;
		int Vox_OrganOBB_z = Z_end_body - Z_start_body + 1;

		const unsigned long obb_MaxNum = Vox_OrganOBB_x * Vox_OrganOBB_y * Vox_OrganOBB_z;
		unsigned char* obb_voxel_matrix = new unsigned char[obb_MaxNum];
		for (int i = 0; i < obb_MaxNum; i++)
		{
			obb_voxel_matrix[i] = 0;
		}

		int x, y, z;
		int AABB_VFlag = 0;
		for (int i = 0; i < voxelized_MaxNum; i++)
		{
			z = i / (VoxelizerSize * VoxelizerSize);
			y = (i % (VoxelizerSize * VoxelizerSize)) / VoxelizerSize;
			x = (i % (VoxelizerSize * VoxelizerSize)) % VoxelizerSize;
			if ((x >= X_start_body) && (x <= X_end_body) && (y >= Y_start_body) && (y <= Y_end_body) && (z >= Z_start_body) && (z <= Z_end_body))
			{
				obb_voxel_matrix[AABB_VFlag] = rot_voxel_matrix[i];
				AABB_VFlag++;
			}
		}

		cout << "Writing3..." << AABBOrganName.c_str() << endl;
		FILE* oFile4;
		fopen_s(&oFile4, AABBOrganName.c_str(), "wb");
		::fwrite(obb_voxel_matrix, sizeof(unsigned char), obb_MaxNum, oFile4);
		::fclose(oFile4);

		int MergedOrganCenter_x = int((MergedOrgan[i][1] + MergedOrgan[i][4]) / 2);
		int MergedOrganCenter_y = int((MergedOrgan[i][2] + MergedOrgan[i][5]) / 2);
		int MergedOrganCenter_z = int((MergedOrgan[i][3] + MergedOrgan[i][6]) / 2);

		cout << MergedOrgan[i][0] << " \n";//organ_ID
		cout << "int(2 * "<<MergedOrganCenter_x << " - " << Vox_OrganOBB_x << ") / 2 "<< " \n"; //x_min
		cout << Vox_OrganOBB_x << " ";//x_rang
		cout << "int(2 * "<< MergedOrganCenter_y << " - " << Vox_OrganOBB_y << ") / 2 "<< " \n"; //y_min
		cout << Vox_OrganOBB_y << " ";//y_rang
		cout << "int(2 * "<< MergedOrganCenter_z << " - " << Vox_OrganOBB_z << ") / 2 "<< " \n"; //z_min
		cout << Vox_OrganOBB_z << " \n\n";//z_rang

		ofstream saveinfoFile(InfoOrganName.c_str(), ios::out);
		saveinfoFile << MergedOrgan[i][0] << " ";//organ_ID
		saveinfoFile << int(2 * MergedOrganCenter_x - Vox_OrganOBB_x) / 2 << " "; //x_min
		saveinfoFile << Vox_OrganOBB_x << " ";//x_rang
		saveinfoFile << int(2 * MergedOrganCenter_y - Vox_OrganOBB_y) / 2 << " "; //y_min
		saveinfoFile << Vox_OrganOBB_y << " ";//y_rang
		saveinfoFile << int(2 * MergedOrganCenter_z - Vox_OrganOBB_z) / 2 << " "; //z_min
		saveinfoFile << Vox_OrganOBB_z << " \n";//z_rang
		saveinfoFile.close();
	}

	///////////////////////////////////////////////////////////

	for (int id = 0; id < MergedOrganNumb; id++)
	{
		string AABBOrganName = path;
		AABBOrganName.append(MergedOrganFiles[id]);
		AABBOrganName.append(".AABB.bin");

		string InfoOrganName = path;
		InfoOrganName.append(MergedOrganFiles[id]);
		InfoOrganName.append(".txt");

		cout << AABBOrganName.c_str() << endl;
		cout << InfoOrganName.c_str() << endl;

		int OrganID = 0;
		double* OrganInfoMatri = new double[6];
		ifstream infoFile(InfoOrganName.c_str());
		if (infoFile.is_open())
		{
			infoFile >> OrganID;
			for (int f = 0; f < 6; f++)
				infoFile >> OrganInfoMatri[f];
		}
		infoFile.close();

		int OrganSize_x = OrganInfoMatri[1];
		int OrganSize_y = OrganInfoMatri[3];
		int OrganSize_z = OrganInfoMatri[5];

		int OrganStart_x = OrganInfoMatri[0];
		int OrganStart_y = OrganInfoMatri[2];
		int OrganStart_z = OrganInfoMatri[4];

		unsigned long OrganMaxNum = OrganSize_x * OrganSize_y * OrganSize_z;
		unsigned char* OrganMatrix = new unsigned char[OrganMaxNum];
		FILE* fp2;
		string readFile = AABBOrganName.c_str();
		fopen_s(&fp2, readFile.c_str(), "r");
		cout << "Re-reading..." << readFile.c_str() << endl;
		fread(OrganMatrix, sizeof(unsigned char), OrganMaxNum, fp2);
		::fclose(fp2);

		for (int i = 0; i < OrganMaxNum; i++)
		{
			if (OrganMatrix[i] > 0)
			{
				int pos_z = i / (OrganSize_x * OrganSize_y) + OrganStart_z + MergedOrgan_posdeta[id][3];
				int pos_y = (i % (OrganSize_x * OrganSize_y)) / OrganSize_x + OrganStart_y + MergedOrgan_posdeta[id][2];
				int pos_x = (i % (OrganSize_x * OrganSize_y)) % OrganSize_x + OrganStart_x + MergedOrgan_posdeta[id][1];

				if (OrganID != 47 || OrganID != 49)
				{
					New_matrix[pos_z * FileSize_y * FileSize_x + pos_y * FileSize_x + pos_x] = (unsigned char)OrganID;
				}
				else
				{
					//					if(New_matrix[pos_z*FileSize_y*FileSize_x + pos_y*FileSize_x + pos_x]>=10 && New_matrix[pos_z*FileSize_y*FileSize_x + pos_y*FileSize_x + pos_x] != 11)
					if (New_matrix[pos_z * FileSize_y * FileSize_x + pos_y * FileSize_x + pos_x] == 46 || New_matrix[pos_z * FileSize_y * FileSize_x + pos_y * FileSize_x + pos_x] == 47)
						New_matrix[pos_z * FileSize_y * FileSize_x + pos_y * FileSize_x + pos_x] = (unsigned char)OrganID;
				}
			}
		}

		cout << OrganID << " " << OrganSize_x << " " << OrganSize_y << " " << OrganSize_z << " " << OrganStart_x << " " << OrganStart_y << " " << OrganStart_z << " " << endl;
	}
	///////////////////////////////////////////////////////////
	for (unsigned long int i = 0; i < MaxNum; i++)
	{
		if (New_matrix[i] < 0)
		{
			cout << New_matrix[i] << endl;
		}
		else
		{
			VoxelN2[New_matrix[i]]++;
		}
	}

	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, MergedPhantomName.c_str(), "wb");
	::fwrite(New_matrix, sizeof(unsigned char), MaxNum, oFile);
	::fclose(oFile);

	ofstream outFile2(InfoFileName.c_str(), ios::out);
	for (int VN = 0; VN < 999; VN++)
	{
		if (VoxelN[VN] > 0 || VoxelN2[VN] > 0)
		{
			outFile2 << VN;
			outFile2 << ": ";
			outFile2 << VoxelN[VN];
			outFile2 << " ";
			outFile2 << VoxelN2[VN];
			outFile2 << "\n";
		}
	}
	outFile2.close();

	return 0;
}

int MergeMeshOrgans(string BaseFilename, string MergedPhantomName, string InfoFileName, int FileSize_x, int FileSize_y, int FileSize_z)
{
	const int MergedOrganNumb = 2;
	float MergedOrgan[MergedOrganNumb][7] = { { 39,95.42,72.54,450.64,186.31,121.10,521.58 },  //Bounding boxes,
	{ 44,95.42,72.54,450.64,186.31,121.10,521.58 } }; //OrganID,min_pos,max_pos
	int MergedOrgan_posdeta[MergedOrganNumb][4] = { { 39,0,0,0 },
	{ 44,1,1,1 } }; //OrganID,movement

	const int ErasedOrganNumb = 1;
	const int ErasedOrganID[ErasedOrganNumb] = { -1 };//no organ was erased

	const unsigned long int MaxNum = FileSize_x * FileSize_y * FileSize_z;

	int VoxelN[1000] = { 0 };
	int VoxelN2[1000] = { 0 };

	unsigned long int VNum = FileSize_x * FileSize_y * FileSize_z;
	unsigned long int VFlag = 0;

	short* Old_matrix = new short[MaxNum];
	short* New_matrix = new short[MaxNum];

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "r");
	cout << BaseFilename.c_str() << endl;
	fread(Old_matrix, sizeof(short), MaxNum, fp);

	cout << "MergeMeshOrgans..." << endl;

	for (unsigned long int i = 0; i < MaxNum; i++)
	{
		New_matrix[i] = 0;
		if (Old_matrix[i] > 0 && Old_matrix[i] < 1000)
		{
			New_matrix[i] = Old_matrix[i];
			for (int id = 0; id < ErasedOrganNumb; id++)
			{
				if (ErasedOrganID[id] == Old_matrix[i])
				{
					New_matrix[i] = BodyID;
					break;
				}
			}
			VoxelN[Old_matrix[i]]++;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < MergedOrganNumb; i++)
	{
		string MergedOrganName = path;
		char* strID = new char[3];
		_itoa_s(MergedOrgan[i][0], strID, 3, 10);
		MergedOrganName.append(strID);
		MergedOrganName.append(".wrl");

		string VoxelizedOrganName = path;
		VoxelizedOrganName.append(strID);
		VoxelizedOrganName.append(".raw");

		string AABBOrganName = path;
		AABBOrganName.append(strID);
		AABBOrganName.append(".AABB.bin");

		string InfoOrganName = path;
		InfoOrganName.append(strID);
		InfoOrganName.append(".txt");

		int MergedOrganSize_x = abs(MergedOrgan[i][1] - MergedOrgan[i][4]);
		int MergedOrganSize_y = abs(MergedOrgan[i][2] - MergedOrgan[i][5]);
		int MergedOrganSize_z = abs(MergedOrgan[i][3] - MergedOrgan[i][6]);

		int VoxelizerSize = MergedOrganSize_x;
		if (VoxelizerSize < MergedOrganSize_y)
			VoxelizerSize = MergedOrganSize_y;
		if (VoxelizerSize < MergedOrganSize_z)
			VoxelizerSize = MergedOrganSize_z;
		cout << "VoxelizerSize: Organ" << MergedOrgan[i][0] << " = " << VoxelizerSize << endl;

		string Voxelizer = path;
		Voxelizer.append(VoxelizerProg.c_str());
		string watcher = VoxelizerProg;
		Voxelizer.append(" -d ");
		char* sizeNum = new char[3];
		_itoa_s(VoxelizerSize, sizeNum, 3, 10);
		Voxelizer.append(sizeNum);
		Voxelizer.append(" -t raw ");
		Voxelizer.append(MergedOrganName);

		DeleteFile(VoxelizedOrganName.c_str());
		cout << "voxelizer execut: " << Voxelizer.c_str() << endl;

		STARTUPINFO si = { 0 };
		PROCESS_INFORMATION proi = { 0 };
		si.cb = sizeof(si);
		DWORD StartTime = GetTickCount();//��ʼʱ��
		DWORD EndTime = 0;
		CreateProcess(NULL, &(Voxelizer[0]), NULL, NULL, FALSE, 0, NULL, NULL, &si, &proi);//����mcnpx.exe,�������ͬһĿ¼����ָ��·��

		while (1)
		{
			Sleep(100);
			if (GetProcessCount(&(watcher[0])) == 0)
			{
				break;
			}
		}

		const unsigned long int voxelized_MaxNum = VoxelizerSize * VoxelizerSize * VoxelizerSize;
		unsigned char* voxelized_matrix = new unsigned char[voxelized_MaxNum];
		unsigned char* rot_voxel_matrix = new unsigned char[voxelized_MaxNum];
		//		unsigned char* mir_voxel_matrix = new unsigned char[voxelized_MaxNum];
		FILE* fov;
		fopen_s(&fov, VoxelizedOrganName.c_str(), "r");
		fread(voxelized_matrix, sizeof(unsigned char), voxelized_MaxNum, fov);
		::fclose(fov);

		for (int z = 0; z < VoxelizerSize; z++)
		{
			for (int y = 0; y < VoxelizerSize; y++)
			{
				for (int x = 0; x < VoxelizerSize; x++)
				{
					rot_voxel_matrix[z * VoxelizerSize * VoxelizerSize + y * VoxelizerSize + x] = voxelized_matrix[y * VoxelizerSize * VoxelizerSize + x * VoxelizerSize + z];
				}
			}
		}

		int Z_start_body = VoxelizerSize;
		int Z_end_body = 0;
		int X_start_body = VoxelizerSize;
		int X_end_body = 0;
		int Y_start_body = VoxelizerSize;
		int Y_end_body = 0;
		int pos_x = 0;
		int pos_y = 0;
		int pos_z = 0;

		for (unsigned long int i = 0; i < voxelized_MaxNum; i++)
		{
			if (rot_voxel_matrix[i] > 0)
			{
				pos_z = i / (VoxelizerSize * VoxelizerSize);
				if (pos_z > Z_end_body)
					Z_end_body = pos_z;
				if (pos_z < Z_start_body)
					Z_start_body = pos_z;

				pos_y = (i % (VoxelizerSize * VoxelizerSize)) / VoxelizerSize;
				if (pos_y > Y_end_body)
					Y_end_body = pos_y;
				if (pos_y < Y_start_body)
					Y_start_body = pos_y;

				pos_x = (i % (VoxelizerSize * VoxelizerSize)) % VoxelizerSize;
				if (pos_x > X_end_body)
					X_end_body = pos_x;
				if (pos_x < X_start_body)
					X_start_body = pos_x;
			}
		}
		cout << Z_start_body << " - " << Z_end_body << ";" << Y_start_body << " - " << Y_end_body << ";" << X_start_body << " - " << X_end_body << endl;
		int Vox_OrganOBB_x = X_end_body - X_start_body + 1;
		int Vox_OrganOBB_y = Y_end_body - Y_start_body + 1;
		int Vox_OrganOBB_z = Z_end_body - Z_start_body + 1;

		const unsigned long obb_MaxNum = Vox_OrganOBB_x * Vox_OrganOBB_y * Vox_OrganOBB_z;
		unsigned char* obb_voxel_matrix = new unsigned char[obb_MaxNum];
		for (int i = 0; i < obb_MaxNum; i++)
		{
			obb_voxel_matrix[i] = 0;
		}

		int x, y, z;
		int AABB_VFlag = 0;
		for (int i = 0; i < voxelized_MaxNum; i++)
		{
			z = i / (VoxelizerSize * VoxelizerSize);
			y = (i % (VoxelizerSize * VoxelizerSize)) / VoxelizerSize;
			x = (i % (VoxelizerSize * VoxelizerSize)) % VoxelizerSize;
			if ((x >= X_start_body) && (x <= X_end_body) && (y >= Y_start_body) && (y <= Y_end_body) && (z >= Z_start_body) && (z <= Z_end_body))
			{
				obb_voxel_matrix[AABB_VFlag] = rot_voxel_matrix[i];
				AABB_VFlag++;
			}
		}

		cout << "Writing3..." << AABBOrganName.c_str() << endl;
		FILE* oFile4;
		fopen_s(&oFile4, AABBOrganName.c_str(), "wb");
		::fwrite(obb_voxel_matrix, sizeof(unsigned char), obb_MaxNum, oFile4);
		::fclose(oFile4);

		int MergedOrganCenter_x = int((MergedOrgan[i][1] + MergedOrgan[i][4]) / 2);
		int MergedOrganCenter_y = int((MergedOrgan[i][2] + MergedOrgan[i][5]) / 2);
		int MergedOrganCenter_z = int((MergedOrgan[i][3] + MergedOrgan[i][6]) / 2);

		ofstream saveinfoFile(InfoOrganName.c_str(), ios::out);
		saveinfoFile << MergedOrgan[i][0] << " ";//organ_ID
		saveinfoFile << int(2 * MergedOrganCenter_x - Vox_OrganOBB_x) / 2 << " "; //x_min
		saveinfoFile << Vox_OrganOBB_x << " ";//x_rang
		saveinfoFile << int(2 * MergedOrganCenter_y - Vox_OrganOBB_y) / 2 << " "; //y_min
		saveinfoFile << Vox_OrganOBB_y << " ";//y_rang
		saveinfoFile << int(2 * MergedOrganCenter_z - Vox_OrganOBB_z) / 2 << " "; //z_min
		saveinfoFile << Vox_OrganOBB_z << " \n";//z_rang
		saveinfoFile.close();
	}

	///////////////////////////////////////////////////////////

	for (int id = 0; id < MergedOrganNumb; id++)
	{
		string MergedOrganName = path;
		char* strID = new char[3];
		_itoa_s(MergedOrgan[id][0], strID, 3, 10);

		string AABBOrganName = path;
		AABBOrganName.append(strID);
		AABBOrganName.append(".AABB.bin");

		string InfoOrganName = path;
		InfoOrganName.append(strID);
		InfoOrganName.append(".txt");

		//		cout<<"Test1"<<endl;

		short OrganID = 0;
		double* OrganInfoMatri = new double[6];
		ifstream infoFile(InfoOrganName.c_str());
		if (infoFile.is_open())
		{
			infoFile >> OrganID;
			for (int f = 0; f < 6; f++)
				infoFile >> OrganInfoMatri[f];
		}
		infoFile.close();

		int OrganSize_x = OrganInfoMatri[1];
		int OrganSize_y = OrganInfoMatri[3];
		int OrganSize_z = OrganInfoMatri[5];

		int OrganStart_x = OrganInfoMatri[0];
		int OrganStart_y = OrganInfoMatri[2];
		int OrganStart_z = OrganInfoMatri[4];

		unsigned long OrganMaxNum = OrganSize_x * OrganSize_y * OrganSize_z;
		unsigned char* OrganMatrix = new unsigned char[OrganMaxNum];
		FILE* fp;
		string readFile = AABBOrganName.c_str();
		fopen_s(&fp, readFile.c_str(), "r");
		cout << "Re-reading..." << readFile.c_str() << endl;
		fread(OrganMatrix, sizeof(unsigned char), OrganMaxNum, fp);
		::fclose(fp);

		for (int i = 0; i < OrganMaxNum; i++)
		{
			//cout<<"OrganMatrix["<<i<<"] = "<<OrganMatrix[i]<<endl;
			if (OrganMatrix[i] > 0)
			{
				int pos_z = i / (OrganSize_x * OrganSize_y) + OrganStart_z + MergedOrgan_posdeta[id][3];
				int pos_y = (i % (OrganSize_x * OrganSize_y)) / OrganSize_x + OrganStart_y + MergedOrgan_posdeta[id][2];
				int pos_x = (i % (OrganSize_x * OrganSize_y)) % OrganSize_x + OrganStart_x + MergedOrgan_posdeta[id][1];

				short PreOrgan = New_matrix[pos_z * FileSize_y * FileSize_x + pos_y * FileSize_x + pos_x];
				if (PreOrgan > 0)
				{
					New_matrix[pos_z * FileSize_y * FileSize_x + pos_y * FileSize_x + pos_x] = OrganID;
				}
			}
		}
	}
	///////////////////////////////////////////////////////////
	for (unsigned long int i = 0; i < MaxNum; i++)
	{
		if (New_matrix[i] < 0)
		{
			cout << New_matrix[i] << endl;
		}
		else
		{
			VoxelN2[New_matrix[i]]++;
		}
	}

	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, MergedPhantomName.c_str(), "wb");
	::fwrite(New_matrix, sizeof(short), MaxNum, oFile);
	::fclose(oFile);

	ofstream outFile2(InfoFileName.c_str(), ios::out);
	for (int VN = 0; VN < 999; VN++)
	{
		if (VoxelN[VN] > 0 || VoxelN2[VN] > 0)
		{
			outFile2 << VN;
			outFile2 << ": ";
			outFile2 << VoxelN[VN];
			outFile2 << " ";
			outFile2 << VoxelN2[VN];
			outFile2 << "\n";
		}
	}
	outFile2.close();

	return 0;
}

int GenerateSkin(string BaseFilename, string AddSkinFilename, string InfoFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z, unsigned char Skin_ID)
{
	cout << "Generate Skin..." << endl;

	const unsigned long int voxel_MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* pMatrix = new unsigned char[voxel_MaxNum];
	unsigned char* voxel_matrix = new unsigned char[voxel_MaxNum];
	unsigned char* Skin_voxel_matrix = new unsigned char[voxel_MaxNum];

	int VoxelN[1000] = { 0 };
	int VoxelN2[1000] = { 0 };

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(pMatrix, sizeof(unsigned char), voxel_MaxNum, fp);

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (pMatrix[i] < 0 || pMatrix[i]>1000)
			pMatrix[i] = 0;

		voxel_matrix[i] = pMatrix[i];
		VoxelN[voxel_matrix[i]]++;
	}

	const int FrameElement_x = 3;
	const int FrameElement_y = 3;
	const int FrameElement_z = 3;
	int ElementOne = 0;
	int FrameElement[FrameElement_z][FrameElement_y][FrameElement_x] = { { { 0,0,0 },{ 0,0,0 },{ 0,0,0 } },
	{ { 0,1,0 },{ 1,1,1 },{ 0,1,0 } },
	{ { 0,0,0 },{ 0,0,0 },{ 0,0,0 } } };
	for (int a = 0; a < FrameElement_z; a++)
	{
		for (int b = 0; b < FrameElement_y; b++)
		{
			for (int c = 0; c < FrameElement_x; c++)
			{
				if (FrameElement[a][b][c] > 0)
					ElementOne++;
			}
		}
	}

	for (int z = 0; z < phantomSize_z; z++)
	{
		for (int y = 0; y < phantomSize_y; y++)
		{
			for (int x = 0; x < phantomSize_x; x++)
			{
				int key = 0;
				for (int k = -1; k < FrameElement_z - 1; k++)
				{
					for (int j = -1; j < FrameElement_y - 1; j++)
					{
						for (int i = -1; i < FrameElement_x - 1; i++)
						{
							long int pos = (z + k) * phantomSize_y * phantomSize_x + (y + j) * phantomSize_x + (x + i);
							if (pos >= 0 && pos < voxel_MaxNum)
							{
								if (voxel_matrix[pos] > 0 && FrameElement[k + 1][j + 1][i + 1] > 0)
									key++;
							}
						}
					}
				}
				if (key == ElementOne && voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] == BodyID)
				{
					Skin_voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] = Skin_ID;
				}
				else
				{
					Skin_voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] = 0;
				}
			}
		}
	}

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (Skin_voxel_matrix[i] == 0 && voxel_matrix[i] == BodyID)
		{
			voxel_matrix[i] = Skin_ID;
		}
	}

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (voxel_matrix[i] == Skin_ID)
		{
			pMatrix[i] = Skin_ID;
		}
		VoxelN2[pMatrix[i]]++;
	}

	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, AddSkinFilename.c_str(), "wb");
	::fwrite(pMatrix, sizeof(unsigned char), voxel_MaxNum, oFile);
	::fclose(oFile);

	ofstream outFile2(InfoFilename.c_str(), ios::out);
	for (int VN = 0; VN < 999; VN++)
	{
		if (VoxelN[VN] > 0 || VoxelN2[VN] > 0)
		{
			outFile2 << VN;
			outFile2 << ": ";
			outFile2 << VoxelN[VN];
			outFile2 << " ";
			outFile2 << VoxelN2[VN];
			outFile2 << "\n";
		}
	}
	outFile2.close();

	delete[] voxel_matrix;
	delete[] Skin_voxel_matrix;
	delete[] pMatrix;
	return 1;
}

int GenerateFetusMarrow(string BaseFilename, string AddSkinFilename, string InfoFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z, unsigned char Skeleton_ID, unsigned char Marrow_ID)
{
	cout << "Generate FetusMarrow..." << endl;

	const unsigned long int voxel_MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* pMatrix = new unsigned char[voxel_MaxNum];
	unsigned char* voxel_matrix = new unsigned char[voxel_MaxNum];
	unsigned char* Skin_voxel_matrix = new unsigned char[voxel_MaxNum];

	int VoxelN[1000] = { 0 };
	int VoxelN2[1000] = { 0 };

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(pMatrix, sizeof(unsigned char), voxel_MaxNum, fp);

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (pMatrix[i] < 0 || pMatrix[i]>1000)
			pMatrix[i] = 0;

		voxel_matrix[i] = pMatrix[i];
		VoxelN[voxel_matrix[i]]++;
	}

	const int FrameElement_x = 3;
	const int FrameElement_y = 3;
	const int FrameElement_z = 3;
	int ElementOne = 0;
	int FrameElement[FrameElement_z][FrameElement_y][FrameElement_x] = { { { 0,0,0 },{ 0,1,0 },{ 0,0,0 } },
	{ { 0,1,0 },{ 1,1,1 },{ 0,1,0 } },
	{ { 0,0,0 },{ 0,1,0 },{ 0,0,0 } } };
	for (int a = 0; a < FrameElement_z; a++)
	{
		for (int b = 0; b < FrameElement_y; b++)
		{
			for (int c = 0; c < FrameElement_x; c++)
			{
				if (FrameElement[a][b][c] > 0)
					ElementOne++;
			}
		}
	}

	for (int z = 0; z < phantomSize_z; z++)
	{
		for (int y = 0; y < phantomSize_y; y++)
		{
			for (int x = 0; x < phantomSize_x; x++)
			{
				int key = 0;
				for (int k = -1; k < FrameElement_z - 1; k++)
				{
					for (int j = -1; j < FrameElement_y - 1; j++)
					{
						for (int i = -1; i < FrameElement_x - 1; i++)
						{
							long int pos = (z + k) * phantomSize_y * phantomSize_x + (y + j) * phantomSize_x + (x + i);
							if (pos >= 0 && pos < voxel_MaxNum)
							{
								if (voxel_matrix[pos] == Skeleton_ID && FrameElement[k + 1][j + 1][i + 1] > 0)
									key++;
							}
						}
					}
				}
				if (key == ElementOne && voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] == Skeleton_ID)
				{
					Skin_voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] = Marrow_ID;
				}
				else
				{
					Skin_voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] = 0;
				}
			}
		}
	}

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (Skin_voxel_matrix[i] > 0 && voxel_matrix[i] == Skeleton_ID)
		{
			voxel_matrix[i] = Marrow_ID;
		}
	}

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (voxel_matrix[i] == Marrow_ID)
		{
			pMatrix[i] = Marrow_ID;
		}
		VoxelN2[pMatrix[i]]++;
	}

	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, AddSkinFilename.c_str(), "wb");
	::fwrite(pMatrix, sizeof(unsigned char), voxel_MaxNum, oFile);
	::fclose(oFile);

	ofstream outFile2(InfoFilename.c_str(), ios::out);
	for (int VN = 0; VN < 999; VN++)
	{
		if (VoxelN[VN] > 0 || VoxelN2[VN] > 0)
		{
			outFile2 << VN;
			outFile2 << ": ";
			outFile2 << VoxelN[VN];
			outFile2 << " ";
			outFile2 << VoxelN2[VN];
			outFile2 << "\n";
		}
	}
	outFile2.close();

	delete[] voxel_matrix;
	delete[] Skin_voxel_matrix;
	delete[] pMatrix;
	return 1;
}

int GenerateSkin_Expand(string BaseFilename, string AddSkinFilename, string InfoFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z, unsigned char Skin_ID)
{
	cout << "Generate Skin..." << endl;

	const unsigned long int voxel_MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* pMatrix = new unsigned char[voxel_MaxNum];
	unsigned char* voxel_matrix = new unsigned char[voxel_MaxNum];
	unsigned char* Skin_voxel_matrix = new unsigned char[voxel_MaxNum];

	int VoxelN[1000] = { 0 };
	int VoxelN2[1000] = { 0 };

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(pMatrix, sizeof(unsigned char), voxel_MaxNum, fp);

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (pMatrix[i] < 0 || pMatrix[i]>1000)
			pMatrix[i] = 0;

		voxel_matrix[i] = pMatrix[i];
		VoxelN[voxel_matrix[i]]++;
	}

	const int FrameElement_x = 3;
	const int FrameElement_y = 3;
	const int FrameElement_z = 3;
	int ElementOne = 0;
	int FrameElement[FrameElement_z][FrameElement_y][FrameElement_x] = { { { 0,0,0 },{ 0,1,0 },{ 0,0,0 } },
	{ { 0,1,0 },{ 1,1,1 },{ 0,1,0 } },
	{ { 0,0,0 },{ 0,1,0 },{ 0,0,0 } } };
	for (int a = 0; a < FrameElement_z; a++)
	{
		for (int b = 0; b < FrameElement_y; b++)
		{
			for (int c = 0; c < FrameElement_x; c++)
			{
				if (FrameElement[a][b][c] > 0)
					ElementOne++;
			}
		}
	}

	for (int z = 0; z < phantomSize_z; z++)
	{
		for (int y = 0; y < phantomSize_y; y++)
		{
			for (int x = 0; x < phantomSize_x; x++)
			{
				int key = 0;
				for (int k = -1; k < FrameElement_z - 1; k++)
				{
					for (int j = -1; j < FrameElement_y - 1; j++)
					{
						for (int i = -1; i < FrameElement_x - 1; i++)
						{
							long int pos = (z + k) * phantomSize_y * phantomSize_x + (y + j) * phantomSize_x + (x + i);
							if (pos >= 0 && pos < voxel_MaxNum)
							{
								if (voxel_matrix[pos] > 0 && FrameElement[k + 1][j + 1][i + 1] > 0)
									key++;
							}
						}
					}
				}
				if (key > 0)
				{
					Skin_voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] = Skin_ID;
				}
				else
				{
					Skin_voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] = 0;
				}
			}
		}
	}

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (Skin_voxel_matrix[i] > 0 && voxel_matrix[i] == 0)
		{
			voxel_matrix[i] = Skin_ID;
		}
	}

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (voxel_matrix[i] == Skin_ID)
		{
			pMatrix[i] = Skin_ID;
		}
		VoxelN2[pMatrix[i]]++;
	}

	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, AddSkinFilename.c_str(), "wb");
	::fwrite(pMatrix, sizeof(unsigned char), voxel_MaxNum, oFile);
	::fclose(oFile);

	ofstream outFile2(InfoFilename.c_str(), ios::out);
	for (int VN = 0; VN < 999; VN++)
	{
		if (VoxelN[VN] > 0 || VoxelN2[VN] > 0)
		{
			outFile2 << VN;
			outFile2 << ": ";
			outFile2 << VoxelN[VN];
			outFile2 << " ";
			outFile2 << VoxelN2[VN];
			outFile2 << "\n";
		}
	}
	outFile2.close();

	delete[] voxel_matrix;
	delete[] Skin_voxel_matrix;
	delete[] pMatrix;
	return 1;
}

int GenerateBlankBaseFile(string BaseFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z)
{
	const unsigned long int MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* New_matrix = new unsigned char[MaxNum];
	for (int i = 0; i < MaxNum; i++)
	{
		New_matrix[i] = 0;
	}
	cout << "Writing..." << BaseFilename.c_str() << endl;
	FILE* oFile;
	fopen_s(&oFile, BaseFilename.c_str(), "wb");
	fwrite(New_matrix, sizeof(unsigned char), MaxNum, oFile);
	fclose(oFile);
	return 1;
}
//keep body and Skin
int GenerateBodyBaseFile2(string BaseFilename, string BodyBaseFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z)
{
	const unsigned long int MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* New_matrix = new unsigned char[MaxNum];

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(New_matrix, sizeof(unsigned char), MaxNum, fp);

	for (int i = 0; i < MaxNum; i++)
	{
		if (New_matrix[i] > 11)
			New_matrix[i] = 10;
	}
	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, BodyBaseFilename.c_str(), "wb");
	::fwrite(New_matrix, sizeof(unsigned char), MaxNum, oFile);
	::fclose(oFile);
	return 1;
}

int GenerateBodyFile(string BaseFilename, string BodyFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z)
{
	const unsigned long int MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* New_matrix = new unsigned char[MaxNum];

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(New_matrix, sizeof(unsigned char), MaxNum, fp);

	for (int i = 0; i < MaxNum; i++)
	{
		if (New_matrix[i] > 0)
			New_matrix[i] = 250;
	}
	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, BodyFilename.c_str(), "wb");
	::fwrite(New_matrix, sizeof(unsigned char), MaxNum, oFile);
	::fclose(oFile);
	return 1;
}

int GenerateFetusBodyFile(string BaseFilename, string BodyFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z)
{
	const unsigned long int MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* New_matrix = new unsigned char[MaxNum];

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(New_matrix, sizeof(unsigned char), MaxNum, fp);

	for (int i = 0; i < MaxNum; i++)
	{
		if (New_matrix[i] > 109 && New_matrix[i] < 150)
			New_matrix[i] = 250;
	}
	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, BodyFilename.c_str(), "wb");
	::fwrite(New_matrix, sizeof(unsigned char), MaxNum, oFile);
	::fclose(oFile);
	return 1;
}
int GenerateTwinsBodyFile(string BaseFilename, string BodyFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z)
{
	const unsigned long int MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* New_matrix = new unsigned char[MaxNum];

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(New_matrix, sizeof(unsigned char), MaxNum, fp);

	for (int i = 0; i < MaxNum; i++)
	{
		if (New_matrix[i] > 209 && New_matrix[i] < 250)
			New_matrix[i] = 250;
	}
	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, BodyFilename.c_str(), "wb");
	::fwrite(New_matrix, sizeof(unsigned char), MaxNum, oFile);
	::fclose(oFile);
	return 1;
}


int GenerateFetusSkin_Expand(string BaseFilename, string AddSkinFilename, string InfoFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z, unsigned char FetusSkin_ID)
{
	cout << "Generate Skin..." << endl;

	const unsigned long int voxel_MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* pMatrix = new unsigned char[voxel_MaxNum];
	unsigned char* voxel_matrix = new unsigned char[voxel_MaxNum];
	unsigned char* Skin_voxel_matrix = new unsigned char[voxel_MaxNum];

	int VoxelN[1000] = { 0 };
	int VoxelN2[1000] = { 0 };

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(pMatrix, sizeof(unsigned char), voxel_MaxNum, fp);

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (pMatrix[i] < 0 || pMatrix[i]>1000)
			pMatrix[i] = 0;

		voxel_matrix[i] = pMatrix[i];
		VoxelN[voxel_matrix[i]]++;
	}

	const int FrameElement_x = 3;
	const int FrameElement_y = 3;
	const int FrameElement_z = 3;
	int ElementOne = 0;
	int FrameElement[FrameElement_z][FrameElement_y][FrameElement_x] = { { { 0,0,0 },{ 0,0,0 },{ 0,0,0 } },
	{ { 0,1,0 },{ 1,1,1 },{ 0,1,0 } },
	{ { 0,0,0 },{ 0,0,0 },{ 0,0,0 } } };
	for (int a = 0; a < FrameElement_z; a++)
	{
		for (int b = 0; b < FrameElement_y; b++)
		{
			for (int c = 0; c < FrameElement_x; c++)
			{
				if (FrameElement[a][b][c] > 0)
					ElementOne++;
			}
		}
	}

	for (int z = 0; z < phantomSize_z; z++)
	{
		for (int y = 0; y < phantomSize_y; y++)
		{
			for (int x = 0; x < phantomSize_x; x++)
			{
				int key = 0;
				for (int k = -1; k < FrameElement_z - 1; k++)
				{
					for (int j = -1; j < FrameElement_y - 1; j++)
					{
						for (int i = -1; i < FrameElement_x - 1; i++)
						{
							long int pos = (z + k) * phantomSize_y * phantomSize_x + (y + j) * phantomSize_x + (x + i);
							if (pos >= 0 && pos < voxel_MaxNum)
							{
								if (voxel_matrix[pos] > 109 && voxel_matrix[pos] < 150 && FrameElement[k + 1][j + 1][i + 1] > 0)
									key++;
							}
						}
					}
				}
				if (key > 0)
				{
					Skin_voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] = FetusSkin_ID;
				}
				else
				{
					Skin_voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] = 0;
				}
			}
		}
	}

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (Skin_voxel_matrix[i] > 0 && voxel_matrix[i] < 109)
		{
			voxel_matrix[i] = FetusSkin_ID;
		}
	}

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (voxel_matrix[i] == FetusSkin_ID)
		{
			pMatrix[i] = FetusSkin_ID;
		}
		VoxelN2[pMatrix[i]]++;
	}

	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, AddSkinFilename.c_str(), "wb");
	::fwrite(pMatrix, sizeof(unsigned char), voxel_MaxNum, oFile);
	::fclose(oFile);

	ofstream outFile2(InfoFilename.c_str(), ios::out);
	for (int VN = 0; VN < 999; VN++)
	{
		if (VoxelN[VN] > 0 || VoxelN2[VN] > 0)
		{
			outFile2 << VN;
			outFile2 << ": ";
			outFile2 << VoxelN[VN];
			outFile2 << " ";
			outFile2 << VoxelN2[VN];
			outFile2 << "\n";
		}
	}
	outFile2.close();

	delete[] voxel_matrix;
	delete[] Skin_voxel_matrix;
	delete[] pMatrix;
	return 1;
}


int GenerateTwinsSkin_Expand(string BaseFilename, string AddSkinFilename, string InfoFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z, unsigned char TwinsSkin_ID)
{
	cout << "Generate TwinsSkin..." << endl;

	const unsigned long int voxel_MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* pMatrix = new unsigned char[voxel_MaxNum];
	unsigned char* voxel_matrix = new unsigned char[voxel_MaxNum];
	unsigned char* Skin_voxel_matrix = new unsigned char[voxel_MaxNum];

	int VoxelN[1000] = { 0 };
	int VoxelN2[1000] = { 0 };

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(pMatrix, sizeof(unsigned char), voxel_MaxNum, fp);

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (pMatrix[i] < 0 || pMatrix[i]>1000)
			pMatrix[i] = 0;

		voxel_matrix[i] = pMatrix[i];
		VoxelN[voxel_matrix[i]]++;
	}

	const int FrameElement_x = 3;
	const int FrameElement_y = 3;
	const int FrameElement_z = 3;
	int ElementOne = 0;
	int FrameElement[FrameElement_z][FrameElement_y][FrameElement_x] = { { { 0,0,0 },{ 0,0,0 },{ 0,0,0 } },
	{ { 0,1,0 },{ 1,1,1 },{ 0,1,0 } },
	{ { 0,0,0 },{ 0,0,0 },{ 0,0,0 } } };
	for (int a = 0; a < FrameElement_z; a++)
	{
		for (int b = 0; b < FrameElement_y; b++)
		{
			for (int c = 0; c < FrameElement_x; c++)
			{
				if (FrameElement[a][b][c] > 0)
					ElementOne++;
			}
		}
	}

	for (int z = 0; z < phantomSize_z; z++)
	{
		for (int y = 0; y < phantomSize_y; y++)
		{
			for (int x = 0; x < phantomSize_x; x++)
			{
				int key = 0;
				for (int k = -1; k < FrameElement_z - 1; k++)
				{
					for (int j = -1; j < FrameElement_y - 1; j++)
					{
						for (int i = -1; i < FrameElement_x - 1; i++)
						{
							long int pos = (z + k) * phantomSize_y * phantomSize_x + (y + j) * phantomSize_x + (x + i);
							if (pos >= 0 && pos < voxel_MaxNum)
							{
								if (voxel_matrix[pos] > 209 && voxel_matrix[pos] < 250 && FrameElement[k + 1][j + 1][i + 1] > 0)
									key++;
							}
						}
					}
				}
				if (key > 0)
				{
					Skin_voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] = TwinsSkin_ID;
				}
				else
				{
					Skin_voxel_matrix[z * phantomSize_y * phantomSize_x + y * phantomSize_x + x] = 0;
				}
			}
		}
	}

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (Skin_voxel_matrix[i] > 0 && voxel_matrix[i] < 109)
		{
			voxel_matrix[i] = TwinsSkin_ID;
		}
	}

	for (int i = 0; i < voxel_MaxNum; i++)
	{
		if (voxel_matrix[i] == TwinsSkin_ID)
		{
			pMatrix[i] = TwinsSkin_ID;
		}
		VoxelN2[pMatrix[i]]++;
	}

	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, AddSkinFilename.c_str(), "wb");
	::fwrite(pMatrix, sizeof(unsigned char), voxel_MaxNum, oFile);
	::fclose(oFile);

	ofstream outFile2(InfoFilename.c_str(), ios::out);
	for (int VN = 0; VN < 999; VN++)
	{
		if (VoxelN[VN] > 0 || VoxelN2[VN] > 0)
		{
			outFile2 << VN;
			outFile2 << ": ";
			outFile2 << VoxelN[VN];
			outFile2 << " ";
			outFile2 << VoxelN2[VN];
			outFile2 << "\n";
		}
	}
	outFile2.close();

	delete[] voxel_matrix;
	delete[] Skin_voxel_matrix;
	delete[] pMatrix;
	return 1;
}

int GenerateFetusSkeletonFile(string BaseFilename, string SkeletonFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z)
{
	const unsigned long int MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* New_matrix = new unsigned char[MaxNum];

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(New_matrix, sizeof(unsigned char), MaxNum, fp);

	for (int i = 0; i < MaxNum; i++)
	{
		if (New_matrix[i] > 109 && New_matrix[i] < 150)
			New_matrix[i] = 250;
	}
	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, SkeletonFilename.c_str(), "wb");
	::fwrite(New_matrix, sizeof(unsigned char), MaxNum, oFile);
	::fclose(oFile);
	return 1;
}
int GenerateTwinsSkeletonFile(string BaseFilename, string SkeletonFilename, int phantomSize_x, int phantomSize_y, int phantomSize_z)
{
	const unsigned long int MaxNum = phantomSize_x * phantomSize_y * phantomSize_z;
	unsigned char* New_matrix = new unsigned char[MaxNum];

	FILE* fp;
	fopen_s(&fp, BaseFilename.c_str(), "rb");
	cout << BaseFilename.c_str() << endl;
	fread(New_matrix, sizeof(unsigned char), MaxNum, fp);

	for (int i = 0; i < MaxNum; i++)
	{
		if (New_matrix[i] > 209 && New_matrix[i] < 250)
			New_matrix[i] = 250;
	}
	cout << "Writing..." << endl;
	FILE* oFile;
	fopen_s(&oFile, SkeletonFilename.c_str(), "wb");
	::fwrite(New_matrix, sizeof(unsigned char), MaxNum, oFile);
	::fclose(oFile);
	return 1;
}

int main()
{
	//635 308 1749
	const int FileSize_x = 131;
	const int FileSize_y = 151;
	const int FileSize_z = 456;
	string FileSize = "131_151_256_";


	string BaseFilename = path;
	BaseFilename.append("Fetus_");
	BaseFilename.append(FileSize);
	BaseFilename.append(".dat");
	string MergedPhantomName = path;
	MergedPhantomName.append("Fetus_");
	MergedPhantomName.append(FileSize);
	MergedPhantomName.append("_Merged.dat");
	string SkinedPhantomName = path;
	SkinedPhantomName.append("Fetus_");
	SkinedPhantomName.append(FileSize);
	SkinedPhantomName.append("_WithSkin2.dat");
	string RevisedFetusPhantomName1 = path;
	RevisedFetusPhantomName1.append("Fetus_");
	RevisedFetusPhantomName1.append(FileSize);
	RevisedFetusPhantomName1.append("_WithFetusSkin.dat");
	string RevisedFetusPhantomName2 = path;
	RevisedFetusPhantomName2.append("Fetus_");
	RevisedFetusPhantomName2.append(FileSize);
	RevisedFetusPhantomName2.append("_WithFetusMarrow.dat");

	string InfoFileName1 = path;
	InfoFileName1.append("VoxelInfo_Fetus_");
	InfoFileName1.append(FileSize);
	InfoFileName1.append("_Merged.txt");
	string InfoFileName2 = path;
	InfoFileName2.append("VoxelInfo_Fetus_");
	InfoFileName2.append(FileSize);
	InfoFileName2.append("_WithSkin2.txt");
	string InfoFileName3 = path;
	InfoFileName3.append("VoxelInfo_Fetus_");
	InfoFileName3.append(FileSize);
	InfoFileName3.append("_WithFetusSkin.txt");
	string InfoFileName4 = path;
	InfoFileName4.append("VoxelInfo_Fetus_");
	InfoFileName4.append(FileSize);
	InfoFileName4.append("_WithFetusMarrow.txt");


	GenerateBlankBaseFile(BaseFilename, FileSize_x, FileSize_y, FileSize_z);
	MergeOrgans(BaseFilename, MergedPhantomName, InfoFileName1, FileSize_x, FileSize_y, FileSize_z);
	GenerateSkin(MergedPhantomName, SkinedPhantomName, InfoFileName2, FileSize_x, FileSize_y, FileSize_z, 11);

	//	GenerateFetusSkin_Expand(SkinedPhantomName, RevisedFetusPhantomName1, InfoFileName3, FileSize_x, FileSize_y, FileSize_z, 128);
	//	GenerateFetusMarrow(RevisedFetusPhantomName1, RevisedFetusPhantomName2, InfoFileName4, FileSize_x, FileSize_y, FileSize_z, 111, 112);

	/*	string RevisedFetusPhantomName3 = path;
		RevisedFetusPhantomName3.append("Fetus_409_321_494_WithTwinsSkin.dat");
		string RevisedFetusPhantomName4 = path;
		RevisedFetusPhantomName4.append("Fetus_409_321_494_WithTwinsMarrow.dat");
		string InfoFileName5 = path;
		InfoFileName5.append("VoxelInfo_Fetus_409_321_494_WithTwinsSkin.txt");
		string InfoFileName6 = path;
		InfoFileName6.append("VoxelInfo_Fetus_409_321_494_WithTwinsMarrow.txt");

		GenerateTwinsSkin_Expand(RevisedFetusPhantomName2, RevisedFetusPhantomName3, InfoFileName5, FileSize_x, FileSize_y, FileSize_z, 228);
		GenerateFetusMarrow(RevisedFetusPhantomName3, RevisedFetusPhantomName4, InfoFileName6, FileSize_x, FileSize_y, FileSize_z, 211, 212);
	*/

	/*	string ExistingBaseFilename = path;
		ExistingBaseFilename.append("Monkey_94_57_369_WithSkin3.dat");
		string BodyBasePhantomName = path;
		BodyBasePhantomName.append("Monkey_94_57_369_BodyBase.dat");
		string UpdatedPhantomName = path;
		UpdatedPhantomName.append("Monkey_94_57_369_Updated_0924.dat");
		string InfoFileName3 = path;
		InfoFileName3.append("VoxelInfo_Monkey_94_57_369_Updated_0924.txt");
		GenerateBodyBaseFile2(ExistingBaseFilename, BodyBasePhantomName, FileSize_x, FileSize_y, FileSize_z);
		MergeOrgans(BodyBasePhantomName, UpdatedPhantomName, InfoFileName3, FileSize_x, FileSize_y, FileSize_z);
	*/
}