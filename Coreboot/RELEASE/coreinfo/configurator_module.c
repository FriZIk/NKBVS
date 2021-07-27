/* SPDX-License-Identifier: GPL-2.0-only */

#include "coreinfo.h"

#include <coreboot_tables.h>
#include <libpayload.h>

#if CONFIG(MODULE_NVRAM)

static int max(int x, int y)
{
	if (x > y)
		return x;
	return y;
}

static int min(int x, int y)
{
	if (x < y)
		return x;
	return y;
}


/* Check configurator pattern */
static int check_pattern(char *str, char *pattern)
{
	int pattern_len = strlen(pattern);
	char *pointer;
	int result = 0;

	for(int i = 0;i < pattern_len;i++)
	{
		pointer = strchr(str, pattern[i]); 
		result = (pointer - str) + result;
	}

	if(result == pattern_len) {
		result = 0; 
	}
	else { 
		result = 1;        
	}

	return result;
}

/* determine number of options, and maximum option name length */
static int count_cmos_options(struct cb_cmos_entries *option, int *numopts,
		int *maxlength)
{
	int n_opts = 0;
	int max_l = 0;

	while (option) 
	{
		// Skip unnecessary registers and enumerations
		int check = check_pattern((char*)(option->name),"CFG"); 

		if ((option->config != 'r') && 
			(check != 0) &&
		    (strcmp("check_sum", (char *)option->name) != 0)) {
			max_l = max(max_l, strlen((char *)option->name));
			n_opts++;
		}

		option = next_cmos_entry(option);
	}

	if (n_opts == 0) {
		printf("NO CMOS OPTIONS FOUND. EXITING!!!");
		return -1;
	}

	*numopts = n_opts;
	*maxlength = max_l;

	return 0;
}

static void cmos_walk_options(struct cb_cmos_option_table *opttbl, int numopts, 
	FIELD **fields,int maxlength)
{
	struct cb_cmos_entries *option = first_cmos_entry(opttbl);
	        
	for (int i = 0; i < numopts; i++) 
	{
		int check = check_pattern((char*)(option->name),"CFG");
		while ((option->config == 'r') || (check == 0) ||
			   (strcmp("check_sum", (char *)option->name) == 0)) {
			option = next_cmos_entry(option);
		}

		fields[2 * i] =
		    new_field(1, strlen((char *)option->name), i * 2, 1, 0, 0);
		set_field_buffer(fields[2 * i], 0, (char *)option->name);
		field_opts_off(fields[2 * i], O_ACTIVE);

		fields[2 * i + 1] =
		    new_field(1, 40, i * 2, maxlength + 2, 0, 0);
		char *buf = NULL;
		int fail =
		    get_option_as_string(use_nvram, opttbl, &buf, (char *)option->name);

		switch(option->config)
		{
			case 'h': {
				set_field_type(fields[2 * i + 1], TYPE_INTEGER, 0, 0,
				       (1 << option->length) - 1);
				field_opts_on(fields[2 * i + 1], O_BLANK);
				
				break;
			}
			case 's': {
				set_max_field(fields[2 * i + 1], option->length / 8);
				field_opts_off(fields[2 * i + 1], O_STATIC);
				break;
			}
			case 'e': {
				int numvals = 0;
				struct cb_cmos_enums *cmos_enum =
					first_cmos_enum_of_id(opttbl, option->config_id);
				
				/* if invalid data in CMOS, set buf to first enum */
				if (fail && cmos_enum) {
					buf = (char *)cmos_enum->text;
				}

				while (cmos_enum) {
					numvals++;
					cmos_enum = next_cmos_enum_of_id(
						cmos_enum, option->config_id);
				}

				char **values = malloc(sizeof(char *) * (numvals + 1));
				int cnt = 0;

				cmos_enum =
					first_cmos_enum_of_id(opttbl, option->config_id);
				while (cmos_enum) {
					values[cnt] = (char *)cmos_enum->text;
					cnt++;
					cmos_enum = next_cmos_enum_of_id(
						cmos_enum, option->config_id);
				}
				values[cnt] = NULL;
				field_opts_off(fields[2 * i + 1], O_EDIT);
				set_field_type(fields[2 * i + 1], TYPE_ENUM, values, 1,
						1);
				free(values); // copied by set_field_type
				break;
			}
			default:
				break;
		}
		if (buf)
			set_field_buffer(fields[2 * i + 1], 0, buf);
#if HOSTED
		// underline is non-trivial on VGA text
		set_field_back(fields[2 * i + 1], A_UNDERLINE);
#endif
		field_opts_off(fields[2 * i + 1],O_BLANK | O_AUTOSKIP | O_NULLOK);
		option = next_cmos_entry(option);
	}

	fields[2 * numopts] = NULL;
}

static void render_form(FORM *form)
{
	int y, x, line;
	WINDOW *w = form_win(form);
	WINDOW *inner_w = form_sub(form);
	int numlines = getmaxy(w) - 2;
	getyx(inner_w, y, x);
	line = y - (y % numlines);
	WINDOW *der = derwin(w, getmaxy(w) - 2, getmaxx(w) - 2, 1, 1);
	wclear(der);
	wrefresh(der);
	delwin(der);
	copywin(inner_w, w, line, 0, 1, 1,
		min(numlines, getmaxy(inner_w) - line), 68, 0);

	// Вертикальная полоса 
	for (int i = 1; i <  getmaxy(w); i++)
		mvwaddch(w, i, getmaxx(w) - 15 , ACS_VLINE);

	// Горизонатльная черта
	for (int i = 1;i < getmaxx(w) - 15; i++)
		mvwaddch(w, getmaxy(w) - 5, i, ACS_HLINE);


	// Нижние поля
	mvwprintw(w, getmaxy(w) - 4, 1, "Coomands:");
	mvwprintw(w, getmaxy(w) - 4, 1, "1. Save configuration - change window, double click(F1, F2, F4)");
	mvwprintw(w, getmaxy(w) - 3, 1, "2. Some functional);");
	mvwprintw(w, getmaxy(w) - 2, 1, "3. Some functional);");

	// Информационное поле 
	mvwprintw(w, 1, getmaxx(w) - 13, "Information");
	for (int i = getmaxx(w) - 14;i < getmaxx(w); i++)
		mvwaddch(w, 2, i, ACS_HLINE);

	wmove(w, y + 1 - line, x + 1);
	wrefresh(w);
}

static int configurator_module_redraw(WINDOW *win)
{
	if (CONFIG(LP_USB))
		usb_initialize();

	/* coreboot data structures */
	lib_get_sysinfo();

	struct cb_cmos_option_table *opttbl = get_system_option_table();

	if (opttbl == NULL) {
		printf("Could not find coreboot option table.\n");
		halt();
	}

	struct cb_cmos_entries *option = first_cmos_entry(opttbl);
	int numopts = 0;
	int maxlength = 0;

	count_cmos_options(option, &numopts, &maxlength);

	FIELD **fields = malloc(sizeof(FIELD *) * (2 * numopts + 1));
	cmos_walk_options(opttbl, numopts, fields, maxlength);

	FORM *form = new_form(fields);
	WINDOW *inner_w = newpad(numopts * 2, 68);

	print_module_title(win, "Configurator");
	
	// Цвета
	init_pair(5,COLOR_BLACK, COLOR_CYAN);
	wbkgd(win, COLOR_PAIR(5));
	wbkgd(inner_w, COLOR_PAIR(5));

	set_form_win(form, win);
	set_form_sub(form, inner_w);
	post_form(form);

	int i;
	int ch, done;

	leaveok(stdscr, TRUE);
	curs_set(1);

	done = 0;
	while (!done) {
		render_form(form);
		ch = getch();
		if (ch == ERR)
			continue;
		switch (ch) {
		case KEY_DOWN:
			form_driver(form, REQ_NEXT_FIELD);
			break;
		case KEY_UP:
			form_driver(form, REQ_PREV_FIELD);
			break;

		case KEY_LEFT:
			if (field_type(current_field(form)) == TYPE_ENUM) {
				form_driver(form, REQ_PREV_CHOICE);
			} else {
				form_driver(form, REQ_LEFT_CHAR);
			}
			break;
		case KEY_RIGHT:
			if (field_type(current_field(form)) == TYPE_ENUM) {
				form_driver(form, REQ_NEXT_CHOICE);
			} else {
				form_driver(form, REQ_RIGHT_CHAR);
			}
			break;
		case KEY_BACKSPACE:
		case '\b':
			form_driver(form, REQ_DEL_PREV);
			break;
		case KEY_DC:
			form_driver(form, REQ_DEL_CHAR);
			break;
		case KEY_F(1):
			done = 1;
			break;
		case KEY_F(2):
			done = 1;
			break;
		case KEY_F(4):
			done = 1;
			break;
		default:
			form_driver(form, ch);
			break;
		}
	}

	endwin();

	for (i = 0; i < numopts; i++) 
	{
		char *name = field_buffer(fields[2 * i], 0);
		char *value = field_buffer(fields[2 * i + 1], 0);
		char *ptr;
		for (ptr = value + strlen(value) - 1;
		     ptr >= value && *ptr == ' '; ptr--)
			;
		ptr[1] = '\0';
		set_option_from_string(use_nvram, opttbl, value, name);
	}

	unpost_form(form);
	free_form(form);

	// Отчистка памяти, которая никак не помогает
	for (int i = 0; i < 2 * numopts + 1; i++) 
	{
        free(fields[i]);
    }
    free(fields);
	return 0;
}

static int configurator_module_init(void)
{
	return 0;
}

struct coreinfo_module configurator_module = {
	.name = "CONFIGURATOR",
	.init = configurator_module_init,
	.redraw = configurator_module_redraw,
};

#else

struct coreinfo_module configurator_module = {
};

#endif