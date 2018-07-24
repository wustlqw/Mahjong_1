#include <string.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
#define MAX_COUNT 14
#define WIK_NULL  0
#define WIK_CHI_HU 0x40

uchar AnalyseDeletePair(const uchar cbCardData[MAX_COUNT], const uchar & cbCardCount){
	//麻将牌张数统计，万条筒字花牌
	uchar arr[0x3f] = {};

	//同种花色牌值总和
	int levelValue = 0;
	//花色界限
	int level = 0x11;
	//同种花色之和是否为3的倍数
	bool checkColorValue = true;

	for (uchar i = 0; i < cbCardCount; i++){
		if (cbCardData[i]>0){
			arr[cbCardData[i]]++;

			//检测该花色的总值是否为3的倍数
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
	//麻将张数不符合要求
	if (cbCardCount <= 1 || (cbCardCount + 1) % 3 != 0){
		return WIK_NULL;
	}
	//不能存在花牌
	if (cbCardData[cbCardCount - 1] >= 0x38)
		return WIK_NULL;
	//是否平胡
	bool isPingHu = false;
	//是否小七对
	bool isXiaoQiDui = (cbCardCount == MAX_COUNT);
	//七对牌型分析
	if (isXiaoQiDui){
		for (uchar i = 0; i < cbCardCount - 1; i += 2){
			if (cbCardData[i] != cbCardData[i + 1]){
				isXiaoQiDui = false;
				break;
			}
		}
	}
	//如果是小七对，则不分析平胡，否则分析平胡累型
	if (!isXiaoQiDui){
		//平胡分析：必须一对将。遍历数组，提取将对，分析剩余麻将牌
		for (uchar index = 0; index < cbCardCount - 1; index++){
			//提取将对，过滤掉重复的将对，花牌不能做将
			if (cbCardData[index] == cbCardData[index + 1]){
				uchar tmpCbData[MAX_COUNT];
				memcpy(tmpCbData, cbCardData, MAX_COUNT);
				tmpCbData[index] = 0;
				tmpCbData[index + 1] = 0;
				//如果能胡牌,则添加平胡类型，同事终止将对提取，平胡检测完毕
				//否则继续提取剩余将对，做下一轮分析
				if (AnalyseDeletePair(tmpCbData, cbCardCount)){
					isPingHu = true;
					break;
				}

				//跳过重复的将对，检测下一对能否满足胡牌条件
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
	
	//特殊牌型在此添加
	return (isXiaoQiDui || isPingHu) ? WIK_CHI_HU : WIK_NULL;
}