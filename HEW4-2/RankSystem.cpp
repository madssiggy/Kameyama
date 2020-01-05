
#define _CRT_SECURE_NO_WARNINGS
#include"RankSystem.h"

//-----------------------------------------------------------------------------
// �O���[�o���ϐ�
//-----------------------------------------------------------------------------
static char FileName[] = "HiScore.txt";	// �n�C�X�R�A���i�[����t�@�C����

int num = 0;
int previous[10] = { 25,25,25,25,25,25,25,25,25,25 };

int BainariWrite(void)
{
	FILE* fp = fopen("test.bin", "wb");
	num = GetScore();

	int h = 0;
	int irekae = 0;

	int i, j = 0;

	//�ŐV�̃X�R�A���A�܂��́A��ԉ��Ɋi�[
	if (num > previous[9])
	{
		previous[9] = num;
	}

	//���ׂĂ̐��l���~���Ƀ\�[�g�A�ォ�琔�����傫����
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
		fputs("�t�@�C���I�[�v���Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&num, sizeof(num), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[0], sizeof(previous[0]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[1], sizeof(previous[1]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[2], sizeof(previous[2]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[3], sizeof(previous[3]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[4], sizeof(previous[4]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[5], sizeof(previous[5]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[6], sizeof(previous[6]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[7], sizeof(previous[7]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[8], sizeof(previous[8]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fwrite(&previous[9], sizeof(previous[9]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fclose(fp) == EOF) {
		fputs("�t�@�C���N���[�Y�Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	return 0;
}


int BainariRead(void)
{
	FILE* fp = fopen("test.bin", "rb");

	if (fp == NULL) {
		fputs("�t�@�C���I�[�v���Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&num, sizeof(num), 1, fp) < 1) {
		fputs("�ǂݍ��ݒ��ɃG���[���������܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[0], sizeof(previous[0]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[1], sizeof(previous[1]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[2], sizeof(previous[2]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[3], sizeof(previous[3]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[4], sizeof(previous[4]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[5], sizeof(previous[5]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[6], sizeof(previous[6]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[7], sizeof(previous[7]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[8], sizeof(previous[8]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	if (fread(&previous[9], sizeof(previous[9]), 1, fp) < 1) {
		fputs("�t�@�C���ւ̏������݂Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}
	DebugProc_Print((char*)"*** �X�R�A ***\n");
	DebugProc_Print((char*)"[���Ȃ��̃X�R�A�F(%d)]\n\n\n\n", num);
	DebugProc_Print((char*)"[�X�R�A�F(%d)]\n", previous[0]);
	DebugProc_Print((char*)"[�X�R�A�F(%d)]\n", previous[1]);
	DebugProc_Print((char*)"[�X�R�A�F(%d)]\n", previous[2]);
	DebugProc_Print((char*)"[�X�R�A�F(%d)]\n", previous[3]);
	DebugProc_Print((char*)"[�X�R�A�F(%d)]\n", previous[4]);
	DebugProc_Print((char*)"[�X�R�A�F(%d)]\n", previous[5]);
	DebugProc_Print((char*)"[�X�R�A�F(%d)]\n", previous[6]);
	DebugProc_Print((char*)"[�X�R�A�F(%d)]\n", previous[7]);
	DebugProc_Print((char*)"[�X�R�A�F(%d)]\n", previous[8]);
	DebugProc_Print((char*)"[�X�R�A�F(%d)]\n", previous[9]);
	DebugProc_Print((char*)"\n\n\n\n�O�ړ�   : W\n");
	DebugProc_Print((char*)"��ړ�   : S\n");

	if (fclose(fp) == EOF) {
		fputs("�t�@�C���N���[�Y�Ɏ��s���܂����B\n", stderr);
		exit(EXIT_FAILURE);
	}

	return 0;
}