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
ID2D1Bitmap* bmpLoose{ nullptr };
ID2D1Bitmap* bmpRecord{ nullptr };
ID2D1Bitmap* bmpRing{ nullptr };
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

bool name_set = false;
wchar_t current_player[16]{ L"TARLYO" };

float speed = 1.0f;
int score = 0;

bool need_left = false;
bool need_right = false;

dirs nature_dir = dirs::stop;

////////////////////////////////////////////////////////////////

dll::RANDIT RandIt{};

dll::FIELD* Intro{ nullptr };
dll::FIELD* Background{ nullptr };

std::vector<dll::FIELD*>vBackgrounds;
std::vector<dll::FIELD*>vTiles;

dll::HERO* Hero{ nullptr };




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
	if (!FreeMem(&bmpLoose))LogErr(L"Error releasing D2D1 main write bmpLoose !");
	if (!FreeMem(&bmpRecord))LogErr(L"Error releasing D2D1 main write bmpRecord !");
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

void GameOver()
{
	PlaySound(NULL, NULL, NULL);







	bMsg.message = WM_QUIT;
	bMsg.wParam = 0;
}
void InitGame()
{
	speed = 1.0f;
	wcscpy_s(current_player, L"TARLYO");
	name_set = false;
	
	FreeMem(&Intro);
	Intro = dll::FIELD::create(fields::intro, 0, 0);

	FreeMem(&Background);
	Background = dll::FIELD::create(fields::background, 0, 50.0f);
	
	hero_killed = false;
	need_left = false;
	need_right = false;

	nature_dir = dirs::stop;
	
	if (!vBackgrounds.empty())
		for (int i = 0; i < vBackgrounds.size(); ++i)
			if (!FreeMem(&vBackgrounds[i]))LogErr(L"Error releasing vBackgrounds !");
	vBackgrounds.clear();

	for (float sx = -scr_width; sx < 2.0f * scr_width; ++sx)vBackgrounds.push_back(dll::FIELD::create(fields::background, 
		sx, 50.0f));

	if (!vTiles.empty())
		for (int i = 0; i < vTiles.size(); ++i)
			if (!FreeMem(&vTiles[i]))LogErr(L"Error releasing vTiles !");
	vTiles.clear();

	if (Hero)Hero->Release();
	Hero = dll::HERO::create(scr_width / 2.0f - 50.0f, ground - 35.0f);

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

	case WM_SETICON:
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
						if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
						b1Hglt = true;
						b2Hglt = false;
						b3Hglt = false;
					}
				}
				else if (cur_pos.x * x_scale >= b2Rect.left && cur_pos.x * x_scale <= b2Rect.right)
				{
					if (!b2Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = true;
						b3Hglt = false;
					}
				}
				else if (cur_pos.x * x_scale >= b3Rect.left && cur_pos.x * x_scale <= b3Rect.right)
				{
					if (!b3Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = false;
						b3Hglt = true;
					}
				}
				else if (b1Hglt || b2Hglt || b3Hglt)
				{
					if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
					b1Hglt = false;
					b2Hglt = false;
					b3Hglt = false;
				}

				SetCursor(outCursor);
				return true;
			}
			else if (b1Hglt || b2Hglt || b3Hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
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
				if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
				b1Hglt = false;
				b2Hglt = false;
				b3Hglt = false;
			}

			SetCursor(LoadCursor(NULL, IDC_ARROW));

			return true;
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



		}
		break;





	default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
	}

	return (LRESULT)(FALSE);
}

void CreateResources()
{
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
			gStops[0].color = D2D1::ColorF(D2D1::ColorF::ColorF::Chocolate);
			gStops[1].position = 1.0f;
			gStops[1].color = D2D1::ColorF(D2D1::ColorF::ColorF::Indigo);

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

			bmpLogo = Load(L".\\res\\img\\logo.png", Draw);
			if (!bmpLogo)
			{
				
				LogErr(L"Error loading bmpLogo");
				ErrExit(eD2D);
			}
			bmpLoose = Load(L".\\res\\img\\Loose.png", Draw);
			if (!bmpLoose)
			{
				LogErr(L"Error loading bmpLoose");
				ErrExit(eD2D);
			}
			bmpRecord = Load(L".\\res\\img\\Record.png", Draw);
			if (!bmpRecord)
			{
				LogErr(L"Error loading bmpRecord");
				ErrExit(eD2D);
			}
			bmpRing = Load(L".\\res\\img\\Ring.png", Draw);
			if (!bmpRing)
			{
				LogErr(L"Error loading bmpRing");
				ErrExit(eD2D);
			}
			bmpRIP = Load(L".\\res\\img\\RIP.png", Draw);
			if (!bmpRIP)
			{
				LogErr(L"Error loading bmpRIP");
				ErrExit(eD2D);
			}
			bmpWin = Load(L".\\res\\img\\Win.png", Draw);
			if (!bmpWin)
			{
				LogErr(L"Error loading bmpWin");
				ErrExit(eD2D);
			}

			bmpChest = Load(L".\\res\\img\\assets\\Chest.png", Draw);
			if (!bmpChest)
			{
				LogErr(L"Error loading bmpChest");
				ErrExit(eD2D);
			}
			bmpArmorIcon = Load(L".\\res\\img\\assets\\Armor.png", Draw);
			if (!bmpArmorIcon)
			{
				LogErr(L"Error loading bmpArmorIcon");
				ErrExit(eD2D);
			}
			bmpPileIcon = Load(L".\\res\\img\\assets\\pile.png", Draw);
			if (!bmpPileIcon)
			{
				LogErr(L"Error loading bmpPileIcon");
				ErrExit(eD2D);
			}
			bmpLifeIcon = Load(L".\\res\\img\\assets\\potion.png", Draw);
			if (!bmpLifeIcon)
			{
				LogErr(L"Error loading bmpLifeIcon");
				ErrExit(eD2D);
			}

			bmpFlat = Load(L".\\res\\img\\field\\flat.png", Draw);
			if (!bmpFlat)
			{
				LogErr(L"Error loading bmpFlat");
				ErrExit(eD2D);
			}
			bmpLeftSlope = Load(L".\\res\\img\\field\\left_slope.png", Draw);
			if (!bmpLeftSlope)
			{
				LogErr(L"Error loading bmpLeftSlope");
				ErrExit(eD2D);
			}
			bmpRightSlope = Load(L".\\res\\img\\field\\right_slope.png", Draw);
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

				bmpBackground[i] = Load(name, Draw);
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

				bmpIntro[i] = Load(name, Draw);
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

				bmpPortal[i] = Load(name, Draw);
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

				bmpHeroL[i] = Load(name, Draw);
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

				bmpHeroR[i] = Load(name, Draw);
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

				bmpFemL[i] = Load(name, Draw);
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

				bmpFemR[i] = Load(name, Draw);
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

				bmpMaleL[i] = Load(name, Draw);
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

				bmpMaleR[i] = Load(name, Draw);
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

				bmpFlyerL[i] = Load(name, Draw);
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

				bmpFlyerR[i] = Load(name, Draw);
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
				DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"", &midFormat);
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

	for (int i = 0; i < 300; ++i)
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

	PlaySound(L".\\res\\snd\\intro.wav", NULL, SND_SYNC);
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
				scr_height / 2.0f - 50.0f), txtBrush);
			Draw->EndDraw();
			continue;
		}


		//////////////////////////////////////////////////////////////











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


		/////////////////////////////////////////////////////////////////

		Draw->EndDraw();
	}

	ReleaseResources();
	std::remove(tmp_file);

    return (int) bMsg.wParam;
}