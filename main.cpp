#include <stdio.h>
#include "easyx/include/easyx.h"
#include "easyx/include/graphics.h"
#include "tools/tools.h"

#define WIN_WIDTH 900
#define WIN_HEIGHT 600

enum{WAN_DOU,XIANG_RI_KUI,DA_ZUI_HUA,YING_TAO_ZHA_DAN,PLANT_CNT};

IMAGE imgBg;	//背景图片（这样快）
IMAGE imgBar;
IMAGE imgTest;

IMAGE imgCards[PLANT_CNT];
void gameInit() {
    // 背景图片
    // 把字符集修改为“多字节字符集”
    loadimage(&imgBg, "./res/bg.jpg");
    loadimage(&imgBar, "./res/bar5.png");
    loadimage(&imgTest, "./res/Cards/card_1.png");
    // 植物卡牌
    char name[64];
    for (int i = 0; i < PLANT_CNT; i++) {
        sprintf_s(name, sizeof(name), "./res/Cards/card_%d.png",i+1);
        loadimage(&imgCards[i], name);
    }
    // 创建图形窗口
    initgraph(WIN_WIDTH, WIN_HEIGHT);
}

void updateWindow() {
    putimage(0,0,&imgBg);
//    putimage(250,0,&imgBar);
    putimagePNG(250,0,&imgBar);

    for (int i = 0; i < PLANT_CNT; i++) {
        int x = 338 + i * 65;
        int y = 6;
        putimage(x, y, &imgCards[i]);
    }
}

int main(void) {
    gameInit();

    updateWindow();

    system("pause");
    return 0;
}