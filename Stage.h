#pragma once
#include "Engine/GameObject.h"
#include "windows.h"

namespace {
    const int MODEL_NUM{ 5 };
    const int XSIZE{ 15 };
    const int ZSIZE{ 15 };
    enum BLOCKTYPE
    {
        DEFAULT, BRICK, GRASS, SAND, WATER
    };
}

//◆◆◆を管理するクラス
class Stage : public GameObject
{
private:
    

    int hModel_[MODEL_NUM];    //モデル番号
    //int table_[XSIZE][ZSIZE];
   
    int mode_;      //0:上げる　1：下げる　2：種類を変える
    int select_;    //種類
    int controlID;  //コントロールのIDを取得
    int comboID;    //コンボのIDを取得
    int notificationCode;//通知コードを取得
    int setComboID;     //コンボIDをセット
    int control;
   
   
public:
    void SetBlock(int _x, int _z, BLOCKTYPE _type);
    void SetBlockHeight(int _x, int _z, int _height);
    //コンストラクタ
    Stage(GameObject* parent);

    //デストラクタ
    ~Stage();

    //初期化
    void Initialize() override;

    //更新
    void Update() override;

    //描画
    void Draw() override;

    //開放
    void Release() override;
    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
    
  
   /* BOOL MENUProc(HWND hMenu, UINT msg, WPARAM wp, LPARAM lp);*/
    void Save();
    void Load();
};