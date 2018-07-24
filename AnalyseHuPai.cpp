#include <string.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
#define MAX_COUNT 14
#define WIK_NULL  0
#define WIK_CHI_HU 0x40

uchar AnalyseDeletePair(const uchar cbCardData[MAX_COUNT], const uchar & cbCardCount){
	//�齫������ͳ�ƣ�����Ͳ�ֻ���
	uchar arr[0x3f] = {};

	//ͬ�ֻ�ɫ��ֵ�ܺ�
	int levelValue = 0;
	//��ɫ����
	int level = 0x11;
	//ͬ�ֻ�ɫ֮���Ƿ�Ϊ3�ı���
	bool checkColorValue = true;

	for (uchar i = 0; i < cbCardCount; i++){
		if (cbCardData[i]>0){
			arr[cbCardData[i]]++;

			//���û�ɫ����ֵ�Ƿ�Ϊ3�ı���
			if (cbCardData[i] >= level){
				checkColorValue = (levelValue % 3 == 0);
				if (!checkColorValue){
					break;
				}
				levelValue = 0;
				while (level <= levelValue){
					level += 0x10;
				}
			}
			levelValue += cbCardData[i];
		}
	}

	checkColorValue = (levelValue % 3 == 0);
	if (!checkColorValue){
		return false;
	}

	if (checkColorValue){
		for (int i = 0; i < cbCardCount; i++){
			if (arr[cbCardData[i]] >= 3){
				arr[cbCardData[i]] -= 3;
			}
			if (arr[cbCardData[i]]>0){
				if (cbCardData[i] < 0x31 && i < cbCardCount - 2 && arr[cbCardData[i] + 1] >= arr[cbCardData[i]] && arr[cbCardData[i] + 2] >= arr[cbCardData[i]]){
					arr[cbCardData[i] + 1] -= arr[cbCardData[i]];
					arr[cbCardData[i] + 2] -= arr[cbCardData[i]];
					arr[cbCardData[i]] = 0;
				}
				else{
					return false;
				}
			}
		}
	}
	return true;
}

ushort AnalyseChiHuCardByTing(const uchar cbCardData[MAX_COUNT], const uchar& cbCardCount){
	//�齫����������Ҫ��
	if (cbCardCount <= 1 || (cbCardCount + 1) % 3 != 0){
		return WIK_NULL;
	}
	//���ܴ��ڻ���
	if (cbCardData[cbCardCount - 1] >= 0x38)
		return WIK_NULL;
	//�Ƿ�ƽ��
	bool isPingHu = false;
	//�Ƿ�С�߶�
	bool isXiaoQiDui = (cbCardCount == MAX_COUNT);
	//�߶����ͷ���
	if (isXiaoQiDui){
		for (uchar i = 0; i < cbCardCount - 1; i += 2){
			if (cbCardData[i] != cbCardData[i + 1]){
				isXiaoQiDui = false;
				break;
			}
		}
	}
	//�����С�߶ԣ��򲻷���ƽ�����������ƽ������
	if (!isXiaoQiDui){
		//ƽ������������һ�Խ����������飬��ȡ���ԣ�����ʣ���齫��
		for (uchar index = 0; index < cbCardCount - 1; index++){
			//��ȡ���ԣ����˵��ظ��Ľ��ԣ����Ʋ�������
			if (cbCardData[index] == cbCardData[index + 1]){
				uchar tmpCbData[MAX_COUNT];
				memcpy(tmpCbData, cbCardData, MAX_COUNT);
				tmpCbData[index] = 0;
				tmpCbData[index + 1] = 0;
				//����ܺ���,�����ƽ�����ͣ�ͬ����ֹ������ȡ��ƽ��������
				//���������ȡʣ�ཫ�ԣ�����һ�ַ���
				if (AnalyseDeletePair(tmpCbData, cbCardCount)){
					isPingHu = true;
					break;
				}

				//�����ظ��Ľ��ԣ������һ���ܷ������������
				for (uchar next = index; next < cbCardCount - 1; next++){
					if (cbCardData[next] == cbCardData[next + 1]){
						index++;
					}
					else{
						break;
					}
				}
			}
		}
	}
	
	//���������ڴ����
	return (isXiaoQiDui || isPingHu) ? WIK_CHI_HU : WIK_NULL;
}