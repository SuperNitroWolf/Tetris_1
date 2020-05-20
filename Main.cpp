#include<iostream>
#include<Windows.h>
#include<thread>
#include<vector>
using namespace std;

//Global Assets
wstring lScoreText = L"Lines: ";
wstring sScoreText = L"Score: ";
wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr;

int nScreenWidth = 120;		// Console screen sice X (columns)
int nScreenHeight = 30;		// Console screen sice Y (rows)

int nLeftspace = 50;		// Space from left margin of console
int nTopspace = 2;			// Space from top margin of console

vector<int> vLines;			// Stores the lines once one is done

int nLines = 0;				// Keep track of the number of lines in current game
int nScore = 0;				// Keep track of the score in current game
int nMultiLine = 0;			// Records how many lines where made in one single move
int nScoreY = 9;			// Y coordinate of the location of the score
int nScoreX = 70;			// X coordinate of the locatiin of the score
int nPieces = 0;			// # of pieces given to the user

int Rotate(int px, int py, int r)
{
	switch (r % 4)
	{
	case 0: return py * 4 + px;			// 0 degrees
	case 1: return 12 + py - (px * 4);	// 90 degrees
	case 2: return 15 - (py * 4) - px;	// 180 degrees
	case 3: return 3 - py + (px * 4);	// 270 degrees
	}

	return 0;
}


// This function determines the collition detection
bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++)
	{
		for (int py = 0; py < 4; py++)
		{
			// Get index into piece
			int pi = Rotate(px, py, nRotation);

			// Get index into field
			int fi = (nPosY + py) * nFieldWidth + nPosX + px;

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
						return false; // fail on first hit
				}
			}
		}
	}

	return true;
}

int main()
{
	// Creation of assets (tetris figures)
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

	// First definition of the field. The field contians what is going to be drawn in the screen
	pField = new unsigned char[nFieldWidth * nFieldHeight];	// Creates the array of char that contains the screen
	for (int x = 0; x < nFieldWidth; x++)	// Board boundary
	{
		for (int y = 0; y < nFieldHeight; y++)
		{
			// draw empty board game. 9 is border and 0 is empty space
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
		}
	}

	// This is the actual screen that is going to be passed to the buffer to be printed
	// Also defines the handle necessary to print.
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// Sets the size of the console window
	//CONSOLE_SCREEN_BUFFER_INFO bScreen;
	//GetConsoleScreenBufferInfo(hConsole, &bScreen);
	//SMALL_RECT bRect;
	//bRect.Top = bScreen.srWindow.Top;
	//bRect.Left = bScreen.srWindow.Left;
	//bRect.Bottom = bScreen.srWindow.Top + nScreenHeight+500;
	//bRect.Right = bScreen.srWindow.Left + nScreenWidth+500;
	//SetConsoleWindowInfo(hConsole, TRUE, &bRect);

	// GAME LOOP

	bool bGameOver = 0;
	int nCurrentPiece = 4;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	bool bKey[4];
	bool bRotateHold = false;

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;

	while (!bGameOver)
	{
		/****************************************************************************/
		/* ============================= Game Timing ============================== */
		/****************************************************************************/
		this_thread::sleep_for(50ms);
		nSpeedCounter++;
		bForceDown = (nSpeed == nSpeedCounter);


		/****************************************************************************/
		/* ================================= Input ================================ */
		/****************************************************************************/
		for (int k = 0; k < 4; k++)
		{														 //R   L   D Z
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
		}
		
		/****************************************************************************/
		/* ============================== GAME LOGIC ============================== */
		/****************************************************************************/
		
		//Right key is pressed
		if (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
			nCurrentX += 1;
		
		//Left key is pressed
		if (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
			nCurrentX -= 1;
		
		//Down key is pressed
		if (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			nCurrentY += 1;

		//Z key is pressed
		if (bKey[3])
		{
			if (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY))
			{
				nCurrentRotation += 1;
				bRotateHold = true;
			}
		}
		else
		{
			bRotateHold = false;
		}
		

		if (bForceDown)
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			else
			{
				// Lock current piece in the field
				for (int px = 0; px < 4; px++)
				{
					for (int py = 0; py < 4; py++)
					{
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
					}
				}

				// Check we have horizontal lines
				for (int py = 0; py < 4; py++)
				{
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						
						// check no empty spaces in the current line
						for (int px = 1; px < nFieldWidth - 1; px++)
						{
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
						}

						// remove line and set to =
						if (bLine)
						{
							for (int px = 1; px < nFieldWidth - 1; px++)
							{
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;
							}

							vLines.push_back(nCurrentY + py);
							nLines++;
							nMultiLine++;
						}
					}
				}

				// Select next piece
				nCurrentPiece = rand() % 7;
				nCurrentRotation = 0;
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;

				nPieces++;
				if (nPieces % 10 == 0) nSpeed--;

				// Game Over if piece does not fit
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}

			nSpeedCounter = 0;
		}

		/****************************************************************************/
		/* ============================ RENDER OUTPUT ============================= */
		/****************************************************************************/

		// Draw field
		for (int x = 0; x < nFieldWidth; x++)
		{
			for (int y = 0; y < nFieldHeight; y++)
			{
				screen[(y + nTopspace) * nScreenWidth + (x + nLeftspace)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
			}
		}

		// Draw current piece
		for (int px = 0; px < 4; px++)
		{
			for (int py = 0; py < 4; py++)
			{
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					screen[(nCurrentY + py + nTopspace) * nScreenWidth + (nCurrentX + px + nLeftspace)] = nCurrentPiece + 65;
			}
		}

		if (!vLines.empty())
		{
			// Display frame (cheekily to draw lines)
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); // delay for ~.5 sec
			
			for (auto& v : vLines)
			{
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
					{
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					}
					pField[px] = 0;
				}
			}
			vLines.clear();

			// update score
			nScore += nMultiLine * nMultiLine * 100;
			nMultiLine = 0;
		}

		// Draw Score
		for (int index = 0; index < lScoreText.size(); index++)
		{
			screen[nScoreY * nScreenWidth + (nScoreX + index)] = lScoreText[index];
		}

		int counter = 14;
		int tLines = nLines;
		bool digits = true;
		while (digits)
		{
			screen[nScoreY * nScreenWidth + (nScoreX + counter)] = (tLines % 10) + 48;
			counter--;
			tLines /= 10;
			if (tLines == 0) digits = false;
		}

		for (int index = 0; index < sScoreText.size(); index++)
		{
			screen[(nScoreY + 2) * nScreenWidth + (nScoreX + index)] = sScoreText.at(index);
		}
		counter = 14;
		tLines = nScore;
		digits = true;
		while (digits)
		{
			screen[(nScoreY + 2) * nScreenWidth + (nScoreX + counter)] = (tLines % 10) + 48;
			counter--;
			tLines /= 10;
			if (tLines == 0) digits = false;
		}

		/*I NEED IDEAS FOR THIS: middle of the screen to the right? display text and score enclosed in nice emoticons*/
		/*Draw different emoticons when you do a single, double, triple, or quadruple line*/

		// Display Frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	CloseHandle(hConsole);
	cout << "Game Over!! Score: " << nScore << endl;
	system("pause");

	return 0;
}

// i am at 29:40