// #include "stdafx.h" 
#include "afxres.h" //replace "stdafx.h" when compiling in VScode
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
using namespace std;
#pragma comment(lib,"psapi.lib")

string path = "D:\\Fudan\\Mass\\";


int main()
{
	float Aim_weight = 3.21;
	// float Aim_Height = 0.692344;

	const int FileSize_x = 218;
	const int FileSize_y = 144;
	const int FileSize_z = 534;

	string filename = "Fetus_218_144_534_Merged.dat";

	float Resolution_x = 0.2; //cm
	float Resolution_y = 0.2; //cm
	float Resolution_z = 0.2; //cm

	//****************************************************
	int Befor_Body_top_z = 0;
	int Befor_Body_foot_z = FileSize_z;
	// double Befor_Height = 0;
	double Befor_Weight = 0;
	//****************************************************
	const int OrganNum = 35;
	const int OrganFlag[OrganNum] = {10,15,16,18,19,21,22,23,24,25,26,27,28,30,31,32,33,34,35,36,38,39,43,44,45,46,47,65,66,67,68,69,70,80,85,};
	double Density[OrganNum] = {1.0,1.06,1.01,1.05,0.98,1.05,1.05,1.05,1.02,1.09,1.04,1.06,1.03,1.02,1.05,1.05,0.41,1.04,1.04,1.09,1.05,1.05,1.05,1.03,1.07,1.91,1.4,1.05,1.04,1.05,1.03,1.03,1.01,1.02,1.05,};

    // int length = sizeof(OrganFlag) / sizeof(int);
    // cout << "OrganFlag的长度为：" << length << endl;


	double VoxelSize = Resolution_x*Resolution_y*Resolution_z;
	const unsigned long int MaxNum = FileSize_x*FileSize_y*FileSize_z;

	int VoxelN[1000] = { 0 };
	int VoxelN2[1000] = { 0 };
	int VoxelN3[1000] = { 0 };
	int VoxelN4[1000] = { 0 };

	FILE *fp;
	string readFile = path;
	readFile.append(filename);
	fp = fopen(readFile.c_str(), "rb");
	cout << readFile.c_str() << endl;

	unsigned long int VNum = FileSize_x*FileSize_y*FileSize_z;
	unsigned long int VFlag = 0;

	unsigned char* a = new unsigned char[MaxNum];
	unsigned char* matrix = new unsigned char[MaxNum];
	fread(a, sizeof(unsigned char), MaxNum, fp);
	fclose(fp);

	int Na = 0;

	for (unsigned long int i = 0; i<MaxNum; i++)
	{
		Na = 0;
		if (a[i]>0 && i<MaxNum)
		{
			Na = int(a[i]);
			if (Na == 99)
			{
				Na = 0;
			}
			matrix[i] = Na;
			VoxelN[Na]++;
		}
		else
		{
			matrix[i] = Na;
			VoxelN[Na]++;
		}
	}

//////////////////////////////////////////////////////
//height
	// int OrganID = 0;
	// for (int k = 0; k<FileSize_z; k++)
	// {
	// 	for (int j = 0; j<FileSize_y; j++)
	// 	{
	// 		for (int i = 0; i<FileSize_x; i++)
	// 		{
	// 			OrganID = matrix[k*FileSize_y*FileSize_x + j*FileSize_x + i];
	// 			if (OrganID>0)
	// 			{
	// 				if (k>Befor_Body_top_z)
	// 					Befor_Body_top_z = k;
	// 				if (k<Befor_Body_foot_z)
	// 					Befor_Body_foot_z = k;
	// 			}
	// 		}
	// 	}
	// }

	// Befor_Height = (Befor_Body_top_z - Befor_Body_foot_z)*Resolution_z / 100;

	double Mass1 = 0;
	for (int VN = 0; VN<OrganNum; VN++)
	{
		if (VoxelN[VN] > 0)
		{
			int pd = -1;
			for (int p = 0; p < OrganNum; p++)
			{
				if (OrganFlag[p] == VN)
				{
					pd = p;
				}
			}
			if (pd >= 0)
			{			
				Mass1 += VoxelN[VN] * Resolution_x*Resolution_y*Resolution_z*Density[pd] / 1000;
			}
		}
	}
	Befor_Weight = Mass1;

	//********************************************************************
	//	                        Scale phantom - Height                   *
	//********************************************************************
	// cout << "Scale phantom - Height" << endl;
	// cout << "Aim_Height: " << Aim_Height << endl;
	// cout << "Befor_Height: " << Befor_Height << endl;

	// float Scale_x = 1.0;
	// float Scale_y = 1.0;
	// float Scale_z = Aim_Height / Befor_Height;

	// int New_FileSize_x = int(FileSize_x*Scale_x);
	// int New_FileSize_y = int(FileSize_y*Scale_y);
	// int New_FileSize_z = int(FileSize_z*Scale_z);

	// if (((float)FileSize_x)*Scale_x>New_FileSize_x)
	// 	New_FileSize_x += 1;
	// if (((float)FileSize_y)*Scale_y>New_FileSize_y)
	// 	New_FileSize_y += 1;
	// if (((float)FileSize_z)*Scale_z>New_FileSize_z)
	// 	New_FileSize_z += 1;

	// const unsigned long int NewMaxNum = New_FileSize_x*New_FileSize_y*New_FileSize_z;
	// const unsigned long int Before_MaxNum = FileSize_x*FileSize_y*FileSize_z;
	// unsigned char* NewMatrix = new unsigned char[NewMaxNum];
	// unsigned char* Before_Matrix = new unsigned char[Before_MaxNum];

	// for (int i = 0; i<MaxNum; i++)
	// {
	// 	if (i<Before_MaxNum)
	// 	{
	// 		Before_Matrix[i] = matrix[i];
	// 	}
	// }

	// float O_i = 0;
	// float O_j = 0;
	// float O_k = 0;
	// int IO_i = 0;
	// int IO_j = 0;
	// int IO_k = 0;
	// int Flag = 0;

	// cout << "Scale phantom - Leg_Length" << endl;
	// for (int k = 0; k<New_FileSize_z; k++)
	// {
	// 	for (int j = 0; j<New_FileSize_y; j++)
	// 	{
	// 		for (int i = 0; i<New_FileSize_x; i++)
	// 		{
	// 			O_i = ((float)i) / Scale_x;
	// 			O_j = ((float)j) / Scale_y;
	// 			O_k = ((float)k) / Scale_z;

	// 			if (O_i>FileSize_x - 1)
	// 				O_i = FileSize_x - 1;
	// 			if (O_i<0)
	// 				O_i = 0;
	// 			if (O_j>FileSize_y - 1)
	// 				O_j = FileSize_y - 1;
	// 			if (O_j<0)
	// 				O_j = 0;
	// 			if (O_k>FileSize_z - 1)
	// 				O_k = FileSize_z - 1;
	// 			if (O_k<0)
	// 				O_k = 0;

	// 			//				cout<<O_i<<" "<<O_j<<" "<<O_k<<" "<<endl;

	// 			int O_i_1 = (int)O_i;
	// 			float O_i_f = O_i - O_i_1;
	// 			if (O_i_f <= (1 - O_i_f))
	// 				IO_i = (int)O_i;
	// 			else
	// 				IO_i = (int)O_i + 1;

	// 			int O_j_1 = (int)O_j;
	// 			float O_j_f = O_j - O_j_1;
	// 			if (O_j_f <= (1 - O_j_f))
	// 				IO_j = (int)O_j;
	// 			else
	// 				IO_j = (int)O_j + 1;

	// 			int O_k_1 = (int)O_k;
	// 			float O_k_f = O_k - O_k_1;
	// 			if (O_k_f <= (1 - O_k_f))
	// 				IO_k = (int)O_k;
	// 			else
	// 				IO_k = (int)O_k + 1;

	// 			if ((IO_k)*FileSize_y*FileSize_x + (IO_j)*FileSize_x + (IO_i)<Before_MaxNum)
	// 			{
	// 				int get_ID = Before_Matrix[(IO_k)*FileSize_y*FileSize_x + (IO_j)*FileSize_x + (IO_i)];
	// 				NewMatrix[k*New_FileSize_y*New_FileSize_x + j*New_FileSize_x + i] = get_ID;
	// 			}
	// 		}
	// 	}
	// }

	// cout << "Scale phantom - Sitting_Height" << endl;
	// cout << "Scale_x: " << Scale_x << endl;
	// cout << "Scale_y: " << Scale_y << endl;
	// cout << "Scale_z: " << Scale_z << endl;

	// cout << "New_FileSize_x: " << New_FileSize_x << endl;
	// cout << "New_FileSize_y: " << New_FileSize_y << endl;
	// cout << "New_FileSize_z: " << New_FileSize_z << endl;

	//***************************************************************************************
	//	                             Check new Weight&Height
	//***************************************************************************************
	cout << "Check new Weight&Height" << endl;


	// int After_Body_top_z = 0;
	// int After_Body_foot_z = New_FileSize_z;
	// double After_Height = 0;
	// double After_Weight = 0;
	// double After_BMI = 0;

	// OrganID = 0;
	// for (int k = 0; k<New_FileSize_z; k++)
	// {
	// 	for (int j = 0; j<New_FileSize_y; j++)
	// 	{
	// 		for (int i = 0; i<New_FileSize_x; i++)
	// 		{
	// 			OrganID = NewMatrix[k*New_FileSize_y*New_FileSize_x + j*New_FileSize_x + i];
	// 			if (OrganID>0)
	// 			{
	// 				if (k>After_Body_top_z)
	// 					After_Body_top_z = k;
	// 				if (k<After_Body_foot_z)
	// 					After_Body_foot_z = k;
	// 			}
	// 			VoxelN2[OrganID]++;
	// 		}
	// 	}
	// }
	// After_Height = (After_Body_top_z - After_Body_foot_z)*Resolution_z / 100;

	// double Mass2 = 0;
	// for (int VN = 0; VN<OrganNum; VN++)
	// {
	// 	if (VoxelN2[VN] > 0)
	// 	{
	// 		int pd = -1;
	// 		for (int p = 0; p < OrganNum; p++)
	// 		{
	// 			if (OrganFlag[p] == VN)
	// 			{
	// 				pd = p;
	// 			}
	// 		}
	// 		if (pd >= 0)
	// 		{
	// 			Mass2 += VoxelN2[VN] * Resolution_x*Resolution_y*Resolution_z*Density[pd] / 1000;
	// 		}
	// 	}
	// }
	// After_Weight = Mass2;
	// After_BMI = After_Weight / (After_Height*After_Height);

	//********************************************************************
	//	                        Scale phantom - Weight                   *
	//********************************************************************
	cout << "Scale phantom - Weight" << endl;

////////////////////////////////////////////////////
//weight only
    double After_Weight = 0;
    After_Weight = Befor_Weight;
	cout <<"afterWeight:" << After_Weight << endl;

    int New_FileSize_x = FileSize_x;
    int New_FileSize_y = FileSize_y;
    int New_FileSize_z = FileSize_z;
	


	// if (((float)FileSize_x)*Scale_x>New_FileSize_x)
	// 	New_FileSize_x += 1;
	// if (((float)FileSize_y)*Scale_y>New_FileSize_y)
	// 	New_FileSize_y += 1;
	// if (((float)FileSize_z)*Scale_z>New_FileSize_z)
	// 	New_FileSize_z += 1;

    // const unsigned long int NewMaxNum = New_FileSize_x*New_FileSize_y*New_FileSize_z;
    // unsigned char* NewMatrix = new unsigned char[NewMaxNum];

//////////////////////////////////////////////////

	float Weight_Scale_x = sqrt(Aim_weight / After_Weight);
	float Weight_Scale_y = sqrt(Aim_weight / After_Weight);
	float Weight_Scale_z = 1.0;
	// cout << "weight_scale_x: "<<Weight_Scale_x << endl;

	int Weight_New_FileSize_x = int(New_FileSize_x*Weight_Scale_x);
	int Weight_New_FileSize_y = int(New_FileSize_y*Weight_Scale_y);
	int Weight_New_FileSize_z = int(New_FileSize_z*Weight_Scale_z);
	// cout << "weight_newFileSize_z: "<<Weight_New_FileSize_x << endl;

	if (((float)New_FileSize_x)*Weight_Scale_x>Weight_New_FileSize_x)
		Weight_New_FileSize_x += 1;
	if (((float)New_FileSize_y)*Weight_Scale_y>Weight_New_FileSize_y)
		Weight_New_FileSize_y += 1;
	if (((float)New_FileSize_z)*Weight_Scale_z>Weight_New_FileSize_z)
		Weight_New_FileSize_z += 1;

	const unsigned long int Weight_NewMaxNum = Weight_New_FileSize_x*Weight_New_FileSize_y*Weight_New_FileSize_z;
	unsigned char* Weight_NewMatrix = new unsigned char[Weight_NewMaxNum];

	float O_i = 0;
	float O_j = 0;
	float O_k = 0;
	int IO_i = 0;
	int IO_j = 0;
	int IO_k = 0;
	int Flag = 0;

 
	//	cout<<"test2"<<endl;

	for (int k = 0; k<Weight_New_FileSize_z; k++)
	{
		for (int j = 0; j<Weight_New_FileSize_y; j++)
		{
			for (int i = 0; i<Weight_New_FileSize_x; i++)
			{
				O_i = ((float)i) / Weight_Scale_x;
				O_j = ((float)j) / Weight_Scale_y;
				O_k = ((float)k) / Weight_Scale_z;

				if (O_i>New_FileSize_x - 1)
					O_i = New_FileSize_x - 1;
				if (O_i<0)
					O_i = 0;
				if (O_j>New_FileSize_y - 1)
					O_j = New_FileSize_y - 1;
				if (O_j<0)
					O_j = 0;
				if (O_k>New_FileSize_z - 1)
					O_k = New_FileSize_z - 1;
				if (O_k<0)
					O_k = 0;

				//				cout<<O_i<<" "<<O_j<<" "<<O_k<<" "<<endl;

				int O_i_1 = (int)O_i;
				float O_i_f = O_i - O_i_1;
				if (O_i_f <= (1 - O_i_f))
					IO_i = (int)O_i;
				else
					IO_i = (int)O_i + 1;

				int O_j_1 = (int)O_j;
				float O_j_f = O_j - O_j_1;
				if (O_j_f <= (1 - O_j_f))
					IO_j = (int)O_j;
				else
					IO_j = (int)O_j + 1;

				int O_k_1 = (int)O_k;
				float O_k_f = O_k - O_k_1;
				if (O_k_f <= (1 - O_k_f))
					IO_k = (int)O_k;
				else
					IO_k = (int)O_k + 1;

				if ((IO_k)*New_FileSize_y*New_FileSize_x + (IO_j)*New_FileSize_x + (IO_i)<VNum)
				{
					int get_ID = matrix[(IO_k)*New_FileSize_y*New_FileSize_x + (IO_j)*New_FileSize_x + (IO_i)];
					Weight_NewMatrix[k*Weight_New_FileSize_y*Weight_New_FileSize_x + j*Weight_New_FileSize_x + i] = get_ID;
				}
			}
		}
	}

	//***************************************************************************************
	//	                             Check new Weight&Height
	//***************************************************************************************
	cout << "Check new Weight&Height" << endl;

	int Weight_After_Body_top_z = 0;
	int Weight_After_Body_foot_z = Weight_New_FileSize_z;
	double Weight_After_Height = 0;
	double Weight_After_Weight = 0;
	double Weight_After_BMI = 0;

	int OrganID = 0;
	for (int k = 0; k<Weight_New_FileSize_z; k++)
	{
		for (int j = 0; j<Weight_New_FileSize_y; j++)
		{
			for (int i = 0; i<Weight_New_FileSize_x; i++)
			{
				OrganID = Weight_NewMatrix[k*Weight_New_FileSize_y*Weight_New_FileSize_x + j*Weight_New_FileSize_x + i];
				if (OrganID>0)
				{
					if (k>Weight_After_Body_top_z)
						Weight_After_Body_top_z = k;
					if (k<Weight_After_Body_foot_z)
						Weight_After_Body_foot_z = k;
				}
				VoxelN3[OrganID]++;
			}
		}
	}
	Weight_After_Height = (Weight_After_Body_top_z - Weight_After_Body_foot_z)*Resolution_z / 100;
	cout << "VoxelN3: " <<VoxelN3<< endl;

	double Mass3 = 0;
	for (int VN = 0; VN<OrganNum; VN++)
	{
		if (VoxelN3[VN] > 0)
		{
			int pd = -1;
			for (int p = 0; p < OrganNum; p++)
			{
				if (OrganFlag[p] == VN)
				{
					pd = p;
				}
			}
			if (pd >= 0)
			{
				Mass3 += VoxelN3[VN] * Resolution_x*Resolution_y*Resolution_z*Density[pd] / 1000;
			}
		}
	}


	cout << "Mass3: " <<Mass3<< endl;
	Weight_After_Weight = Mass3;

	Weight_After_BMI = Weight_After_Weight / (Weight_After_Height*Weight_After_Height);

	//********************************************************************
	//                           Writting
	//********************************************************************

	string Newsize;
	char* pNum = new char[3];
	itoa(New_FileSize_x, pNum, 10);
	Newsize.append("NewSize(");
	Newsize.append(pNum);
	itoa(New_FileSize_y, pNum, 10);
	Newsize.append("_");
	Newsize.append(pNum);
	itoa(New_FileSize_z, pNum, 10);
	Newsize.append("_");
	Newsize.append(pNum);
	Newsize.append(")");

	cout << "Writing1..." << endl;
	FILE* oFile;
	string phantomName = path;
	phantomName.append(Newsize);
	phantomName.append("_");
	phantomName.append(filename);
	oFile = fopen(phantomName.c_str(), "wb");
	fwrite(matrix, sizeof(unsigned char), VNum, oFile);
	fclose(oFile);
	//********************************

	string Final_Newsize;
	char* pNum2 = new char[3];
	itoa(Weight_New_FileSize_x, pNum2, 10);
	Final_Newsize.append("FinalSize(");
	Final_Newsize.append(pNum2);
	itoa(Weight_New_FileSize_y, pNum2, 10);
	Final_Newsize.append("_");
	Final_Newsize.append(pNum2);
	itoa(Weight_New_FileSize_z, pNum2, 10);
	Final_Newsize.append("_");
	Final_Newsize.append(pNum2);
	Final_Newsize.append(")");

	cout << "Writing2..." << endl;
	FILE* oFile2;
	string phantomName2 = path;
	phantomName2.append(Final_Newsize);
	phantomName2.append("_");
	phantomName2.append(filename);
	oFile2 = fopen(phantomName2.c_str(), "wb");
	fwrite(Weight_NewMatrix, sizeof(unsigned char), Weight_NewMaxNum, oFile2);
	fclose(oFile2);
	//********************************
	cout << "Writing3..." << endl;
	string InfoFileName = path;
	InfoFileName.append("VoxelInfo2_");
	InfoFileName.append(Final_Newsize);
	InfoFileName.append("_");
	InfoFileName.append(filename);
	InfoFileName.append(".txt");
	ofstream outFile3(InfoFileName.c_str(), ios::out);
	outFile3 << "NewSize = " << New_FileSize_x << " * " << New_FileSize_y << " * " << New_FileSize_z << endl;
	outFile3 << "Final_NewSize = " << Weight_New_FileSize_x << " * " << Weight_New_FileSize_y << " * " << Weight_New_FileSize_z << endl;
	for (int VN = 0; VN<999; VN++)
	{
		if (VoxelN[VN]>0 || VoxelN2[VN]>0 || VoxelN3[VN]>0 || VoxelN4[VN]>0)
		{
			outFile3 << VN;
			outFile3 << ": ";
			outFile3 << VoxelN[VN];
			outFile3 << " ";
			outFile3 << VoxelN2[VN];
			outFile3 << " ";
			outFile3 << VoxelN3[VN];
			outFile3 << "  ";
			outFile3 << 100 * (((float)VoxelN3[VN]) / ((float)VoxelN[VN]) - 1);
			outFile3 << "%\n";
		}
	}
	outFile3 << "***********************************\n";
	double Mass11 = 0;
	double Mass22 = 0;
	double Mass33 = 0;
	for (int VN2 = 11; VN2<OrganNum + 11; VN2++)
	{
		outFile3 << VN2;
		outFile3 << "(Kg): ";
		outFile3 << VoxelN[VN2] * Resolution_x*Resolution_y*Resolution_z*Density[VN2 - 11] / 1000;
		outFile3 << " ";
		outFile3 << VoxelN2[VN2] * Resolution_x*Resolution_y*Resolution_z*Density[VN2 - 11] / 1000;
		outFile3 << " ";
		outFile3 << VoxelN3[VN2] * Resolution_x*Resolution_y*Resolution_z*Density[VN2 - 11] / 1000;
		outFile3 << "\n";
		Mass11 += VoxelN[VN2] * Resolution_x*Resolution_y*Resolution_z*Density[VN2 - 11] / 1000;
		Mass22 += VoxelN2[VN2] * Resolution_x*Resolution_y*Resolution_z*Density[VN2 - 11] / 1000;
		Mass33 += VoxelN3[VN2] * Resolution_x*Resolution_y*Resolution_z*Density[VN2 - 11] / 1000;
	}
	outFile3 << "\nMass: " << Mass11 << " " << Mass22 << " " << Mass33 << endl;

	outFile3 << "***************************************" << endl;
	// outFile3 << "Aim_Height: " << Aim_Height << endl;
	outFile3 << "Aim_Weight: " << Aim_weight << endl;
	outFile3 << "***************************************" << endl;
	// outFile3 << "Befor_Height: " << Befor_Height << endl;
	outFile3 << "Befor_Weight: " << Befor_Weight << endl;
	outFile3 << "***************************************" << endl;
	// outFile3 << "After_Height: " << After_Height << endl;
	outFile3 << "After_Weight: " << After_Weight << endl;
	// outFile3 << "After_BMI: " << After_BMI << " = " << After_Weight << " /( " << After_Height << " * " << After_Height << ")\n";
	outFile3 << "***************************************" << endl;
	outFile3 << "Weight_After_Height: " << Weight_After_Height << endl;
	outFile3 << "Weight_After_Weight: " << Weight_After_Weight << endl;
	outFile3 << "Weight_After_BMI: " << Weight_After_BMI << " = " << Weight_After_Weight << " /( " << Weight_After_Height << " * " << Weight_After_Height << ")\n";
	outFile3 << "***************************************" << endl;

	outFile3.close();

	//********************Write specific organs*********************
/*	const unsigned long int Organ_NewMaxNum = Weight_New_FileSize_x*Weight_New_FileSize_y*Weight_New_FileSize_z;
	char* Organ_NewMatrix = new char[Organ_NewMaxNum];
	int Op_Organ_ID = 13;
	for (int i = 0; i<Organ_NewMaxNum; i++)
	{
		if (Weight_NewMatrix[i] == Op_Organ_ID)
			Organ_NewMatrix[i] = 10;
		else
			Organ_NewMatrix[i] = 0;
	}

	string Organ_Newsize;
	Organ_Newsize.append("CHAR_Organ");
	char* pNum3 = new char[3];
	itoa(Op_Organ_ID, pNum3, 10);
	Organ_Newsize.append(pNum3);
	Organ_Newsize.append("_");
	itoa(Weight_New_FileSize_x, pNum3, 10);
	Organ_Newsize.append("FinalSize(");
	Organ_Newsize.append(pNum3);
	itoa(Weight_New_FileSize_y, pNum3, 10);
	Organ_Newsize.append("X");
	Organ_Newsize.append(pNum3);
	itoa(Weight_New_FileSize_z, pNum3, 10);
	Organ_Newsize.append("X");
	Organ_Newsize.append(pNum3);
	Organ_Newsize.append(")");

	cout << "Writing temp..." << endl;
	FILE* oFile3;
	string phantomName3 = path;
	phantomName3.append(Organ_Newsize);
	phantomName3.append("_");
	phantomName3.append(filename);
	oFile3 = fopen(phantomName3.c_str(), "wb");
	fwrite(Organ_NewMatrix, sizeof(char), Organ_NewMaxNum, oFile3);
	fclose(oFile3);
*/
	//**************************************************************
	delete[] a; 
	delete[] matrix;
	return 0;
}