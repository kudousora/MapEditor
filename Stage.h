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

//���������Ǘ�����N���X
class Stage : public GameObject
{
    int hModel_[MODEL_NUM];    //���f���ԍ�
    //int table_[XSIZE][ZSIZE];
    struct
    {
        int type;
        int height;
    } table_[15][15];

    int mode_;      //0:�グ��@1�F������@2�F��ނ�ς���
    int select_;    //���
    int controlID;  //�R���g���[����ID���擾
    int comboID;    //�R���{��ID���擾
    int notifficationCode;//�ʒm�R�[�h���擾
    int setComboID;     //�R���{ID���Z�b�g

public:
    void SetBlock(int _x, int _z, BLOCKTYPE _type);
    void SetBlockHeight(int _x, int _z, int _height);
    //�R���X�g���N�^
    Stage(GameObject* parent);

    //�f�X�g���N�^
    ~Stage();

    //������
    void Initialize() override;

    //�X�V
    void Update() override;

    //�`��
    void Draw() override;

    //�J��
    void Release() override;
    BOOL DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
};