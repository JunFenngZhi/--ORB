#pragma once
#include<iostream>
#include<stdio.h>
#include<opencv2/opencv.hpp>
#include <math.h>
#include<time.h>

using namespace cv;
using namespace std;

//ORB����
typedef struct Fast_keypoint
{
	int x;					//��ĺ�����   ԭ�������Ͻ�
	int y;					//���������
	int real_x = 0;			//ӳ��س�ʼͼ�������x
	int real_y = 0;			//ӳ��س�ʼͼ�������y
	int Fast_response = 0;	//�õ��FAST��Ӧֵ
	int scale_level = 0;		//�õ����ڽ���������
	float Harris_response = 0;	//Harris��Ӧֵ
	unsigned char Descriptor[32] = { 0 };  //BRIEF�����ӣ�256bits��
	int Get_brief_bit(int index)		   //��ȡ���������ض���һλ
	{
		unsigned char n = Descriptor[(index - 1) / 8];
		unsigned char a = 1;
		a = a << (index - 1) % 8;
		n = n & a;
		if (n == a)
			return 1;
		return 0;
	}
}Fast_Keypoint;

//�洢��Ժ�ĵ���Ϣ
typedef struct Match
{
	Point2f base_point;		//��׼ͼ�еĵ�����
	Point2f corresponding_point;  //Ŀ��ͼ�еĵ�����
	int base_index;			//���ڻ�׼�㼯�е��±�
	int corresponding_index;//����Ŀ��㼯�е��±�
}Match_pairs;

class ORB_extractor
{
public:
	ORB_extractor(int nFeatures, float scaleFactor, int nlevels);
	~ORB_extractor();
	void Get_features(Mat grey_image, vector<Fast_Keypoint>&FAST_Keypoints);		//��ȡORB����

private:
	int nFeatures;				//��ȡ������������	
	int nlevels;				//����������
	float scaleFactor;			//�߶�����
	vector<Mat>scale_pyramid;   //�߶Ƚ�����
	void Build_pyramid(Mat image);		//�����߶Ƚ�����
	void Compute_Keypoints(Mat image, int nPoints, int level, vector<Fast_Keypoint>&List);		//������ͼ����ȡָ������������
	bool FAST16_9(Mat image, vector<Fast_Keypoint>&List, int level, int nPoints, int threshold);	//���FAST16-9�����㲢���Ǽ���ֵ���ƣ�����ǰ2N��������
	void Compute_Harris(Mat image, vector<Fast_Keypoint>&List);		//��ÿ�������������harris����ֵ
	void Compute_BRIEF(vector<Fast_Keypoint>&List);		//����������  

};

class LSH_match
{
public:
	LSH_match(int K, int L, float threshold);
	~LSH_match();
	void Create_HashLists(vector<Fast_Keypoint>&ORB_Features);	//����׼���ݷ���hash����(����hash��)
	void BF_match(vector<Fast_Keypoint>&ORB_Features, vector<Fast_Keypoint>&Base_Features, vector<Match_pairs>& match_result);//�����ϣֵ������ӦͰ�ڵĵ㱩��ƥ��
	int ComputeHamming_Distance(unsigned char Descriptor_1[32], unsigned char Descriptor_2[32]);		//�������������Ӽ�ĺ�������

private:
	int K;		//��ϣ����
	int L;		//��ϣ������
	float threshold;     //ƥ����ֵ
	vector<map<unsigned long, vector<int>>>Hash_lists;	//��ϣ����
	vector<vector<int>>HashFunctions;	//��ϣ������
	void Create_HashFunction();		    //������ϣ����

};

Mat My_RANSAC(vector<Match_pairs>&Match_List);