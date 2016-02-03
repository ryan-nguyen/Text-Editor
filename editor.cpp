//Ryan Nguyen
//Contact: ryan.nguyen25@uga.edu
//Create a text editor using ncurses library and low level I/O. This file holds main.

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <string>

#include <ncurses.h>
#include <menu.h>
#include "pfunc.h"

WINDOW *create_newwin(int height, int width, int starty, int startx);

int main(const int argc, const char * argv[]){
	initscr();

	refresh();
	
	char a = 'a';
	const char * filename = &a;
	const char * newfilename = &a;
	int fd;
	keypad(stdscr, TRUE);
	do{
		//if the there is an argument for filename upon execution, saves to filename else prompts user
		if(argc==2){
			filename = argv[1];
//			fd = open(filename, O_RDONLY);
			
		}
		else {
			char mesg[] = "Enter a file name: ";
			printw(mesg);
			refresh();
			getstr((char*)filename);
//			fd = open(filename, O_RDONLY);
		}
		fd = open(filename, O_RDONLY);
		
		//creates an error window if there is an error aka fd ==-1
		//keeps repeating process if user inputs non existing file
		if(fd==-1){
			WINDOW * errorwindow;
			errorwindow = create_newwin(6,50 , (LINES-(LINES/2))/2, (COLS-(COLS/2))/2);
			wborder(errorwindow, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
			
			mvwprintw(errorwindow, 1, 1, "%s", "An error has occurred. Could not open file.");
//			wprintw(errorwindow, "%c", filename);
			mvwprintw(errorwindow, 2, 1, "%s", "Error: ");
			wprintw(errorwindow, "%s", strerror(errno));	
			mvwprintw(errorwindow, 4, 1, "%s", "PRESS ENTER TO CONTINUE");
			wrefresh(errorwindow);

			do{
				noecho();
			}while(getch() != '\n');

			echo();
			werase(errorwindow);
			wrefresh(errorwindow);
			delwin(errorwindow);
			clear();
			refresh();
		}
		clear();
		refresh();
	
	}while(fd ==-1);

	noecho();	

	//stdscr window labels
	printw("F1: Menu");
	mvprintw(LINES-1, 0, "%s", filename);
	mvprintw(0, COLS/2-8, "%s", "CSCI 1730 Editor!");
	refresh();

	//border window
	int startx, starty, width, height;
	height = LINES-6;
	width = COLS-5;
	starty = (LINES-height)/2;
	startx = (COLS-width)/2;
	WINDOW * border;
	border = create_newwin(height, width, starty,startx+3);
	wborder(border, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	
	//input window
	WINDOW * textfield;
	int textfieldrows = height-2;
	int textfieldcols = width-5;
	textfield = create_newwin(textfieldrows, textfieldcols, starty+1, startx+4);
	keypad(textfield, TRUE);
	scrollok(textfield, TRUE);
//	wrefresh(textfield);
//	wrefresh(border);
	
	//number window
	WINDOW * number_win;
	number_win = create_newwin(textfieldrows, 4, starty+1, startx-2);
	scrollok(number_win, TRUE);
	int numwinnum = 1;
	int rownum = 0;
	mvwprintw(number_win, rownum, 1, "%d", numwinnum);
//	wprintw(number_win, "%c", '\n');
//	mvwprintw(number_win, 1, 1, "%d", numwinnum);

	
	wrefresh(textfield);
	wrefresh(border);
	wrefresh(number_win);
	
	fd = open(filename, O_RDONLY);
	int n;
	char buffer[256];

	//preparing buffer that stores everything that is present in text editor
	int p = 0;
//	int size = 3000;
	char editorbuffer[3000];
	int b;
	for(b = 0; b<3000; ++b){
		editorbuffer[b] = '\0';
	}

	//reads from file and prints and stores char into editor buffer. editorbuffer is gonna save read data in addition to user input data.
	while((n=read(fd, buffer, 256)) >0) {
		if(n!=256) buffer[n] = '\0';
		wprintw(textfield, "%s", buffer);
		wrefresh(textfield);
//		editorbuffer[p] = (char)buffer;
		strcpy(editorbuffer, buffer);
		
	}

	int cr = close(fd);
	int temp = 0;
	
	//while loop used to set p to correct position after reading from file so that it can be used to store things written after
	//rownum also incremented here to accomodate for lines from read file
	while(editorbuffer[p] != '\0'){
		p++;
		if(editorbuffer[p] == '\n'){
			temp = 0;
			rownum++;
		}
		temp++;
		if(temp==textfieldcols){
			rownum++;
			temp = 0;
		}
	}
	//print row numbers from read file
	for(b = 1; b<rownum+1; ++b){
		mvwprintw(number_win, b, 1, "%d", b+1);
	}
	
	rownum++;
	wrefresh(number_win);
	wrefresh(textfield);

	int ch;
	int cursorx = 0;
	int cursory = 0;
	int len;
	int g;
	int maxx, maxy;
	maxx=0;
	maxy=0;
	
	fd = open(filename, O_WRONLY);
	while((ch=wgetch(textfield))){
		//switch statment for user input into textfield
		switch(ch){
			//menu is created if F1 is pressed.
			case(KEY_F(1)):
			{	
				//menu
				char const * choices[] = {
							"Open",
							"Save",
							"Save As",
							"Exit"
//(char*)NULL
						  };
				int n_choices;
				int c;
				WINDOW * menuop_win;
				MENU * menuop;
				ITEM ** menuop_items;
//				n_choices = (sizeof(choices)/sizeof(choices[0]));
				n_choices = 4;
				menuop_items = (ITEM **)calloc(n_choices, sizeof(ITEM **));
				for(int j=0; j<n_choices; ++j){
					menuop_items[j] = new_item(choices[j], "");
				}
				menuop = new_menu((ITEM **)menuop_items);
				menuop_win = newwin(10,40, (LINES -(5))/2,(COLS-(40))/2);
				mvwprintw(menuop_win, 8, 1, "%s", "press F1 to exit menu");
				wrefresh(menuop_win);
				keypad(menuop_win, TRUE);
				
				set_menu_win(menuop, menuop_win);
				set_menu_sub(menuop, derwin(menuop_win, 6, 38, 3, 1));
				
				set_menu_mark(menuop, " * ");

				wborder(menuop_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);	
				mvwprintw(menuop_win, 1, 6, "%s", "My Menu");
				mvwhline(menuop_win, 2, 1, ACS_HLINE, 38);				
				refresh();
				post_menu(menuop);
				wrefresh(menuop_win);
				//menucounter used to keep track of which option is currently highlighted
				int menucounter = 0;
				bool more = true;
				bool ynt = true;
				int yn = 0;
				while((more == true) && (c=wgetch(menuop_win))){
					switch(c) {
						case(KEY_DOWN): //moves highlighter and increments menucounter
							menu_driver(menuop,REQ_DOWN_ITEM);
							if(menucounter != 3){
								menucounter++;
							}
							break;
						case(KEY_UP):
							menu_driver(menuop,REQ_UP_ITEM);
							if(menucounter != 0){
								menucounter--;
							}
							break;
						case('\n'): //when user selects using enter key, another switch statement determines what happens based on option selected
							switch(menucounter){
								case(0): //open option, prompts to save current file
									WINDOW * open_win;
									open_win = create_newwin(8, 50, LINES/5, COLS/5);
									wborder(open_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
									mvwprintw(open_win, 1, 1, "%s", "Would you like to save the current file?");
									mvwprintw(open_win, 4, 1, "%s", "PRESS 'Y' FOR YES");
									mvwprintw(open_win, 5, 1, "%s", "PRESS 'N' FOR NO");
									wrefresh(open_win);
									ynt = true;

									while((ynt == true) && (yn = wgetch(open_win))){
										if(yn == 'y'){
											//save
											len = strlen(editorbuffer);
											g = write(fd, editorbuffer, len);
											cr =  close(fd);
											ynt = false;
										}
										if(yn == 'n'){
											cr = close(fd);
											ynt = false;
										}
									}
									
									werase(open_win);
									wrefresh(open_win);
									do{ //prompts user for new file to open and then opens and deletes current data on screen and prints new data from new file to screen
										echo();
										wborder(open_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
										mvwprintw(open_win, 1, 1, "%s", "Enter a new file name: ");
										wrefresh(open_win);
										wgetstr(open_win, (char*)filename);
										fd = open(filename, O_RDONLY);

										if(fd==-1){
											mvwprintw(open_win, 3, 1, "%s", "An error has occurred. Could not open file.");
											mvwprintw(open_win, 4, 1, "%s", "Error: ");
											wprintw(open_win, "%s", strerror(errno));	
											mvwprintw(open_win, 6, 1, "%s", "PRESS ENTER TO CONTINUE");
											wrefresh(open_win);

											do{	
												noecho();
											}while(getch() != '\n');

											werase(open_win);
											wrefresh(open_win);
										}else{
											werase(open_win);
											wrefresh(open_win);
											noecho();
											redrawwin(textfield);
											wclear(textfield);
											wrefresh(textfield);
											p = 0;
											for(b = 0; b<3000; ++b){
												editorbuffer[b] = '\0';
											}

											while((n=read(fd, buffer, 256)) >0) {
												if(n!=256) buffer[n] = '\0';
												wprintw(textfield, "%s", buffer);
												wrefresh(textfield);
												strcpy(editorbuffer, buffer);
											}
											cr = close(fd);
											mvprintw(LINES-1, 0, "%s", filename);
											refresh();
											redrawwin(menuop_win);
											wrefresh(menuop_win);	
										}
	
									}while(fd ==-1);
									
									delwin(open_win);
									
									break;
								case(1): //writes to the file that is open
									fd = open(filename, O_WRONLY, 0644);
									len = strlen(editorbuffer);
									g = write(fd, editorbuffer, len);
									cr = close(fd);
									WINDOW * save_win;
									save_win = create_newwin(9, 60, LINES/5, COLS/5);
									wborder(save_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
									mvwprintw(save_win, 4, 19, "%s", "Save success");
									mvwprintw(save_win, 7, 1, "%s", "PRESS ANY KEY TO CONTINUE");
									wrefresh(save_win);
									wgetch(save_win);
									werase(save_win);
									wrefresh(save_win);
									delwin(save_win);
									
									break;
								case(2): //if it exists, writes to the file, if it doesnt exist, then creates new file
									
									WINDOW * saveas_win;
									saveas_win = create_newwin(9, 60, LINES/5, COLS/5);
									wborder(saveas_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
									mvwprintw(saveas_win, 1, 1, "%s", "A new file will be created if it does not exist");	
									mvwprintw(saveas_win, 2, 1, "%s", "Enter the name of the file to save to: ");
									wrefresh(saveas_win);
									echo();
									wgetstr(saveas_win, (char*)newfilename);
									noecho();
									fd = open(newfilename,O_WRONLY);
									
									if(fd == -1){
										werase(saveas_win);
										wrefresh(saveas_win);
										wborder(saveas_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
										mvwprintw(saveas_win, 4, 19, "%s", "Save success");
										mvwprintw(saveas_win, 7, 1, "%s", "PRESS ANY KEY TO CONTINUE");
										wrefresh(saveas_win);
										wgetch(saveas_win);
										werase(saveas_win);
										wrefresh(saveas_win);
										delwin(saveas_win);
										fd = open(newfilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
										len = strlen(editorbuffer);
										g = write(fd, editorbuffer, len);
										cr = close(fd);
									}
									else{
										werase(saveas_win);
										wrefresh(saveas_win);
										wborder(saveas_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
										mvwprintw(saveas_win, 1, 1, "%s", "The file already exists.");
										mvwprintw(saveas_win, 2, 1, "%s", "Would you like to overrite it?");
										mvwprintw(saveas_win, 4, 1, "%s", "PRESS 'Y' FOR YES");
										mvwprintw(saveas_win, 5, 1, "%s", "PRESS 'N' FOR NO");					
										wrefresh(saveas_win);
										ynt = true;
										while((ynt == true) && (yn = wgetch(saveas_win))){
											if(yn == 'y'){
												fd = open(newfilename, O_WRONLY | O_CREAT | O_TRUNC);
												len = strlen(editorbuffer);
												g = write(fd, editorbuffer, len);
												cr =  close(fd);
												ynt = false;
												werase(saveas_win);
												wrefresh(saveas_win);
												wborder(saveas_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);		
												mvwprintw(saveas_win, 4, 19, "%s", "Save success");
												mvwprintw(saveas_win, 7, 1, "%s", "PRESS ANY KEY TO CONTINUE");
												wgetch(saveas_win);
												werase(saveas_win);
												wrefresh(saveas_win);
												delwin(saveas_win);							
											}
											if(yn == 'n'){
												cr = close(fd);
												ynt = false;
												werase(saveas_win);
												wrefresh(saveas_win);
												wborder(saveas_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);		
												mvwprintw(saveas_win, 4, 19, "%s", "Save cancelled");
												mvwprintw(saveas_win, 7, 1, "%s", "PRESS ANY KEY TO CONTINUE");
												wgetch(saveas_win);
												werase(saveas_win);
												wrefresh(saveas_win);
												delwin(saveas_win);							
												
											}
										}
										
										
									}
									break;
								case(3): //prompts for save before exit
									WINDOW * close_win;
									close_win = create_newwin(8, 50, LINES/5, COLS/5);
									wborder(close_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
									mvwprintw(close_win, 1, 1, "%s", "Would you like to save the current file?");
									mvwprintw(close_win, 4, 1, "%s", "PRESS 'Y' FOR YES");
									mvwprintw(close_win, 5, 1, "%s", "PRESS 'N' FOR NO");
									wrefresh(close_win);
									ynt = true;

									while((ynt == true) && (yn = wgetch(close_win))){
										if(yn == 'y'){
											//save
											len = strlen(editorbuffer);
											g = write(fd, editorbuffer, len);
											ynt = false;
										}
										if(yn == 'n'){
											ynt = false;
										}
									}

									werase(close_win);
									
									goto stop;
									break;
								}
							break;
						case(KEY_F(1)): //closes the menu by deleting it
							unpost_menu(menuop);
							free_menu(menuop);
							for(int k=0; k<n_choices; ++k)
								free_item(menuop_items[k]);
							//wborder(menuop_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
							werase(menuop_win);
							wrefresh(menuop_win);
							delwin(menuop_win);
							redrawwin(textfield);
							//wrefresh(textfield);
							more = false;
							break;
					}
					wrefresh(menuop_win);
				}
				
			}	break;
				
			case(KEY_BACKSPACE): //deletes char on screen and increments p accoringly
				//bkspce
				wprintw(textfield, "%c", '\b');
				wdelch(textfield);
				wrefresh(textfield);

				if(p>0){
					editorbuffer[(p-1)] = '\0';
					p--;
				}
				break;
			case(KEY_DOWN): //moves cursor if it is not on bottom most editted line and increments p
				if(getcury(textfield)<maxy){
					getyx(textfield, cursory, cursorx);
					wmove(textfield, cursory+1, cursorx);
					if(p<(3000-textfieldcols)){
						p+=textfieldcols;
					}
				}
			//	if(getcury(textfield) >= textfieldrows){
			//		
			//	}
					break;
			case(KEY_UP): //moves cursor up if it is below screen height, increments p
				getyx(textfield, cursory, cursorx);
				wmove(textfield, cursory-1, cursorx);
				if(p>textfieldcols){
					p-=textfieldcols;
				}
				break;
			case(KEY_RIGHT): //increments p, moves cursor right 
			//	if(getcurx(textfield)<maxx){
					getyx(textfield, cursory, cursorx);
					wmove(textfield, cursory, cursorx+1);
					if(p<3000){
						p++;
					}
			//	}
				break;
			case(KEY_LEFT): //same..
				getyx(textfield, cursory, cursorx);
				wmove(textfield, cursory, cursorx-1);
				if(p>0){
					p--;
				}
				break;
			default: //prints using winsch if not enter so that the characters are moved to the right when inserted.
			//	wprintw(textfield, "%c", ch);
		//		waddch(textfield, ch);
				if(ch!= '\n'){
					winsch(textfield, ch);
					getyx(textfield, cursory, cursorx);
					wmove(textfield, cursory, cursorx+1);
				}
				else{
					waddch(textfield, ch);
					rownum++;
					mvwprintw(number_win, rownum-1, 1, "%d", rownum);
					wrefresh(number_win);
				}
				break;
		}	
		wrefresh(textfield);
		if(ch!=KEY_F(1) && ch!=KEY_BACKSPACE){ //adds to editorbuffer here if not f1 or backspaces
			editorbuffer[p] = ch;
		//	if(ch=='\n'){
		//		if(p<(3000-textfieldcols)){
		//			p+=textfieldcols;
				
					//for(int r = p-textfieldcols; r<p; ++r){
					//	editorbuffer[r] = ' ';
					//}
		//		}
		//	}
		//	else{
				p++;
		//	}
		}
		if(maxx<getcurx(textfield)){
			maxx = getcurx(textfield);
		}
		if(maxy<getcury(textfield)){
			maxy = getcury(textfield);
		}
	}	

	stop: //goto statement close and end win
	int cw = close(fd);
	
	endwin();
	return 0;
	
}



WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
		
	wrefresh(local_win);

	return local_win;
}

/*void readfile(const char * filename){
	int fd = open(filename, O_RDONLY);
	if(fd == -1){				//when fd is -1, an error has occurred. print error message to ncurses window
		char mg[] = "An error has occurred. Could not open ";
		printw(mg);
		printw(filename);
		printw("\nError: ");
		printw(strerror(errno));
		refresh();
	} //if(fd == -1)
	int n;
	char buffer[256];
	while((n=read(fd, buffer, 256)) > 0) {	//reads file that has been opened with fd. reads 256 characters (same as buffer size)
		if(n!=256) buffer[n]='\0';
		printw(buffer);
	}//while
	
	int cr = close(fd);			//closes file
	if (cr == -1){				//when cr is -1, an error has occurred. print error message to ncurses window
		clear();
		char mg2[] = "An error has occurred. Could not close. \nError: ";
		printw(mg2);
		printw(strerror(errno));
		refresh();
	} //if(cr == -1)
} //readfile()
*/
