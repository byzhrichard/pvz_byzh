#include <stdio.h>
#include <easyx.h>
#include <time.h>
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

struct mapPlant{
    int type;
    int frameindex; //序列帧
};

struct mapPlant map[3][9];

struct sunshineBall{
    int x,y;    //飘落过程中的坐标
    int frameindex;
    int destY;  //飘落的目标位置
    bool used;   //是否在使用
    int timer;
};

struct sunshineBall balls[10];
IMAGE imgSunshineBall[29];

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
    curPlant = 0;

    memset(balls,0,sizeof(balls));
    for (int i = 0; i < 29; i++){
        sprintf_s(name, sizeof(name),"../res/sunshine/%d.png",i+1);
        loadimage(&imgSunshineBall[i],name);
    }
    //配置随机种子
    srand(time(NULL));
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
    //场上-植物
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
    //拖动-植物
    if (curPlant != 0){
        IMAGE* img = imgPlant[curPlant-1][0];
        putimagePNG(curX - img->getwidth()/2,curY - img->getheight()/2,img);
    }

    int ballMax = sizeof(balls) / sizeof(balls[0]);
    for (int i = 0;i<ballMax;i++){
        if (balls[i].used){
            IMAGE* img = &imgSunshineBall[balls[i].frameindex];
            putimagePNG(balls[i].x,balls[i].y,img);
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

void createSunshine(){
    static int count = 0;
    static int fre = 400;
    count++;
    if (count >= fre){
        fre = 200 + rand() % 200;
        count = 0;

        //从阳光池中去一个可以使用的
        int ballMax = sizeof(balls)/ sizeof(balls[0]);

        int i;
        for (i = 0;i<ballMax && balls[i].used;i++);
        if (i>=ballMax) return;

        balls[i].used = true;
        balls[i].frameindex = 0;
        balls[i].x = 260 + rand() % (900-260);//260-900
        balls[i].y = 60;
        balls[i].destY = 200 + (rand() % 4) * 90;
        balls[i].timer = 0;
    }

}
void updateSunshine(){
    int ballMax = sizeof(balls)/ sizeof(balls[0]);
    for (int i = 0;i<ballMax;i++){
        if (balls[i].used) {
            balls[i].frameindex = (balls[i].frameindex + 1) % 29;
            if (balls[i].timer == 0){
                balls[i].y += 2;
            }
            if (balls[i].y > balls[i].destY){
                balls[i].timer ++;
                if (balls[i].timer > 100){
                    balls[i].used = false;
                }
            }
        }
    }
}
void updateGame(){
    //帧增加
    for (int i = 0;i<3;i++){
        for (int j = 0;j<9;j++){
            if (map[i][j].type > 0){
                map[i][j].frameindex++;

                int plantType = map[i][j].type-1;
                int index = map[i][j].frameindex;
                if (imgPlant[plantType][index] == NULL){
                    map[i][j].frameindex = 0;
                }
            }
        }
    }

    createSunshine();
    updateSunshine();
}

void startUI() {
    IMAGE imgBg, imgMenu1,imgMenu2;
    loadimage(&imgBg,"../res/menu.png");
    loadimage(&imgMenu1,"../res/menu1.png");
    loadimage(&imgMenu2,"../res/menu2.png");

    int flag = 0;
    while(1){
        BeginBatchDraw();
        putimage(0,0,&imgBg);
        putimagePNG(474,75,flag ? &imgMenu2 : &imgMenu1);
        EndBatchDraw();
        ExMessage msg;
        if (peekmessage(&msg)){
            if (msg.message == WM_LBUTTONDOWN &&
                msg.x > 474 && msg.x < 474 + 300 &&
                msg.y > 75 && msg.y < 75 + 140){
                    flag = 1;
                }
            } else if(msg.message == WM_LBUTTONUP && flag){
            return;
        }

    }
}

int main() {
    gameInit();
    startUI();
    int timer = 0;
    bool flag = true;
    while(1){
        userClick();

        timer += getDelay();
        if (timer > 20){
            flag = true;
            timer = 0;
        }
        if (flag){
            flag = false;
            updateWindow();
            updateGame();
        }
    }
    system("pause");
    return 0;
}

