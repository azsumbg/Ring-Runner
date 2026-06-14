#include "framework.h"
#include "Ring Runner.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "FCheck.h"
#include "errh.h"
#include "D2BMPLOADER.h"
#include "gifresizer.h"
#include "ringhelp.h"
#include <chrono>
#include <clocale>
#include <objbase.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "D2BMPLOADER.lib")
#pragma comment(lib, "gifresizer.lib")
#pragma comment(lib, "ringhelp.lib")

constexpr wchar_t bWinClassName[]{ L"son_run" };
constexpr char tmp_file[] { ".\\res\\data\\temp.dat" };
constexpr wchar_t Ltmp_file[]{ L".\\res\\data\\temp.dat" };
constexpr wchar_t sound_file[]{ L".\\res\\snd\\main.wav" };
constexpr wchar_t help_file[]{ L".\\res\\data\\help.dat" };
constexpr wchar_t record_file[]{ L".\\res\\data\\record.dat" };
constexpr wchar_t save_file[]{ L".\\res\\data\\save.dat" };

constexpr int mNew{ 1001 };
constexpr int mSpeed{ 1002 };
constexpr int mExit{ 1003 };
constexpr int mSave{ 1004 };
constexpr int mLoad{ 1005 };
constexpr int mHoF{ 1006 };

constexpr int record{ 2001 };
constexpr int no_record{ 2002 };
constexpr int first_record{ 2003 };

WNDCLASS bWinClass{};
HINSTANCE bIns{ nullptr };
HWND bHwnd{ nullptr };
HICON bIcon{ nullptr };
HCURSOR bCursor{ nullptr };
HCURSOR outCursor{ nullptr };
HMENU bBar{ nullptr };
HMENU bMain{ nullptr };
HMENU bStore{ nullptr };
HDC PaintDC{ nullptr };
PAINTSTRUCT bPaint{};
MSG bMsg{};
BOOL bRet{};

POINT cur_pos{};

float x_scale{ 0 };
float y_scale{ 0 };

UINT bTimer{ 0 };

D2D1_RECT_F b1Rect{ 50.0f, 10.0f, scr_width / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b2Rect{ scr_width / 3.0f + 50.0f, 10.0f, scr_width * 2.0f / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b3Rect{ scr_width * 2.0f / 3.0f + 50.0f, 10.0f, scr_width - 50.0f, 40.0f };

D2D1_RECT_F b1TxtRect{ 80.0f, 15.0f, scr_width / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b2TxtRect{ scr_width / 3.0f + 80.0f, 15.0f, scr_width * 2.0f / 3.0f - 50.0f, 40.0f };
D2D1_RECT_F b3TxtRect{ scr_width * 2.0f / 3.0f + 80.0f, 15.0f, scr_width - 50.0f, 40.0f };

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };

ID2D1RadialGradientBrush* b1BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b2BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b3BckgBrush{ nullptr };

ID2D1SolidColorBrush* statBrush{ nullptr };
ID2D1SolidColorBrush* txtBrush{ nullptr };
ID2D1SolidColorBrush* inactBrush{ nullptr };
ID2D1SolidColorBrush* hgltBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmFormat{ nullptr };
IDWriteTextFormat* midFormat{ nullptr };
IDWriteTextFormat* bigFormat{ nullptr };

ID2D1Bitmap* bmpLogo{ nullptr };
ID2D1Bitmap* bmpLevelUp{ nullptr };
ID2D1Bitmap* bmpLoose{ nullptr };
ID2D1Bitmap* bmpRecord{ nullptr };
ID2D1Bitmap* bmpRing{ nullptr };
ID2D1Bitmap* bmpSmallRing{ nullptr };
ID2D1Bitmap* bmpSpit{ nullptr };
ID2D1Bitmap* bmpWin{ nullptr };
ID2D1Bitmap* bmpRIP{ nullptr };

ID2D1Bitmap* bmpArmorIcon{ nullptr };
ID2D1Bitmap* bmpChest{ nullptr };
ID2D1Bitmap* bmpPileIcon{ nullptr };
ID2D1Bitmap* bmpLifeIcon{ nullptr };

ID2D1Bitmap* bmpFlat{ nullptr };
ID2D1Bitmap* bmpRightSlope{ nullptr };
ID2D1Bitmap* bmpLeftSlope{ nullptr };

ID2D1Bitmap* bmpBackground[64]{ nullptr };
ID2D1Bitmap* bmpIntro[93]{ nullptr };
ID2D1Bitmap* bmpPortal[24]{ nullptr };

ID2D1Bitmap* bmpHeroL[6]{ nullptr };
ID2D1Bitmap* bmpHeroR[6]{ nullptr };

ID2D1Bitmap* bmpFemL[8]{ nullptr };
ID2D1Bitmap* bmpFemR[8]{ nullptr };

ID2D1Bitmap* bmpMaleL[8]{ nullptr };
ID2D1Bitmap* bmpMaleR[8]{ nullptr };

ID2D1Bitmap* bmpFlyerL[11]{ nullptr };
ID2D1Bitmap* bmpFlyerR[11]{ nullptr };

////////////////////////////////////////////////////////////////

bool pause = false;
bool show_help = false;
bool sound = true;
bool in_client = true;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;

bool hero_killed = false;
D2D1_RECT_F RipRect{};

bool portal_opened = false;
D2D1_RECT_F PortalRect{};

bool name_set = false;
wchar_t current_player[16]{ L"TARLYO" };

float speed = 1.0f;
float distance = 240.0f;
int score = 0;

bool need_left = false;
bool need_right = false;

dirs nature_dir = dirs::stop;

////////////////////////////////////////////////////////////////

dll::RANDIT RandIt{};

dll::FIELD* Intro{ nullptr };
dll::FIELD* Background{ nullptr };

std::vector<dll::FIELD*>vMainGround;
std::vector<dll::FIELD*>vTiles;

dll::HERO* Hero{ nullptr };

std::vector<dll::EVIL*> vEvils;

std::vector<dll::SHOT*> vEvilShots;
std::vector<dll::SHOT*> vHeroShots;
std::vector<dll::PROTON*>vRings;

std::vector<FADING> vAssets;

/////////////////////////////////////////////////////////////////

template<typename T>concept HasRelease = requires(T check)
{
	check.Release();
};
template<HasRelease T>bool FreeMem(T** var)
{
	if ((*var))
	{
		(*var)->Release();
		(*var) = nullptr;
		return true;
	}

	return false;
};
int portal_frame()
{
	static int frame = 0;
	static int frame_delay = 3;

	--frame_delay;
	if (frame_delay <= 0)
	{
		frame_delay = 3;
		++frame;
		if (frame > 23)frame = 0;
	}

	return frame;
}
void LogErr(const wchar_t* what)
{
	std::wofstream err{ L".\\res\\data\\error.log",std::ios::app };

	err << what << L" time stamp: " << std::chrono::system_clock::now() << std::endl;
	
	err.close();
}
void ReleaseResources()
{
	if (!FreeMem(&iFactory))LogErr(L"Error releasing D2D1 main Factory !");
	if (!FreeMem(&Draw))LogErr(L"Error releasing D2D1 hwndRenderTarget !");
	if (!FreeMem(&b1BckgBrush))LogErr(L"Error releasing D2D1 b1BckgBrush !");
	if (!FreeMem(&b2BckgBrush))LogErr(L"Error releasing D2D1 b2BckgBrush !");
	if (!FreeMem(&b3BckgBrush))LogErr(L"Error releasing D2D1 b3BckgBrush !");
	if (!FreeMem(&statBrush))LogErr(L"Error releasing D2D1 statBrush !");
	if (!FreeMem(&txtBrush))LogErr(L"Error releasing D2D1 txtBrush !");
	if (!FreeMem(&inactBrush))LogErr(L"Error releasing D2D1 inactBrush !");
	if (!FreeMem(&hgltBrush))LogErr(L"Error releasing D2D1 hgltBrush !");

	if (!FreeMem(&iWriteFactory))LogErr(L"Error releasing D2D1 main write Factory !");
	if (!FreeMem(&nrmFormat))LogErr(L"Error releasing D2D1 main write nrmTextFormat !");
	if (!FreeMem(&midFormat))LogErr(L"Error releasing D2D1 main write midTextFormat !");
	if (!FreeMem(&bigFormat))LogErr(L"Error releasing D2D1 main write bigTextFormat !");

	if (!FreeMem(&bmpLogo))LogErr(L"Error releasing D2D1 main write bmpLogo !");
	if (!FreeMem(&bmpLevelUp))LogErr(L"Error releasing D2D1 main write bmpLevelUp !");
	if (!FreeMem(&bmpLoose))LogErr(L"Error releasing D2D1 main write bmpLoose !");
	if (!FreeMem(&bmpRecord))LogErr(L"Error releasing D2D1 main write bmpRecord !");
	if (!FreeMem(&bmpRing))LogErr(L"Error releasing D2D1 main write bmpRing !");
	if (!FreeMem(&bmpSmallRing))LogErr(L"Error releasing D2D1 main write bmpSmallRing !");
	if (!FreeMem(&bmpSpit))LogErr(L"Error releasing D2D1 main write bmpSpit !");
	if (!FreeMem(&bmpWin))LogErr(L"Error releasing D2D1 main write bmpWin !");
	if (!FreeMem(&bmpRIP))LogErr(L"Error releasing D2D1 main write bmpRIP !");

	if (!FreeMem(&bmpChest))LogErr(L"Error releasing D2D1 main write bmpChest !");
	if (!FreeMem(&bmpArmorIcon))LogErr(L"Error releasing D2D1 main write bmpArmorIcon !");
	if (!FreeMem(&bmpPileIcon))LogErr(L"Error releasing D2D1 main write bmpPileIcon !");
	if (!FreeMem(&bmpLifeIcon))LogErr(L"Error releasing D2D1 main write bmpLifeIcon !");

	if (!FreeMem(&bmpFlat))LogErr(L"Error releasing D2D1 main write bmpFlat !");
	if (!FreeMem(&bmpRightSlope))LogErr(L"Error releasing D2D1 main write bmpRightSlope !");
	if (!FreeMem(&bmpLeftSlope))LogErr(L"Error releasing D2D1 main write bmpLeftSlope !");

	for (int i = 0; i < 64; ++i)if (!FreeMem(&bmpBackground[i]))LogErr(L"Error releasing D2D1 main write bmpBackground !");
	for (int i = 0; i < 93; ++i)if (!FreeMem(&bmpIntro[i]))LogErr(L"Error releasing D2D1 main write bmpIntro !");
	for (int i = 0; i < 24; ++i)if (!FreeMem(&bmpPortal[i]))LogErr(L"Error releasing D2D1 main write bmpPortal !");

	for (int i = 0; i < 6; ++i)if (!FreeMem(&bmpHeroL[i]))LogErr(L"Error releasing D2D1 main write bmpHeroL !");
	for (int i = 0; i < 6; ++i)if (!FreeMem(&bmpHeroR[i]))LogErr(L"Error releasing D2D1 main write bmpHeroR !");

	for (int i = 0; i < 8; ++i)if (!FreeMem(&bmpFemL[i]))LogErr(L"Error releasing D2D1 main write bmpFemL !");
	for (int i = 0; i < 8; ++i)if (!FreeMem(&bmpFemR[i]))LogErr(L"Error releasing D2D1 main write bmpFemR !");

	for (int i = 0; i < 8; ++i)if (!FreeMem(&bmpMaleL[i]))LogErr(L"Error releasing D2D1 main write bmpMaleL !");
	for (int i = 0; i < 8; ++i)if (!FreeMem(&bmpMaleR[i]))LogErr(L"Error releasing D2D1 main write bmpMaleR !");

	for (int i = 0; i < 11; ++i)if (!FreeMem(&bmpFlyerL[i]))LogErr(L"Error releasing D2D1 main write bmpFlyerL !");
	for (int i = 0; i < 11; ++i)if (!FreeMem(&bmpFlyerR[i]))LogErr(L"Error releasing D2D1 main write bmpFlyerR !");
}
void ErrExit(int what)
{
	MessageBeep(MB_ICONERROR);
	MessageBox(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

	ReleaseResources();
	std::remove(tmp_file);
	exit(1);
}
BOOL CheckRecord()
{
	if (score < 1)return no_record;

	int result{ 0 };
	CheckFile(record_file, &result);
	if (result == FILE_NOT_EXIST)
	{
		std::wofstream rec(record_file);
		rec << score << std::endl;
		for (int i = 0; i < 16; ++i)rec << current_player[i] << std::endl;
		rec.close();
		return first_record;
	}
	else
	{
		std::wifstream check(record_file);
		check >> result;
		check.close();
	}

	if (result < score)
	{
		std::wofstream rec(record_file);
		rec << score << std::endl;
		for (int i = 0; i < 16; ++i)rec << current_player[i] << std::endl;
		rec.close();
		return record;
	}

	return no_record;
}
void GameOver()
{
	PlaySound(NULL, NULL, NULL);
	KillTimer(bHwnd, bTimer);

	switch (CheckRecord())
	{
	case no_record:
		Draw->BeginDraw();
		Draw->DrawBitmap(bmpLoose, D2D1::RectF(0, 0, scr_width, scr_height));
		Draw->EndDraw();
		if (sound)PlaySound(L".\\res\\snd\\loose.wav", NULL, SND_SYNC);
		else Sleep(4000);
		break;

	case first_record:
		Draw->BeginDraw();
		Draw->DrawBitmap(bmpWin, D2D1::RectF(0, 0, scr_width, scr_height));
		Draw->EndDraw();
		if (sound)PlaySound(L".\\res\\snd\\win.wav", NULL, SND_SYNC);
		else Sleep(4000);
		break;

	case record:
		Draw->BeginDraw();
		Draw->DrawBitmap(bmpRecord, D2D1::RectF(0, 0, scr_width, scr_height));
		Draw->EndDraw();
		if (sound)PlaySound(L".\\res\\snd\\record.wav", NULL, SND_SYNC);
		else Sleep(4000);
		break;
	}

	bMsg.message = WM_QUIT;
	bMsg.wParam = 0;
}
void InitGame()
{
	speed = 1.0f;
	distance = 2400.0f;

	wcscpy_s(current_player, L"TARLYO");
	name_set = false;
	
	FreeMem(&Intro);
	Intro = dll::FIELD::create(fields::intro, 0, 0);

	FreeMem(&Background);
	Background = dll::FIELD::create(fields::background, 0, 50.0f);
	
	hero_killed = false;
	need_left = false;
	need_right = false;
	portal_opened = false;

	nature_dir = dirs::stop;
	
	if (!vTiles.empty())
		for (int i = 0; i < vTiles.size(); ++i)
			if (!FreeMem(&vTiles[i]))LogErr(L"Error releasing vTiles !");
	vTiles.clear();

	if (!vMainGround.empty())
		for (int i = 0; i < vMainGround.size(); ++i)
			if (!FreeMem(&vMainGround[i]))LogErr(L"Error releasing vMainGround !");
	vMainGround.clear();
	for (float sx = -scr_width; sx < 2.0f * scr_width; sx += 100.0f)
		vMainGround.push_back(dll::FIELD::create(fields::flat_ground, sx, ground));

	if (Hero)Hero->Release();
	Hero = dll::HERO::create(scr_width / 2.0f - 50.0f, ground - 35.0f);

	if (!vEvils.empty())
		for (int i = 0; i < vEvils.size(); ++i)
			if (!FreeMem(&vEvils[i]))LogErr(L"Error releasing vEvils !");
	vEvils.clear();

	if (!vEvilShots.empty())
		for (int i = 0; i < vEvilShots.size(); ++i)
			if (!FreeMem(&vEvilShots[i]))LogErr(L"Error releasing vEvilShots !");
	vEvilShots.clear();

	if (!vHeroShots.empty())
		for (int i = 0; i < vHeroShots.size(); ++i)
			if (!FreeMem(&vHeroShots[i]))LogErr(L"Error releasing vHeroShots !");
	vHeroShots.clear();

	if (!vRings.empty())for (int i = 0; i < vRings.size(); ++i)delete vRings[i];
	vRings.clear();

	vAssets.clear();
}
void LevelUp()
{
	Draw->BeginDraw();
	Draw->DrawBitmap(bmpLevelUp, D2D1::RectF(0, 0, scr_width, scr_height));
	Draw->EndDraw();

	if (sound)
	{
		PlaySound(NULL, NULL, NULL);
		PlaySound(L".\\res\\snd\\levelup.wav", NULL, SND_SYNC);
		PlaySound(sound_file, NULL, SND_ASYNC | SND_LOOP);
	}
	else Sleep(3500);

	++speed;
	score += 10 * (int)(speed);

	distance = 2400.0f + speed + 50.0f;

	FreeMem(&Background);
	Background = dll::FIELD::create(fields::background, 0, 50.0f);

	hero_killed = false;
	need_left = false;
	need_right = false;
	portal_opened = false;

	nature_dir = dirs::stop;

	if (!vTiles.empty())
		for (int i = 0; i < vTiles.size(); ++i)
			if (!FreeMem(&vTiles[i]))LogErr(L"Error releasing vTiles !");
	vTiles.clear();

	if (!vMainGround.empty())
		for (int i = 0; i < vMainGround.size(); ++i)
			if (!FreeMem(&vMainGround[i]))LogErr(L"Error releasing vMainGround !");
	vMainGround.clear();
	for (float sx = -scr_width; sx < 2.0f * scr_width; sx += 100.0f)
		vMainGround.push_back(dll::FIELD::create(fields::flat_ground, sx, ground));

	if (Hero)Hero->Release();
	Hero = dll::HERO::create(scr_width / 2.0f - 50.0f, ground - 35.0f);

	if (!vEvils.empty())
		for (int i = 0; i < vEvils.size(); ++i)
			if (!FreeMem(&vEvils[i]))LogErr(L"Error releasing vEvils !");
	vEvils.clear();

	if (!vEvilShots.empty())
		for (int i = 0; i < vEvilShots.size(); ++i)
			if (!FreeMem(&vEvilShots[i]))LogErr(L"Error releasing vEvilShots !");
	vEvilShots.clear();

	if (!vHeroShots.empty())
		for (int i = 0; i < vHeroShots.size(); ++i)
			if (!FreeMem(&vHeroShots[i]))LogErr(L"Error releasing vHeroShots !");
	vHeroShots.clear();

	if (!vRings.empty())for (int i = 0; i < vRings.size(); ++i)delete vRings[i];
	vRings.clear();

	vAssets.clear();
}
void ShowRecord()
{
	int result{ 0 };
	CheckFile(record_file, &result);

	if (result == FILE_NOT_EXIST)
	{
		if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
		MessageBox(bHwnd, L"Все ще няма рекорд на играта !\n\nПостарай се повече !", L"Липсва файл",
			MB_OK | MB_APPLMODAL | MB_ICONASTERISK);
		return;
	}

	std::wifstream rec(record_file);

	wchar_t txt[100]{ L"НАЙ-ДОБЪР ИГРАЧ: " };
	wchar_t saved_info[16]{ L"\0" };
	
	rec >> result;

	for (int i = 0; i < 16; ++i)
	{
		int letter = 0;
		rec >> letter;
		saved_info[i] = static_cast<wchar_t>(letter);
	}

	wcscat_s(txt, saved_info);
	wcscat_s(txt, L"\nСВЕТОВЕН РЕКОРД: ");

	wsprintf(saved_info, L"%d", result);
	wcscat_s(txt, saved_info);

	result = 0;

	for (int i = 0; i < 100; ++i)
	{
		if (txt[i] != '\0')++result;
		else break;
	}

	Draw->BeginDraw();
	Draw->DrawBitmap(bmpIntro[Intro->get_frame()], Intro->get_rect());
	if (hgltBrush && midFormat)Draw->DrawTextW(txt, result, midFormat, D2D1::RectF(100.0f, 100.0f, scr_width, scr_height), 
		hgltBrush);
	Draw->EndDraw();

	if (sound)mciSendString(L"play .\\res\\snd\\showrec.wav", NULL, NULL, NULL);
	Sleep(4000);
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
	switch (ReceivedMsg)
	{
	case WM_INITDIALOG:
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(bIcon));
		return true;

	case WM_CLOSE:
		EndDialog(hwnd, IDCANCEL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;

		case IDOK:
			if (GetDlgItemText(hwnd, IDC_NAME, current_player, 16) < 1)
			{
				wcscpy_s(current_player, L"TARLYO");
				if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
				MessageBox(bHwnd, L"Ха, ха, ха ! Забрави си името !", L"Забраватор", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
				EndDialog(hwnd, IDCANCEL);
				break;
			}
			EndDialog(hwnd, IDOK);
			break;
		}
	}

	return (INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
	switch (ReceivedMsg)
	{
	case WM_CREATE:
		if (bIns)
		{
			SetTimer(hwnd, (UINT_PTR)(bTimer), 100, NULL);
			bBar = CreateMenu();
			bMain = CreateMenu();
			bStore = CreateMenu();

			AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
			AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

			AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
			AppendMenu(bMain, MF_STRING, mSpeed, L"Турбо режим");
			AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
			AppendMenu(bMain, MF_STRING, mExit, L"Изход");

			AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
			AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
			AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
			AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

			SetMenu(hwnd, bBar);

			InitGame();
		}
		break;

	case WM_CLOSE:
		pause = true;
		if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
		if (MessageBox(hwnd, L"Ако излезеш, губиш тази игра !\n\nНаистина ли излизаш ?", L"Изход",
			MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
		{
			pause = false;
			break;
		}
		GameOver();
		break;

	case WM_PAINT:
		PaintDC = BeginPaint(hwnd, &bPaint);
		FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(10, 10, 10)));
		EndPaint(hwnd, &bPaint);
		break;

	case WM_SETCURSOR:
		GetCursorPos(&cur_pos);
		ScreenToClient(hwnd, &cur_pos);
		if (LOWORD(lParam) == HTCLIENT)
		{
			if (!in_client)
			{
				in_client = true;
				pause = false;
			}

			if (cur_pos.y * y_scale <= 50)
			{
				if (cur_pos.x * x_scale >= b1Rect.left && cur_pos.x * x_scale <= b1Rect.right)
				{
					if (!b1Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = true;
						b2Hglt = false;
						b3Hglt = false;
					}
				}
				else if (cur_pos.x * x_scale >= b2Rect.left && cur_pos.x * x_scale <= b2Rect.right)
				{
					if (!b2Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = true;
						b3Hglt = false;
					}
				}
				else if (cur_pos.x * x_scale >= b3Rect.left && cur_pos.x * x_scale <= b3Rect.right)
				{
					if (!b3Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = false;
						b3Hglt = true;
					}
				}
				else if (b1Hglt || b2Hglt || b3Hglt)
				{
					if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
					b1Hglt = false;
					b2Hglt = false;
					b3Hglt = false;
				}

				SetCursor(outCursor);
				return true;
			}
			else if (b1Hglt || b2Hglt || b3Hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
				b1Hglt = false;
				b2Hglt = false;
				b3Hglt = false;
			}

			SetCursor(bCursor);

			return true;
		}
		else
		{
			if (in_client)
			{
				in_client = false;
				pause = true;
			}

			if (b1Hglt || b2Hglt || b3Hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
				b1Hglt = false;
				b2Hglt = false;
				b3Hglt = false;
			}

			SetCursor(LoadCursor(NULL, IDC_ARROW));

			return true;
		}
		break;

	case WM_LBUTTONDOWN:
		if (HIWORD(lParam) * y_scale > sky)
		{
			if (!pause)
			{
				pause = true;
				break;
			}
			else
			{
				if (show_help)break;
				pause = false;
				break;
			}
		}
		else
		{
			if (LOWORD(lParam) * x_scale >= b1Rect.left && LOWORD(lParam) * x_scale <= b1Rect.right)
			{
				if (name_set)
				{
					if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
					break;
				}
				
				if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
				if (DialogBox(bIns, MAKEINTRESOURCE(IDD_PLAYER), hwnd, &DlgProc) == IDOK)name_set = true;
				break;
			}
			if (LOWORD(lParam) * x_scale >= b2Rect.left && LOWORD(lParam) * x_scale <= b2Rect.right)
			{
				mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);

				if (sound)
				{
					sound = false;
					PlaySound(NULL, NULL, NULL);
					break;
				}
				else
				{
					sound = true;
					PlaySound(sound_file, NULL, SND_ASYNC | SND_LOOP);
					break;
				}
			}
			if (LOWORD(lParam) * x_scale >= b3Rect.left && LOWORD(lParam) * x_scale <= b3Rect.right)
			{

			}
		}
		break;

	case WM_TIMER:
		if (Hero)
		{
			if (distance > 0)
				switch (nature_dir)
			{
			case dirs::left:
				distance--;
				break;

			case dirs::right:
				distance++;
				break;
			}
			else
			{
				if (!portal_opened)
				{
					portal_opened = true;
					
					float sx = scr_width + RandIt(200.0f, scr_width - 100.0f);

					PortalRect.left = sx;
					PortalRect.right = sx + 100.0f;
					PortalRect.top = ground - 100.0f;
					PortalRect.bottom = ground;

					if (sound)mciSendString(L"play .\\res\\snd\\portal.wav", NULL, NULL, NULL);
				}
			}
		}
		break;

	case WM_KEYDOWN:
		if (Hero)
		{
			switch (LOWORD(wParam))
			{
			case VK_LEFT:
				Hero->dir = dirs::left;
				Hero->action = actions::run;
				break;

			case VK_RIGHT:
				Hero->dir = dirs::right;
				Hero->action = actions::run;
				break;

			case VK_DOWN:
				Hero->action = actions::stop;
				Hero->dir = dirs::stop;
				break;

			case VK_UP:
				if (Hero->action != actions::jump && Hero->action != actions::fall)
				{
					dll::BAG<D2D1_RECT_F> bGrounds;
					
					if (!vMainGround.empty())
					{
						for (int i = 0; i < vMainGround.size(); ++i) bGrounds.push_back(vMainGround[i]->get_rect());
					}
					if (!vTiles.empty())
					{
						for (int i = 0; i < vTiles.size(); ++i) bGrounds.push_back(vTiles[i]->get_rect());
					}

					Hero->jump(bGrounds);
				}
				break;

			case VK_SHIFT:
				if (Hero)
				{
					if (score <= 0 || vEvils.empty())
					{
						if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
						break;
					}

					dll::BAG<D2D1_RECT_F> bEvils(vEvils.size());

					if (!vEvils.empty())
					{
						for (int i = 0; i < vEvils.size(); ++i)bEvils.push_back(vEvils[i]->get_rect());

						dll::sort(bEvils, Hero->get_rect());

						if (sound)mciSendString(L"play .\\res\\snd\\shoot.wav", NULL, NULL, NULL);
						--score;
						if (score < 0)score = 0;

						vHeroShots.push_back(dll::SHOT::create(Hero->center.x, Hero->center.y,
							bEvils[0].left + 15.0f, bEvils[0].top + 15.0f));
						vHeroShots.back()->damage = Hero->damage;
					}
				}
				break;
			}
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case mNew:
			pause = true;
			if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
			if (MessageBox(hwnd, L"Ако рестартираш, губиш тази игра !\n\nНаистина ли рестартираш ?", L"Рестарт",
				MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
			{
				pause = false;
				break;
			}
			InitGame();
			break;

		case mSpeed:
			pause = true;
			if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
			if (MessageBox(hwnd, L"Наистина ли включваш турбото ?", L"Турбо",
				MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
			{
				pause = false;
				break;
			}
			++speed;
			break;

		case mExit:
			SendMessage(hwnd, WM_CLOSE, NULL, NULL);
			break;


		case mHoF:
			pause = true;
			ShowRecord();
			pause = false;
			break;
		}
		break;

	default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
	}

	return (LRESULT)(FALSE);
}

void CreateResources()
{
	int result = 0;
	CheckFile(Ltmp_file, &result);
	if (result == FILE_EXIST)ErrExit(eStarted);
	else
	{
		std::wofstream start{ Ltmp_file };
		start << L"Game_started at: " << std::chrono::system_clock::now();
		start.close();
	}

	
	int win_x{ GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2.0f) };
	int win_y = 10;

	if (GetSystemMetrics(SM_CXSCREEN) < win_x + (int)(scr_width) ||
		GetSystemMetrics(SM_CYSCREEN) < win_y + (int)(scr_height))ErrExit(eScreen);
	
	bIcon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", ICON_BIG, 256, 256, LR_LOADFROMFILE));
	if (!bIcon)ErrExit(eIcon);
	bCursor = LoadCursorFromFile(L".\\res\\main.ani");
	outCursor = LoadCursorFromFile(L".\\res\\out.ani");
	if (!bCursor || !outCursor)ErrExit(eCursor);

	bWinClass.lpszClassName = bWinClassName;
	bWinClass.hInstance = bIns;
	bWinClass.hbrBackground = CreateSolidBrush(RGB(10, 10, 10));
	bWinClass.lpfnWndProc = &WinProc;
	bWinClass.hIcon = bIcon;
	bWinClass.hCursor = bCursor;
	bWinClass.style = CS_DROPSHADOW;

	if (!RegisterClass(&bWinClass))ErrExit(eClass);

	bHwnd = CreateWindow(bWinClassName, L"RING HUNTER", WS_CAPTION | WS_SYSMENU, win_x, win_y, (int)(scr_width),
		(int)(scr_height), NULL, NULL, bIns, NULL);
	if (!bHwnd)ErrExit(eWindow);
	else
	{
		ShowWindow(bHwnd, SW_SHOWDEFAULT);

		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
		if (hr != S_OK)
		{
			LogErr(L"Error creating D2D1 Factory !");
			ErrExit(eD2D);
		}

		if (iFactory)
		{
			hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
				D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);

			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1 HwndRenderTarget !");
				ErrExit(eD2D);
			}
		}
		
		if (Draw)
		{
			RECT DPIRect{};
			GetClientRect(bHwnd, &DPIRect);

			D2D1_SIZE_F DIPRect{ Draw->GetSize() };

			x_scale = DIPRect.width / (DPIRect.right - DPIRect.left);
			y_scale = DIPRect.height / (DPIRect.bottom - DPIRect.top);

			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Goldenrod), &statBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Lime), &txtBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Maroon), &inactBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), &hgltBrush);
			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1 text brushes !");
				ErrExit(eD2D);
			}

			D2D1_GRADIENT_STOP gStops[2]{};
			ID2D1GradientStopCollection* gColl{ nullptr };

			gStops[0].position = 0;
			gStops[0].color = D2D1::ColorF(D2D1::ColorF::Chocolate);
			gStops[1].position = 1.0f;
			gStops[1].color = D2D1::ColorF(D2D1::ColorF::Indigo);

			hr = Draw->CreateGradientStopCollection(gStops, 2, &gColl);
			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1 gradient stop collection for butBckg brushes !");
				ErrExit(eD2D);
			}
			if (gColl)
			{
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b1Rect.left +
					(b1Rect.right - b1Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b1Rect.right - b1Rect.left) / 2.0f, 25.0f),
					gColl, &b1BckgBrush);
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b2Rect.left +
					(b2Rect.right - b2Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b2Rect.right - b2Rect.left) / 2.0f, 25.0f),
					gColl, &b2BckgBrush);
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(b3Rect.left +
					(b3Rect.right - b3Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b3Rect.right - b3Rect.left) / 2.0f, 25.0f),
					gColl, &b3BckgBrush);
				if (hr != S_OK)
				{
					LogErr(L"Error creating D2D1 butBckg brushes !");
					ErrExit(eD2D);
				}
				FreeMem(&gColl);
			}

			HRESULT bmp_result{};

			bmpLogo = Load(L".\\res\\img\\logo.png", Draw, bmp_result);
			if (!bmpLogo)
			{
				LogErr(L"Error loading bmpLogo");
				ErrExit(eD2D);
			}
			bmpLevelUp = Load(L".\\res\\img\\field\\level.png", Draw, bmp_result);
			if (!bmpLevelUp)
			{
				LogErr(L"Error loading bmpLevelUp");
				ErrExit(eD2D);
			}
			bmpLoose = Load(L".\\res\\img\\Loose.png", Draw, bmp_result);
			if (!bmpLoose)
			{
				LogErr(L"Error loading bmpLoose");
				ErrExit(eD2D);
			}
			bmpRecord = Load(L".\\res\\img\\Record.png", Draw, bmp_result);
			if (!bmpRecord)
			{
				LogErr(L"Error loading bmpRecord");
				ErrExit(eD2D);
			}
			bmpRing = Load(L".\\res\\img\\Ring.png", Draw, bmp_result);
			if (!bmpRing)
			{
				LogErr(L"Error loading bmpRing");
				ErrExit(eD2D);
			}
			bmpSmallRing = Load(L".\\res\\img\\SmallRing.png", Draw, bmp_result);
			if (!bmpSmallRing)
			{
				LogErr(L"Error loading bmpSmallRing");
				ErrExit(eD2D);
			}
			bmpSpit = Load(L".\\res\\img\\spit.png", Draw, bmp_result);
			if (!bmpSpit)
			{
				LogErr(L"Error loading bmpSpit");
				ErrExit(eD2D);
			}
			bmpRIP = Load(L".\\res\\img\\RIP.png", Draw, bmp_result);
			if (!bmpRIP)
			{
				LogErr(L"Error loading bmpRIP");
				ErrExit(eD2D);
			}
			bmpWin = Load(L".\\res\\img\\Win.png", Draw, bmp_result);
			if (!bmpWin)
			{
				LogErr(L"Error loading bmpWin");
				ErrExit(eD2D);
			}

			bmpChest = Load(L".\\res\\img\\assets\\Chest.png", Draw, bmp_result);
			if (!bmpChest)
			{
				LogErr(L"Error loading bmpChest");
				ErrExit(eD2D);
			}
			bmpArmorIcon = Load(L".\\res\\img\\assets\\Armor.png", Draw, bmp_result);
			if (!bmpArmorIcon)
			{
				LogErr(L"Error loading bmpArmorIcon");
				ErrExit(eD2D);
			}
			bmpPileIcon = Load(L".\\res\\img\\assets\\pile.png", Draw, bmp_result);
			if (!bmpPileIcon)
			{
				LogErr(L"Error loading bmpPileIcon");
				ErrExit(eD2D);
			}
			bmpLifeIcon = Load(L".\\res\\img\\assets\\potion.png", Draw, bmp_result);
			if (!bmpLifeIcon)
			{
				LogErr(L"Error loading bmpLifeIcon");
				ErrExit(eD2D);
			}

			bmpFlat = Load(L".\\res\\img\\field\\flat.png", Draw, bmp_result);
			if (!bmpFlat)
			{
				LogErr(L"Error loading bmpFlat");
				ErrExit(eD2D);
			}
			bmpLeftSlope = Load(L".\\res\\img\\field\\left_slope.png", Draw, bmp_result);
			if (!bmpLeftSlope)
			{
				LogErr(L"Error loading bmpLeftSlope");
				ErrExit(eD2D);
			}
			bmpRightSlope = Load(L".\\res\\img\\field\\right_slope.png", Draw, bmp_result);
			if (!bmpRightSlope)
			{
				LogErr(L"Error loading bmpRightSlope");
				ErrExit(eD2D);
			}

			for (int i = 0; i < 64; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\field\\background\\0" };
				wchar_t add[5]{ L"\0" };

				if (i < 10)wcscat_s(name, L"0");

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpBackground[i] = Load(name, Draw, bmp_result);
				if (!bmpBackground[i])
				{
					LogErr(L"Error loading bmpBackground");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 93; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\field\\intro\\0" };
				wchar_t add[5]{ L"\0" };

				if (i < 10)wcscat_s(name, L"0");

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpIntro[i] = Load(name, Draw, bmp_result);
				if (!bmpIntro[i])
				{
					LogErr(L"Error loading bmpIntro");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 24; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\field\\portal\\0" };
				wchar_t add[5]{ L"\0" };

				if (i < 10)wcscat_s(name, L"0");

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpPortal[i] = Load(name, Draw, bmp_result);
				if (!bmpPortal[i])
				{
					LogErr(L"Error loading bmpPortal");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 6; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\hero\\l\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpHeroL[i] = Load(name, Draw, bmp_result);
				if (!bmpHeroL[i])
				{
					LogErr(L"Error loading bmpHeroL");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 6; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\hero\\r\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpHeroR[i] = Load(name, Draw, bmp_result);
				if (!bmpHeroR[i])
				{
					LogErr(L"Error loading bmpHeroR");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 8; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\zombies\\female\\l\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpFemL[i] = Load(name, Draw, bmp_result);
				if (!bmpFemL[i])
				{
					LogErr(L"Error loading bmpFemL");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 8; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\zombies\\female\\r\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpFemR[i] = Load(name, Draw, bmp_result);
				if (!bmpFemR[i])
				{
					LogErr(L"Error loading bmpFemR");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 8; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\zombies\\male\\l\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpMaleL[i] = Load(name, Draw, bmp_result);
				if (!bmpMaleL[i])
				{
					LogErr(L"Error loading bmpMaleL");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 8; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\zombies\\male\\r\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpMaleR[i] = Load(name, Draw, bmp_result);
				if (!bmpMaleR[i])
				{
					LogErr(L"Error loading bmpMaleR");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 11; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\zombies\\flyer\\l\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpFlyerL[i] = Load(name, Draw, bmp_result);
				if (!bmpFlyerL[i])
				{
					LogErr(L"Error loading bmpFlyerL");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 11; ++i)
			{
				wchar_t name[100]{ L".\\res\\img\\zombies\\flyer\\r\\" };
				wchar_t add[5]{ L"\0" };

				wsprintf(add, L"%d", i);
				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpFlyerR[i] = Load(name, Draw, bmp_result);
				if (!bmpFlyerR[i])
				{
					LogErr(L"Error loading bmpFlyerR");
					ErrExit(eD2D);
				}
			}
		}
		
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>
			(&iWriteFactory));
		if (hr != S_OK)
		{
			LogErr(L"Error creating D2D1 WriteFactory !");
			ErrExit(eD2D);
		}

		if (iWriteFactory)
		{
			hr = iWriteFactory->CreateTextFormat(L"Copperplate Gothic", NULL, DWRITE_FONT_WEIGHT_HEAVY, DWRITE_FONT_STYLE_OBLIQUE,
				DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"", &nrmFormat);
			hr = iWriteFactory->CreateTextFormat(L"Copperplate Gothic", NULL, DWRITE_FONT_WEIGHT_HEAVY, DWRITE_FONT_STYLE_OBLIQUE,
				DWRITE_FONT_STRETCH_NORMAL, 28.0f, L"", &midFormat);
			hr = iWriteFactory->CreateTextFormat(L"Copperplate Gothic", NULL, DWRITE_FONT_WEIGHT_HEAVY, DWRITE_FONT_STYLE_OBLIQUE,
				DWRITE_FONT_STRETCH_NORMAL, 72.0f, L"", &bigFormat);
			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1 WriteFactory text formats !");
				ErrExit(eD2D);
			}
		}
	}
	
	PlaySound(L".\\res\\snd\\intro.wav", NULL, SND_ASYNC);

	for (int i = 0; i < 280; ++i)
	{
		Draw->BeginDraw();
		Draw->DrawBitmap(bmpIntro[Intro->get_frame()], Intro->get_rect());
		Draw->DrawBitmap(bmpLogo, D2D1::RectF(0, 0, scr_width, scr_height));
		Draw->EndDraw();
	}

	Draw->BeginDraw();
	Draw->DrawBitmap(bmpIntro[Intro->get_frame()], Intro->get_rect());
	Draw->DrawBitmap(bmpLogo, D2D1::RectF(0, 0, scr_width, scr_height));
	Draw->EndDraw();
	
	PlaySound(L".\\res\\snd\\boom.wav", NULL, SND_SYNC);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_wsetlocale(LC_ALL, L"");

	bIns = hInstance;
	if (!bIns)
	{
		LogErr(L"Error obtaining hInstance from Windows !");
		ErrExit(eClass);
	}

	CreateResources();

	PlaySound(sound_file, NULL, SND_ASYNC | SND_LOOP);

	while (bMsg.message != WM_QUIT)
	{
		if ((bRet = PeekMessage(&bMsg, NULL, NULL, NULL, PM_REMOVE)) != 0)
		{
			if (bRet == -1)ErrExit(eMsg);

			TranslateMessage(&bMsg);
			DispatchMessage(&bMsg);
		}

		if (pause)
		{
			if (show_help)continue;

			Draw->BeginDraw();
			Draw->DrawBitmap(bmpIntro[Intro->get_frame()], Intro->get_rect());
			if (txtBrush && bigFormat)Draw->DrawTextW(L"ПАУЗА", 6, bigFormat, D2D1::RectF(scr_width / 2.0f - 100.0f,
				scr_height / 2.0f - 50.0f, scr_width, scr_height), txtBrush);
			Draw->EndDraw();
			continue;
		}

		//////////////////////////////////////////////////////////////

		if (Hero)
		{
			switch (Hero->dir)
			{
			case dirs::right:
				nature_dir = dirs::left;
				break;

			case dirs::left:
				nature_dir = dirs::right;
				break;

			default: nature_dir = dirs::stop;
			}
		}

		if (!vMainGround.empty())
		{
			for (std::vector<dll::FIELD*>::iterator tile = vMainGround.begin(); tile < vMainGround.end(); ++tile)
			{
				switch (nature_dir)
				{
				case dirs::left:
					if (!(*tile)->move(speed, nature_dir))
					{
						(*tile)->Release();
						vMainGround.erase(tile);
						need_right = true;
						break;
					}
					break;

				case dirs::right:
					if (!(*tile)->move(speed, nature_dir))
					{
						(*tile)->Release();
						vMainGround.erase(tile);
						need_left = true;
						break;
					}
					break;
				}

				if (need_left || need_right)break;
			}
		}
		if (!vMainGround.empty())
		{
			if (need_left)
			{
				need_left = false;
				vMainGround.insert(vMainGround.begin(),
					dll::FIELD::create(fields::flat_ground, (*vMainGround.begin())->start.x - 100.0f, ground));

			}
			if (need_right)
			{
				need_right = false;
				vMainGround.push_back(dll::FIELD::create(fields::flat_ground, vMainGround.back()->start.x + 100.0f, ground));
			}
		}

		// vTiles ********************************************

		if (vTiles.size() < 6 && RandIt(0, 100) == 66)
		{
			float sx = scr_width + RandIt(0.0f, scr_width / 1.5f);
			float sy = ground - 100.0f;

			D2D1_RECT_F dummy{ sx, sy, sx + 300.0f, sy + 100.0f };

			bool ok = true;
			if (!vTiles.empty())
			{
				for (int i = 0; i < vTiles.size(); ++i)
				{
					if (dll::intersect(vTiles[i]->get_rect(), dummy))
					{
						ok = false;
						break;
					}
				}
			}

			if (ok)
			{
				vTiles.push_back(dll::FIELD::create(fields::left_slope, sx, sy));
				sx += 100.0f;
				if (RandIt(0, 3) == 2)
				{
					vTiles.push_back(dll::FIELD::create(fields::flat_ground, sx, sy));
					sx += 100.0f;
				}
				vTiles.push_back(dll::FIELD::create(fields::right_slope, sx, sy));
			}
		}
		if (!vTiles.empty())
		{
			for (std::vector<dll::FIELD*>::iterator tile = vTiles.begin(); tile < vTiles.end(); ++tile)
			{
				if (!(*tile)->move(speed, nature_dir))
				{
					(*tile)->Release();
					vTiles.erase(tile);
					break;
				}
			}
		}

		if (Hero && !vTiles.empty() && Hero->action != actions::jump)
		{
			bool on_hill = false;

			for (int i = 0; i < vTiles.size(); ++i)
			{
				if (dll::intersect(Hero->get_rect(), vTiles[i]->get_rect()))
				{
					if (vTiles[i]->type == fields::flat_ground)break;

					Hero->climb(speed, vTiles[i]->get_rect(), vTiles[i]->type);
					
					if (Hero->action == actions::run)
					{
						Hero->start.y += 1.0f;
						Hero->set_edges();
					}

					on_hill = true;

					break;
				}
			}

			if ((!on_hill && Hero->end.y < ground))
			{
				dll::BAG<D2D1_RECT_F> bGrounds;

				if (!vMainGround.empty())
				{
					for (int i = 0; i < vMainGround.size(); ++i) bGrounds.push_back(vMainGround[i]->get_rect());
				}
				if (!vTiles.empty())
				{
					for (int i = 0; i < vTiles.size(); ++i) bGrounds.push_back(vTiles[i]->get_rect());
				}
				
				Hero->fall(bGrounds);
			}
		}

		if (Hero)
		{
			if (Hero->action == actions::jump || Hero->action == actions::fall)
			{
				dll::BAG<D2D1_RECT_F> bGrounds;

				if (!vMainGround.empty())
				{
					for (int i = 0; i < vMainGround.size(); ++i) bGrounds.push_back(vMainGround[i]->get_rect());
				}
				if (!vTiles.empty())
				{
					for (int i = 0; i < vTiles.size(); ++i) bGrounds.push_back(vTiles[i]->get_rect());
				}

				if (Hero->action == actions::fall)Hero->fall(bGrounds);
				else Hero->jump(bGrounds);
			}
			
			
			if (!vTiles.empty())
			{
				dll::BAG<D2D1_RECT_F>bTiles(vTiles.size());

				for (int i = 0; i < vTiles.size(); ++i)
					if (vTiles[i]->type != fields::flat_ground)bTiles.push_back(vTiles[i]->get_rect());

				dll::sort(bTiles, Hero->get_rect());

				if (Hero->end.y < ground && dll::intersect(Hero->get_rect(), bTiles[0]))
				{

					float an_slope{ (bTiles[0].bottom - bTiles[0].top) / (bTiles[0].right - bTiles[0].left) };
					float an_intercept = bTiles[0].bottom - bTiles[0].left * an_slope;

					if (!(Hero->end.x == bTiles[0].left || Hero->end.x == bTiles[0].right))
					{
						D2D1_POINT_2F point_on_line{ Hero->end.x, Hero->end.x * an_slope + an_intercept };

						if (dll::distance(Hero->end, point_on_line) > 0)
						{
							Hero->start.y += 2.0f + speed / 10.0f;
							Hero->set_edges();
						}
					}
				}
				
				if (Hero->end.y > ground)
				{
					Hero->end.y = ground;
					Hero->start.y = Hero->end.y - Hero->get_height();
					Hero->set_edges();
				}
			}
		}

		///////////////////////////////////////////////////////

		// vRings ********************************************

		if (vRings.size() < 40 && RandIt(0, 100) == 66)
		{
			float sx = scr_width + RandIt(10.0f, 30.0f);
			D2D1_RECT_F dummy{};
			dummy.left = sx;
			dummy.top = ground - 25.0f; 
			dummy.right = sx + 20.0f; 
			dummy.bottom = ground - 5.0f;

			bool ok = true;

			if (!vTiles.empty())
			{
				for(int i=0;i<vTiles.size();++i)
					if (dll::intersect(dummy, vTiles[i]->get_rect()))
					{
						ok = false;
						break;
					}
			}

			if (ok)
			{
				if (!vRings.empty())
				{
					bool overlap = true;

					while (overlap)
					{
						overlap = false;

						for (int i = 0; i < vRings.size(); ++i)
						{
							if (dll::intersect(dummy, vRings[i]->get_rect()))
							{
								overlap = true;
								dummy.left++;
								sx = dummy.left;
								break;
							}
						}
					}
				}
				vRings.push_back(new dll::PROTON(sx, ground - 25.0f, 20.0f, 20.0f));
			}
			
		}

		if (!vRings.empty())
		{
			for (std::vector<dll::PROTON*>::iterator ring = vRings.begin(); ring < vRings.end(); ++ring)
			{
				bool erased = false;
				switch (nature_dir)
				{
				case dirs::left:
					(*ring)->start.x -= 2.0f + speed / 10.0f;
					(*ring)->set_edges();
					if ((*ring)->end.x <= 0)
						{
							delete* ring;
							vRings.erase(ring);
							erased = true;
						}
					break;

				case dirs::right:
					(*ring)->start.x += 2.0f + speed / 10.0f;
					(*ring)->set_edges();
					if ((*ring)->start.x >= scr_width * 2.0f)
					{
						delete* ring;
						vRings.erase(ring);
						erased = true;
					}
					break;
				}

				if (erased)break;
			}
		}

		if (!vRings.empty() && Hero)
		{
			for (std::vector<dll::PROTON*>::iterator ring = vRings.begin(); ring < vRings.end(); ++ring)
			{
				if (dll::intersect(Hero->get_rect(), (*ring)->get_rect()))
				{
					if (sound)mciSendString(L"play .\\res\\snd\\ring.wav", NULL, NULL, NULL);
					++score;
					delete (*ring);
					vRings.erase(ring);
					break;
				}
			}
		}

		if (portal_opened)
		{
			switch (nature_dir)
			{
			case dirs::left:
				PortalRect.left -= 2.0f + speed / 10.0f;
				PortalRect.right -= 2.0f + speed / 10.0f;
				break;

			case dirs::right:
				PortalRect.left += 2.0f + speed / 10.0f;
				PortalRect.right += 2.0f + speed / 10.0f;
				break;
			}

			if (Hero)
			{
				if (dll::intersect(Hero->get_rect(), PortalRect))LevelUp();
			}
		}

		//////////////////////////////////////////////////////

		// vEvils *********************************************

		if (vEvils.size() < 5 + speed && RandIt(0, 350) == 66)
		{
			creatures type = static_cast<creatures>(RandIt(0, 2));
			
			if (sound)mciSendString(L"play .\\res\\snd\\evil_born.wav", NULL, NULL, NULL);

			if (type != creatures::zombie_flyer)vEvils.push_back(dll::EVIL::create(type, scr_width + RandIt(20.0f, 100.0f),
				ground - 35.0f));
			else
				vEvils.push_back(dll::EVIL::create(type, scr_width + RandIt(20.0f, 100.0f),
					scr_height / 2.0f + RandIt(20.0f, 100.0f)));
		}

		if (!vEvils.empty())
		{
			for (std::vector<dll::EVIL*>::iterator evil = vEvils.begin(); evil < vEvils.end(); ++evil)
			{
				dll::BAG<dll::FIELD*> bGrounds;

				if (!vTiles.empty())
				{
					for (int i = 0; i < vTiles.size(); ++i) bGrounds.push_back(vTiles[i]);
				}

				if (!(*evil)->move(speed, bGrounds, nature_dir))
				{
					(*evil)->Release();
					vEvils.erase(evil);
					break;
				}
			}
		}

		if (!vEvils.empty() && Hero)
		{
			for (int i = 0; i < vEvils.size(); ++i)
			{
				int damage = vEvils[i]->attack(Hero->center);

				if (damage > 0)
				{
					vEvilShots.push_back(dll::SHOT::create(vEvils[i]->center.x, vEvils[i]->center.y,
						Hero->center.x, Hero->center.y));
					vEvilShots.back()->damage = damage;
					break;
				}
			}
		}

		if (!vEvilShots.empty())
		{
			for (std::vector<dll::SHOT*>::iterator shot = vEvilShots.begin(); shot < vEvilShots.end(); ++shot)
			{
				if (!(*shot)->move(speed))
				{
					(*shot)->Release();
					vEvilShots.erase(shot);
					break;
				}
			}
		}

		////////////////////////////////////////////////////////

		if (Hero && !vEvilShots.empty())
		{
			for (std::vector<dll::SHOT*>::iterator shot = vEvilShots.begin(); shot < vEvilShots.end(); ++shot)
			{
				if (dll::intersect(Hero->get_rect(), (*shot)->get_rect()))
				{
					int current_damage = ((*shot)->damage - Hero->armor);
					if (current_damage <= 0)current_damage = 1;
					Hero->lifes -= current_damage;
					if (sound)mciSendString(L"play .\\res\\snd\\hurt.wav", NULL, NULL, NULL);
					(*shot)->Release();
					vEvilShots.erase(shot);
					if (Hero->lifes <= 0)
					{
						hero_killed = true;
						RipRect.left = Hero->start.x;
						RipRect.top = Hero->start.y;
						RipRect.right = Hero->end.x;
						RipRect.bottom = Hero->end.y;
						Hero->Release();
					}
					break;
				}
			}
		}

		if (!vHeroShots.empty())
		{
			for (std::vector<dll::SHOT*>::iterator shot = vHeroShots.begin(); shot < vHeroShots.end(); ++shot)
			{
				if (!(*shot)->move(speed))
				{
					(*shot)->Release();
					vHeroShots.erase(shot);
					break;
				}
			}
		}
		
		if (!vHeroShots.empty() && !vEvils.empty() && Hero)
		{
			bool killed = false;

			for (std::vector<dll::EVIL*>::iterator evil = vEvils.begin(); evil < vEvils.end(); ++evil)
			{
				for (std::vector<dll::SHOT*>::iterator shot = vHeroShots.begin(); shot < vHeroShots.end(); ++shot)
				{
					if (dll::intersect((*shot)->get_rect(), (*evil)->get_rect()))
					{
						int current_damage = ((*shot)->damage - (*evil)->armor);
						if (current_damage <= 0)current_damage = 1;

						(*evil)->lifes -= current_damage;
						(*shot)->Release();
						vHeroShots.erase(shot);

						if ((*evil)->lifes <= 0)
						{
							if (sound)mciSendString(L"play .\\res\\snd\\evil_killed.wav", NULL, NULL, NULL);
							(*evil)->Release();
							vEvils.erase(evil);
							score += 5 * (int)(speed);
							killed = true;
						}
						break;
					}
				}

				if (killed)break;
			}
		}

		///////////////////////////////////////////////////////

		// ASSETS **********************************************

		if (vAssets.size() < 3 && RandIt(0, 400) == 33)
		{
			FADING dummy{};
			dummy.type = static_cast<assets>(RandIt(0, 2));
			dummy.view_rect.left = scr_width + RandIt(10.0f, scr_width / 3.0f);
			dummy.view_rect.right = dummy.view_rect.left + 32.0f;
			dummy.view_rect.top = ground - 32.0f;
			dummy.view_rect.bottom = ground;

			bool ok = true;

			if(!vTiles.empty())
				for (int i = 0; i < vTiles.size(); ++i)
				{
					if (dll::intersect(dummy.view_rect, vTiles[i]->get_rect()))
					{
						ok = false;
						break;
					}
				}

			if (ok)vAssets.push_back(dummy);
		}

		if (!vAssets.empty())
		{
			for (std::vector<FADING>::iterator asset = vAssets.begin(); asset < vAssets.end(); ++asset)
			{
				switch (nature_dir)
				{
				case dirs::left:
					asset->view_rect.left -= 2.0f + speed / 10.0f;
					asset->view_rect.right -= 2.0f + speed / 10.0f;
					break;
				
				case dirs::right:
					asset->view_rect.left += 2.0f + speed / 10.0f;
					asset->view_rect.right += 2.0f + speed / 10.0f;
					break;
				}

				if (asset->view_rect.right <= -scr_width * 0.5f || asset->view_rect.left >= scr_width + scr_width * 0.5f)
				{
					vAssets.erase(asset);
					break;
				}
			}
		}

		if (!vAssets.empty() && Hero)
		{
			for (int i = 0; i < vAssets.size(); ++i)
			{
				if (dll::intersect(Hero->get_rect(), vAssets[i].view_rect))
				{
					if (vAssets[i].chest_opened)continue;

					vAssets[i].chest_opened = true;

					switch (vAssets[i].type)
					{
					case assets::rings:
						if (sound)mciSendString(L"play .\\res\\snd\\gold.wav", NULL, NULL, NULL);
						score += 10;
						break;

					case assets::armor:
						if (sound)mciSendString(L"play .\\res\\snd\\armor.wav", NULL, NULL, NULL);
						++Hero->armor;
						break;

					case assets::potion:
						if (sound)mciSendString(L"play .\\res\\snd\\life.wav", NULL, NULL, NULL);
						if (Hero->lifes + 20 <= 100)Hero->lifes += 20;
						else Hero->lifes = 100;
						break;
					}
				}
			}
		}

		// DRAW THINGS **************************************************

		Draw->BeginDraw();
		
		if (txtBrush && inactBrush && hgltBrush && statBrush && nrmFormat && b1BckgBrush && b2BckgBrush && b3BckgBrush)
		{
			Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), statBrush);
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b1Rect, 15.0f, 20.0f), b1BckgBrush);
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b2Rect, 15.0f, 20.0f), b2BckgBrush);
			Draw->FillRoundedRectangle(D2D1::RoundedRect(b3Rect, 15.0f, 20.0f), b3BckgBrush);

			if (name_set)Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmFormat, b1TxtRect, inactBrush);
			else
			{
				if (!b1Hglt)Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmFormat, b1TxtRect, txtBrush);
				else Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmFormat, b1TxtRect, hgltBrush);
			}
			if (!b2Hglt)Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmFormat, b2TxtRect, txtBrush);
			else Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmFormat, b2TxtRect, hgltBrush);
			if (!b3Hglt)Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmFormat, b3TxtRect, txtBrush);
			else Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmFormat, b3TxtRect, hgltBrush);
		}

		if (Background)Draw->DrawBitmap(bmpBackground[Background->get_frame()], D2D1::RectF(0, 50.0f, scr_width, scr_height));

		if (!vMainGround.empty())
		{
			for (int i = 0; i < vMainGround.size(); ++i)Draw->DrawBitmap(bmpFlat, vMainGround[i]->get_rect());
		}
		
		if (!vTiles.empty())
		{
			for (int i = 0; i < vTiles.size(); ++i)
			{
				switch (vTiles[i]->type)
				{
				case fields::flat_ground:
					Draw->DrawBitmap(bmpFlat, vTiles[i]->get_rect());
					break;

				case fields::right_slope:
					Draw->DrawBitmap(bmpRightSlope, vTiles[i]->get_rect());
					break;

				case fields::left_slope:
					Draw->DrawBitmap(bmpLeftSlope, vTiles[i]->get_rect());
					break;
				}
			}
		}

		if (Hero)
		{
			Draw->SetTransform(D2D1::Matrix3x2F::Rotation(0.0f, Hero->center));

			if (Hero->action == actions::climb_up)Draw->SetTransform(D2D1::Matrix3x2F::Rotation(-45.0f, Hero->center));
			else if (Hero->action == actions::climb_down)Draw->SetTransform(D2D1::Matrix3x2F::Rotation(45.0f, Hero->center));


			if (Hero->dir == dirs::left)
			{
				int aframe = Hero->get_frame();
				Draw->DrawBitmap(bmpHeroL[aframe], Resizer(bmpHeroL[aframe], Hero->start.x, Hero->start.y));
			}
			else if (Hero->dir == dirs::right)
			{
				int aframe = Hero->get_frame();
				Draw->DrawBitmap(bmpHeroR[aframe], Resizer(bmpHeroR[aframe], Hero->start.x, Hero->start.y));
			}
			else Draw->DrawBitmap(bmpHeroR[0], Resizer(bmpHeroR[0], Hero->start.x, Hero->start.y));

			if (inactBrush && txtBrush)
			{
				Draw->DrawLine(D2D1::Point2F(Hero->start.x - 8.0f, Hero->start.y),
					D2D1::Point2F(Hero->start.x - 8.0f, Hero->start.y + 25.0f), inactBrush, 5.0f);
				Draw->DrawLine(D2D1::Point2F(Hero->start.x - 8.0f, Hero->start.y),
					D2D1::Point2F(Hero->start.x - 8.0f, Hero->start.y + Hero->lifes / 4.0f), txtBrush, 3.0f);
			}
			Draw->SetTransform(D2D1::Matrix3x2F::Rotation(0.0f, Hero->center));
		}

		if (!vEvils.empty())
		{
			for (int i = 0; i < vEvils.size(); ++i)
			{
				int frame = vEvils[i]->get_frame();

				switch (vEvils[i]->type)
				{
				case creatures::zombie_girl:
					if (vEvils[i]->dir == dirs::left)Draw->DrawBitmap(bmpFemL[frame], Resizer(bmpFemL[frame],
						vEvils[i]->start.x, vEvils[i]->start.y));
					else Draw->DrawBitmap(bmpFemR[frame], Resizer(bmpFemR[frame], vEvils[i]->start.x, vEvils[i]->start.y));
					break;

				case creatures::zombie_boy:
					if (vEvils[i]->dir == dirs::left)Draw->DrawBitmap(bmpMaleL[frame], Resizer(bmpMaleL[frame],
						vEvils[i]->start.x, vEvils[i]->start.y));
					else Draw->DrawBitmap(bmpMaleR[frame], Resizer(bmpMaleR[frame], vEvils[i]->start.x, vEvils[i]->start.y));
					break;

				case creatures::zombie_flyer:
					if (vEvils[i]->dir == dirs::left)Draw->DrawBitmap(bmpFlyerL[frame], Resizer(bmpFlyerL[frame],
						vEvils[i]->start.x, vEvils[i]->start.y));
					else Draw->DrawBitmap(bmpFlyerR[frame], Resizer(bmpFlyerR[frame], vEvils[i]->start.x, vEvils[i]->start.y));
					break;
				}
			}
		}

		if (!vEvilShots.empty())
		{
			for (int i = 0; i < vEvilShots.size(); ++i)Draw->DrawBitmap(bmpSpit, vEvilShots[i]->get_rect());
		}
		if (!vHeroShots.empty())
		{
			for (int i = 0; i < vHeroShots.size(); ++i)Draw->DrawBitmap(bmpSmallRing, vHeroShots[i]->get_rect());

		}

		if (!vRings.empty())
		{
			for (int i = 0; i < vRings.size(); ++i)Draw->DrawBitmap(bmpRing, vRings[i]->get_rect());
		}

		if (!vAssets.empty())
		{
			for (int i = 0; i < vAssets.size(); ++i)
			{
				if (!vAssets[i].chest_opened)Draw->DrawBitmap(bmpChest, vAssets[i].view_rect);
				else
				{
					float opacity = vAssets[i].get_opacity();

					switch (vAssets[i].type)
					{
					case assets::rings:
						Draw->DrawBitmap(bmpPileIcon, D2D1::RectF(vAssets[i].view_rect.left, vAssets[i].view_rect.top,
							vAssets[i].view_rect.left + 30.0f, vAssets[i].view_rect.top + 20.0f), opacity);
						break;

					case assets::armor:
						Draw->DrawBitmap(bmpArmorIcon, D2D1::RectF(vAssets[i].view_rect.left, vAssets[i].view_rect.top,
							vAssets[i].view_rect.left + 20.0f, vAssets[i].view_rect.top + 20.0f), opacity);
						break;

					case assets::potion:
						Draw->DrawBitmap(bmpLifeIcon, D2D1::RectF(vAssets[i].view_rect.left, vAssets[i].view_rect.top,
							vAssets[i].view_rect.left + 20.0f, vAssets[i].view_rect.top + 20.0f), opacity);
						break;
					}

					if (opacity <= 0)
					{
						vAssets.erase(vAssets.begin() + i);
						break;
					}
				}
			}
		}

		if (portal_opened)Draw->DrawBitmap(bmpPortal[portal_frame()], PortalRect);

		/////////////////////////////////////////////////////////////////

		// STATUS TEXT *********************************************

		if (inactBrush && midFormat && Hero)
		{
			wchar_t txt[200]{ L"герой: " };
			wchar_t add[5]{ L"\0" };
			int stat_size{ 0 };

			wcscat_s(txt, current_player);
			
			wcscat_s(txt, L", рингове: ");
			wsprintf(add, L"%d", score);
			wcscat_s(txt, add);

			wcscat_s(txt, L", броня: ");
			wsprintf(add, L"%d", Hero->armor);
			wcscat_s(txt, add);

			wcscat_s(txt, L", скорост: ");
			wsprintf(add, L"%d", (int)(speed));
			wcscat_s(txt, add);

			for (int i = 0; i < 200; ++i)
			{
				if (txt[i] != '\0')++stat_size;
				else break;
			}

			Draw->DrawTextW(txt, stat_size, midFormat, D2D1::RectF(10.0f, ground + 20.0f, scr_width, scr_height), inactBrush);

			stat_size = 0;
			swprintf_s(txt, 200, L"остават: %.2f м.", distance / 100.0f);

			for (int i = 0; i < 200; ++i)
			{
				if (txt[i] != '\0')++stat_size;
				else break;
			}

			Draw->DrawTextW(txt, stat_size, midFormat, D2D1::RectF(scr_width - 400.0f, sky + 10.0f, scr_width, scr_height), inactBrush);
		}

		////////////////////////////////////////////////////////////

		Draw->EndDraw();

		if (hero_killed)
		{
			Draw->BeginDraw();
			Draw->DrawBitmap(bmpRIP, RipRect);
			if (sound)
			{
				PlaySound(NULL, NULL, NULL);
				if (sound)PlaySound(L".\\res\\snd\\killed.wav", NULL, SND_SYNC);
				else Sleep(4000);
				GameOver();
			}
			Draw->EndDraw();
		}
	}

	ReleaseResources();
	std::remove(tmp_file);

    return (int) bMsg.wParam;
}