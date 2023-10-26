#include "Stage.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"
#include "Engine/Fbx.h"
#include "resource.h"
#include <string>
#include <ios>
#include <sstream>



void Stage::SetBlock(int _x, int _z, BLOCKTYPE _type)
{
	table_[_x][_z].type = _type;
}

void Stage::SetBlockHeight(int _x, int _z, int _height)
{
	table_[_x][_z].height = _height;
}

//コンストラクタ
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

//デストラクタ
Stage::~Stage()
{
}

//初期化
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
	//モデルデータのロード
	for (int i = 0; i < MODEL_NUM; i++) {
		hModel_[i] = Model::Load(fname_base + modelname[i]);
		assert(hModel_[i] >= 0);
	}
	//tableにブロックのタイプをセットしてやろう！
	for (int z = 0; z < ZSIZE; z++) {
		for (int x = 0; x < XSIZE; x++) {
			SetBlock(x, z, (BLOCKTYPE)(0));
			SetBlockHeight(x, z, 0);
		}
	}

}

//更新
void Stage::Update()
{
	if (!Input::IsMouseButtonDown(0)) {
		return;
	}
	float w = (float)(Direct3D::scrWidth / 2.0f);
	float h = (float)(Direct3D::scrHeight / 2.0f);
	//Offsetx,y は0
	//minZ =0 maxZ = 1

	XMMATRIX vp =
	{
		 w,  0,  0, 0,
		 0, -h,  0, 0,
		 0,  0,  1, 0,
		 w,  h,  0, 1
	};

	//ビューポート
	XMMATRIX invVP = XMMatrixInverse(nullptr, vp);
	//プロジェクション変換
	XMMATRIX invProj = XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());
	//ビュー変換
	XMMATRIX invView = XMMatrixInverse(nullptr, Camera::GetViewMatrix());
	XMFLOAT3 mousePosFront = Input::GetMousePosition();
	mousePosFront.z = 0.0f;
	XMFLOAT3 mousePosBack = Input::GetMousePosition();
	mousePosBack.z = 1.0f;
	//①　mousePosFrontをベクトルに変換
	XMVECTOR vMouseFront = XMLoadFloat3(&mousePosFront);
	//②　①にinvVP、invPrj、invViewをかける
	vMouseFront = XMVector3TransformCoord(vMouseFront, invVP * invProj * invView);
	//③　mousePosBackをベクトルに変換
	XMVECTOR vMouseBack = XMLoadFloat3(&mousePosBack);
	//④　③にinvVP、invPrj、invViewをかける
	vMouseBack = XMVector3TransformCoord(vMouseBack, invVP * invProj * invView);

	for (int x = 0; x < 15; x++)
	{
		for (int z = 0; z < 15; z++)
		{
			for (int y = 0; y < table_[x][z].height + 1; y++)
			{
				//⑤　②から④に向かってレイをうつ（とりあえずモデル番号はhModel_[0]）
				RayCastData data;
				XMStoreFloat4(&data.start, vMouseFront);
				XMStoreFloat4(&data.dir, vMouseBack - vMouseFront);
				Transform trans;
				trans.position_.x = x;
				trans.position_.y = y;
				trans.position_.z = z;
				Model::SetTransform(hModel_[0], trans);

				Model::RayCast(hModel_[0], data);

				//⑥　レイが当たったらブレークポイントで止める
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
}

//描画
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
				//table[x][z]からオブジェクトのタイプを取り出して書く！
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
//	char fileName[MAX_PATH] = "無題.map";
//	std::string buffer;
//	std::stringstream oss;
//
//
//	//OPENFILENAME構造体を初期化
//	OPENFILENAME ofn; {
//		ZeroMemory(&ofn, sizeof(ofn));
//		ofn.lStructSize = sizeof(OPENFILENAME);
//		ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0");
//		ofn.lpstrFile = fileName;
//		ofn.nMaxFile = MAX_PATH;
//		ofn.Flags = OFN_OVERWRITEPROMPT;
//		ofn.lpstrDefExt = TEXT("map");
//	}
//
//	//ファイルに保存
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
//	char fileName[MAX_PATH] = "無題.map";
//	std::string buffer;
//	std::stringstream oss;
//
//	//OPENFILENAME構造体を初期化
//	OPENFILENAME ofn; {
//		ZeroMemory(&ofn, sizeof(ofn));
//		ofn.lStructSize = sizeof(OPENFILENAME);
//		ofn.lpstrFilter = TEXT("マップデータ(*.map)\0*.map\0");
//		ofn.lpstrFile = fileName;
//		ofn.nMaxFile = MAX_PATH;
//		ofn.Flags = OFN_FILEMUSTEXIST;
//		ofn.lpstrDefExt = TEXT("map");
//		ofn.lpstrDefExt;
//	}
//
//	//ファイルを開く
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
//開放
void Stage::Release()
{
}
//偽物のダイアログプロシージャ
BOOL Stage::DialogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		//ダイアログできた
	case WM_INITDIALOG:
		//ラジオボタンの初期値
		SendMessage(GetDlgItem(hDlg, IDC_RADIO_UP), BM_SETCHECK, BST_CHECKED, 0);
		//コンボボックスの初期値
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"デフォルト");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"レンガ");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"草原");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"砂地");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)"水");
		SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_SETCURSEL, 0, 0);
		return 0;
	case WM_COMMAND:
		controlID = LOWORD(wp);//コントロールのIDを取得
		notificationCode = HIWORD(wp);//通知コードを取得
		if (controlID == IDC_COMBO2 || notificationCode == CBN_SELCHANGE) {
			comboID = SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_GETCURSEL, 0, 0);
			controlID = IDC_RADIO_CHANGE;
		}
		return 0;
	}
	return FALSE;
}

//BOOL CALLBACK MENUProc(HWND hMenu, UINT msg, WPARAM wp, LPARAM lp)
//{
//	switch (msg)
//	{
//	case WM_INITMENU:
//		return 0;
//	case WM_COMMAND:
//		switch (LOWORD(wp)) {
//		case ID_MENU_SAVE:
//		
//			return 0;
//		}
//	}
//}

