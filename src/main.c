/*
 *  Copyright (C) 2018 qwikrazor87
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pspkernel.h>
#include <pspctrl.h>
#include <psputility.h>
#include <string.h>
#include <stdio.h>
#include "lib.h"

PSP_MODULE_INFO("ps1undoc", 0, 1, 1);
PSP_HEAP_SIZE_KB(0);

char path[256], docs[128][128];
int count = 0;

#define printf pspDebugScreenPrintf
#define __PS1UNDOCVERSION__ "v1.0"

int main_thread(SceSize args __attribute__((unused)), void *argp __attribute__((unused)))
{
	int i = 0, j, k, l;
	SceUID fd;
	pspDebugScreenInit();

	printf("PS1 Undoc %s\nCustom PS1 DOCUMENT.DAT dumper.\nby qwikrazor87\n\n", __PS1UNDOCVERSION__);

	SceUID dfd = sceIoDopen("ms0:/PSP/GAME");

	if (dfd >= 0) {
		SceIoDirent dir;
		memset(&dir, 0, sizeof(SceIoDirent));

		while (sceIoDread(dfd, &dir) > 0) {
			if (dir.d_stat.st_mode & FIO_S_IFDIR) {
				sprintf(path, "ms0:/PSP/GAME/%s/DOCUMENT.DAT", dir.d_name);

				fd = sceIoOpen(path, 1, 0777);

				if (fd >= 0) {
					sceIoRead(fd, path, 4);

					if (!memcmp(path, "DOC ", 4))
						sprintf(docs[count++], "%s", dir.d_name);

					sceIoClose(fd);
				}
			}
		}

		sceIoDclose(dfd);

		if (count) {
			printf("Custom manuals found: %d\n\n", count);
			printf("Up/Down - select manual to dump.\n");
			printf("X - dump selected manual.\n");
			printf("[] - dump all manuals.\n");
			printf("O - exit.\n\n");

			sceIoMkdir("ms0:/DOCS", 0777);

			int y = pspDebugScreenGetY();
			pspDebugScreenSetTextColor(0xFF00);
			printf("%s\n%d/%d selected\n", docs[i], i + 1, count);
			pspDebugScreenSetTextColor(-1);

			SceCtrlData pad;
			u32 oldbutton = 0;
			while (1) {
				sceCtrlPeekBufferPositive(&pad, 1);

				if ((pad.Buttons & PSP_CTRL_UP) && (oldbutton != pad.Buttons)) {
					i--;

					if (i < 0)
						i = 0;

					pspDebugScreenSetXY(0, y - 1);
					pspDebugScreenSetTextColor(0xFF00);
					printf("\n%s\n%d/%d selected\n", docs[i], i + 1, count);
					pspDebugScreenSetTextColor(-1);
				}

				if ((pad.Buttons & PSP_CTRL_DOWN) && (oldbutton != pad.Buttons)) {
					i++;

					if (i >= count)
						i = count - 1;

					pspDebugScreenSetXY(0, y - 1);
					pspDebugScreenSetTextColor(0xFF00);
					printf("\n%s\n%d/%d selected\n", docs[i], i + 1, count);
					pspDebugScreenSetTextColor(-1);
				}

				if (pad.Buttons & PSP_CTRL_CROSS) {
					sprintf(path, "ms0:/DOCS/PS1_%s", docs[i]);
					sceIoMkdir(path, 0777);

					sprintf(path, "ms0:/PSP/GAME/%s/DOCUMENT.DAT", docs[i]);
					fd = sceIoOpen(path, 1, 0777);
					int sz = sceIoLseek(fd, 0, 2);
					sceIoLseek(fd, 0, 0);
					SceUID mem = sceKernelAllocPartitionMemory(2, "tmpbuf", PSP_SMEM_Low, sz, NULL);
					u8 *tmpbuf = (u8 *)sceKernelGetBlockHeadAddr(mem);

					sceIoRead(fd, tmpbuf, sz);
					sceIoClose(fd);

					k = _lw((u32)tmpbuf + 0x84);

					for (j = 0; j < k; j++) {
						u32 ofs = _lw((u32)tmpbuf + 0x88 + (j << 7));
						u32 sz = _lw((u32)tmpbuf + 0x94 + (j << 7));

						sprintf(path, "ms0:/DOCS/PS1_%s/DOC_%03d.PNG", docs[i], j);
						fd = sceIoOpen(path, 0x602, 0777);
						sceIoWrite(fd, (const void *)((u32)tmpbuf + ofs), sz);
						sceIoClose(fd);
					}

					sceKernelFreePartitionMemory(mem);

					pspDebugScreenSetXY(0, y + 2);
					printf("%s/DOCUMENT.DAT dumped.\n", docs[i]);
				}

				if (pad.Buttons & PSP_CTRL_SQUARE) {
					for (l = 0; l < count; l++) {
						sprintf(path, "ms0:/DOCS/PS1_%s", docs[l]);
						sceIoMkdir(path, 0777);

						sprintf(path, "ms0:/PSP/GAME/%s/DOCUMENT.DAT", docs[l]);
						fd = sceIoOpen(path, 1, 0777);
						u32 sz = sceIoLseek(fd, 0, 2);
						sceIoLseek(fd, 0, 0);
						SceUID mem = sceKernelAllocPartitionMemory(2, "tmpbuf", PSP_SMEM_Low, sz, NULL);
						u8 *tmpbuf = (u8 *)sceKernelGetBlockHeadAddr(mem);

						sceIoRead(fd, tmpbuf, sz);
						sceIoClose(fd);

						k = _lw((u32)tmpbuf + 0x84);

						for (j = 0; j < k; j++) {
							u32 ofs = _lw((u32)tmpbuf + 0x88 + (j << 7));
							sz = _lw((u32)tmpbuf + 0x94 + (j << 7));

							sprintf(path, "ms0:/DOCS/PS1_%s/DOC_%03d.PNG", docs[l], j);
							fd = sceIoOpen(path, 0x602, 0777);
							sceIoWrite(fd, (const void *)((u32)tmpbuf + ofs), sz);
							sceIoClose(fd);
						}

						sceKernelFreePartitionMemory(mem);

						pspDebugScreenSetXY(0, y + 2);
						printf("%s/DOCUMENT.DAT dumped.\n", docs[i]);
					}

					pspDebugScreenSetXY(0, y + 2);
					printf("%d manuals dumped.\n", count);
				}

				if (pad.Buttons & PSP_CTRL_CIRCLE)
					break;

				oldbutton = pad.Buttons;
			}
		} else
			printf("No custom PS1 manuals found in game folders.\n");
	} else
		printf("Unable to open ms0:/PSP/GAME/\n");

	printf("\nExiting...");
	sceKernelDelayThread(5000000);
	sceKernelExitGame();

	return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp)
{
	SceUID thid = sceKernelCreateThread("main_thread", main_thread, 32, 0x40000, THREAD_ATTR_USER, NULL);
	sceKernelStartThread(thid, args, argp);

	return 0;
}

int module_stop(SceSize args __attribute__((unused)), void *argp __attribute__((unused)))
{
	return 0;
}
