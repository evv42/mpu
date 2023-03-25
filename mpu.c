/* See LICENSE file for copyright and license details. */
//Mtk Program manager Utility (dmtk version)
//2020,2021,2022,2023 evv42
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define DMTK_IMPLEMENTATION
#include "dmtk.h"
#include "apps.h"
#include "xdummy.h"

#define MASKED 184,91,84
#define SHOWED 210,210,225
#define USED 84,166,184

#define BUTTON_BORDER 3

int mpu_print_help(){
	printf("Mtk Program manager Utility\n");
	printf("Copyright 2020,2021,2022,2023 evv42.\n");
	printf("\n");
	printf("mpu syntax:\n");
	printf("\n");
	printf("Run mpu:                         mpu <application list>\n");
	printf("Put mpu on top of other windows: mpu -r\n");
	printf("Move mpu:                        mpu -m <x> <y>\n");
	printf("Show this:                       mpu -v\n");
	printf("\n");
	return 'h';
}

char* mpu_get_time(){
    time_t rawtime;
    time(&rawtime);
	char* s = asctime(localtime(&rawtime));
	s[strlen(s)-1] = 0;
    return s;
}

void mpu_put_tab(DWindow* win, ButtonArray* bwn, int h, int v, XDummy* dmy, int idx){
	char* tabname = dmy->wnames[idx];
	if(dmy->wstates[idx]){
		mtk_add_coloured_button(win,bwn,h,v,tabname,BUTTON_BORDER,MASKED);
	}else{
		if(idx == dmy->wfocused){
			mtk_add_coloured_button(win,bwn,h,v,tabname,BUTTON_BORDER,USED);
		}else{
			mtk_add_coloured_button(win,bwn,h,v,tabname,BUTTON_BORDER,SHOWED);
		}
	}
}

ButtonArray* mpu_refr_bwn(XDummy* dmy, DWindow* win, ButtonArray* bwn, int shift){
	unsigned long oldsum = dmy->checksum;
	XDummyGetWindows(dmy);
	if(dmy->checksum == oldsum)return bwn;
	if(bwn != NULL){mtk_free_buttons(bwn);}
	bwn = mtk_init_buttons();
	mtk_put_rectangle(win,0,shift,win->rx-1,win->ry-shift-1,BG);
	mpu_put_tab(win,bwn,0,shift,dmy,0);
	for(int i=1; i<dmy->wn; i++){
		if((bwn->bts[i-1]->hxanchor + (BUTTON_BORDER*2) + (mtk_str_width(dmy->wnames[i]))) < win->rx){
			mpu_put_tab(win,bwn,bwn->bts[i-1]->hxanchor,bwn->bts[i-1]->hyanchor,dmy,i);
		}else{
			mpu_put_tab(win,bwn,0,bwn->bts[i-1]->vyanchor,dmy,i);
		}
	}
	mtk_put_rectangle(win,win->rx-1,0,1,win->ry,BORDER);
	return bwn;
}

int mpu_raise_window(XDummy* dmy){
    XDummyGetWindows(dmy);
    int p = XDummyGetWindowByName(dmy,"mpu");
    if(p == -1){printf("mpu not found.\n");return 1;}
    XDummyToggleWindow(dmy, p);
    XDummyDummy(dmy);
    return 0;
}

int mpu_move_window(XDummy* dmy, int x, int y){
    XDummyGetWindows(dmy);
    int p = XDummyGetWindowByName(dmy,"mpu");
    if(p == -1){printf("mpu not found.\n");return 1;}
    XDummyMoveWindow(dmy, p, x, y);
    XDummyDummy(dmy);
    return 0;
}

char* mpu_refr_status(int nfiles, char** files, char* status){
	if(status != NULL){free(status);}
        status = malloc(256);
        *status = 0;
        for(int i=0; i<=nfiles; i++){
            FILE* fd = fopen(files[i],"r");
            if(fd == NULL){
                perror("fopen");
            }else{
            	char* s = malloc(8);
                fgets(s, 7, fd);
				char* r = strrchr(s,'\n');
				if(r != NULL)*r=' ';
                strcat(status, s);
                free(s);
                fclose(fd);
            }
        }
	return status;
}

void mpu_draw_static(DWindow* win, char drawcatbar, char* home, ButtonArray* apps){
	unsigned long yshift;
    mtk_put_rectangle(win,0,0,win->rx,win->ry,BORDER);
    mtk_put_rectangle(win,0,0,win->rx-1,win->ry-1,BG);

	const char* title = "program manager";
	mtk_put_astring(win,((win->rx - mtk_str_width(title))/2)-8,2,title,BLK);
	DFlush(win);
	DMove(win, 0, 2+mtk_font_height()-4, win->rx, 2, 1, 2+mtk_font_height()-4);
	DMove(win, 0, 2+mtk_font_height()-6, win->rx, 2, 2, 2+mtk_font_height()-6);
	DMove(win, 0, 2+mtk_font_height()-8, win->rx, 2, 3, 2+mtk_font_height()-8);
	DMove(win, 0, 2+mtk_font_height()-10, win->rx, 2, 4, 2+mtk_font_height()-10);
	DMove(win, 0, 2+mtk_font_height()-12, win->rx, 2, 5, 2+mtk_font_height()-12);


    yshift=mtk_font_height()+4;
	mtk_put_backbox(win, 2, yshift, win->rx-5, mtk_font_height(), 1);
	yshift+=mtk_font_height()+2;
	mtk_put_rectangle(win,0,yshift,win->rx-1,1,SEP);
    if(drawcatbar){
		yshift+=4;
		mtk_put_backbox(win, 2, yshift, win->rx-5, mtk_font_height(), 1);
		yshift+=mtk_font_height()+2;
		mtk_put_rectangle(win,0,yshift,win->rx-1,1,SEP);
	}
	mtk_redraw_buttonarray(win, apps);
}

int main(int argc, char** argv){

    //check argc length
    if(argc<2){printf("Too few arguments.\n");return mpu_print_help();}

    XDummy* dmy = CreateXDummy();

    //other program behaviours here
    if(*argv[1] == '-'){
        switch(*(argv[1]+1)){
            case 'r':
                return mpu_raise_window(dmy);
            case 'm':
                if(argc>2)return mpu_move_window(dmy, atoi(argv[2]), atoi(argv[3]));
                printf("mpu -m: Not enough arguments\n");
                return mpu_print_help();
            case 'h':
                return mpu_print_help();
            default:
		printf("mpu: Invalid switch\n");
		return mpu_print_help();
        }
    }
    //main program

    char* home = getenv("HOME");
    printf("$HOME is %s\n",home);

	//compute window size
    int xsize = 224;
    unsigned int ysize = mtk_font_height() + (MAX_WINDOWS_DISPLAYED)*(mtk_font_height() + BUTTON_BORDER*2);
	
	//open list of apps
    int napps;
	char fileloc[256];
	snprintf(fileloc,256,"%s/.config/mpu/%s",home,argv[1]);
    char** appsloc = filetotable(fileloc, &napps);
    if(appsloc == NULL)return 12;
    //getfiles
    *fileloc = 0;
	snprintf(fileloc,256,"%s/.config/mpu/files.tbl",home);
    int nfiles = 0;
    char** files = filetotable(fileloc,&nfiles);

	char drawcatbar = 1;
	char* status = NULL;
    if(files == NULL){
		printf("%s missing. Perhaps you would want to add one.\n",fileloc);
		drawcatbar = 0;
	}
    if(drawcatbar){
		status = malloc(256);
		status = mpu_refr_status(nfiles,files,NULL);
		
	}

    //open window and draw static objects once
    DWindow* win = DInitBorderless(xsize,ysize,"mpu");

    unsigned long yshift;
	
    yshift=mtk_font_height()+4;
	yshift+=mtk_font_height()+2;
    if(drawcatbar){
		yshift+=4;
		yshift+=mtk_font_height()+2;
	}
	yshift+=2;
    ////generate buttons
    ButtonArray* apps = mtk_init_buttons();
	mtk_add_button(win,apps,0,yshift,appsloc[0],BUTTON_BORDER);
    for(int i=1; i<=napps; i++){
		if((apps->bts[i-1]->hxanchor + (BUTTON_BORDER*2) + (mtk_str_width(appsloc[i]))) < win->rx){
			mtk_add_button(win,apps,apps->bts[i-1]->hxanchor,apps->bts[i-1]->hyanchor,appsloc[i],BUTTON_BORDER);
		}else{
			mtk_add_button(win,apps,0,apps->bts[i-1]->vyanchor,appsloc[i],BUTTON_BORDER);
		}
    }
    yshift = apps->bts[apps->last_button]->vyanchor;
    free(appsloc);

    ButtonArray* bwn = mpu_refr_bwn(dmy, win, NULL, yshift);

    char cycles = 0;

    //refresh time, get button press, if any
    while(win->alive){

        if(cycles == 4){
            if(drawcatbar)status = mpu_refr_status(nfiles,files,status);
            mtk_put_string(win, 2, mtk_font_height()+4, mpu_get_time(), BGB, FG);
            if(drawcatbar)mtk_put_string(win, 2, mtk_font_height()+4+2+mtk_font_height()+4, status, BGB, FG);
            bwn = mpu_refr_bwn(dmy, win, bwn, yshift);
			DFlush(win);
            cycles = 0;
        }
        
        GUIRequest grq = DGetRequest(win);

		int wasted;
		switch(grq.type){
			case RESIZE_RQ:
				dmy->checksum = 0;//Forces redraw of button array
				mpu_draw_static(win, drawcatbar, home, apps);
				break;
			case MOUSE_RQ:
				if(grq.data == 1){
					wasted = mtk_get_button(win,apps,grq.x,grq.y);
					if(wasted >= 0)runcommand(apps->bts[wasted]->text);
					wasted = mtk_get_button(win,bwn,grq.x,grq.y);
					if(wasted >= 0)XDummyToggleWindow(dmy, wasted);
				}else if(grq.data == 3){
					wasted = mtk_get_button(win,bwn,grq.x,grq.y);
					if(wasted >= 0)XDummyHideWindow(dmy, wasted);
				}
				dmy->checksum = 0;
				break;
			default:
				break;
		}

        usleep(150000);
        cycles += 1;
    }

    //free all stuff
    XDummyDummy(dmy);
    mtk_free_buttons(bwn);
    mtk_free_buttons(apps);
	if(drawcatbar)free(status);
    return 0;
}
