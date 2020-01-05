
#define _CRT_SECURE_NO_WARNINGS
#include"RankSystem.h"

//-----------------------------------------------------------------------------
// グローバル変数
//-----------------------------------------------------------------------------
static char FileName[] = "HiScore.txt";	// ハイスコアを格納するファイル名

int num = 0;
int previous[10] = { 25,25,25,25,25,25,25,25,25,25 };

int BainariWrite(void)
{
	FILE* fp = fopen("test.bin", "wb");
	num = GetScore();

	int h = 0;
	int irekae = 0;

	int i, j = 0;

	//最新のスコアを、まずは、一番下に格納
	if (num > previous[9])
	{
		previous[9] = num;
	}

	//すべての数値を降順にソート、上から数字が大きい順
	for (i = 0; i < 10; ++i) 
	{
		for (j = i + 1; j < 10; ++j) 
		{
			if (previous[i] < previous[j]) 
			{
				irekae = previous[i];
				previous[i] = previous[j];
				previous[j] = irekae;
			}
		}
	}

	if (fp == NULL) {
		fputs("ファイルオープンに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&num, sizeof(num), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[0], sizeof(previous[0]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[1], sizeof(previous[1]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[2], sizeof(previous[2]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[3], sizeof(previous[3]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[4], sizeof(previous[4]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[5], sizeof(previous[5]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[6], sizeof(previous[6]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[7], sizeof(previous[7]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[8], sizeof(previous[8]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[9], sizeof(previous[9]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fclose(fp) == EOF) {
		fputs("ファイルクローズに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	return 0;
}


int BainariRead(void)
{
	FILE* fp = fopen("test.bin", "rb");

	if (fp == NULL) {
		fputs("ファイルオープンに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&num, sizeof(num), 1, fp) < 1) {
		fputs("読み込み中にエラーが発生しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[0], sizeof(previous[0]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[1], sizeof(previous[1]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[2], sizeof(previous[2]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[3], sizeof(previous[3]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[4], sizeof(previous[4]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[5], sizeof(previous[5]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[6], sizeof(previous[6]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[7], sizeof(previous[7]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[8], sizeof(previous[8]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[9], sizeof(previous[9]), 1, fp) < 1) {
		fputs("ファイルへの書き込みに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}
	DebugProc_Print((char*)"*** スコア ***\n");
	DebugProc_Print((char*)"[あなたのスコア：(%d)]\n\n\n\n", num);
	DebugProc_Print((char*)"[スコア：(%d)]\n", previous[0]);
	DebugProc_Print((char*)"[スコア：(%d)]\n", previous[1]);
	DebugProc_Print((char*)"[スコア：(%d)]\n", previous[2]);
	DebugProc_Print((char*)"[スコア：(%d)]\n", previous[3]);
	DebugProc_Print((char*)"[スコア：(%d)]\n", previous[4]);
	DebugProc_Print((char*)"[スコア：(%d)]\n", previous[5]);
	DebugProc_Print((char*)"[スコア：(%d)]\n", previous[6]);
	DebugProc_Print((char*)"[スコア：(%d)]\n", previous[7]);
	DebugProc_Print((char*)"[スコア：(%d)]\n", previous[8]);
	DebugProc_Print((char*)"[スコア：(%d)]\n", previous[9]);
	DebugProc_Print((char*)"\n\n\n\n前移動   : W\n");
	DebugProc_Print((char*)"後移動   : S\n");

	if (fclose(fp) == EOF) {
		fputs("ファイルクローズに失敗しました。\n", stderr);
		exit(EXIT_FAILURE);
	}

	return 0;
}