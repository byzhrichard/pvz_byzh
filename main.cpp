#include <stdio.h>
#include <easyx.h>
#include <graphics.h>
#include <conio.h>
#include "tools/tools.h"

#define WIN_WIDTH 900
#define WIN_HEIGHT 600

enum{WAN_DOU,XIANG_RI_KUI,DA_ZUI_HUA,YING_TAO_ZHA_DAN,PLANT_CNT};

IMAGE imgBg;	//背景图片（这样快）
IMAGE imgBar;
IMAGE imgCards[PLANT_CNT];
IMAGE* imgPlant[PLANT_CNT][20];

int curX,curY;  // 当前选中的植物，在移动过程中的位置
int curPlant;   // 0:没有选中 1:选第一种 ......

struct Plant{
    int type;
    int frameindex; //序列帧
};

struct Plant map[3][9];

bool fileExist(const char* name){
    FILE *fp = fopen(name,"r");

    if (fp == NULL){
        return false;
    }else{
        fclose(fp);
        return true;

    }
}
void gameInit() {
    // 背景图片
    // 把字符集修改为“多字节字符集”
    loadimage(&imgBg, "../res/bg.jpg");
    loadimage(&imgBar, "../res/bar5.png");
    // 植物/植物卡牌
    memset(imgPlant,0, sizeof(imgPlant));
    memset(map,0, sizeof(map));
    char name[64];
    for (int i = 0; i < PLANT_CNT; i++) {
        sprintf_s(name, sizeof(name), "../res/Cards/card_%d.png",i+1);
        loadimage(&imgCards[i], name);

        for (int j=0;j<20;j++){
            sprintf_s(name, sizeof(name), "../res/zhiwu/%d/%d.png",i,j+1);
            //先判断文件是否存在
            if (fileExist(name)){
                imgPlant[i][j] = new IMAGE;
                loadimage(imgPlant[i][j],name);
            }else{
                break;
            }
        }
    }
    // 创建图形窗口
    initgraph(WIN_WIDTH, WIN_HEIGHT);
}

void updateWindow() {
    BeginBatchDraw(); //开始缓冲

    //背景
    putimage(0,0,&imgBg);
    putimagePNG(250,0,&imgBar);
    //卡牌
    for (int i = 0; i < PLANT_CNT; i++) {
        int x = 338 + i * 65;
        int y = 6;
        putimage(x, y, &imgCards[i]);
    }
    //拖动-植物
    if (curPlant != 0){
        IMAGE* img = imgPlant[curPlant-1][0];
        putimagePNG(curX - img->getwidth()/2,curY - img->getheight()/2,img);
    }

    for (int i=0;i<3;i++){
        for (int j=0;j<9;j++){
            if (map[i][j].type > 0){
                int x = 256 + j * 81;
                int y = 179 + i * 102 + 12;
                int plantType = map[i][j].type - 1;
                int index = map[i][j].frameindex;
                putimagePNG(x,y,imgPlant[plantType][index]);
            }
        }
    }
    EndBatchDraw(); //结束缓冲
}

void userClick(){
    ExMessage msg;
    static int status = 0;
    if (peekmessage(&msg)){
        if (msg.message == WM_LBUTTONDOWN){
            if (msg.x > 338 && msg.x < 338 + 65 * PLANT_CNT && msg.y < 96){
                int index = (msg.x - 338) / 65;
//                printf("%d\n",index);
                status = 1;
                curPlant = index+1;
            }
        } else if (msg.message == WM_MOUSEMOVE && status == 1){
            curX = msg.x;
            curY = msg.y;
        } else if (msg.message == WM_LBUTTONUP){
            if (msg.x>256 && msg.y>179 && msg.y<489){
                int row = (msg.y - 179) / 102;
                int col = (msg.x - 256) / 81;
                printf("%d %d\n",row,col);
                if (map[row][col].type == 0){
                    map[row][col].type = curPlant;
                    map[row][col].frameindex = 0;
                }
            }


            curPlant = 0;
            status = 0;
        }
    };
}

//void updateGame(){
//    for (int i = 0;i<3;i++){
//        for (int j = 0;j<9;j++){
//            if (map[i][j].type > 0){
//                map[i][j].frameindex++;
//                if (imgPlant[map[i][j].type-1]){
//                    map[i][j].frameindex = 0;
//                }
//            }
//        }
//    }
//}

int main() {
    gameInit();

    while(1){
        userClick();

        updateWindow();
//        updateGame();
    }
//    updateWindow();

    system("pause");
    return 0;
}

