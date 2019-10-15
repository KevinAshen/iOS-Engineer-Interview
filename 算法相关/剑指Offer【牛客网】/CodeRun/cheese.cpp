#include<iostream>
#include<string>
#include<limits.h>
#include<queue>
#include<stack>
#include<vector>
#include<iomanip>
#include<assert.h>
using namespace std;
#define BOARDSIZE 8

typedef int Array[BOARDSIZE][BOARDSIZE];

void Print2Array(Array a)
{
	for (int i = 0; i < BOARDSIZE; ++i)
	{
		for (int j = 0; j < BOARDSIZE; ++j)
		{
			cout << setw(3) << a[i][j];
		}
		cout << endl;
	}
	cout << endl;
}
int title = 1;
void ChessBoard(Array board, int tr, int tc, int dr, int dc, int size)
{
	if (size <= 1) return;
	Print2Array(board);
	int s = size / 2, t = title++;
	if (dr < tr + s && dc < tc + s)
	{
		ChessBoard(board, tr, tc, dr, dc, s);
	}
	else
	{
		board[tr + s - 1][tc + s - 1] = t;
		ChessBoard(board, tr, tc, tr + s - 1, tc + s - 1, s);
	}

	if (dr < tr + s && dc >= tc + s)
	{
		ChessBoard(board, tr, tc + s, dr, dc, s);
	}
	else
	{
		board[tr + s - 1][tc + s] = t;
		ChessBoard(board, tr, tc + s, tr + s - 1, tc + s, s);
	}

	if (dr >= tr + s && dc >= tc + s)
	{
		ChessBoard(board, tr + s, tc + s, dr, dc, s);
	}
	else
	{
		board[tr + s][tc + s] = t;
		ChessBoard(board, tr + s, tc + s, tr + s, tc + s, s);
	}
	if (dr >= tr + s && dc < tc + s)
	{
		ChessBoard(board, tr + s, tc, dr, dc, s);
	}
	else
	{
		board[tr + s][tc + s - 1] = t;
		ChessBoard(board, tr + s, tc, tr + s, tc + s - 1, s);
	}

}

int main()
{
	Array board = { 0 };
	int tr = 0, tc = 0; // row col
	int dr = 2, dc = 1;
	board[dr][dc] = -1;
	Print2Array(board);

	ChessBoard(board, tr, tc, dr, dc, BOARDSIZE);
	Print2Array(board);
	return 0;
}