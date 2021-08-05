#include <DxLib.h>
#include <cmath>
#include "Geometry.h"

///ìñÇΩÇËîªíËä÷êî
///@param posA AÇÃç¿ïW
///@param radiusA AÇÃîºåa
///@param posB BÇÃç¿ïW
///@param radiusB BÇÃîºåa
bool IsHit(const Position2& posA, float radiusA, const Position2& posB,  float radiusB) {
	//ìñÇΩÇËîªíËÇé¿ëïÇµÇƒÇ≠ÇæÇ≥Ç¢
	return (radiusA + radiusB >= hypot((posA.x - posB.x), (posA.y - posB.y)));
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(true);
	SetMainWindowText("1916020_ìcíÜ ê^êl");
	if (DxLib_Init() != 0) {
		return -1;
	}

	int screen_ =  MakeScreen(660,500,false);
	Position2 scPos(-10,-10);
	int scFrame = 0;
	bool skFlag = false;
	//îwåióp
	int bgH[4];
	LoadDivGraph("img/bganim.png", 4, 4, 1, 256, 192, bgH);

	int skyH = LoadGraph("img/sky.png");
	int sky2H = LoadGraph("img/sky2.png");

	auto bulletH=LoadGraph("img/bullet.png");
	int playerH[10];
	LoadDivGraph("img/player.png", 10, 5, 2, 16, 24,playerH );

	int enemyH[2];
	LoadDivGraph("img/enemy.png", 2, 2, 1, 32, 32, enemyH);

	struct Bullet {
		Position2 pos;//ç¿ïW
		Vector2 vel;//ë¨ìx
		bool isActive = false;//ê∂Ç´ÇƒÇÈÇ©Å`ÅH
	};

	//íeÇÃîºåa
	float bulletRadius = 5.0f;

	//é©ã@ÇÃîºåa
	float playerRadius = 10.0f;

	//ìKìñÇ…256å¬Ç≠ÇÁÇ¢çÏÇ¡Ç∆Ç≠
	Bullet bullets[256];

	Position2 enemypos(320,25);//ìGç¿ïW
	Position2 playerpos(320, 400);//é©ã@ç¿ïW

	unsigned int frame = 0;//ÉtÉåÅ[ÉÄä«óùóp

	char keystate[256];
	bool isDebugMode = false;
	int skyy = 0;
	int skyy2 = 0;
	int bgidx = 0;

	while (ProcessMessage() == 0) {
		SetDrawScreen(screen_);
		ClearDrawScreen();

		GetHitKeyStateAll(keystate);

		isDebugMode = keystate[KEY_INPUT_P];

		//îwåi
		DrawExtendGraph(0,0, 660 , 500, bgH[bgidx / 8], false);
		bgidx = (bgidx + 1) % 32;

		//SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
		skyy = (skyy + 1) % 490 ;
		skyy2 = (skyy2 + 2) % 490 ;
		DrawExtendGraph(0, skyy , 660, skyy + 480, skyH, true);
		DrawExtendGraph(0, skyy - 480, 660, skyy, skyH, true);
		DrawExtendGraph(0, skyy2 , 660, skyy2 + 480, sky2H, true);
		DrawExtendGraph(0, skyy2 - 480, 660, skyy2 , sky2H, true);


		//ÉvÉåÉCÉÑÅ[
		if (keystate[KEY_INPUT_RIGHT]) {
			playerpos.x = min(640,playerpos.x+4);
		}
		else if (keystate[KEY_INPUT_LEFT]) {
			playerpos.x = max(0,playerpos.x-4);
		}
		if (keystate[KEY_INPUT_UP]) {
			playerpos.y = max(0,playerpos.y-4);
		}else if (keystate[KEY_INPUT_DOWN]) {
			playerpos.y = min(480,playerpos.y+4);
		}

		int pidx = (frame/4 % 2)*5+3;
		DrawRotaGraph(playerpos.x + 10, playerpos.y + 10, 2.0f, 0.0f, playerH[pidx], true);
		if (isDebugMode) {
			//é©ã@ÇÃñ{ëÃ(ìñÇΩÇËîªíË)
			DrawCircle(playerpos.x + 10, playerpos.y + 10, playerRadius, 0xffaaaa, false, 3);
		}

		//íeî≠éÀ
		if (frame < 300)
		{
			if (frame % 12 == 0) {
				for (auto& b : bullets) {
					if (!b.isActive) {
						b.pos = enemypos;
						b.vel = (playerpos - enemypos).Normalized() * 5;
						b.isActive = true;
						break;
					}
				}
			}
		}
		if (301 < frame && frame < 600)
		{
			int bCount = 0;
			if (frame % 12 == 0) {
				while (bCount < 3)
				{
					for (auto& b : bullets) {
						if (!b.isActive)
						{
							Vector2 tmpPos;
							switch (bCount)
							{
							case 0:
								b.pos = enemypos;
								b.vel = (playerpos - enemypos).Normalized() * 5;
								b.isActive = true;
								bCount++;
								break;
							case 1:
								tmpPos = playerpos;
								tmpPos.x -= 50;
								b.pos = enemypos;
								b.vel = (tmpPos - enemypos).Normalized() * 5;
								b.isActive = true;
								bCount++;
								break;
							case 2:
								tmpPos = playerpos;
								tmpPos.x += 50;
								b.pos = enemypos;
								b.vel = (tmpPos - enemypos).Normalized() * 5;
								b.isActive = true;
								bCount++;
								break;
							default:
								break;
							}
						}
					}
				}
				bCount = 0;
			}
		}

		//íeÇÃçXêVÇ®ÇÊÇ—ï\é¶
		for (auto& b : bullets) {
			if (!b.isActive) {
				continue;
			}

			//íeÇÃåªç›ç¿ïWÇ…íeÇÃåªç›ë¨ìxÇâ¡éZÇµÇƒÇ≠ÇæÇ≥Ç¢
			b.pos = b.pos + b.vel;
			float angle = atan2(b.vel.y, b.vel.x);
			//íeÇÃäpìxÇatan2Ç≈åvéZÇµÇƒÇ≠ÇæÇ≥Ç¢ÅBangleÇ…ílÇì¸ÇÍÇÈÇÒÇæÇÊÉIÉD
			
			DrawRotaGraph(b.pos.x +10, b.pos.y + 10,1.0f,angle, bulletH, true);
			
			if (isDebugMode) {
				//íeÇÃñ{ëÃ(ìñÇΩÇËîªíË)
				DrawCircle(b.pos.x + 10, b.pos.y + 10, bulletRadius, 0x0000ff, false, 3);
			}
			//íeÇéEÇ∑
			if (b.pos.x + 16 < 0 || b.pos.x - 16 > 640 ||
				b.pos.y + 24 < 0 || b.pos.y - 24 > 480) {
				b.isActive = false;
			}

			//Ç†ÇΩÇËÅI
			//Å´ÇÃIsHitÇÕé¿ëïÇèëÇ¢ÇƒÇ‹ÇπÇÒÅBé©ï™Ç≈èëÇ¢ÇƒÇ≠ÇæÇ≥Ç¢ÅB
			if (IsHit(b.pos, bulletRadius, playerpos, playerRadius)) {
				//ìñÇΩÇ¡ÇΩîΩâûÇèëÇ¢ÇƒÇ≠ÇæÇ≥Ç¢ÅB
				skFlag = true;
				scFrame = 0;
			}
		}
		if (skFlag == true)
		{
			scPos = {-10,-10};
			scPos = scPos + (GetRand(10) - 5);
			scFrame++;
			if (scFrame > 15)
			{
				scPos = { -10,-10 };
				skFlag = false;
			}
		}

		//ìGÇÃï\é¶
		enemypos.x = abs((int)((frame+320) % 1280) - 640);
		int eidx = (frame / 4 % 2);
		DrawRotaGraph(enemypos.x, enemypos.y, 2.0f, 0.0f, enemyH[eidx],true);

		if (isDebugMode) {
			//ìGÇÃñ{ëÃ(ìñÇΩÇËîªíË)
			DrawCircle(enemypos.x + 10, enemypos.y + 10, 5, 0xffffff, false, 3);
		}


		SetDrawScreen(DX_SCREEN_BACK);
		ClsDrawScreen();
		DrawGraph(scPos.x,scPos.y,screen_,false);

		++frame;
		ScreenFlip();
	}

	DxLib_End();

	return 0;
}