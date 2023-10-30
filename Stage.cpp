#include "Stage.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"
#include "Engine/Fbx.h"
#include "resource.h"
#include <string>
#include <ios>
#include <sstream>
#include <fstream>
#include <iostream>
#include <locale>



void Stage::SetBlock(int _x, int _z, BLOCKTYPE _type)
{
	table_[_x][_z].type = _type;
}

void Stage::SetBlockHeight(int _x, int _z, int _height)
{
	table_[_x][_z].height = _height;
}

//�R���X�g���N�^
Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage")
{
	for (int i = 0; i < MODEL_NUM; i++) {
		hModel_[i] = -1;
	}
	for (int z = 0; z < ZSIZE; z++) {
		for (int x = 0; x < XSIZE; x++) {
			SetBlock(x, z, DEFAULT);
		}
	}
}

//�f�X�g���N�^
Stage::~Stage()
{
}

//������
void Stage::Initialize()
{
	string modelname[] = {
		"BoxDefault.fbx",
		"BoxBrick.fbx",
		"BoxGrass.fbx",
		"BoxSand.fbx",
		"BoxWater.fbx"
	};
	string fname_base = "assets/";
	//���f���f�[�^�̃��[�h
	for (int i = 0; i < MODEL_NUM; i++) {
		hModel_[i] = Model::Load(fname_base + modelname[i]);
		assert(hModel_[i] >= 0);
	}
	//table�Ƀu���b�N�̃^�C�v���Z�b�g���Ă�낤�I
	for (int z = 0; z < ZSIZE; z++) {
		for (int x = 0; x < XSIZE; x++) {
			SetBlock(x, z, (BLOCKTYPE)(0));
			SetBlockHeight(x, z, 0);
		}
	}

}

//�X�V
void Stage::Update()
{
	if (!Input::IsMouseButtonDown(0)) {
		return;
	}
	float w = (float)(Direct3D::scrWidth / 2.0f);
	float h = (float)(Direct3D::scrHeight / 2.0f);
	//Offsetx,y ��0
	//minZ =0 maxZ = 1

	XMMATRIX vp =
	{
		 w,  0,  0, 0,
		 0, -h,  0, 0,
		 0,  0,  1, 0,
		 w,  h,  0, 1
	};

	//�r���[�|�[�g
	XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
	//�v���W�F�N�V�����ϊ�
	XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
	//�r���[�ϊ�
	XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());
	XMFLOAT3 mousePosFront = Input::GetMousePosition();
	mousePosFront.z = 0.0f;
	XMFLOAT3 mousePosBack = Input::GetMousePosition();
	mousePosBack.z = 1.0f;
	//�@�@mousePosFront���x�N�g���ɕϊ�
	XMVECTOR vMouseFront = XMLoadFloat3(&mousePosFront);
	//�A�@�@��invVP�AinvPrj�AinvView��������
	vMouseFront = XMVector3TransformCoord(vMouseFront, invVP * invProj * invView);
	//�B�@mousePosBack���x�N�g���ɕϊ�
	XMVECTOR vMouseBack = XMLoadFloat3(&mousePosBack);
	//�C�@�B��invVP�AinvPrj�AinvView��������
	vMouseBack = XMVector3TransformCoord(vMouseBack, invVP * invProj * invView);

	for (int x = 0; x < 15; x++)
	{
		for (int z = 0; z < 15; z++)
		{
			for (int y = 0; y < table_[x][z].height + 1; y++)
			{
				//�D�@�A����C�Ɍ������ă��C�����i�Ƃ肠�������f���ԍ���hModel_[0]�j
				RayCastData data;
				XMStoreFloat4(&data.start, vMouseFront);
				XMStoreFloat4(&data.dir, vMouseBack - vMouseFront);
				Transform trans;
				trans.position_.x = x;
				trans.position_.y = y;
				trans.position_.z = z;
				Model::SetTransform(hModel_[0], trans);

				Model::RayCast(hModel_[0], data);

				//�E�@���C������������u���[�N�|�C���g�Ŏ~�߂�
				if (controlID == IDC_RADIO_UP) {
					if (data.hit)
					{
						table_[x][z].height++;
						break;
					}

				}
				else if (controlID == IDC_RADIO_DOWN) {
					if (data.hit)
					{
						
						if (y > 0)
						{
							table_[x][z].height--;
							break;
						}

					}
				}
				else if (controlID == IDC_RADIO_CHANGE) {
					if (data.hit) {
						BLOCKTYPE newBlockType = (BLOCKTYPE)comboID;
						SetBlock(x, z, newBlockType);
					}
				}
				
			}

		}
	}
	if (controlID == IDC_RADIO_encircle) {
		if (Input::IsMouseButtonDown(0)) {
			// �}�E�X���N���b�N���ꂽ�ꍇ

			// �O���̃u���b�N�������𑝂₷
			for (int x = 0; x < 15; x++) {
				for (int z = 0; z < 15; z++) {
					// �O���̃u���b�N�̏ꍇ�A�����𑝂₷
					if (x == 0 || z == 0 || x == 14 || z == 14) {
						table_[x][z].height++;
					}
				}
			}
		}
	}
}

//�`��
void Stage::Draw()
{
	//Model::SetTransform(hModel_, transform_);
	//Model::Draw(hModel_);

	for (int x = 0; x < 15; x++)
	{
		for (int z = 0; z < 15; z++)
		{
			for (int y = 0; y < table_[x][z].height + 1; y++)
			{
				//table[x][z]����I�u�W�F�N�g�̃^�C�v�����o���ď����I
				int type = table_[x][z].type;
				Transform trans;
				trans.position_.x = x;
				trans.position_.y = y;
				trans.position_.z = z;
				Model::SetTransform(hModel_[type], trans);
				Model::Draw(hModel_[type]);
			}
		}
	}
}
//void Stage::Save()
//{
//	char fileName[MAX_PATH] = "����.map";
//	std::string buffer;
//	std::stringstream oss;
//
//
//	//OPENFILENAME�\���̂�������
//	OPENFILENAME ofn; {
//		ZeroMemory(&ofn, sizeof(ofn));
//		ofn.lStructSize = sizeof(OPENFILENAME);
//		ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.map)\0*.map\0");
//		ofn.lpstrFile = fileName;
//		ofn.nMaxFile = MAX_PATH;
//		ofn.Flags = OFN_OVERWRITEPROMPT;
//		ofn.lpstrDefExt = TEXT("map");
//	}
//
//	//�t�@�C���ɕۑ�
//	if (GetSaveFileName(&ofn)) {
//		std::fstream outputFile(fileName, std::ios::binary | std::ios::out);
//		for (int x = 0; x < XSIZE; x++) {
//			for (int z = 0; z < ZSIZE; z++) {
//				outputFile.write((char*)&table_[x][z], sizeof(BlockData));
//			}
//		}
//		outputFile.close();
//	}
//}
//
//void Stage::Load()
//{
//	char fileName[MAX_PATH] = "����.map";
//	std::string buffer;
//	std::stringstream oss;
//
//	//OPENFILENAME�\���̂�������
//	OPENFILENAME ofn; {
//		ZeroMemory(&ofn, sizeof(ofn));
//		ofn.lStructSize = sizeof(OPENFILENAME);
//		ofn.lpstrFilter = TEXT("�}�b�v�f�[�^(*.map)\0*.map\0");
//		ofn.lpstrFile = fileName;
//		ofn.nMaxFile = MAX_PATH;
//		ofn.Flags = OFN_FILEMUSTEXIST;
//		ofn.lpstrDefExt = TEXT("map");
//		ofn.lpstrDefExt;
//	}
//
//	//�t�@�C�����J��
//	if (GetOpenFileName(&ofn)) {
//		std::fstream inputFile(fileName, std::ios::binary | std::ios::in);
//
//		for (int x = 0; x < XSIZE; x++) {
//			for (int z = 0; z < ZSIZE; z++) {
//				inputFile.read((char*)&table_[x][z], sizeof(BlockData));
//			}
//		}
//
//		inputFile.close();
//	}
//}
void Stage::Save()
{
	char fileName[MAX_PATH] = "����.map";  //�t�@�C����������ϐ�
	//�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
	OPENFILENAME ofn;

	// �_�C�A���O�{�b�N�X�̏�����
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = "�e�L�X�g�t�@�C�� (*.txt)\0*.txt\0���ׂẴt�@�C�� (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	// "�t�@�C����ۑ�" �_�C�A���O��\��
	if (GetSaveFileName(&ofn) == TRUE)
	{
		// �t�@�C��������ɑI�����ꂽ�ꍇ
		HANDLE hFile = CreateFile(
			fileName,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		std::string data;

		if (hFile != INVALID_HANDLE_VALUE)
		{

			// �t�@�C���Ƀf�[�^����������
			for (int x = 0; x < XSIZE; x++)
			{
				for (int z = 0; z < ZSIZE; z++)
				{
					std::stringstream typeStream;
					typeStream << table_[x][z].type;

					std::stringstream heightStream;
					heightStream << table_[x][z].height;

					data += typeStream.str() + " " + heightStream.str() + "\n";
				}
			}

			bytes = 0;
			WriteFile(
				hFile,
				data.c_str(),
				(DWORD)data.length(),
				&bytes,
				NULL);

			CloseHandle(hFile);
		}
	}
}

void Stage::Load()
{

	char fileName[MAX_PATH] = "����.map";  //�t�@�C����������ϐ�
	//�u�t�@�C����ۑ��v�_�C�A���O�̐ݒ�
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = TEXT("�e�L�X�g�t�@�C�� (*.txt)\0*.txt\0���ׂẴt�@�C�� (*.*)\0*.*\0");
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;

	selFile = GetOpenFileName(&ofn);

	if (selFile == FALSE)
		return;

	hFile = CreateFile(
		fileName,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"�t�@�C���I�[�v���Ɏ��s " << GetLastError() << std::endl;
		return;
	}

	DWORD fileSize = GetFileSize(hFile, NULL);
	char* fileData = new char[fileSize];
	bytes = 0;
	res = ReadFile(
		hFile,
		fileData,
		(DWORD)fileSize,
		&bytes,
		NULL);

	if (res == FALSE)
	{
		std::wcout << L"�t�@�C���ǂݍ��݂Ɏ��s" << GetLastError() << std::endl;
		CloseHandle(hFile);
		return;
	}

	for (int x = 0; x < XSIZE; x++)
	{
		for (int z = 0; z < ZSIZE; z++)
		{
			int type, height;
			if (sscanf_s(fileData, "%d %d", &type, &height) == 2)
			{
				table_[x][z].type = type;
				table_[x][z].height = height;
			}

			// ���̃f�[�^�ɐi��
			fileData = strchr(fileData, '\n');

			if (fileData != nullptr)
				++fileData; // ���s�������X�L�b�v
		}
	}

	CloseHandle(hFile);
	std::string fileContent(fileData, fileSize);
}
//�J��
void Stage::Release()
{
}
//�U���̃_�C�A���O�v���V�[�W��
BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		//�_�C�A���O�ł���
	case WM_INITDIALOG:
		//���W�I�{�^���̏����l
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);
		//�R���{�{�b�N�X�̏����l
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"�f�t�H���g");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"�����K");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"����");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"���n");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"��");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_SETCURSEL, 0, 0);
		return 0;
	case WM_COMMAND:
		controlID = LOWORD(wp);//�R���g���[����ID���擾
		notificationCode = HIWORD(wp);//�ʒm�R�[�h���擾
		if (controlID == IDC_COMBO2 || notificationCode == CBN_SELCHANGE) {
			comboID = SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_GETCURSEL, 0, 0);
			controlID = IDC_RADIO_CHANGE;
		}
		return 0;
	}
	return FALSE;
}

BOOL Stage::MENUProc(HWND hMenu, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_INITMENU:
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case ID_MENU_SAVE:
			Save();
			return 0;
		case ID_MENU_OPEN:
			Load();
			return 0;
		}
	}
	return 0;
}

