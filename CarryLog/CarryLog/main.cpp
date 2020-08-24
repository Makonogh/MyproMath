#include<DxLib.h>
#include<cmath>
#include"Geometry.h"


using namespace std;

void DrawWood(const Capsule& cap, int handle) {

	auto v = cap.posB - cap.posA;
	auto angle = atan2(v.y, v.x) + DX_PI_F / 2.0f;
	auto w = cos(angle)*cap.radius;
	auto h = sin(angle)*cap.radius;

	DrawModiGraph(
		cap.posA.x - w, cap.posA.y - h,
		cap.posB.x - w, cap.posB.y - h,
		cap.posB.x + w, cap.posB.y + h,
		cap.posA.x + w, cap.posA.y + h, handle,true);
}


///とある点を中心に回転して、その回転後の座標を返します。
///@param center 回転中心点
///@param angle 回転角度
///@param pos 元の座標
///@return 変換後の座標
Matrix RotatePosition(const Position2& center, float angle) {
	//①中心を原点に平行移動して
	//②原点中心に回転して
	//③中心を元の座標へ戻す

	Matrix mat = MultipleMat(TranslateMat(center.x, center.y),
		MultipleMat(RotateMat(angle),
			TranslateMat(-center.x, -center.y)));

	return mat;
	//これを書き換えて、特定の点を中心に回転を行うようにしてください。
}

//カプセルと円が当たったか？
bool IsHit(const Capsule& cap, const Circle& cc) {

	return HitCheck_Capsule_Capsule(cap.posA, cap.posB, float Cap1R, VECTOR Cap2Pos1, VECTOR Cap2Pos2, float Cap2R);
}	


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(true);
	SetGraphMode(512, 800, 32);
	DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	int sw, sh;//スクリーン幅、高さ
	GetDrawScreenSize(&sw, &sh);

	auto woodH = LoadGraph("img/wood.png");
	int wdW, wdH;
	auto rockH = LoadGraph("img/rock.png");
	GetGraphSize(woodH, &wdW, &wdH);
	wdW = 200;

	auto cascadeH = LoadGraph("img/cascade_chip.png");
	auto chipH = LoadGraph("img/atlas0.png");

	auto GameClear = LoadGraph("img/clear.png");
	auto GameOver = LoadGraph("img/over.png");

	Capsule cap(20,Position2((sw-wdW)/2,sh-100),Position2((sw - wdW) / 2+wdW,sh-100));
	Circle rock(12,Position2(32 + GetRand(sw - 64),32));

	char keystate[256];
	
	float angle = 0.0f;

	int frame = 0;
	int oldR = 0;
	int newR = 0;
	bool isLeft = false;

	bool win = false;
	bool lose = false;
	while (ProcessMessage() == 0) {
		ClearDrawScreen();
		GetHitKeyStateAll(keystate);

		DrawBox(0, 0, sw, sh, 0xaaffff, true);

		int mx;
		int my;
		if (!win && !lose)
		{
			mx = 0;
			my = 0;
			if (keystate[KEY_INPUT_LEFT]) {
				isLeft = true;
			}
			else if (keystate[KEY_INPUT_RIGHT]) {
				isLeft = false;
			}

			if (isLeft) {
				mx = cap.posA.x;
				my = cap.posA.y;
			}
			else {
				mx = cap.posB.x;
				my = cap.posB.y;
			}
			if (keystate[KEY_INPUT_Z]) {

				angle = -0.05f;
			}
			else if (keystate[KEY_INPUT_X]) {

				angle = 0.05f;
			}
			else {
				angle = 0.0f;
			}

			//当たり判定を完成させて当たったときの反応を書いてください
			//if(IsHit(cap,circle)){
			//	反応をここに書いてください。
			//}

			//カプセル回転
			Matrix rotMat = RotatePosition(Position2(mx, my), angle);
			cap.posA = MultipleVec(rotMat, cap.posA);
			cap.posB = MultipleVec(rotMat, cap.posB);
			if (cap.posA.x <= 32 || cap.posB.x >= sw - 32 || cap.posA.y <= 32 || cap.posB.y <= 32)
			{
				Matrix rotMat = RotatePosition(Position2(mx, my), -angle);
				cap.posA = MultipleVec(rotMat, cap.posA);
				cap.posB = MultipleVec(rotMat, cap.posB);
			}
			if (cap.posA.y <= 48 && cap.posB.y <= 48)
			{
				win = true;
			}
			else
			{
				win = false;
			}

			rock.pos.y++;
		}

		if ((abs(cap.posA.x - cap.posB.x) <= 5) || (cap.posA.y > sh + 16 && cap.posB.y > sh + 16))
		{
			lose = true;
		}

		if (lose)
		{
			cap.posA.y += 6;
			cap.posB.y += 6;
		}

		//背景の描画
		//滝
		int chipIdx = (frame / 4) % 3;
		constexpr int dest_chip_size = 32;
		int destY = 48;
		while (destY < sh) {
			int destX = 0;
			while (destX < sw) {
				DrawRectExtendGraph(
					destX, destY, destX + dest_chip_size, destY + dest_chip_size,
					chipIdx * 16, 0,
					16, 16,
					cascadeH, false);
				destX += dest_chip_size;
			}
			destY += dest_chip_size;
		}

		DrawRectGraph(0, 0, 96, 0, 32, 32, chipH, true);
		DrawRectGraph(sw - 32, 0, 32, 0, 32, 32, chipH, true);
		DrawRectGraph(0, 32, 96, 32, 32, 32, chipH, true);
		DrawRectGraph(sw - 32, 32, 32, 32, 32, 32, chipH, true);
		destY = 64;
		while (destY < sh) {
			DrawRectGraph(0, destY, 96, 64, 32, 32, chipH, true);
			DrawRectGraph(sw - 32, destY, 32, 64, 32, 32, chipH, true);
			destY += dest_chip_size;
		}
		DrawGraph(rock.pos.x,rock.pos.y,rockH,true);

		DrawWood(cap, woodH);
		if (!win && !lose)
		{
			DrawCircle(mx, my, 30, 0xff0000, false, 3);
			++frame;
		}
		if (win)
		{
			DrawRotaGraph(sw / 2, sh / 2, 1.0, 0.0, GameClear, true);
		}
		if (lose)
		{
			DrawRotaGraph(sw / 2, sh / 2, 0.75, 0.0, GameOver, true);
		}
	
		oldR = newR;
		newR = keystate[KEY_INPUT_R];
		if (!oldR && newR)
		{
			angle = 0.0f;

			frame = 0;
	
			isLeft = false;

			win = false;
			lose = false;

			cap.posA = Position2((sw - wdW) / 2, sh - 100);
			cap.posB = Position2((sw - wdW) / 2 + wdW, sh - 100);

			rock.pos = Position2(32 + GetRand(sw - 64), 32);
		}

		ScreenFlip();
	}

	DxLib_End();
}