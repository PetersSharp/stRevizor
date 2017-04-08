
/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Released under the terms of the GNU GPL v2.0.
 *
 *  and
 *
 *  ORIGINAL AUTHOR: Savio Lam (lam836@cs.cuhk.hk)
 *  MODIFIED FOR LINUX KERNEL CONFIG BY: William Roadcap (roadcap@cfw.com)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

//--------------------------------------------------------------------------------------
//
//    stRevizor package - Registry RosKomNadzor blacklist converter to iptables/ipset,
//              Win GUI client, *nix server part and more..
//
//    You can find latest source:
//     - https://github.com/PetersSharp/stRevizor
//     - https://github.com/PetersSharp/stRevizor/releases/latest
//     - https://github.com/PetersSharp/stRevizor/pulse
//
//    * $Id$
//    * commit date: $Format:%cd by %aN$
//
//--------------------------------------------------------------------------------------


#if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#endif
#if !defined(__USE_GNU)
#    define __USE_GNU
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <time.h>
#include <locale.h>
#include <signal.h>
#include <regex.h>
#ifndef YY_NO_UNISTD_H
#include <unistd.h>
#endif
#ifndef KBUILD_NO_NLS
#include <libintl.h>
#else
#define gettext(Msgid) ((const char *) (Msgid))
#endif
/* #include <ncursesw/curses.h> */
#define PRINTD 0x0001
#define DEBUG_PARSE 0x0002
#define printd(mask,fmt,...) ((cdebug & (mask)) ? (void*)printf(fmt,__VA_ARGS__) : (void*)0)
#define TR(params) _tracef params
#define KEY_ESC 27
#define TAB 9
#define MAX_LEN 2048
#define BUF_SIZE (10*1024)
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)
#define M_EVENT (KEY_MAX+1)
#define SRCTREE "srctree"
#ifndef PACKAGE
#define PACKAGE "linux"
#endif
#define LOCALEDIR "/usr/share/locale"
#define _(text) gettext(text)
#define N_(text) text
#ifndef CONFIG_
#define CONFIG_ "BUILD_"
#endif
int cdebug = PRINTD;
int zconfdebug;
int saved_x, saved_y;
static int menu_width, item_x;
char dialog_input_result[MAX_LEN + 1];
static int list_width, check_x, item_x;
FILE *zconfin = (FILE *) 0, *zconfout = (FILE *) 0;
int zconfleng, zconflineno = 1;
char *zconftext;
#undef offsetof
#define offsetof(TYPE,MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr,type,member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})
struct list_head {
    struct list_head *next, *prev;
};
#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)
#define list_entry(ptr,type,member) \
    container_of(ptr, type, member)
#define list_for_each_entry(pos,head,member) \
    for (pos = list_entry((head)->next, typeof(*pos), member); \
     &pos->member != (head); \
     pos = list_entry(pos->member.next, typeof(*pos), member))
static inline int list_empty(const struct list_head *head) {
    return head->next == head;
}
static inline void __list_add(struct list_head *_new, struct list_head *prev, struct list_head *next) {
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}
static inline void list_add_tail(struct list_head *_new, struct list_head *head) {
    __list_add(_new, head->prev, head);
}
struct file {
    struct file *next;
    struct file *parent;
    const char *name;
    int lineno;
};
typedef enum tristate {
    no, mod, yes
} tristate;
enum expr_type {
    E_NONE, E_OR, E_AND, E_NOT, E_EQUAL, E_UNEQUAL, E_LIST, E_SYMBOL, E_RANGE
};
union expr_data {
    struct expr *expr;
    struct symbol *sym;
};
struct expr {
    enum expr_type type;
    union expr_data left, right;
};
#define EXPR_OR(dep1,dep2) (((dep1)>(dep2))?(dep1):(dep2))
#define EXPR_AND(dep1,dep2) (((dep1)<(dep2))?(dep1):(dep2))
#define EXPR_NOT(dep) (2-(dep))
#define expr_list_for_each_sym(l,e,s) for (e = (l); e && (s = e->right.sym); e = e->left.expr)
struct expr_value {
    struct expr *expr;
    tristate tri;
};
struct symbol_value {
    void *val;
    tristate tri;
};
enum symbol_type {
    S_UNKNOWN, S_BOOLEAN, S_TRISTATE, S_INT, S_HEX, S_STRING, S_OTHER
};
enum {
    S_DEF_USER,
    S_DEF_AUTO,
    S_DEF_DEF3,
    S_DEF_DEF4,
    S_DEF_COUNT
};
struct symbol {
    struct symbol *next;
    char *name;
    enum symbol_type type;
    struct symbol_value curr;
    struct symbol_value def[S_DEF_COUNT];
    tristate visible;
    int flags;
    struct property *prop;
    struct expr_value dir_dep;
    struct expr_value rev_dep;
};
#define for_all_symbols(i,sym) for (i = 0; i < SYMBOL_HASHSIZE; i++) for (sym = symbol_hash[i]; sym; sym = sym->next) if (sym->type != S_OTHER)
#define SYMBOL_CONST 0x0001
#define SYMBOL_CHECK 0x0008
#define SYMBOL_CHOICE 0x0010
#define SYMBOL_CHOICEVAL 0x0020
#define SYMBOL_VALID 0x0080
#define SYMBOL_OPTIONAL 0x0100
#define SYMBOL_WRITE 0x0200
#define SYMBOL_CHANGED 0x0400
#define SYMBOL_AUTO 0x1000
#define SYMBOL_CHECKED 0x2000
#define SYMBOL_WARNED 0x8000
#define SYMBOL_DEF 0x10000
#define SYMBOL_DEF_USER 0x10000
#define SYMBOL_DEF_AUTO 0x20000
#define SYMBOL_DEF3 0x40000
#define SYMBOL_DEF4 0x80000
#define SYMBOL_MAXLENGTH 256
#define SYMBOL_HASHSIZE 9973
enum prop_type {
    P_UNKNOWN,
    P_PROMPT,
    P_COMMENT,
    P_MENU,
    P_DEFAULT,
    P_CHOICE,
    P_SELECT,
    P_RANGE,
    P_ENV,
    P_SYMBOL
};
struct property {
    struct property *next;
    struct symbol *sym;
    enum prop_type type;
    const char *text;
    struct expr_value visible;
    struct expr *expr;
    struct menu *menu;
    struct file *file;
    int lineno;
};
#define for_all_properties(sym,st,tok) \
 for (st = sym->prop; st; st = st->next) \
  if (st->type == (tok))
#define for_all_defaults(sym,st) for_all_properties(sym, st, P_DEFAULT)
#define for_all_choices(sym,st) for_all_properties(sym, st, P_CHOICE)
#define for_all_prompts(sym,st) \
 for (st = sym->prop; st; st = st->next) \
  if (st->text)
struct menu {
 struct menu *next;
 struct menu *parent;
 struct menu *list;
 struct symbol *sym;
 struct property *prompt;
 struct expr *visibility;
 struct expr *dep;
 unsigned int flags;
 char *help;
 struct file *file;
 int lineno;
 void *data;
};
#define MENU_CHANGED 0x0001
#define MENU_ROOT 0x0002
struct jump_key {
 struct list_head entries;
 size_t offset;
 struct menu *target;
 int index;
};
#define JUMP_NB 9
struct file *file_list;
struct file *current_file;
struct file *lookup_file(const char *name);
struct symbol symbol_yes, symbol_no, symbol_mod;
struct symbol *modules_sym;
struct symbol *sym_defconfig_list;
struct expr *expr_alloc_symbol(struct symbol *sym);
struct expr *expr_alloc_one(enum expr_type type, struct expr *ce);
struct expr *expr_alloc_two(enum expr_type type, struct expr *e1, struct expr *e2);
struct expr *expr_alloc_comp(enum expr_type type, struct symbol *s1, struct symbol *s2);
struct expr *expr_alloc_and(struct expr *e1, struct expr *e2);
struct expr *expr_alloc_or(struct expr *e1, struct expr *e2);
struct expr *expr_copy(const struct expr *org);
void expr_free(struct expr *e);
int expr_eq(struct expr *e1, struct expr *e2);
void expr_eliminate_eq(struct expr **ep1, struct expr **ep2);
tristate expr_calc_value(struct expr *e);
struct expr *expr_eliminate_yn(struct expr *e);
struct expr *expr_trans_bool(struct expr *e);
struct expr *expr_eliminate_dups(struct expr *e);
struct expr *expr_transform(struct expr *e);
int expr_contains_symbol(struct expr *dep, struct symbol *sym);
bool expr_depends_symbol(struct expr *dep, struct symbol *sym);
struct expr *expr_extract_eq_and(struct expr **ep1, struct expr **ep2);
struct expr *expr_extract_eq_or(struct expr **ep1, struct expr **ep2);
void expr_extract_eq(enum expr_type type, struct expr **ep, struct expr **ep1, struct expr **ep2);
struct expr *expr_trans_compare(struct expr *e, enum expr_type type, struct symbol *sym);
struct expr *expr_simplify_unmet_dep(struct expr *e1, struct expr *e2);
void expr_fprint(struct expr *e, FILE *out);
struct gstr;
void expr_gstr_print(struct expr *e, struct gstr *gs);
static inline int expr_is_yes(struct expr *e)
{
 return !e || (e->type == E_SYMBOL && e->left.sym == &symbol_yes);
}
static inline int expr_is_no(struct expr *e)
{
 return e && (e->type == E_SYMBOL && e->left.sym == &symbol_no);
}
#ifndef KBUILD_NO_NLS
# include <libintl.h>
#else
static inline const char *gettext(const char *txt) { return txt; }
static inline void textdomain(const char *domainname) {}
static inline void bindtextdomain(const char *name, const char *dir) {}
static inline char *bind_textdomain_codeset(const char *dn, char *c) { return c; }
#endif
#define P(name,type,arg) type name arg
#define PP(name,type) type name
P(conf_parse,void,(const char *name));
P(conf_read,int,(const char *name));
P(conf_read_simple,int,(const char *name, int));
P(conf_write_defconfig,int,(const char *name));
P(conf_write,int,(const char *name));
P(conf_write_autoconf,int,(const char* filename));
P(conf_get_changed,bool,(void));
P(conf_set_changed_callback, void,(void (*fn)(void)));
P(conf_set_message_callback, void,(void (*fn)(const char *fmt, va_list ap)));
PP(rootmenu,struct menu);
P(menu_is_visible, bool, (struct menu *menu));
P(menu_has_prompt, bool, (struct menu *menu));
P(menu_get_prompt,const char *,(struct menu *menu));
P(menu_get_root_menu,struct menu *,(struct menu *menu));
P(menu_get_parent_menu,struct menu *,(struct menu *menu));
P(menu_has_help,bool,(struct menu *menu));
P(menu_get_help,const char *,(struct menu *menu));
P(get_symbol_str, void, (struct gstr *r, struct symbol *sym, struct list_head
    *head));
P(get_relations_str, struct gstr, (struct symbol **sym_arr, struct list_head
       *head));
P(menu_get_ext_help,void,(struct menu *menu, struct gstr *help));
P(symbol_hash,struct symbol *,[SYMBOL_HASHSIZE]);
P(sym_lookup,struct symbol *,(const char *name, int flags));
P(sym_find,struct symbol *,(const char *name));
P(sym_expand_string_value,const char *,(const char *in));
P(sym_escape_string_value, const char *,(const char *in));
P(sym_re_search,struct symbol **,(const char *pattern));
P(sym_type_name,const char *,(enum symbol_type type));
P(sym_calc_value,void,(struct symbol *sym));
P(sym_get_type,enum symbol_type,(struct symbol *sym));
P(sym_tristate_within_range,bool,(struct symbol *sym,tristate tri));
P(sym_set_tristate_value,bool,(struct symbol *sym,tristate tri));
P(sym_toggle_tristate_value,tristate,(struct symbol *sym));
P(sym_string_valid,bool,(struct symbol *sym, const char *newval));
P(sym_string_within_range,bool,(struct symbol *sym, const char *str));
P(sym_set_string_value,bool,(struct symbol *sym, const char *newval));
P(sym_is_changable,bool,(struct symbol *sym));
P(sym_get_choice_prop,struct property *,(struct symbol *sym));
P(sym_get_default_prop,struct property *,(struct symbol *sym));
P(sym_get_string_value,const char *,(struct symbol *sym));
P(prop_get_type_name,const char *,(enum prop_type type));
P(expr_compare_type,int,(enum expr_type t1, enum expr_type t2));
P(expr_print,void,(struct expr *e, void (*fn)(void *, struct symbol *, const char *), void *data, int prevtoken));
#undef PP
#undef P
#define TF_COMMAND 0x0001
#define TF_PARAM 0x0002
#define TF_OPTION 0x0004
enum conf_def_mode {
 def_default,
 def_yes,
 def_mod,
 def_no,
 def_random
};
#define T_OPT_MODULES 1
#define T_OPT_DEFCONFIG_LIST 2
#define T_OPT_ENV 3
struct kconf_id {
 int name;
 int token;
 unsigned int flags;
 enum symbol_type stype;
};
int zconfparse(void);
void zconfdump(FILE *out);
void zconf_starthelp(void);
FILE *zconf_fopen(const char *name);
void zconf_initscan(const char *name);
void zconf_nextfile(const char *name);
int zconf_lineno(void);
const char *zconf_curname(void);
const char *conf_get_configname(void);
const char *conf_get_autoconfig_name(void);
char *conf_get_default_confname(void);
void sym_set_change_count(int count);
void sym_add_change_count(int count);
void conf_set_all_new_symbols(enum conf_def_mode mode);
struct conf_printer {
 void (*print_symbol)(FILE *, struct symbol *, const char *, void *);
 void (*print_comment)(FILE *, const char *, void *);
};
static inline void xfwrite(const void *str, size_t len, size_t count, FILE *out)
{
 assert(len != 0);
 if (fwrite(str, len, count, out) != count)
  fprintf(stderr, "Error in writing or end of file.\n");
}
void _menu_init(void);
void menu_warn(struct menu *menu, const char *fmt, ...);
struct menu *menu_add_menu(void);
void menu_end_menu(void);
void menu_add_entry(struct symbol *sym);
void menu_end_entry(void);
void menu_add_dep(struct expr *dep);
void menu_add_visibility(struct expr *dep);
struct property *menu_add_prop(enum prop_type type, char *prompt, struct expr *expr, struct expr *dep);
struct property *menu_add_prompt(enum prop_type type, char *prompt, struct expr *dep);
void menu_add_expr(enum prop_type type, struct expr *expr, struct expr *dep);
void menu_add_symbol(enum prop_type type, struct symbol *sym, struct expr *dep);
void menu_add_option(int token, char *arg);
void menu_finalize(struct menu *parent);
void menu_set_type(int type);
struct file *file_lookup(const char *name);
int file_write_dep(const char *name);
struct gstr {
 size_t len;
 char *s;
 int max_width;
};
struct gstr str_new(void);
struct gstr str_assign(const char *s);
void str_free(struct gstr *gs);
void str_append(struct gstr *gs, const char *s);
void str_printf(struct gstr *gs, const char *fmt, ...);
const char *str_get(struct gstr *gs);
struct expr *sym_env_list;
void sym_init(void);
void sym_clear_all_valid(void);
void sym_set_all_changed(void);
void sym_set_changed(struct symbol *sym);
struct symbol *sym_choice_default(struct symbol *sym);
const char *sym_get_string_default(struct symbol *sym);
struct symbol *sym_check_deps(struct symbol *sym);
struct property *prop_alloc(enum prop_type type, struct symbol *sym);
struct symbol *prop_get_symbol(struct property *prop);
struct property *sym_get_env_prop(struct symbol *sym);
static inline tristate sym_get_tristate_value(struct symbol *sym)
{
 return sym->curr.tri;
}
static inline struct symbol *sym_get_choice_value(struct symbol *sym)
{
 return (struct symbol *)sym->curr.val;
}
static inline bool sym_set_choice_value(struct symbol *ch, struct symbol *chval)
{
 return sym_set_tristate_value(chval, yes);
}
static inline bool sym_is_choice(struct symbol *sym)
{
 return sym->flags & SYMBOL_CHOICE ? true : false;
}
static inline bool sym_is_choice_value(struct symbol *sym)
{
 return sym->flags & SYMBOL_CHOICEVAL ? true : false;
}
static inline bool sym_is_optional(struct symbol *sym)
{
 return sym->flags & SYMBOL_OPTIONAL ? true : false;
}
static inline bool sym_has_value(struct symbol *sym)
{
 return sym->flags & SYMBOL_DEF_USER ? true : false;
}
#ifndef ACS_ULCORNER
#define ACS_ULCORNER '+'
#endif
#ifndef ACS_LLCORNER
#define ACS_LLCORNER '+'
#endif
#ifndef ACS_URCORNER
#define ACS_URCORNER '+'
#endif
#ifndef ACS_LRCORNER
#define ACS_LRCORNER '+'
#endif
#ifndef ACS_HLINE
#define ACS_HLINE '-'
#endif
#ifndef ACS_VLINE
#define ACS_VLINE '|'
#endif
#ifndef ACS_LTEE
#define ACS_LTEE '+'
#endif
#ifndef ACS_RTEE
#define ACS_RTEE '+'
#endif
#ifndef ACS_UARROW
#define ACS_UARROW '^'
#endif
#ifndef ACS_DARROW
#define ACS_DARROW 'v'
#endif
#define ERRDISPLAYTOOSMALL (KEY_MAX + 1)
struct dialog_color {
 chtype atr;
 int fg;
 int bg;
 int hl;
};
struct dialog_info {
 const char *backtitle;
 struct dialog_color screen;
 struct dialog_color shadow;
 struct dialog_color dialog;
 struct dialog_color title;
 struct dialog_color border;
 struct dialog_color button_active;
 struct dialog_color button_inactive;
 struct dialog_color button_key_active;
 struct dialog_color button_key_inactive;
 struct dialog_color button_label_active;
 struct dialog_color button_label_inactive;
 struct dialog_color inputbox;
 struct dialog_color inputbox_border;
 struct dialog_color searchbox;
 struct dialog_color searchbox_title;
 struct dialog_color searchbox_border;
 struct dialog_color position_indicator;
 struct dialog_color menubox;
 struct dialog_color menubox_border;
 struct dialog_color item;
 struct dialog_color item_selected;
 struct dialog_color tag;
 struct dialog_color tag_selected;
 struct dialog_color tag_key;
 struct dialog_color tag_key_selected;
 struct dialog_color check;
 struct dialog_color check_selected;
 struct dialog_color uarrow;
 struct dialog_color darrow;
};
struct dialog_info dlg;
void item_reset(void);
void item_make(const char *fmt, ...);
void item_add_str(const char *fmt, ...);
void item_set_tag(char tag);
void item_set_data(void *p);
void item_set_selected(int val);
int item_activate_selected(void);
void *item_data(void);
char item_tag(void);
#define MAXITEMSTR 200
struct dialog_item {
 char str[MAXITEMSTR];
 char tag;
 void *data;
 int selected;
};
struct dialog_list {
 struct dialog_item node;
 struct dialog_list *next;
};
struct dialog_list *item_cur;
struct dialog_list item_nil;
struct dialog_list *item_head;
int item_count(void);
void item_set(int n);
int item_n(void);
const char *item_str(void);
int item_is_selected(void);
int item_is_tag(char tag);
#define item_foreach() \
 for (item_cur = item_head ? item_head: item_cur; \
      item_cur && (item_cur != &item_nil); item_cur = item_cur->next)
int on_key_esc(WINDOW *win);
int on_key_resize(void);
int init_dialog(const char *backtitle);
void set_dialog_backtitle(const char *backtitle);
void end_dialog(int x, int y);
void attr_clear(WINDOW * win, int height, int width, chtype attr);
void dialog_clear(void);
void print_autowrap(WINDOW * win, const char *prompt, int width, int y, int x);
void print_button(WINDOW * win, const char *label, int y, int x, int selected);
void print_title(WINDOW *dialog, const char *title, int width);
void draw_box(WINDOW * win, int y, int x, int height, int width, chtype box,
       chtype border);
void draw_shadow(WINDOW * win, int y, int x, int height, int width);
int first_alpha(const char *string, const char *exempt);
int dialog_yesno(const char *title, const char *prompt, int height, int width);
int dialog_msgbox(const char *title, const char *prompt, int height,
    int width, int pause);
typedef void (*update_text_fn)(char *buf, size_t start, size_t end, void
          *_data);
int dialog_textbox(const char *title, char *tbuf, int initial_height,
     int initial_width, int *keys, int *_vscroll, int *_hscroll,
     update_text_fn update_text, void *data);
int dialog_menu(const char *title, const char *prompt,
  const void *selected, int *s_scroll);
int dialog_checklist(const char *title, const char *prompt, int height,
       int width, int list_height);
int dialog_inputbox(const char *title, const char *prompt, int height,
      int width, const char *init);
#define YYBISON 1
#define YYBISON_VERSION "2.4.3"
#define YYSKELETON_NAME "yacc.c"
#define YYPURE 0
#define YYPUSH 0
#define YYPULL 1
#define YYLSP_NEEDED 0
#define yyparse zconfparse
#define yylex zconflex
#define yyerror zconferror
#define yylval zconflval
#define yychar zconfchar
#define yydebug zconfdebug
#define yynerrs zconfnerrs
static void back_lines(int n);
static void print_page(WINDOW *win, int height, int width, update_text_fn update_text, void *data);
static void print_line(WINDOW *win, int row, int width);
static char *get_line(void);
static void print_position(WINDOW * win);
static int hscroll;
static int begin_reached, end_reached, page_length;
static char *buf;
static char *page;
static void set_mono_theme(void)
{
 dlg.screen.atr = A_NORMAL;
 dlg.shadow.atr = A_NORMAL;
 dlg.dialog.atr = A_NORMAL;
 dlg.title.atr = A_BOLD;
 dlg.border.atr = A_NORMAL;
 dlg.button_active.atr = A_REVERSE;
 dlg.button_inactive.atr = A_DIM;
 dlg.button_key_active.atr = A_REVERSE;
 dlg.button_key_inactive.atr = A_BOLD;
 dlg.button_label_active.atr = A_REVERSE;
 dlg.button_label_inactive.atr = A_NORMAL;
 dlg.inputbox.atr = A_NORMAL;
 dlg.inputbox_border.atr = A_NORMAL;
 dlg.searchbox.atr = A_NORMAL;
 dlg.searchbox_title.atr = A_BOLD;
 dlg.searchbox_border.atr = A_NORMAL;
 dlg.position_indicator.atr = A_BOLD;
 dlg.menubox.atr = A_NORMAL;
 dlg.menubox_border.atr = A_NORMAL;
 dlg.item.atr = A_NORMAL;
 dlg.item_selected.atr = A_REVERSE;
 dlg.tag.atr = A_BOLD;
 dlg.tag_selected.atr = A_REVERSE;
 dlg.tag_key.atr = A_BOLD;
 dlg.tag_key_selected.atr = A_REVERSE;
 dlg.check.atr = A_BOLD;
 dlg.check_selected.atr = A_REVERSE;
 dlg.uarrow.atr = A_BOLD;
 dlg.darrow.atr = A_BOLD;
}
#define DLG_COLOR(dialog,f,b,h) \
do { \
 dlg.dialog.fg = (f); \
 dlg.dialog.bg = (b); \
 dlg.dialog.hl = (h); \
} while (0)
static void set_classic_theme(void)
{
 DLG_COLOR(screen, COLOR_CYAN, COLOR_BLUE, true);
 DLG_COLOR(shadow, COLOR_BLACK, COLOR_BLACK, true);
 DLG_COLOR(dialog, COLOR_BLACK, COLOR_WHITE, false);
 DLG_COLOR(title, COLOR_YELLOW, COLOR_WHITE, true);
 DLG_COLOR(border, COLOR_WHITE, COLOR_WHITE, true);
 DLG_COLOR(button_active, COLOR_WHITE, COLOR_BLUE, true);
 DLG_COLOR(button_inactive, COLOR_BLACK, COLOR_WHITE, false);
 DLG_COLOR(button_key_active, COLOR_WHITE, COLOR_BLUE, true);
 DLG_COLOR(button_key_inactive, COLOR_RED, COLOR_WHITE, false);
 DLG_COLOR(button_label_active, COLOR_YELLOW, COLOR_BLUE, true);
 DLG_COLOR(button_label_inactive, COLOR_BLACK, COLOR_WHITE, true);
 DLG_COLOR(inputbox, COLOR_BLACK, COLOR_WHITE, false);
 DLG_COLOR(inputbox_border, COLOR_BLACK, COLOR_WHITE, false);
 DLG_COLOR(searchbox, COLOR_BLACK, COLOR_WHITE, false);
 DLG_COLOR(searchbox_title, COLOR_YELLOW, COLOR_WHITE, true);
 DLG_COLOR(searchbox_border, COLOR_WHITE, COLOR_WHITE, true);
 DLG_COLOR(position_indicator, COLOR_YELLOW, COLOR_WHITE, true);
 DLG_COLOR(menubox, COLOR_BLACK, COLOR_WHITE, false);
 DLG_COLOR(menubox_border, COLOR_WHITE, COLOR_WHITE, true);
 DLG_COLOR(item, COLOR_BLACK, COLOR_WHITE, false);
 DLG_COLOR(item_selected, COLOR_WHITE, COLOR_BLUE, true);
 DLG_COLOR(tag, COLOR_YELLOW, COLOR_WHITE, true);
 DLG_COLOR(tag_selected, COLOR_YELLOW, COLOR_BLUE, true);
 DLG_COLOR(tag_key, COLOR_YELLOW, COLOR_WHITE, true);
 DLG_COLOR(tag_key_selected, COLOR_YELLOW, COLOR_BLUE, true);
 DLG_COLOR(check, COLOR_BLACK, COLOR_WHITE, false);
 DLG_COLOR(check_selected, COLOR_WHITE, COLOR_BLUE, true);
 DLG_COLOR(uarrow, COLOR_GREEN, COLOR_WHITE, true);
 DLG_COLOR(darrow, COLOR_GREEN, COLOR_WHITE, true);
}
static void set_blackbg_theme(void)
{
 DLG_COLOR(screen, COLOR_RED, COLOR_BLACK, true);
 DLG_COLOR(shadow, COLOR_BLACK, COLOR_BLACK, false);
 DLG_COLOR(dialog, COLOR_WHITE, COLOR_BLACK, false);
 DLG_COLOR(title, COLOR_RED, COLOR_BLACK, false);
 DLG_COLOR(border, COLOR_BLACK, COLOR_BLACK, true);
 DLG_COLOR(button_active, COLOR_YELLOW, COLOR_RED, false);
 DLG_COLOR(button_inactive, COLOR_YELLOW, COLOR_BLACK, false);
 DLG_COLOR(button_key_active, COLOR_YELLOW, COLOR_RED, true);
 DLG_COLOR(button_key_inactive, COLOR_RED, COLOR_BLACK, false);
 DLG_COLOR(button_label_active, COLOR_WHITE, COLOR_RED, false);
 DLG_COLOR(button_label_inactive, COLOR_BLACK, COLOR_BLACK, true);
 DLG_COLOR(inputbox, COLOR_YELLOW, COLOR_BLACK, false);
 DLG_COLOR(inputbox_border, COLOR_YELLOW, COLOR_BLACK, false);
 DLG_COLOR(searchbox, COLOR_YELLOW, COLOR_BLACK, false);
 DLG_COLOR(searchbox_title, COLOR_YELLOW, COLOR_BLACK, true);
 DLG_COLOR(searchbox_border, COLOR_BLACK, COLOR_BLACK, true);
 DLG_COLOR(position_indicator, COLOR_RED, COLOR_BLACK, false);
 DLG_COLOR(menubox, COLOR_YELLOW, COLOR_BLACK, false);
 DLG_COLOR(menubox_border, COLOR_BLACK, COLOR_BLACK, true);
 DLG_COLOR(item, COLOR_WHITE, COLOR_BLACK, false);
 DLG_COLOR(item_selected, COLOR_WHITE, COLOR_RED, false);
 DLG_COLOR(tag, COLOR_RED, COLOR_BLACK, false);
 DLG_COLOR(tag_selected, COLOR_YELLOW, COLOR_RED, true);
 DLG_COLOR(tag_key, COLOR_RED, COLOR_BLACK, false);
 DLG_COLOR(tag_key_selected, COLOR_YELLOW, COLOR_RED, true);
 DLG_COLOR(check, COLOR_YELLOW, COLOR_BLACK, false);
 DLG_COLOR(check_selected, COLOR_YELLOW, COLOR_RED, true);
 DLG_COLOR(uarrow, COLOR_RED, COLOR_BLACK, false);
 DLG_COLOR(darrow, COLOR_RED, COLOR_BLACK, false);
}
static void set_bluetitle_theme(void)
{
 set_classic_theme();
 DLG_COLOR(title, COLOR_BLUE, COLOR_WHITE, true);
 DLG_COLOR(button_key_active, COLOR_YELLOW, COLOR_BLUE, true);
 DLG_COLOR(button_label_active, COLOR_WHITE, COLOR_BLUE, true);
 DLG_COLOR(searchbox_title, COLOR_BLUE, COLOR_WHITE, true);
 DLG_COLOR(position_indicator, COLOR_BLUE, COLOR_WHITE, true);
 DLG_COLOR(tag, COLOR_BLUE, COLOR_WHITE, true);
 DLG_COLOR(tag_key, COLOR_BLUE, COLOR_WHITE, true);
}
static int set_theme(const char *theme)
{
 int use_color = 1;
 if (!theme)
  set_bluetitle_theme();
 else if (strcmp(theme, "classic") == 0)
  set_classic_theme();
 else if (strcmp(theme, "bluetitle") == 0)
  set_bluetitle_theme();
 else if (strcmp(theme, "blackbg") == 0)
  set_blackbg_theme();
 else if (strcmp(theme, "mono") == 0)
  use_color = 0;
 return use_color;
}
static void init_one_color(struct dialog_color *color)
{
 static int pair = 0;
 pair++;
 init_pair(pair, color->fg, color->bg);
 if (color->hl)
  color->atr = A_BOLD | COLOR_PAIR(pair);
 else
  color->atr = COLOR_PAIR(pair);
}
static void init_dialog_colors(void)
{
 init_one_color(&dlg.screen);
 init_one_color(&dlg.shadow);
 init_one_color(&dlg.dialog);
 init_one_color(&dlg.title);
 init_one_color(&dlg.border);
 init_one_color(&dlg.button_active);
 init_one_color(&dlg.button_inactive);
 init_one_color(&dlg.button_key_active);
 init_one_color(&dlg.button_key_inactive);
 init_one_color(&dlg.button_label_active);
 init_one_color(&dlg.button_label_inactive);
 init_one_color(&dlg.inputbox);
 init_one_color(&dlg.inputbox_border);
 init_one_color(&dlg.searchbox);
 init_one_color(&dlg.searchbox_title);
 init_one_color(&dlg.searchbox_border);
 init_one_color(&dlg.position_indicator);
 init_one_color(&dlg.menubox);
 init_one_color(&dlg.menubox_border);
 init_one_color(&dlg.item);
 init_one_color(&dlg.item_selected);
 init_one_color(&dlg.tag);
 init_one_color(&dlg.tag_selected);
 init_one_color(&dlg.tag_key);
 init_one_color(&dlg.tag_key_selected);
 init_one_color(&dlg.check);
 init_one_color(&dlg.check_selected);
 init_one_color(&dlg.uarrow);
 init_one_color(&dlg.darrow);
}
static void color_setup(const char *theme)
{
 int use_color;
 use_color = set_theme(theme);
 if (use_color && has_colors()) {
  start_color();
  init_dialog_colors();
 } else
  set_mono_theme();
}
void attr_clear(WINDOW * win, int height, int width, chtype attr)
{
 int i, j;
 wattrset(win, attr);
 for (i = 0; i < height; i++) {
  wmove(win, i, 0);
  for (j = 0; j < width; j++)
   waddch(win, ' ');
 }
 touchwin(win);
}
void dialog_clear(void)
{
 attr_clear(stdscr, LINES, COLS, dlg.screen.atr);
 if (dlg.backtitle != NULL) {
  int i;
  wattrset(stdscr, dlg.screen.atr);
  mvwaddstr(stdscr, 0, 1, (char *)dlg.backtitle);
  wmove(stdscr, 1, 1);
  for (i = 1; i < COLS - 1; i++)
   waddch(stdscr, ACS_HLINE);
 }
 wnoutrefresh(stdscr);
}
int init_dialog(const char *backtitle)
{
 int height, width;
 initscr();
 getyx(stdscr, saved_y, saved_x);
 getmaxyx(stdscr, height, width);
 if (height < 19 || width < 80) {
  endwin();
  return -ERRDISPLAYTOOSMALL;
 }
 dlg.backtitle = backtitle;
 color_setup(getenv("MENUCONFIG_COLOR"));
 keypad(stdscr, TRUE);
 cbreak();
 noecho();
 dialog_clear();
 return 0;
}
void set_dialog_backtitle(const char *backtitle)
{
 dlg.backtitle = backtitle;
}
void end_dialog(int x, int y)
{
 move(y, x);
 refresh();
 endwin();
}
void print_title(WINDOW *dialog, const char *title, int width)
{
    if (title) {
        int tlen = MIN(width - 2, strlen(title));
        wattrset(dialog, dlg.title.atr);
        mvwaddch(dialog, 0, (width - tlen) / 2 - 1, ' ');
        mvwaddnstr(dialog, 0, (width - tlen)/2, title, tlen);
        waddch(dialog, ' ');
    }
}
void print_autowrap(WINDOW * win, const char *prompt, int width, int y, int x)
{
 int newl, cur_x, cur_y;
 int i, prompt_len, room, wlen;
 char tempstr[MAX_LEN + 1], *word, *sp, *sp2;
 strcpy(tempstr, prompt);
 prompt_len = strlen(tempstr);
 for (i = 0; i < prompt_len; i++) {
  if (tempstr[i] == '\n')
   tempstr[i] = ' ';
 }
 if (prompt_len <= width - x * 2) {
  wmove(win, y, (width - prompt_len) / 2);
  waddstr(win, tempstr);
 } else {
  cur_x = x;
  cur_y = y;
  newl = 1;
  word = tempstr;
  while (word && *word) {
   sp = strchr(word, ' ');
   if (sp)
    *sp++ = 0;
   room = width - cur_x;
   wlen = strlen(word);
   if (wlen > room ||
       (newl && wlen < 4 && sp
        && wlen + 1 + strlen(sp) > room
        && (!(sp2 = strchr(sp, ' '))
     || wlen + 1 + (sp2 - sp) > room))) {
    cur_y++;
    cur_x = x;
   }
   wmove(win, cur_y, cur_x);
   waddstr(win, word);
   getyx(win, cur_y, cur_x);
   cur_x++;
   if (sp && *sp == ' ') {
    cur_x++;
    while (*++sp == ' ') ;
    newl = 1;
   } else
    newl = 0;
   word = sp;
  }
 }
}
void print_button(WINDOW * win, const char *label, int y, int x, int selected)
{
 int i, temp;
 wmove(win, y, x);
 wattrset(win, selected ? dlg.button_active.atr
   : dlg.button_inactive.atr);
 waddstr(win, "<");
 temp = strspn(label, " ");
 label += temp;
 wattrset(win, selected ? dlg.button_label_active.atr
   : dlg.button_label_inactive.atr);
 for (i = 0; i < temp; i++)
  waddch(win, ' ');
 wattrset(win, selected ? dlg.button_key_active.atr
   : dlg.button_key_inactive.atr);
 waddch(win, label[0]);
 wattrset(win, selected ? dlg.button_label_active.atr
   : dlg.button_label_inactive.atr);
 waddstr(win, (char *)label + 1);
 wattrset(win, selected ? dlg.button_active.atr
   : dlg.button_inactive.atr);
 waddstr(win, ">");
 wmove(win, y, x + temp + 1);
}
void
draw_box(WINDOW * win, int y, int x, int height, int width,
  chtype box, chtype border)
{
 int i, j;
 wattrset(win, 0);
 for (i = 0; i < height; i++) {
  wmove(win, y + i, x);
  for (j = 0; j < width; j++)
   if (!i && !j)
    waddch(win, border | ACS_ULCORNER);
   else if (i == height - 1 && !j)
    waddch(win, border | ACS_LLCORNER);
   else if (!i && j == width - 1)
    waddch(win, box | ACS_URCORNER);
   else if (i == height - 1 && j == width - 1)
    waddch(win, box | ACS_LRCORNER);
   else if (!i)
    waddch(win, border | ACS_HLINE);
   else if (i == height - 1)
    waddch(win, box | ACS_HLINE);
   else if (!j)
    waddch(win, border | ACS_VLINE);
   else if (j == width - 1)
    waddch(win, box | ACS_VLINE);
   else
    waddch(win, box | ' ');
 }
}
void draw_shadow(WINDOW * win, int y, int x, int height, int width)
{
 int i;
 if (has_colors()) {
  wattrset(win, dlg.shadow.atr);
  wmove(win, y + height, x + 2);
  for (i = 0; i < width; i++)
   waddch(win, winch(win) & A_CHARTEXT);
  for (i = y + 1; i < y + height + 1; i++) {
   wmove(win, i, x + width);
   waddch(win, winch(win) & A_CHARTEXT);
   waddch(win, winch(win) & A_CHARTEXT);
  }
  wnoutrefresh(win);
 }
}
int first_alpha(const char *string, const char *exempt)
{
 int i, in_paren = 0, c;
 for (i = 0; i < strlen(string); i++) {
  c = tolower(string[i]);
  if (strchr("<[(", c))
   ++in_paren;
  if (strchr(">])", c) && in_paren > 0)
   --in_paren;
  if ((!in_paren) && isalpha(c) && strchr(exempt, c) == 0)
   return i;
 }
 return 0;
}
# 1195 "dialog.all.c"
int on_key_esc(WINDOW *win)
{
 int key;
 int key2;
 int key3;
 nodelay(win, TRUE);
 keypad(win, FALSE);
 key = wgetch(win);
 key2 = wgetch(win);
 do {
  key3 = wgetch(win);
 } while (key3 != ERR);
 nodelay(win, FALSE);
 keypad(win, TRUE);
 if (key == KEY_ESC && key2 == ERR)
  return KEY_ESC;
 else if (key != ERR && key != KEY_ESC && key2 == ERR)
  ungetch(key);
 return -1;
}
int on_key_resize(void)
{
 dialog_clear();
 return KEY_RESIZE;
}
void item_reset(void)
{
 struct dialog_list *p, *next;
 for (p = item_head; p; p = next) {
  next = p->next;
  free(p);
 }
 item_head = NULL;
 item_cur = &item_nil;
}
void item_make(const char *fmt, ...)
{
 va_list ap;
 struct dialog_list *p = malloc(sizeof(*p));
 if (item_head)
  item_cur->next = p;
 else
  item_head = p;
 item_cur = p;
 memset(p, 0, sizeof(*p));
 va_start(ap, fmt);
 vsnprintf(item_cur->node.str, sizeof(item_cur->node.str), fmt, ap);
 va_end(ap);
}
void item_add_str(const char *fmt, ...)
{
 va_list ap;
        size_t avail;
 avail = sizeof(item_cur->node.str) - strlen(item_cur->node.str);
 va_start(ap, fmt);
 vsnprintf(item_cur->node.str + strlen(item_cur->node.str),
    avail, fmt, ap);
 item_cur->node.str[sizeof(item_cur->node.str) - 1] = '\0';
 va_end(ap);
}
void item_set_tag(char tag)
{
 item_cur->node.tag = tag;
}
void item_set_data(void *ptr)
{
 item_cur->node.data = ptr;
}
void item_set_selected(int val)
{
 item_cur->node.selected = val;
}
int item_activate_selected(void)
{
 item_foreach()
  if (item_is_selected())
   return 1;
 return 0;
}
void *item_data(void)
{
 return item_cur->node.data;
}
char item_tag(void)
{
 return item_cur->node.tag;
}
int item_count(void)
{
 int n = 0;
 struct dialog_list *p;
 for (p = item_head; p; p = p->next)
  n++;
 return n;
}
void item_set(int n)
{
 int i = 0;
 item_foreach()
  if (i++ == n)
   return;
}
int item_n(void)
{
 int n = 0;
 struct dialog_list *p;
 for (p = item_head; p; p = p->next) {
  if (p == item_cur)
   return n;
  n++;
 }
 return 0;
}
const char *item_str(void)
{
 return item_cur->node.str;
}
int item_is_selected(void)
{
 return (item_cur->node.selected != 0);
}
int item_is_tag(char tag)
{
 return (item_cur->node.tag == tag);
}
static void print_buttons(WINDOW * dialog, int height, int width, int selected)
{
 int x = width / 2 - 10;
 int y = height - 2;
 print_button(dialog, gettext(" Yes "), y, x, selected == 0);
 print_button(dialog, gettext("  No  "), y, x + 13, selected == 1);
 wmove(dialog, y, x + 1 + 13 * selected);
 wrefresh(dialog);
}
int dialog_yesno(const char *title, const char *prompt, int height, int width)
{
 int i, x, y, key = 0, button = 0;
 WINDOW *dialog;
do_resize:
 if (getmaxy(stdscr) < (height + 4))
  return -ERRDISPLAYTOOSMALL;
 if (getmaxx(stdscr) < (width + 4))
  return -ERRDISPLAYTOOSMALL;
 x = (COLS - width) / 2;
 y = (LINES - height) / 2;
 draw_shadow(stdscr, y, x, height, width);
 dialog = newwin(height, width, y, x);
 keypad(dialog, TRUE);
 draw_box(dialog, 0, 0, height, width,
   dlg.dialog.atr, dlg.border.atr);
 wattrset(dialog, dlg.border.atr);
 mvwaddch(dialog, height - 3, 0, ACS_LTEE);
 for (i = 0; i < width - 2; i++)
  waddch(dialog, ACS_HLINE);
 wattrset(dialog, dlg.dialog.atr);
 waddch(dialog, ACS_RTEE);
 print_title(dialog, title, width);
 wattrset(dialog, dlg.dialog.atr);
 print_autowrap(dialog, prompt, width - 2, 1, 3);
 print_buttons(dialog, height, width, 0);
 while (key != KEY_ESC) {
  key = wgetch(dialog);
  switch (key) {
  case 'Y':
  case 'y':
  case '1':
   delwin(dialog);
   return 0;
  case 'N':
  case 'n':
  case '0':
   delwin(dialog);
   return 1;
  case TAB:
  case KEY_LEFT:
  case KEY_RIGHT:
   button = ((key == KEY_LEFT ? --button : ++button) < 0) ? 1 : (button > 1 ? 0 : button);
   print_buttons(dialog, height, width, button);
   wrefresh(dialog);
   break;
  case ' ':
  case '\n':
   delwin(dialog);
   return button;
  case KEY_ESC:
   key = on_key_esc(dialog);
   break;
  case KEY_RESIZE:
   delwin(dialog);
   on_key_resize();
   goto do_resize;
  }
 }
 delwin(dialog);
 return key;
}
# 1497 "dialog.all.c"
static void do_print_item(WINDOW * win, const char *item, int line_y, int selected, int hotkey) {
 int j;
 char *menu_item = malloc(menu_width + 1);
 strncpy(menu_item, item, menu_width - item_x);
 menu_item[menu_width - item_x] = '\0';
 j = first_alpha(menu_item, "YyNnMmHh");
 wattrset(win, dlg.menubox.atr);
 wmove(win, line_y, 0);
#if OLD_NCURSES
 {
  int i;
  for (i = 0; i < menu_width; i++)
   waddch(win, ' ');
 }
#else
 wclrtoeol(win);
#endif
 wattrset(win, selected ? dlg.item_selected.atr : dlg.item.atr);
 mvwaddstr(win, line_y, item_x, menu_item);
 if (hotkey) {
  wattrset(win, selected ? dlg.tag_key_selected.atr
    : dlg.tag_key.atr);
  mvwaddch(win, line_y, item_x + j, menu_item[j]);
 }
 if (selected) {
  wmove(win, line_y, item_x + 1);
 }
 free(menu_item);
 wrefresh(win);
}
#define print_item(index,choice,selected) \
    do { \
 item_set(index); \
 do_print_item(menu, item_str(), choice, selected, !item_is_tag(':')); \
    } while (0)
static void print_arrows(WINDOW * win, int item_no, int scroll, int y, int x,
    int height)
{
 int cur_y, cur_x;
 getyx(win, cur_y, cur_x);
 wmove(win, y, x);
 if (scroll > 0) {
  wattrset(win, dlg.uarrow.atr);
  waddch(win, ACS_UARROW);
  waddstr(win, "(-)");
 } else {
  wattrset(win, dlg.menubox.atr);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
 }
 y = y + height + 1;
 wmove(win, y, x);
 wrefresh(win);
 if ((height < item_no) && (scroll + height < item_no)) {
  wattrset(win, dlg.darrow.atr);
  waddch(win, ACS_DARROW);
  waddstr(win, "(+)");
 } else {
  wattrset(win, dlg.menubox_border.atr);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
 }
 wmove(win, cur_y, cur_x);
 wrefresh(win);
}
static void print_buttons2(WINDOW * win, int height, int width, int selected)
{
 int x = width / 2 - 16;
 int y = height - 2;
 print_button(win, gettext("Select"), y, x, selected == 0);
 print_button(win, gettext(" Exit "), y, x + 12, selected == 1);
 print_button(win, gettext(" Help "), y, x + 24, selected == 2);
 wmove(win, y, x + 1 + 12 * selected);
 wrefresh(win);
}
static void do_scroll(WINDOW *win, int *scroll, int n)
{
 scrollok(win, TRUE);
 wscrl(win, n);
 scrollok(win, FALSE);
 *scroll = *scroll + n;
 wrefresh(win);
}
int dialog_menu(const char *title, const char *prompt,
                const void *selected, int *s_scroll)
{
 int i, j, x, y, box_x, box_y;
 int height, width, menu_height;
 int key = 0, button = 0, scroll = 0, choice = 0;
 int first_item = 0, max_choice;
 WINDOW *dialog, *menu;
do_resize:
 height = getmaxy(stdscr);
 width = getmaxx(stdscr);
 if (height < 15 || width < 65)
  return -ERRDISPLAYTOOSMALL;
 height -= 4;
 width -= 5;
 menu_height = height - 10;
 max_choice = MIN(menu_height, item_count());
 x = (COLS - width) / 2;
 y = (LINES - height) / 2;
 draw_shadow(stdscr, y, x, height, width);
 dialog = newwin(height, width, y, x);
 keypad(dialog, TRUE);
 draw_box(dialog, 0, 0, height, width,
   dlg.dialog.atr, dlg.border.atr);
 wattrset(dialog, dlg.border.atr);
 mvwaddch(dialog, height - 3, 0, ACS_LTEE);
 for (i = 0; i < width - 2; i++)
  waddch(dialog, ACS_HLINE);
 wattrset(dialog, dlg.dialog.atr);
 wbkgdset(dialog, dlg.dialog.atr & A_COLOR);
 waddch(dialog, ACS_RTEE);
 print_title(dialog, title, width);
 wattrset(dialog, dlg.dialog.atr);
 print_autowrap(dialog, prompt, width - 2, 1, 3);
 menu_width = width - 6;
 box_y = height - menu_height - 5;
 box_x = (width - menu_width) / 2 - 1;
 menu = subwin(dialog, menu_height, menu_width,
        y + box_y + 1, x + box_x + 1);
 keypad(menu, TRUE);
 draw_box(dialog, box_y, box_x, menu_height + 2, menu_width + 2,
   dlg.menubox_border.atr, dlg.menubox.atr);
 if (menu_width >= 80)
  item_x = (menu_width - 70) / 2;
 else
  item_x = 4;
 item_foreach()
  if (selected && (selected == item_data()))
   choice = item_n();
 scroll = *s_scroll;
 if ((scroll <= choice) && (scroll + max_choice > choice) &&
    (scroll >= 0) && (scroll + max_choice <= item_count())) {
  first_item = scroll;
  choice = choice - scroll;
 } else {
  scroll = 0;
 }
 if ((choice >= max_choice)) {
  if (choice >= item_count() - max_choice / 2)
   scroll = first_item = item_count() - max_choice;
  else
   scroll = first_item = choice - max_choice / 2;
  choice = choice - scroll;
 }
 for (i = 0; i < max_choice; i++) {
  print_item(first_item + i, i, i == choice);
 }
 wnoutrefresh(menu);
 print_arrows(dialog, item_count(), scroll,
       box_y, box_x + item_x + 1, menu_height);
 print_buttons2(dialog, height, width, 0);
 wmove(menu, choice, item_x + 1);
 wrefresh(menu);
 while (key != KEY_ESC) {
  key = wgetch(menu);
  if (key < 256 && isalpha(key))
   key = tolower(key);
  if (strchr("ynmh", key))
   i = max_choice;
  else {
   for (i = choice + 1; i < max_choice; i++) {
    item_set(scroll + i);
    j = first_alpha(item_str(), "YyNnMmHh");
    if (key == tolower(item_str()[j]))
     break;
   }
   if (i == max_choice)
    for (i = 0; i < max_choice; i++) {
     item_set(scroll + i);
     j = first_alpha(item_str(), "YyNnMmHh");
     if (key == tolower(item_str()[j]))
      break;
    }
  }
  if (i < max_choice ||
      key == KEY_UP || key == KEY_DOWN ||
      key == '-' || key == '+' ||
      key == KEY_PPAGE || key == KEY_NPAGE) {
   print_item(scroll + choice, choice, FALSE);
   if (key == KEY_UP || key == '-') {
    if (choice < 2 && scroll) {
     do_scroll(menu, &scroll, -1);
     print_item(scroll, 0, FALSE);
    } else
     choice = MAX(choice - 1, 0);
   } else if (key == KEY_DOWN || key == '+') {
    print_item(scroll+choice, choice, FALSE);
    if ((choice > max_choice - 3) &&
        (scroll + max_choice < item_count())) {
     do_scroll(menu, &scroll, 1);
     print_item(scroll+max_choice - 1,
         max_choice - 1, FALSE);
    } else
     choice = MIN(choice + 1, max_choice - 1);
   } else if (key == KEY_PPAGE) {
    scrollok(menu, TRUE);
    for (i = 0; (i < max_choice); i++) {
     if (scroll > 0) {
      do_scroll(menu, &scroll, -1);
      print_item(scroll, 0, FALSE);
     } else {
      if (choice > 0)
       choice--;
     }
    }
   } else if (key == KEY_NPAGE) {
    for (i = 0; (i < max_choice); i++) {
     if (scroll + max_choice < item_count()) {
      do_scroll(menu, &scroll, 1);
      print_item(scroll+max_choice-1,
          max_choice - 1, FALSE);
     } else {
      if (choice + 1 < max_choice)
       choice++;
     }
    }
   } else
    choice = i;
   print_item(scroll + choice, choice, TRUE);
   print_arrows(dialog, item_count(), scroll,
         box_y, box_x + item_x + 1, menu_height);
   wnoutrefresh(dialog);
   wrefresh(menu);
   continue;
  }
  switch (key) {
  case KEY_LEFT:
  case TAB:
  case KEY_RIGHT:
   button = ((key == KEY_LEFT ? --button : ++button) < 0)
       ? 2 : (button > 2 ? 0 : button);
   print_buttons2(dialog, height, width, button);
   wrefresh(menu);
   break;
  case ' ':
  case 's':
  case 'y':
  case 'n':
  case 'm':
  case '0':
  case '1':
  case '2':
  case '/':
  case 'h':
  case '?':
  case 'z':
  case '\n':
   *s_scroll = scroll;
   delwin(menu);
   delwin(dialog);
   item_set(scroll + choice);
   item_set_selected(1);
   switch (key) {
   case 'h':
   case '?':
    return 2;
   case 's':
   case 'y':
   case '1':
    return 3;
   case 'n':
   case '0':
    return 4;
   case 'm':
   case '2':
    return 5;
   case ' ':
    return 6;
   case '/':
    return 7;
   case 'z':
    return 8;
   case '\n':
    return button;
   }
   return 0;
  case 'e':
  case 'x':
   key = KEY_ESC;
   break;
  case KEY_ESC:
   key = on_key_esc(menu);
   break;
  case KEY_RESIZE:
   on_key_resize();
   delwin(menu);
   delwin(dialog);
   goto do_resize;
  }
 }
 delwin(menu);
 delwin(dialog);
 return key;
}
#undef print_item
# 1889 "dialog.all.c"
static void print_buttons3(WINDOW * dialog, int height, int width, int selected)
{
 int x = width / 2 - 11;
 int y = height - 2;
 print_button(dialog, gettext("  Ok  "), y, x, selected == 0);
 print_button(dialog, gettext(" Help "), y, x + 14, selected == 1);
 wmove(dialog, y, x + 1 + 14 * selected);
 wrefresh(dialog);
}
int dialog_inputbox(const char *title, const char *prompt, int height, int width,
                    const char *init)
{
 int i, x, y, box_y, box_x, box_width;
 int input_x = 0, scroll = 0, key = 0, button = -1;
 char *instr = dialog_input_result;
 WINDOW *dialog;
 if (!init)
  instr[0] = '\0';
 else
  strcpy(instr, init);
do_resize:
 if (getmaxy(stdscr) <= (height - 2))
  return -ERRDISPLAYTOOSMALL;
 if (getmaxx(stdscr) <= (width - 2))
  return -ERRDISPLAYTOOSMALL;
 x = (COLS - width) / 2;
 y = (LINES - height) / 2;
 draw_shadow(stdscr, y, x, height, width);
 dialog = newwin(height, width, y, x);
 keypad(dialog, TRUE);
 draw_box(dialog, 0, 0, height, width,
   dlg.dialog.atr, dlg.border.atr);
 wattrset(dialog, dlg.border.atr);
 mvwaddch(dialog, height - 3, 0, ACS_LTEE);
 for (i = 0; i < width - 2; i++)
  waddch(dialog, ACS_HLINE);
 wattrset(dialog, dlg.dialog.atr);
 waddch(dialog, ACS_RTEE);
 print_title(dialog, title, width);
 wattrset(dialog, dlg.dialog.atr);
 print_autowrap(dialog, prompt, width - 2, 1, 3);
 box_width = width - 6;
 getyx(dialog, y, x);
 box_y = y + 2;
 box_x = (width - box_width) / 2;
 draw_box(dialog, y + 1, box_x - 1, 3, box_width + 2,
   dlg.dialog.atr, dlg.border.atr);
 print_buttons3(dialog, height, width, 0);
 wmove(dialog, box_y, box_x);
 wattrset(dialog, dlg.inputbox.atr);
 input_x = strlen(instr);
 if (input_x >= box_width) {
  scroll = input_x - box_width + 1;
  input_x = box_width - 1;
  for (i = 0; i < box_width - 1; i++)
   waddch(dialog, instr[scroll + i]);
 } else {
  waddstr(dialog, instr);
 }
 wmove(dialog, box_y, box_x + input_x);
 wrefresh(dialog);
 while (key != KEY_ESC) {
  key = wgetch(dialog);
  if (button == -1) {
   switch (key) {
   case TAB:
   case KEY_UP:
   case KEY_DOWN:
    break;
   case KEY_LEFT:
    continue;
   case KEY_RIGHT:
    continue;
   case KEY_BACKSPACE:
   case 127:
    if (input_x || scroll) {
     wattrset(dialog, dlg.inputbox.atr);
     if (!input_x) {
      scroll = scroll < box_width - 1 ? 0 : scroll - (box_width - 1);
      wmove(dialog, box_y, box_x);
      for (i = 0; i < box_width; i++)
       waddch(dialog,
              instr[scroll + input_x + i] ?
              instr[scroll + input_x + i] : ' ');
      input_x = strlen(instr) - scroll;
     } else
      input_x--;
     instr[scroll + input_x] = '\0';
     mvwaddch(dialog, box_y, input_x + box_x, ' ');
     wmove(dialog, box_y, input_x + box_x);
     wrefresh(dialog);
    }
    continue;
   default:
    if (key < 0x100 && isprint(key)) {
     if (scroll + input_x < MAX_LEN) {
      wattrset(dialog, dlg.inputbox.atr);
      instr[scroll + input_x] = key;
      instr[scroll + input_x + 1] = '\0';
      if (input_x == box_width - 1) {
       scroll++;
       wmove(dialog, box_y, box_x);
       for (i = 0; i < box_width - 1; i++)
        waddch(dialog, instr [scroll + i]);
      } else {
       wmove(dialog, box_y, input_x++ + box_x);
       waddch(dialog, key);
      }
      wrefresh(dialog);
     } else
      flash();
     continue;
    }
   }
  }
  switch (key) {
  case 'O':
  case 'o':
   delwin(dialog);
   return 0;
  case 'H':
  case 'h':
   delwin(dialog);
   return 1;
  case KEY_UP:
  case KEY_LEFT:
   switch (button) {
   case -1:
    button = 1;
    print_buttons3(dialog, height, width, 1);
    break;
   case 0:
    button = -1;
    print_buttons3(dialog, height, width, 0);
    wmove(dialog, box_y, box_x + input_x);
    wrefresh(dialog);
    break;
   case 1:
    button = 0;
    print_buttons3(dialog, height, width, 0);
    break;
   }
   break;
  case TAB:
  case KEY_DOWN:
  case KEY_RIGHT:
   switch (button) {
   case -1:
    button = 0;
    print_buttons3(dialog, height, width, 0);
    break;
   case 0:
    button = 1;
    print_buttons3(dialog, height, width, 1);
    break;
   case 1:
    button = -1;
    print_buttons3(dialog, height, width, 0);
    wmove(dialog, box_y, box_x + input_x);
    wrefresh(dialog);
    break;
   }
   break;
  case ' ':
  case '\n':
   delwin(dialog);
   return (button == -1 ? 0 : button);
  case 'X':
  case 'x':
   key = KEY_ESC;
   break;
  case KEY_ESC:
   key = on_key_esc(dialog);
   break;
  case KEY_RESIZE:
   delwin(dialog);
   on_key_resize();
   goto do_resize;
  }
 }
 delwin(dialog);
 return KEY_ESC;
}
# 2122 "dialog.all.c"
static void refresh_text_box(WINDOW *dialog, WINDOW *box, int boxh, int boxw,
        int cur_y, int cur_x, update_text_fn update_text,
        void *data)
{
 print_page(box, boxh, boxw, update_text, data);
 print_position(dialog);
 wmove(dialog, cur_y, cur_x);
 wrefresh(dialog);
}
# 2139 "dialog.all.c"
int dialog_textbox(const char *title, char *tbuf, int initial_height,
     int initial_width, int *keys, int *_vscroll, int *_hscroll,
     update_text_fn update_text, void *data)
{
 int i, x, y, cur_x, cur_y, key = 0;
 int height, width, boxh, boxw;
 WINDOW *dialog, *box;
 bool done = false;
 begin_reached = 1;
 end_reached = 0;
 page_length = 0;
 hscroll = 0;
 buf = tbuf;
 page = buf;
 if (_vscroll && *_vscroll) {
  begin_reached = 0;
  for (i = 0; i < *_vscroll; i++)
   get_line();
 }
 if (_hscroll)
  hscroll = *_hscroll;
do_resize:
 getmaxyx(stdscr, height, width);
 if (height < 8 || width < 8)
  return -ERRDISPLAYTOOSMALL;
 if (initial_height != 0)
  height = initial_height;
 else
  if (height > 4)
   height -= 4;
  else
   height = 0;
 if (initial_width != 0)
  width = initial_width;
 else
  if (width > 5)
   width -= 5;
  else
   width = 0;
 x = (COLS - width) / 2;
 y = (LINES - height) / 2;
 draw_shadow(stdscr, y, x, height, width);
 dialog = newwin(height, width, y, x);
 keypad(dialog, TRUE);
 boxh = height - 4;
 boxw = width - 2;
 box = subwin(dialog, boxh, boxw, y + 1, x + 1);
 wattrset(box, dlg.dialog.atr);
 wbkgdset(box, dlg.dialog.atr & A_COLOR);
 keypad(box, TRUE);
 draw_box(dialog, 0, 0, height, width,
   dlg.dialog.atr, dlg.border.atr);
 wattrset(dialog, dlg.border.atr);
 mvwaddch(dialog, height - 3, 0, ACS_LTEE);
 for (i = 0; i < width - 2; i++)
  waddch(dialog, ACS_HLINE);
 wattrset(dialog, dlg.dialog.atr);
 wbkgdset(dialog, dlg.dialog.atr & A_COLOR);
 waddch(dialog, ACS_RTEE);
 print_title(dialog, title, width);
 print_button(dialog, gettext(" Exit "), height - 2, width / 2 - 4, TRUE);
 wnoutrefresh(dialog);
 getyx(dialog, cur_y, cur_x);
 attr_clear(box, boxh, boxw, dlg.dialog.atr);
 refresh_text_box(dialog, box, boxh, boxw, cur_y, cur_x, update_text,
    data);
 while (!done) {
  key = wgetch(dialog);
  switch (key) {
  case 'E':
  case 'e':
  case 'X':
  case 'x':
  case 'q':
  case '\n':
   done = true;
   break;
  case 'g':
  case KEY_HOME:
   if (!begin_reached) {
    begin_reached = 1;
    page = buf;
    refresh_text_box(dialog, box, boxh, boxw,
       cur_y, cur_x, update_text,
       data);
   }
   break;
  case 'G':
  case KEY_END:
   end_reached = 1;
   page = buf + strlen(buf);
   back_lines(boxh);
   refresh_text_box(dialog, box, boxh, boxw, cur_y,
      cur_x, update_text, data);
   break;
  case 'K':
  case 'k':
  case KEY_UP:
   if (begin_reached)
    break;
   back_lines(page_length + 1);
   refresh_text_box(dialog, box, boxh, boxw, cur_y,
      cur_x, update_text, data);
   break;
  case 'B':
  case 'b':
  case 'u':
  case KEY_PPAGE:
   if (begin_reached)
    break;
   back_lines(page_length + boxh);
   refresh_text_box(dialog, box, boxh, boxw, cur_y,
      cur_x, update_text, data);
   break;
  case 'J':
  case 'j':
  case KEY_DOWN:
   if (end_reached)
    break;
   back_lines(page_length - 1);
   refresh_text_box(dialog, box, boxh, boxw, cur_y,
      cur_x, update_text, data);
   break;
  case KEY_NPAGE:
  case ' ':
  case 'd':
   if (end_reached)
    break;
   begin_reached = 0;
   refresh_text_box(dialog, box, boxh, boxw, cur_y,
      cur_x, update_text, data);
   break;
  case '0':
  case 'H':
  case 'h':
  case KEY_LEFT:
   if (hscroll <= 0)
    break;
   if (key == '0')
    hscroll = 0;
   else
    hscroll--;
   back_lines(page_length);
   refresh_text_box(dialog, box, boxh, boxw, cur_y,
      cur_x, update_text, data);
   break;
  case 'L':
  case 'l':
  case KEY_RIGHT:
   if (hscroll >= MAX_LEN)
    break;
   hscroll++;
   back_lines(page_length);
   refresh_text_box(dialog, box, boxh, boxw, cur_y,
      cur_x, update_text, data);
   break;
  case KEY_ESC:
   if (on_key_esc(dialog) == KEY_ESC)
    done = true;
   break;
  case KEY_RESIZE:
   back_lines(height);
   delwin(box);
   delwin(dialog);
   on_key_resize();
   goto do_resize;
  default:
   for (i = 0; keys[i]; i++) {
    if (key == keys[i]) {
     done = true;
     break;
    }
   }
  }
 }
 delwin(box);
 delwin(dialog);
 if (_vscroll) {
  const char *s;
  s = buf;
  *_vscroll = 0;
  back_lines(page_length);
  while (s < page && (s = strchr(s, '\n'))) {
   (*_vscroll)++;
   s++;
  }
 }
 if (_hscroll)
  *_hscroll = hscroll;
 return key;
}
static void back_lines(int n)
{
 int i;
 begin_reached = 0;
 for (i = 0; i < n; i++) {
  if (*page == '\0') {
   if (end_reached) {
    end_reached = 0;
    continue;
   }
  }
  if (page == buf) {
   begin_reached = 1;
   return;
  }
  page--;
  do {
   if (page == buf) {
    begin_reached = 1;
    return;
   }
   page--;
  } while (*page != '\n');
  page++;
 }
}
static void print_page(WINDOW *win, int height, int width, update_text_fn
         update_text, void *data)
{
 int i, passed_end = 0;
 if (update_text) {
  char *end;
  for (i = 0; i < height; i++)
   get_line();
  end = page;
  back_lines(height);
  update_text(buf, page - buf, end - buf, data);
 }
 page_length = 0;
 for (i = 0; i < height; i++) {
  print_line(win, i, width);
  if (!passed_end)
   page_length++;
  if (end_reached && !passed_end)
   passed_end = 1;
 }
 wnoutrefresh(win);
}
static void print_line(WINDOW * win, int row, int width)
{
 char *line;
 line = get_line();
 line += MIN(strlen(line), hscroll);
 wmove(win, row, 0);
 waddch(win, ' ');
 waddnstr(win, line, MIN(strlen(line), width - 2));
#if OLD_NCURSES
 {
  int x = getcurx(win);
  int i;
  for (i = 0; i < width - x; i++)
   waddch(win, ' ');
 }
#else
 wclrtoeol(win);
#endif
}
static char *get_line(void)
{
 int i = 0;
 static char line[MAX_LEN + 1];
 end_reached = 0;
 while (*page != '\n') {
  if (*page == '\0') {
   end_reached = 1;
   break;
  } else if (i < MAX_LEN)
   line[i++] = *(page++);
  else {
   if (i == MAX_LEN)
    line[i++] = '\0';
   page++;
  }
 }
 if (i <= MAX_LEN)
  line[i] = '\0';
 if (!end_reached)
  page++;
 return line;
}
static void print_position(WINDOW * win)
{
 int percent;
 wattrset(win, dlg.position_indicator.atr);
 wbkgdset(win, dlg.position_indicator.atr & A_COLOR);
 percent = (page - buf) * 100 / strlen(buf);
 wmove(win, getmaxy(win) - 3, getmaxx(win) - 9);
 wprintw(win, "(%3d%%)", percent);
}
# 2518 "dialog.all.c"
static void print_item(WINDOW * win, int choice, int selected) {
 int i;
 char *list_item = malloc(list_width + 1);
 strncpy(list_item, item_str(), list_width - item_x);
 list_item[list_width - item_x] = '\0';
 wattrset(win, dlg.menubox.atr);
 wmove(win, choice, 0);
 for (i = 0; i < list_width; i++)
  waddch(win, ' ');
 wmove(win, choice, check_x);
 wattrset(win, selected ? dlg.check_selected.atr
   : dlg.check.atr);
 if (!item_is_tag(':'))
  wprintw(win, "(%c)", item_is_tag('X') ? 'X' : ' ');
 wattrset(win, selected ? dlg.tag_selected.atr : dlg.tag.atr);
 mvwaddch(win, choice, item_x, list_item[0]);
 wattrset(win, selected ? dlg.item_selected.atr : dlg.item.atr);
 waddstr(win, list_item + 1);
 if (selected) {
  wmove(win, choice, check_x + 1);
  wrefresh(win);
 }
 free(list_item);
}
static void print_arrows1(WINDOW * win, int choice, int item_no, int scroll,
      int y, int x, int height)
{
 wmove(win, y, x);
 if (scroll > 0) {
  wattrset(win, dlg.uarrow.atr);
  waddch(win, ACS_UARROW);
  waddstr(win, "(-)");
 } else {
  wattrset(win, dlg.menubox.atr);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
 }
 y = y + height + 1;
 wmove(win, y, x);
 if ((height < item_no) && (scroll + choice < item_no - 1)) {
  wattrset(win, dlg.darrow.atr);
  waddch(win, ACS_DARROW);
  waddstr(win, "(+)");
 } else {
  wattrset(win, dlg.menubox_border.atr);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
  waddch(win, ACS_HLINE);
 }
}
static void print_buttons4(WINDOW * dialog, int height, int width, int selected)
{
 int x = width / 2 - 11;
 int y = height - 2;
 print_button(dialog, gettext("Select"), y, x, selected == 0);
 print_button(dialog, gettext(" Help "), y, x + 14, selected == 1);
 wmove(dialog, y, x + 1 + 14 * selected);
 wrefresh(dialog);
}
int dialog_checklist(const char *title, const char *prompt, int height,
       int width, int list_height)
{
 int i, x, y, box_x, box_y;
 int key = 0, button = 0, choice = 0, scroll = 0, max_choice;
 WINDOW *dialog, *list;
 item_foreach() {
  if (item_is_tag('X'))
   choice = item_n();
  if (item_is_selected()) {
   choice = item_n();
   break;
  }
 }
do_resize:
 if (getmaxy(stdscr) < (height + 6))
  return -ERRDISPLAYTOOSMALL;
 if (getmaxx(stdscr) < (width + 6))
  return -ERRDISPLAYTOOSMALL;
 max_choice = MIN(list_height, item_count());
 x = (COLS - width) / 2;
 y = (LINES - height) / 2;
 draw_shadow(stdscr, y, x, height, width);
 dialog = newwin(height, width, y, x);
 keypad(dialog, TRUE);
 draw_box(dialog, 0, 0, height, width,
   dlg.dialog.atr, dlg.border.atr);
 wattrset(dialog, dlg.border.atr);
 mvwaddch(dialog, height - 3, 0, ACS_LTEE);
 for (i = 0; i < width - 2; i++)
  waddch(dialog, ACS_HLINE);
 wattrset(dialog, dlg.dialog.atr);
 waddch(dialog, ACS_RTEE);
 print_title(dialog, title, width);
 wattrset(dialog, dlg.dialog.atr);
 print_autowrap(dialog, prompt, width - 2, 1, 3);
 list_width = width - 6;
 box_y = height - list_height - 5;
 box_x = (width - list_width) / 2 - 1;
 list = subwin(dialog, list_height, list_width, y + box_y + 1,
               x + box_x + 1);
 keypad(list, TRUE);
 draw_box(dialog, box_y, box_x, list_height + 2, list_width + 2,
          dlg.menubox_border.atr, dlg.menubox.atr);
 check_x = 0;
 item_foreach()
  check_x = MAX(check_x, strlen(item_str()) + 4);
 check_x = MIN(check_x, list_width);
 check_x = (list_width - check_x) / 2;
 item_x = check_x + 4;
 if (choice >= list_height) {
  scroll = choice - list_height + 1;
  choice -= scroll;
 }
 for (i = 0; i < max_choice; i++) {
  item_set(scroll + i);
  print_item(list, i, i == choice);
 }
 print_arrows1(dialog, choice, item_count(), scroll,
       box_y, box_x + check_x + 5, list_height);
 print_buttons4(dialog, height, width, 0);
 wnoutrefresh(dialog);
 wnoutrefresh(list);
 doupdate();
 while (key != KEY_ESC) {
  key = wgetch(dialog);
  for (i = 0; i < max_choice; i++) {
   item_set(i + scroll);
   if (toupper(key) == toupper(item_str()[0]))
    break;
  }
  if (i < max_choice || key == KEY_UP || key == KEY_DOWN ||
      key == '+' || key == '-') {
   if (key == KEY_UP || key == '-') {
    if (!choice) {
     if (!scroll)
      continue;
     if (list_height > 1) {
      item_set(scroll);
      print_item(list, 0, FALSE);
      scrollok(list, TRUE);
      wscrl(list, -1);
      scrollok(list, FALSE);
     }
     scroll--;
     item_set(scroll);
     print_item(list, 0, TRUE);
     print_arrows1(dialog, choice, item_count(),
           scroll, box_y, box_x + check_x + 5, list_height);
     wnoutrefresh(dialog);
     wrefresh(list);
     continue;
    } else
     i = choice - 1;
   } else if (key == KEY_DOWN || key == '+') {
    if (choice == max_choice - 1) {
     if (scroll + choice >= item_count() - 1)
      continue;
     if (list_height > 1) {
      item_set(scroll + max_choice - 1);
      print_item(list,
           max_choice - 1,
           FALSE);
      scrollok(list, TRUE);
      wscrl(list, 1);
      scrollok(list, FALSE);
     }
     scroll++;
     item_set(scroll + max_choice - 1);
     print_item(list, max_choice - 1, TRUE);
     print_arrows1(dialog, choice, item_count(),
           scroll, box_y, box_x + check_x + 5, list_height);
     wnoutrefresh(dialog);
     wrefresh(list);
     continue;
    } else
     i = choice + 1;
   }
   if (i != choice) {
    item_set(scroll + choice);
    print_item(list, choice, FALSE);
    choice = i;
    item_set(scroll + choice);
    print_item(list, choice, TRUE);
    wnoutrefresh(dialog);
    wrefresh(list);
   }
   continue;
  }
  switch (key) {
  case 'H':
  case 'h':
  case '?':
   button = 1;
  case 'S':
  case 's':
  case ' ':
  case '\n':
   item_foreach()
    item_set_selected(0);
   item_set(scroll + choice);
   item_set_selected(1);
   delwin(list);
   delwin(dialog);
   return button;
  case TAB:
  case KEY_LEFT:
  case KEY_RIGHT:
   button = ((key == KEY_LEFT ? --button : ++button) < 0)
       ? 1 : (button > 1 ? 0 : button);
   print_buttons4(dialog, height, width, button);
   wrefresh(dialog);
   break;
  case 'X':
  case 'x':
   key = KEY_ESC;
   break;
  case KEY_ESC:
   key = on_key_esc(dialog);
   break;
  case KEY_RESIZE:
   delwin(list);
   delwin(dialog);
   on_key_resize();
   goto do_resize;
  }
  doupdate();
 }
 delwin(list);
 delwin(dialog);
 return key;
}
static int zconflex(void);
static void zconfprint(const char *err, ...);
static void zconf_error(const char *err, ...);
static void zconferror(const char *err);
static bool zconf_endtoken(const struct kconf_id *id, int starttoken, int endtoken);
struct symbol *symbol_hash[SYMBOL_HASHSIZE];
static struct menu *current_menu, *current_entry;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYERROR_VERBOSE 1
#ifndef YYTOKEN_TABLE
#define YYTOKEN_TABLE 0
#endif
#ifndef YYTOKENTYPE
#define YYTOKENTYPE 
   enum yytokentype {
     T_MAINMENU = 258,
     T_MENU = 259,
     T_ENDMENU = 260,
     T_SOURCE = 261,
     T_CHOICE = 262,
     T_ENDCHOICE = 263,
     T_COMMENT = 264,
     T_CONFIG = 265,
     T_MENUCONFIG = 266,
     T_HELP = 267,
     T_HELPTEXT = 268,
     T_IF = 269,
     T_ENDIF = 270,
     T_DEPENDS = 271,
     T_OPTIONAL = 272,
     T_PROMPT = 273,
     T_TYPE = 274,
     T_DEFAULT = 275,
     T_SELECT = 276,
     T_RANGE = 277,
     T_VISIBLE = 278,
     T_OPTION = 279,
     T_ON = 280,
     T_WORD = 281,
     T_WORD_QUOTE = 282,
     T_UNEQUAL = 283,
     T_CLOSE_PAREN = 284,
     T_OPEN_PAREN = 285,
     T_EOL = 286,
     T_OR = 287,
     T_AND = 288,
     T_EQUAL = 289,
     T_NOT = 290
   };
#endif
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
 char *string;
 struct file *file;
 struct symbol *symbol;
 struct expr *expr;
 struct menu *menu;
 const struct kconf_id *id;
} YYSTYPE;
#define YYSTYPE_IS_TRIVIAL 1
#define yystype YYSTYPE
#define YYSTYPE_IS_DECLARED 1
#endif
#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif
#line 10 "scripts/kconfig/zconf.gperf"
struct kconf_id;
static const struct kconf_id *kconf_id_lookup(register const char *str, register unsigned int len);
#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
kconf_id_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 25, 25,
       0, 0, 0, 5, 0, 0, 73, 73, 5, 0,
      10, 5, 45, 73, 20, 20, 0, 15, 15, 73,
      20, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73, 73, 73, 73, 73,
      73, 73, 73, 73, 73, 73
    };
  register int hval = len;
  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}
struct kconf_id_strings_t
  {
    char kconf_id_strings_str2[sizeof("if")];
    char kconf_id_strings_str3[sizeof("int")];
    char kconf_id_strings_str5[sizeof("endif")];
    char kconf_id_strings_str7[sizeof("default")];
    char kconf_id_strings_str8[sizeof("tristate")];
    char kconf_id_strings_str9[sizeof("endchoice")];
    char kconf_id_strings_str12[sizeof("def_tristate")];
    char kconf_id_strings_str13[sizeof("def_bool")];
    char kconf_id_strings_str14[sizeof("defconfig_list")];
    char kconf_id_strings_str17[sizeof("on")];
    char kconf_id_strings_str18[sizeof("optional")];
    char kconf_id_strings_str21[sizeof("option")];
    char kconf_id_strings_str22[sizeof("endmenu")];
    char kconf_id_strings_str23[sizeof("mainmenu")];
    char kconf_id_strings_str25[sizeof("menuconfig")];
    char kconf_id_strings_str27[sizeof("modules")];
    char kconf_id_strings_str29[sizeof("menu")];
    char kconf_id_strings_str31[sizeof("select")];
    char kconf_id_strings_str32[sizeof("comment")];
    char kconf_id_strings_str33[sizeof("env")];
    char kconf_id_strings_str35[sizeof("range")];
    char kconf_id_strings_str36[sizeof("choice")];
    char kconf_id_strings_str39[sizeof("bool")];
    char kconf_id_strings_str41[sizeof("source")];
    char kconf_id_strings_str42[sizeof("visible")];
    char kconf_id_strings_str43[sizeof("hex")];
    char kconf_id_strings_str46[sizeof("config")];
    char kconf_id_strings_str47[sizeof("boolean")];
    char kconf_id_strings_str51[sizeof("string")];
    char kconf_id_strings_str54[sizeof("help")];
    char kconf_id_strings_str56[sizeof("prompt")];
    char kconf_id_strings_str72[sizeof("depends")];
  };
static const struct kconf_id_strings_t kconf_id_strings_contents =
  {
    "if",
    "int",
    "endif",
    "default",
    "tristate",
    "endchoice",
    "def_tristate",
    "def_bool",
    "defconfig_list",
    "on",
    "optional",
    "option",
    "endmenu",
    "mainmenu",
    "menuconfig",
    "modules",
    "menu",
    "select",
    "comment",
    "env",
    "range",
    "choice",
    "bool",
    "source",
    "visible",
    "hex",
    "config",
    "boolean",
    "string",
    "help",
    "prompt",
    "depends"
  };
#define kconf_id_strings ((const char *) &kconf_id_strings_contents)
#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct kconf_id *
kconf_id_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 32,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 14,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 72
    };
  static const struct kconf_id wordlist[] =
    {
      {-1}, {-1},
#line 25 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str2, T_IF, TF_COMMAND|TF_PARAM},
#line 36 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str3, T_TYPE, TF_COMMAND, S_INT},
      {-1},
#line 26 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str5, T_ENDIF, TF_COMMAND},
      {-1},
#line 29 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str7, T_DEFAULT, TF_COMMAND, S_UNKNOWN},
#line 31 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str8, T_TYPE, TF_COMMAND, S_TRISTATE},
#line 20 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str9, T_ENDCHOICE, TF_COMMAND},
      {-1}, {-1},
#line 32 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str12, T_DEFAULT, TF_COMMAND, S_TRISTATE},
#line 35 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str13, T_DEFAULT, TF_COMMAND, S_BOOLEAN},
#line 45 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str14, T_OPT_DEFCONFIG_LIST,TF_OPTION},
      {-1}, {-1},
#line 43 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str17, T_ON, TF_PARAM},
#line 28 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str18, T_OPTIONAL, TF_COMMAND},
      {-1}, {-1},
#line 42 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str21, T_OPTION, TF_COMMAND},
#line 17 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str22, T_ENDMENU, TF_COMMAND},
#line 15 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str23, T_MAINMENU, TF_COMMAND},
      {-1},
#line 23 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str25, T_MENUCONFIG, TF_COMMAND},
      {-1},
#line 44 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str27, T_OPT_MODULES, TF_OPTION},
      {-1},
#line 16 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str29, T_MENU, TF_COMMAND},
      {-1},
#line 39 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str31, T_SELECT, TF_COMMAND},
#line 21 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str32, T_COMMENT, TF_COMMAND},
#line 46 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str33, T_OPT_ENV, TF_OPTION},
      {-1},
#line 40 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str35, T_RANGE, TF_COMMAND},
#line 19 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str36, T_CHOICE, TF_COMMAND},
      {-1}, {-1},
#line 33 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str39, T_TYPE, TF_COMMAND, S_BOOLEAN},
      {-1},
#line 18 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str41, T_SOURCE, TF_COMMAND},
#line 41 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str42, T_VISIBLE, TF_COMMAND},
#line 37 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str43, T_TYPE, TF_COMMAND, S_HEX},
      {-1}, {-1},
#line 22 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str46, T_CONFIG, TF_COMMAND},
#line 34 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str47, T_TYPE, TF_COMMAND, S_BOOLEAN},
      {-1}, {-1}, {-1},
#line 38 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str51, T_TYPE, TF_COMMAND, S_STRING},
      {-1}, {-1},
#line 24 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str54, T_HELP, TF_COMMAND},
      {-1},
#line 30 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str56, T_PROMPT, TF_COMMAND},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 27 "scripts/kconfig/zconf.gperf"
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str72, T_DEPENDS, TF_COMMAND}
    };
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = kconf_id_hash (str, len);
      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int o = wordlist[key].name;
          if (o >= 0)
            {
              register const char *s = o + kconf_id_strings;
              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &wordlist[key];
            }
        }
    }
  return 0;
}
#line 47 "scripts/kconfig/zconf.gperf"
#ifdef short
#undef short
#endif
#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif
#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif
#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif
#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif
#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
# include <stddef.h>
#define YYSIZE_T size_t
# else
#define YYSIZE_T unsigned int
# endif
#endif
#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)
#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
# if ENABLE_NLS
# include <libintl.h>
#define YY_(msgid) dgettext ("bison-runtime", msgid)
# endif
# endif
# ifndef YY_
#define YY_(msgid) msgid
# endif
#endif
#if ! defined lint || defined __GNUC__
#define YYUSE(e) ((void) (e))
#else
#define YYUSE(e) 
#endif
#ifndef lint
#define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif
#if ! defined yyoverflow || YYERROR_VERBOSE
# ifdef YYSTACK_USE_ALLOCA
# if YYSTACK_USE_ALLOCA
# ifdef __GNUC__
#define YYSTACK_ALLOC __builtin_alloca
# elif defined __BUILTIN_VA_ARG_INCR
# include <alloca.h>
# elif defined _AIX
#define YYSTACK_ALLOC __alloca
# elif defined _MSC_VER
# include <malloc.h>
#define alloca _alloca
# else
#define YYSTACK_ALLOC alloca
# if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
# include <stdlib.h>
# ifndef _STDLIB_H
#define _STDLIB_H 1
# endif
# endif
# endif
# endif
# endif
# ifdef YYSTACK_ALLOC
#define YYSTACK_FREE(Ptr) do { ; } while (YYID (0))
# ifndef YYSTACK_ALLOC_MAXIMUM
#define YYSTACK_ALLOC_MAXIMUM 4032
# endif
# else
#define YYSTACK_ALLOC YYMALLOC
#define YYSTACK_FREE YYFREE
# ifndef YYSTACK_ALLOC_MAXIMUM
#define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
# endif
# if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
      && (defined YYFREE || defined free)))
# include <stdlib.h>
# ifndef _STDLIB_H
#define _STDLIB_H 1
# endif
# endif
# ifndef YYMALLOC
#define YYMALLOC malloc
# if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T);
# endif
# endif
# ifndef YYFREE
#define YYFREE free
# if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *);
# endif
# endif
# endif
#endif
#if (! defined yyoverflow \
     && (! defined __cplusplus \
  || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};
#define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)
#define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)
# ifndef YYCOPY
# if defined __GNUC__ && 1 < __GNUC__
#define YYCOPY(To,From,Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
# else
#define YYCOPY(To,From,Count) \
      do \
 { \
   YYSIZE_T yyi; \
   for (yyi = 0; yyi < (Count); yyi++) \
     (To)[yyi] = (From)[yyi]; \
 } \
      while (YYID (0))
# endif
# endif
#define YYSTACK_RELOCATE(Stack_alloc,Stack) \
    do \
      { \
 YYSIZE_T yynewbytes; \
 YYCOPY (&yyptr->Stack_alloc, Stack, yysize); \
 Stack = &yyptr->Stack_alloc; \
 yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
 yyptr += yynewbytes / sizeof (*yyptr); \
      } \
    while (YYID (0))
#endif
#define YYFINAL 11
#define YYLAST 290
#define YYNTOKENS 36
#define YYNNTS 50
#define YYNRULES 118
#define YYNSTATES 191
#define YYUNDEFTOK 2
#define YYMAXUTOK 290
#define YYTRANSLATE(YYX) \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)
static const yytype_uint8 yytranslate[] =
{
       0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
       2, 2, 2, 2, 2, 2, 1, 2, 3, 4,
       5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
      15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
      25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
      35
};
#if YYDEBUG
static const yytype_uint16 yyprhs[] =
{
       0, 0, 3, 6, 8, 11, 13, 14, 17, 20,
      23, 26, 31, 36, 40, 42, 44, 46, 48, 50,
      52, 54, 56, 58, 60, 62, 64, 66, 68, 72,
      75, 79, 82, 86, 89, 90, 93, 96, 99, 102,
     105, 108, 112, 117, 122, 127, 133, 137, 138, 142,
     143, 146, 150, 153, 155, 159, 160, 163, 166, 169,
     172, 175, 180, 184, 187, 192, 193, 196, 200, 202,
     206, 207, 210, 213, 216, 220, 224, 228, 230, 234,
     235, 238, 241, 244, 248, 252, 255, 258, 261, 262,
     265, 268, 271, 276, 277, 280, 283, 286, 287, 290,
     292, 294, 297, 300, 303, 305, 308, 309, 312, 314,
     318, 322, 326, 329, 333, 337, 339, 341, 342
};
static const yytype_int8 yyrhs[] =
{
      37, 0, -1, 81, 38, -1, 38, -1, 63, 39,
      -1, 39, -1, -1, 39, 41, -1, 39, 55, -1,
      39, 67, -1, 39, 80, -1, 39, 26, 1, 31,
      -1, 39, 40, 1, 31, -1, 39, 1, 31, -1,
      16, -1, 18, -1, 19, -1, 21, -1, 17, -1,
      22, -1, 20, -1, 23, -1, 31, -1, 61, -1,
      71, -1, 44, -1, 46, -1, 69, -1, 26, 1,
      31, -1, 1, 31, -1, 10, 26, 31, -1, 43,
      47, -1, 11, 26, 31, -1, 45, 47, -1, -1,
      47, 48, -1, 47, 49, -1, 47, 75, -1, 47,
      73, -1, 47, 42, -1, 47, 31, -1, 19, 78,
      31, -1, 18, 79, 82, 31, -1, 20, 83, 82,
      31, -1, 21, 26, 82, 31, -1, 22, 84, 84,
      82, 31, -1, 24, 50, 31, -1, -1, 50, 26,
      51, -1, -1, 34, 79, -1, 7, 85, 31, -1,
      52, 56, -1, 80, -1, 53, 58, 54, -1, -1,
      56, 57, -1, 56, 75, -1, 56, 73, -1, 56,
      31, -1, 56, 42, -1, 18, 79, 82, 31, -1,
      19, 78, 31, -1, 17, 31, -1, 20, 26, 82,
      31, -1, -1, 58, 41, -1, 14, 83, 81, -1,
      80, -1, 59, 62, 60, -1, -1, 62, 41, -1,
      62, 67, -1, 62, 55, -1, 3, 79, 81, -1,
       4, 79, 31, -1, 64, 76, 74, -1, 80, -1,
      65, 68, 66, -1, -1, 68, 41, -1, 68, 67,
      -1, 68, 55, -1, 6, 79, 31, -1, 9, 79,
      31, -1, 70, 74, -1, 12, 31, -1, 72, 13,
      -1, -1, 74, 75, -1, 74, 31, -1, 74, 42,
      -1, 16, 25, 83, 31, -1, -1, 76, 77, -1,
      76, 31, -1, 23, 82, -1, -1, 79, 82, -1,
      26, -1, 27, -1, 5, 31, -1, 8, 31, -1,
      15, 31, -1, 31, -1, 81, 31, -1, -1, 14,
      83, -1, 84, -1, 84, 34, 84, -1, 84, 28,
      84, -1, 30, 83, 29, -1, 35, 83, -1, 83,
      32, 83, -1, 83, 33, 83, -1, 26, -1, 27,
      -1, -1, 26, -1
};
static const yytype_uint16 yyrline[] =
{
       0, 104, 104, 104, 106, 106, 108, 110, 111, 112,
     113, 114, 115, 119, 123, 123, 123, 123, 123, 123,
     123, 123, 127, 128, 129, 130, 131, 132, 136, 137,
     143, 151, 157, 165, 175, 177, 178, 179, 180, 181,
     182, 185, 193, 199, 209, 215, 221, 224, 226, 237,
     238, 243, 252, 257, 265, 268, 270, 271, 272, 273,
     274, 277, 283, 294, 300, 310, 312, 317, 325, 333,
     336, 338, 339, 340, 345, 352, 359, 364, 372, 375,
     377, 378, 379, 382, 390, 397, 404, 410, 417, 419,
     420, 421, 424, 432, 434, 435, 438, 445, 447, 452,
     453, 456, 457, 458, 462, 463, 466, 467, 470, 471,
     472, 473, 474, 475, 476, 479, 480, 483, 484
};
#endif
#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_MAINMENU", "T_MENU", "T_ENDMENU",
  "T_SOURCE", "T_CHOICE", "T_ENDCHOICE", "T_COMMENT", "T_CONFIG",
  "T_MENUCONFIG", "T_HELP", "T_HELPTEXT", "T_IF", "T_ENDIF", "T_DEPENDS",
  "T_OPTIONAL", "T_PROMPT", "T_TYPE", "T_DEFAULT", "T_SELECT", "T_RANGE",
  "T_VISIBLE", "T_OPTION", "T_ON", "T_WORD", "T_WORD_QUOTE", "T_UNEQUAL",
  "T_CLOSE_PAREN", "T_OPEN_PAREN", "T_EOL", "T_OR", "T_AND", "T_EQUAL",
  "T_NOT", "$accept", "input", "start", "stmt_list", "option_name",
  "common_stmt", "option_error", "config_entry_start", "config_stmt",
  "menuconfig_entry_start", "menuconfig_stmt", "config_option_list",
  "config_option", "symbol_option", "symbol_option_list",
  "symbol_option_arg", "choice", "choice_entry", "choice_end",
  "choice_stmt", "choice_option_list", "choice_option", "choice_block",
  "if_entry", "if_end", "if_stmt", "if_block", "mainmenu_stmt", "menu",
  "menu_entry", "menu_end", "menu_stmt", "menu_block", "source_stmt",
  "comment", "comment_stmt", "help_start", "help", "depends_list",
  "depends", "visibility_list", "visible", "prompt_stmt_opt", "prompt",
  "end", "nl", "if_expr", "expr", "symbol", "word_opt", 0
};
#endif
# ifdef YYPRINT
static const yytype_uint16 yytoknum[] =
{
       0, 256, 257, 258, 259, 260, 261, 262, 263, 264,
     265, 266, 267, 268, 269, 270, 271, 272, 273, 274,
     275, 276, 277, 278, 279, 280, 281, 282, 283, 284,
     285, 286, 287, 288, 289, 290
};
# endif
static const yytype_uint8 yyr1[] =
{
       0, 36, 37, 37, 38, 38, 39, 39, 39, 39,
      39, 39, 39, 39, 40, 40, 40, 40, 40, 40,
      40, 40, 41, 41, 41, 41, 41, 41, 42, 42,
      43, 44, 45, 46, 47, 47, 47, 47, 47, 47,
      47, 48, 48, 48, 48, 48, 49, 50, 50, 51,
      51, 52, 53, 54, 55, 56, 56, 56, 56, 56,
      56, 57, 57, 57, 57, 58, 58, 59, 60, 61,
      62, 62, 62, 62, 63, 64, 65, 66, 67, 68,
      68, 68, 68, 69, 70, 71, 72, 73, 74, 74,
      74, 74, 75, 76, 76, 76, 77, 78, 78, 79,
      79, 80, 80, 80, 81, 81, 82, 82, 83, 83,
      83, 83, 83, 83, 83, 84, 84, 85, 85
};
static const yytype_uint8 yyr2[] =
{
       0, 2, 2, 1, 2, 1, 0, 2, 2, 2,
       2, 4, 4, 3, 1, 1, 1, 1, 1, 1,
       1, 1, 1, 1, 1, 1, 1, 1, 3, 2,
       3, 2, 3, 2, 0, 2, 2, 2, 2, 2,
       2, 3, 4, 4, 4, 5, 3, 0, 3, 0,
       2, 3, 2, 1, 3, 0, 2, 2, 2, 2,
       2, 4, 3, 2, 4, 0, 2, 3, 1, 3,
       0, 2, 2, 2, 3, 3, 3, 1, 3, 0,
       2, 2, 2, 3, 3, 2, 2, 2, 0, 2,
       2, 2, 4, 0, 2, 2, 2, 0, 2, 1,
       1, 2, 2, 2, 1, 2, 0, 2, 1, 3,
       3, 3, 2, 3, 3, 1, 1, 0, 1
};
static const yytype_uint8 yydefact[] =
{
       6, 0, 104, 0, 3, 0, 6, 6, 99, 100,
       0, 1, 0, 0, 0, 0, 117, 0, 0, 0,
       0, 0, 0, 14, 18, 15, 16, 20, 17, 19,
      21, 0, 22, 0, 7, 34, 25, 34, 26, 55,
      65, 8, 70, 23, 93, 79, 9, 27, 88, 24,
      10, 0, 105, 2, 74, 13, 0, 101, 0, 118,
       0, 102, 0, 0, 0, 115, 116, 0, 0, 0,
     108, 103, 0, 0, 0, 0, 0, 0, 0, 88,
       0, 0, 75, 83, 51, 84, 30, 32, 0, 112,
       0, 0, 67, 0, 0, 11, 12, 0, 0, 0,
       0, 97, 0, 0, 0, 47, 0, 40, 39, 35,
      36, 0, 38, 37, 0, 0, 97, 0, 59, 60,
      56, 58, 57, 66, 54, 53, 71, 73, 69, 72,
      68, 106, 95, 0, 94, 80, 82, 78, 81, 77,
      90, 91, 89, 111, 113, 114, 110, 109, 29, 86,
       0, 106, 0, 106, 106, 106, 0, 0, 0, 87,
      63, 106, 0, 106, 0, 96, 0, 0, 41, 98,
       0, 0, 106, 49, 46, 28, 0, 62, 0, 107,
      92, 42, 43, 44, 0, 0, 48, 61, 64, 45,
      50
};
static const yytype_int16 yydefgoto[] =
{
      -1, 3, 4, 5, 33, 34, 108, 35, 36, 37,
      38, 74, 109, 110, 157, 186, 39, 40, 124, 41,
      76, 120, 77, 42, 128, 43, 78, 6, 44, 45,
     137, 46, 80, 47, 48, 49, 111, 112, 81, 113,
      79, 134, 152, 153, 50, 7, 165, 69, 70, 60
};
#define YYPACT_NINF -90
static const yytype_int16 yypact[] =
{
       4, 42, -90, 96, -90, 111, -90, 15, -90, -90,
      75, -90, 82, 42, 104, 42, 110, 107, 42, 115,
     125, -4, 121, -90, -90, -90, -90, -90, -90, -90,
     -90, 162, -90, 163, -90, -90, -90, -90, -90, -90,
     -90, -90, -90, -90, -90, -90, -90, -90, -90, -90,
     -90, 139, -90, -90, 138, -90, 142, -90, 143, -90,
     152, -90, 164, 167, 168, -90, -90, -4, -4, 77,
     -18, -90, 177, 185, 33, 71, 195, 247, 236, -2,
     236, 171, -90, -90, -90, -90, -90, -90, 41, -90,
      -4, -4, 138, 97, 97, -90, -90, 186, 187, 194,
      42, 42, -4, 196, 97, -90, 219, -90, -90, -90,
     -90, 210, -90, -90, 204, 42, 42, 199, -90, -90,
     -90, -90, -90, -90, -90, -90, -90, -90, -90, -90,
     -90, 222, -90, 223, -90, -90, -90, -90, -90, -90,
     -90, -90, -90, -90, 215, -90, -90, -90, -90, -90,
      -4, 222, 228, 222, -5, 222, 97, 35, 229, -90,
     -90, 222, 232, 222, -4, -90, 135, 233, -90, -90,
     234, 235, 222, 240, -90, -90, 237, -90, 239, -13,
     -90, -90, -90, -90, 244, 42, -90, -90, -90, -90,
     -90
};
static const yytype_int16 yypgoto[] =
{
     -90, -90, 269, 271, -90, 23, -70, -90, -90, -90,
     -90, 243, -90, -90, -90, -90, -90, -90, -90, -48,
     -90, -90, -90, -90, -90, -90, -90, -90, -90, -90,
     -90, -20, -90, -90, -90, -90, -90, 206, 205, -68,
     -90, -90, 169, -1, 27, -7, 118, -66, -89, -90
};
#define YYTABLE_NINF -86
static const yytype_int16 yytable[] =
{
      10, 88, 89, 54, 146, 147, 119, 1, 122, 164,
      93, 141, 56, 142, 58, 156, 94, 62, 1, 90,
      91, 131, 65, 66, 144, 145, 67, 90, 91, 132,
     127, 68, 136, -31, 97, 2, 154, -31, -31, -31,
     -31, -31, -31, -31, -31, 98, 52, -31, -31, 99,
     -31, 100, 101, 102, 103, 104, -31, 105, 129, 106,
     138, 173, 92, 141, 107, 142, 174, 172, 8, 9,
     143, -33, 97, 90, 91, -33, -33, -33, -33, -33,
     -33, -33, -33, 98, 166, -33, -33, 99, -33, 100,
     101, 102, 103, 104, -33, 105, 11, 106, 179, 151,
     123, 126, 107, 135, 125, 130, 2, 139, 2, 90,
      91, -5, 12, 55, 161, 13, 14, 15, 16, 17,
      18, 19, 20, 65, 66, 21, 22, 23, 24, 25,
      26, 27, 28, 29, 30, 57, 59, 31, 61, -4,
      12, 63, 32, 13, 14, 15, 16, 17, 18, 19,
      20, 64, 71, 21, 22, 23, 24, 25, 26, 27,
      28, 29, 30, 72, 73, 31, 180, 90, 91, 52,
      32, -85, 97, 82, 83, -85, -85, -85, -85, -85,
     -85, -85, -85, 84, 190, -85, -85, 99, -85, -85,
     -85, -85, -85, -85, -85, 85, 97, 106, 86, 87,
     -52, -52, 140, -52, -52, -52, -52, 98, 95, -52,
     -52, 99, 114, 115, 116, 117, 96, 148, 149, 150,
     158, 106, 155, 159, 97, 163, 118, -76, -76, -76,
     -76, -76, -76, -76, -76, 160, 164, -76, -76, 99,
      13, 14, 15, 16, 17, 18, 19, 20, 91, 106,
      21, 22, 14, 15, 140, 17, 18, 19, 20, 168,
     175, 21, 22, 177, 181, 182, 183, 32, 187, 167,
     188, 169, 170, 171, 185, 189, 53, 51, 32, 176,
      75, 178, 121, 0, 133, 162, 0, 0, 0, 0,
     184
};
static const yytype_int16 yycheck[] =
{
       1, 67, 68, 10, 93, 94, 76, 3, 76, 14,
      28, 81, 13, 81, 15, 104, 34, 18, 3, 32,
      33, 23, 26, 27, 90, 91, 30, 32, 33, 31,
      78, 35, 80, 0, 1, 31, 102, 4, 5, 6,
       7, 8, 9, 10, 11, 12, 31, 14, 15, 16,
      17, 18, 19, 20, 21, 22, 23, 24, 78, 26,
      80, 26, 69, 133, 31, 133, 31, 156, 26, 27,
      29, 0, 1, 32, 33, 4, 5, 6, 7, 8,
       9, 10, 11, 12, 150, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 24, 0, 26, 164, 100,
      77, 78, 31, 80, 77, 78, 31, 80, 31, 32,
      33, 0, 1, 31, 115, 4, 5, 6, 7, 8,
       9, 10, 11, 26, 27, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 31, 26, 26, 31, 0,
       1, 26, 31, 4, 5, 6, 7, 8, 9, 10,
      11, 26, 31, 14, 15, 16, 17, 18, 19, 20,
      21, 22, 23, 1, 1, 26, 31, 32, 33, 31,
      31, 0, 1, 31, 31, 4, 5, 6, 7, 8,
       9, 10, 11, 31, 185, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 31, 1, 26, 31, 31,
       5, 6, 31, 8, 9, 10, 11, 12, 31, 14,
      15, 16, 17, 18, 19, 20, 31, 31, 31, 25,
       1, 26, 26, 13, 1, 26, 31, 4, 5, 6,
       7, 8, 9, 10, 11, 31, 14, 14, 15, 16,
       4, 5, 6, 7, 8, 9, 10, 11, 33, 26,
      14, 15, 5, 6, 31, 8, 9, 10, 11, 31,
      31, 14, 15, 31, 31, 31, 31, 31, 31, 151,
      31, 153, 154, 155, 34, 31, 7, 6, 31, 161,
      37, 163, 76, -1, 79, 116, -1, -1, -1, -1,
     172
};
static const yytype_uint8 yystos[] =
{
       0, 3, 31, 37, 38, 39, 63, 81, 26, 27,
      79, 0, 1, 4, 5, 6, 7, 8, 9, 10,
      11, 14, 15, 16, 17, 18, 19, 20, 21, 22,
      23, 26, 31, 40, 41, 43, 44, 45, 46, 52,
      53, 55, 59, 61, 64, 65, 67, 69, 70, 71,
      80, 39, 31, 38, 81, 31, 79, 31, 79, 26,
      85, 31, 79, 26, 26, 26, 27, 30, 35, 83,
      84, 31, 1, 1, 47, 47, 56, 58, 62, 76,
      68, 74, 31, 31, 31, 31, 31, 31, 83, 83,
      32, 33, 81, 28, 34, 31, 31, 1, 12, 16,
      18, 19, 20, 21, 22, 24, 26, 31, 42, 48,
      49, 72, 73, 75, 17, 18, 19, 20, 31, 42,
      57, 73, 75, 41, 54, 80, 41, 55, 60, 67,
      80, 23, 31, 74, 77, 41, 55, 66, 67, 80,
      31, 42, 75, 29, 83, 83, 84, 84, 31, 31,
      25, 79, 78, 79, 83, 26, 84, 50, 1, 13,
      31, 79, 78, 26, 14, 82, 83, 82, 31, 82,
      82, 82, 84, 26, 31, 31, 82, 31, 82, 83,
      31, 31, 31, 31, 82, 34, 51, 31, 31, 31,
      79
};
#define yyerrok (yyerrstatus = 0)
#define yyclearin (yychar = YYEMPTY)
#define YYEMPTY (-2)
#define YYEOF 0
#define YYACCEPT goto yyacceptlab
#define YYABORT goto yyabortlab
#define YYERROR goto yyerrorlab
# 3789 "dialog.all.c"
#define YYFAIL goto yyerrlab
#if defined YYFAIL
#endif
#define YYRECOVERING() (!!yyerrstatus)
#define YYBACKUP(Token,Value) \
do \
  if (yychar == YYEMPTY && yylen == 1) \
    { \
      yychar = (Token); \
      yylval = (Value); \
      yytoken = YYTRANSLATE (yychar); \
      YYPOPSTACK (1); \
      goto yybackup; \
    } \
  else \
    { \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR; \
    } \
while (YYID (0))
#define YYTERROR 1
#define YYERRCODE 256
#define YYRHSLOC(Rhs,K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(Current,Rhs,N) \
    do \
      if (YYID (N)) \
 { \
   (Current).first_line = YYRHSLOC (Rhs, 1).first_line; \
   (Current).first_column = YYRHSLOC (Rhs, 1).first_column; \
   (Current).last_line = YYRHSLOC (Rhs, N).last_line; \
   (Current).last_column = YYRHSLOC (Rhs, N).last_column; \
 } \
      else \
 { \
   (Current).first_line = (Current).last_line = \
     YYRHSLOC (Rhs, 0).last_line; \
   (Current).first_column = (Current).last_column = \
     YYRHSLOC (Rhs, 0).last_column; \
 } \
    while (YYID (0))
#endif
#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#define YY_LOCATION_PRINT(File,Loc) \
     fprintf (File, "%d.%d-%d.%d", \
       (Loc).first_line, (Loc).first_column, \
       (Loc).last_line, (Loc).last_column)
# else
#define YY_LOCATION_PRINT(File,Loc) ((void) 0)
# endif
#endif
#ifdef YYLEX_PARAM
#define YYLEX yylex (YYLEX_PARAM)
#else
#define YYLEX yylex ()
#endif
#if YYDEBUG
# ifndef YYFPRINTF
# include <stdio.h>
#define YYFPRINTF fprintf
# endif
#define YYDPRINTF(Args) \
do { \
  if (yydebug) \
    YYFPRINTF Args; \
} while (YYID (0))
#define YY_SYMBOL_PRINT(Title,Type,Value,Location) \
do { \
  if (yydebug) \
    { \
      YYFPRINTF (stderr, "%s ", Title); \
      yy_symbol_print (stderr, \
    Type, Value); \
      YYFPRINTF (stderr, "\n"); \
    } \
} while (YYID (0))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
 break;
    }
}
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);
  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}
#define YY_STACK_PRINT(Bottom,Top) \
do { \
  if (yydebug) \
    yy_stack_print ((Bottom), (Top)); \
} while (YYID (0))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
      yyrule - 1, yylno);
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
         &(yyvsp[(yyi + 1) - (yynrhs)])
                  );
      YYFPRINTF (stderr, "\n");
    }
}
#define YY_REDUCE_PRINT(Rule) \
do { \
  if (yydebug) \
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))
int yydebug;
#else
#define YYDPRINTF(Args) 
#define YY_SYMBOL_PRINT(Title,Type,Value,Location) 
#define YY_STACK_PRINT(Bottom,Top) 
#define YY_REDUCE_PRINT(Rule) 
#endif
#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif
# 4047 "dialog.all.c"
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif
#if YYERROR_VERBOSE
# ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#define yystrlen strlen
# else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
# endif
# ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#define yystpcpy stpcpy
# else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;
  while ((*yyd++ = *yys++) != '\0')
    continue;
  return yyd - 1;
}
# endif
# endif
# ifndef yytnamerr
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
 switch (*++yyp)
   {
   case '\'':
   case ',':
     goto do_not_strip_quotes;
   case '\\':
     if (*++yyp != '\\')
       goto do_not_strip_quotes;
   default:
     if (yyres)
       yyres[yyn] = *yyp;
     yyn++;
     break;
   case '"':
     if (yyres)
       yyres[yyn] = '\0';
     return yyn;
   }
    do_not_strip_quotes: ;
    }
  if (! yyres)
    return yystrlen (yystr);
  return yystpcpy (yyres, yystr) - yyres;
}
# endif
# 4161 "dialog.all.c"
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];
  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;
# if 0
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
      + sizeof yyexpecting - 1
      + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
         * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;
      int yyxbegin = yyn < 0 ? -yyn : 0;
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;
      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
 if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
   {
     if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
       {
  yycount = 1;
  yysize = yysize0;
  yyformat[sizeof yyunexpected - 1] = '\0';
  break;
       }
     yyarg[yycount++] = yytname[yyx];
     yysize1 = yysize + yytnamerr (0, yytname[yyx]);
     yysize_overflow |= (yysize1 < yysize);
     yysize = yysize1;
     yyfmt = yystpcpy (yyfmt, yyprefix);
     yyprefix = yyor;
   }
      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;
      if (yysize_overflow)
 return YYSIZE_MAXIMUM;
      if (yyresult)
 {
   char *yyp = yyresult;
   int yyi = 0;
   while ((*yyp = *yyf) != '\0')
     {
       if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
  {
    yyp += yytnamerr (yyp, yyarg[yyi++]);
    yyf += 2;
  }
       else
  {
    yyp++;
    yyf++;
  }
     }
 }
      return yysize;
    }
}
#endif
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);
  switch (yytype)
    {
      case 53:
 {
 fprintf(stderr, "%s:%d: missing end statement for this entry\n",
  (yyvaluep->menu)->file->name, (yyvaluep->menu)->lineno);
 if (current_menu == (yyvaluep->menu))
  menu_end_menu();
};
 break;
      case 59:
 {
 fprintf(stderr, "%s:%d: missing end statement for this entry\n",
  (yyvaluep->menu)->file->name, (yyvaluep->menu)->lineno);
 if (current_menu == (yyvaluep->menu))
  menu_end_menu();
};
 break;
      case 65:
 {
 fprintf(stderr, "%s:%d: missing end statement for this entry\n",
  (yyvaluep->menu)->file->name, (yyvaluep->menu)->lineno);
 if (current_menu == (yyvaluep->menu))
  menu_end_menu();
};
 break;
      default:
 break;
    }
}
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif
int yychar;
YYSTYPE yylval;
int yynerrs;
#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()
#endif
#endif
{
    int yystate;
    int yyerrstatus;
# 4386 "dialog.all.c"
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;
    YYSIZE_T yystacksize;
  int yyn;
  int yyresult;
  int yytoken;
  YYSTYPE yyval;
#if YYERROR_VERBOSE
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif
#define YYPOPSTACK(N) (yyvsp -= (N), yyssp -= (N))
  int yylen = 0;
  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;
  YYDPRINTF ((stderr, "Starting parse\n"));
  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;
  yyssp = yyss;
  yyvsp = yyvs;
  goto yysetstate;
 yynewstate:
  yyssp++;
 yysetstate:
  *yyssp = yystate;
  if (yyss + yystacksize - 1 <= yyssp)
    {
      YYSIZE_T yysize = yyssp - yyss + 1;
#ifdef yyoverflow
      {
 YYSTYPE *yyvs1 = yyvs;
 yytype_int16 *yyss1 = yyss;
 yyoverflow (YY_("memory exhausted"),
      &yyss1, yysize * sizeof (*yyssp),
      &yyvs1, yysize * sizeof (*yyvsp),
      &yystacksize);
 yyss = yyss1;
 yyvs = yyvs1;
      }
#else
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      if (YYMAXDEPTH <= yystacksize)
 goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
 yystacksize = YYMAXDEPTH;
      {
 yytype_int16 *yyss1 = yyss;
 union yyalloc *yyptr =
   (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
 if (! yyptr)
   goto yyexhaustedlab;
 YYSTACK_RELOCATE (yyss_alloc, yyss);
 YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#undef YYSTACK_RELOCATE
 if (yyss1 != yyssa)
   YYSTACK_FREE (yyss1);
      }
# endif
#endif
      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
    (unsigned long int) yystacksize));
      if (yyss + yystacksize - 1 <= yyssp)
 YYABORT;
    }
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  if (yystate == YYFINAL)
    YYACCEPT;
  goto yybackup;
yybackup:
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }
  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
 goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  if (yyerrstatus)
    yyerrstatus--;
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yychar = YYEMPTY;
  yystate = yyn;
  *++yyvsp = yylval;
  goto yynewstate;
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;
yyreduce:
  yylen = yyr2[yyn];
# 4607 "dialog.all.c"
  yyval = yyvsp[1-yylen];
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 10:
    { zconf_error("unexpected end statement"); ;}
    break;
  case 11:
    { zconf_error("unknown statement \"%s\"", (yyvsp[(2) - (4)].string)); ;}
    break;
  case 12:
    {
 zconf_error("unexpected option \"%s\"", kconf_id_strings + (yyvsp[(2) - (4)].id)->name);
;}
    break;
  case 13:
    { zconf_error("invalid statement"); ;}
    break;
  case 28:
    { zconf_error("unknown option \"%s\"", (yyvsp[(1) - (3)].string)); ;}
    break;
  case 29:
    { zconf_error("invalid option"); ;}
    break;
  case 30:
    {
 struct symbol *sym = sym_lookup((yyvsp[(2) - (3)].string), 0);
 sym->flags |= SYMBOL_OPTIONAL;
 menu_add_entry(sym);
 printd(DEBUG_PARSE, "%s:%d:config %s\n", zconf_curname(), zconf_lineno(), (yyvsp[(2) - (3)].string));
;}
    break;
  case 31:
    {
 menu_end_entry();
 printd(DEBUG_PARSE, "%s:%d:endconfig\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 32:
    {
 struct symbol *sym = sym_lookup((yyvsp[(2) - (3)].string), 0);
 sym->flags |= SYMBOL_OPTIONAL;
 menu_add_entry(sym);
 printd(DEBUG_PARSE, "%s:%d:menuconfig %s\n", zconf_curname(), zconf_lineno(), (yyvsp[(2) - (3)].string));
;}
    break;
  case 33:
    {
 if (current_entry->prompt)
  current_entry->prompt->type = P_MENU;
 else
  zconfprint("warning: menuconfig statement without prompt");
 menu_end_entry();
 printd(DEBUG_PARSE, "%s:%d:endconfig\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 41:
    {
 menu_set_type((yyvsp[(1) - (3)].id)->stype);
 printd(DEBUG_PARSE, "%s:%d:type(%u)\n",
  zconf_curname(), zconf_lineno(),
  (yyvsp[(1) - (3)].id)->stype);
;}
    break;
  case 42:
    {
 menu_add_prompt(P_PROMPT, (yyvsp[(2) - (4)].string), (yyvsp[(3) - (4)].expr));
 printd(DEBUG_PARSE, "%s:%d:prompt\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 43:
    {
 menu_add_expr(P_DEFAULT, (yyvsp[(2) - (4)].expr), (yyvsp[(3) - (4)].expr));
 if ((yyvsp[(1) - (4)].id)->stype != S_UNKNOWN)
  menu_set_type((yyvsp[(1) - (4)].id)->stype);
 printd(DEBUG_PARSE, "%s:%d:default(%u)\n",
  zconf_curname(), zconf_lineno(),
  (yyvsp[(1) - (4)].id)->stype);
;}
    break;
  case 44:
    {
 menu_add_symbol(P_SELECT, sym_lookup((yyvsp[(2) - (4)].string), 0), (yyvsp[(3) - (4)].expr));
 printd(DEBUG_PARSE, "%s:%d:select\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 45:
    {
 menu_add_expr(P_RANGE, expr_alloc_comp(E_RANGE,(yyvsp[(2) - (5)].symbol), (yyvsp[(3) - (5)].symbol)), (yyvsp[(4) - (5)].expr));
 printd(DEBUG_PARSE, "%s:%d:range\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 48:
    {
 const struct kconf_id *id = kconf_id_lookup((yyvsp[(2) - (3)].string), strlen((yyvsp[(2) - (3)].string)));
 if (id && id->flags & TF_OPTION)
  menu_add_option(id->token, (yyvsp[(3) - (3)].string));
 else
  zconfprint("warning: ignoring unknown option %s", (yyvsp[(2) - (3)].string));
 free((yyvsp[(2) - (3)].string));
;}
    break;
  case 49:
    { (yyval.string) = NULL; ;}
    break;
  case 50:
    { (yyval.string) = (yyvsp[(2) - (2)].string); ;}
    break;
  case 51:
    {
 struct symbol *sym = sym_lookup((yyvsp[(2) - (3)].string), SYMBOL_CHOICE);
 sym->flags |= SYMBOL_AUTO;
 menu_add_entry(sym);
 menu_add_expr(P_CHOICE, NULL, NULL);
 printd(DEBUG_PARSE, "%s:%d:choice\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 52:
    {
 (yyval.menu) = menu_add_menu();
;}
    break;
  case 53:
    {
 if (zconf_endtoken((yyvsp[(1) - (1)].id), T_CHOICE, T_ENDCHOICE)) {
  menu_end_menu();
  printd(DEBUG_PARSE, "%s:%d:endchoice\n", zconf_curname(), zconf_lineno());
 }
;}
    break;
  case 61:
    {
 menu_add_prompt(P_PROMPT, (yyvsp[(2) - (4)].string), (yyvsp[(3) - (4)].expr));
 printd(DEBUG_PARSE, "%s:%d:prompt\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 62:
    {
 if ((yyvsp[(1) - (3)].id)->stype == S_BOOLEAN || (yyvsp[(1) - (3)].id)->stype == S_TRISTATE) {
  menu_set_type((yyvsp[(1) - (3)].id)->stype);
  printd(DEBUG_PARSE, "%s:%d:type(%u)\n",
   zconf_curname(), zconf_lineno(),
   (yyvsp[(1) - (3)].id)->stype);
 } else
  YYERROR;
;}
    break;
  case 63:
    {
 current_entry->sym->flags |= SYMBOL_OPTIONAL;
 printd(DEBUG_PARSE, "%s:%d:optional\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 64:
    {
 if ((yyvsp[(1) - (4)].id)->stype == S_UNKNOWN) {
  menu_add_symbol(P_DEFAULT, sym_lookup((yyvsp[(2) - (4)].string), 0), (yyvsp[(3) - (4)].expr));
  printd(DEBUG_PARSE, "%s:%d:default\n",
   zconf_curname(), zconf_lineno());
 } else
  YYERROR;
;}
    break;
  case 67:
    {
 printd(DEBUG_PARSE, "%s:%d:if\n", zconf_curname(), zconf_lineno());
 menu_add_entry(NULL);
 menu_add_dep((yyvsp[(2) - (3)].expr));
 (yyval.menu) = menu_add_menu();
;}
    break;
  case 68:
    {
 if (zconf_endtoken((yyvsp[(1) - (1)].id), T_IF, T_ENDIF)) {
  menu_end_menu();
  printd(DEBUG_PARSE, "%s:%d:endif\n", zconf_curname(), zconf_lineno());
 }
;}
    break;
  case 74:
    {
 menu_add_prompt(P_MENU, (yyvsp[(2) - (3)].string), NULL);
;}
    break;
  case 75:
    {
 menu_add_entry(NULL);
 menu_add_prompt(P_MENU, (yyvsp[(2) - (3)].string), NULL);
 printd(DEBUG_PARSE, "%s:%d:menu\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 76:
    {
 (yyval.menu) = menu_add_menu();
;}
    break;
  case 77:
    {
 if (zconf_endtoken((yyvsp[(1) - (1)].id), T_MENU, T_ENDMENU)) {
  menu_end_menu();
  printd(DEBUG_PARSE, "%s:%d:endmenu\n", zconf_curname(), zconf_lineno());
 }
;}
    break;
  case 83:
    {
 printd(DEBUG_PARSE, "%s:%d:source %s\n", zconf_curname(), zconf_lineno(), (yyvsp[(2) - (3)].string));
 zconf_nextfile((yyvsp[(2) - (3)].string));
;}
    break;
  case 84:
    {
 menu_add_entry(NULL);
 menu_add_prompt(P_COMMENT, (yyvsp[(2) - (3)].string), NULL);
 printd(DEBUG_PARSE, "%s:%d:comment\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 85:
    {
 menu_end_entry();
;}
    break;
  case 86:
    {
 printd(DEBUG_PARSE, "%s:%d:help\n", zconf_curname(), zconf_lineno());
 zconf_starthelp();
;}
    break;
  case 87:
    {
 current_entry->help = (yyvsp[(2) - (2)].string);
;}
    break;
  case 92:
    {
 menu_add_dep((yyvsp[(3) - (4)].expr));
 printd(DEBUG_PARSE, "%s:%d:depends on\n", zconf_curname(), zconf_lineno());
;}
    break;
  case 96:
    {
 menu_add_visibility((yyvsp[(2) - (2)].expr));
;}
    break;
  case 98:
    {
 menu_add_prompt(P_PROMPT, (yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].expr));
;}
    break;
  case 101:
    { (yyval.id) = (yyvsp[(1) - (2)].id); ;}
    break;
  case 102:
    { (yyval.id) = (yyvsp[(1) - (2)].id); ;}
    break;
  case 103:
    { (yyval.id) = (yyvsp[(1) - (2)].id); ;}
    break;
  case 106:
    { (yyval.expr) = NULL; ;}
    break;
  case 107:
    { (yyval.expr) = (yyvsp[(2) - (2)].expr); ;}
    break;
  case 108:
    { (yyval.expr) = expr_alloc_symbol((yyvsp[(1) - (1)].symbol)); ;}
    break;
  case 109:
    { (yyval.expr) = expr_alloc_comp(E_EQUAL, (yyvsp[(1) - (3)].symbol), (yyvsp[(3) - (3)].symbol)); ;}
    break;
  case 110:
    { (yyval.expr) = expr_alloc_comp(E_UNEQUAL, (yyvsp[(1) - (3)].symbol), (yyvsp[(3) - (3)].symbol)); ;}
    break;
  case 111:
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); ;}
    break;
  case 112:
    { (yyval.expr) = expr_alloc_one(E_NOT, (yyvsp[(2) - (2)].expr)); ;}
    break;
  case 113:
    { (yyval.expr) = expr_alloc_two(E_OR, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;
  case 114:
    { (yyval.expr) = expr_alloc_two(E_AND, (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)); ;}
    break;
  case 115:
    { (yyval.symbol) = sym_lookup((yyvsp[(1) - (1)].string), 0); free((yyvsp[(1) - (1)].string)); ;}
    break;
  case 116:
    { (yyval.symbol) = sym_lookup((yyvsp[(1) - (1)].string), SYMBOL_CONST); free((yyvsp[(1) - (1)].string)); ;}
    break;
  case 117:
    { (yyval.string) = NULL; ;}
    break;
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  *++yyvsp = yyval;
  yyn = yyr1[yyn];
  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];
  goto yynewstate;
yyerrlab:
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
 YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
 if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
   {
     YYSIZE_T yyalloc = 2 * yysize;
     if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
       yyalloc = YYSTACK_ALLOC_MAXIMUM;
     if (yymsg != yymsgbuf)
       YYSTACK_FREE (yymsg);
     yymsg = (char *) YYSTACK_ALLOC (yyalloc);
     if (yymsg)
       yymsg_alloc = yyalloc;
     else
       {
  yymsg = yymsgbuf;
  yymsg_alloc = sizeof yymsgbuf;
       }
   }
 if (0 < yysize && yysize <= yymsg_alloc)
   {
     (void) yysyntax_error (yymsg, yystate, yychar);
     yyerror (yymsg);
   }
 else
   {
     yyerror (YY_("syntax error"));
     if (yysize != 0)
       goto yyexhaustedlab;
   }
      }
#endif
    }
  if (yyerrstatus == 3)
    {
      if (yychar <= YYEOF)
 {
   if (yychar == YYEOF)
     YYABORT;
 }
      else
 {
   yydestruct ("Error: discarding",
        yytoken, &yylval);
   yychar = YYEMPTY;
 }
    }
  goto yyerrlab1;
yyerrorlab:
  if ( 0)
     goto yyerrorlab;
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;
yyerrlab1:
  yyerrstatus = 3;
  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
 {
   yyn += YYTERROR;
   if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
     {
       yyn = yytable[yyn];
       if (0 < yyn)
  break;
     }
 }
      if (yyssp == yyss)
 YYABORT;
      yydestruct ("Error: popping",
    yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }
  *++yyvsp = yylval;
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);
  yystate = yyn;
  goto yynewstate;
yyacceptlab:
  yyresult = 0;
  goto yyreturn;
yyabortlab:
  yyresult = 1;
  goto yyreturn;
#if !defined(yyoverflow) || YYERROR_VERBOSE
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
#endif
yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
   yytoken, &yylval);
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
    yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return YYID (yyresult);
}
void conf_parse(const char *name)
{
 struct symbol *sym;
 int i;
 zconf_initscan(name);
 sym_init();
 _menu_init();
 modules_sym = sym_lookup(NULL, 0);
 modules_sym->type = S_BOOLEAN;
 modules_sym->flags |= SYMBOL_AUTO;
 rootmenu.prompt = menu_add_prompt(P_MENU, "Linux Kernel Configuration", NULL);
 if (getenv("ZCONF_DEBUG"))
  zconfdebug = 1;
 zconfparse();
 if (zconfnerrs)
  exit(1);
 if (!modules_sym->prop) {
  struct property *prop;
  prop = prop_alloc(P_DEFAULT, modules_sym);
  prop->expr = expr_alloc_symbol(sym_lookup("MODULES", 0));
 }
 rootmenu.prompt->text = _(rootmenu.prompt->text);
 rootmenu.prompt->text = sym_expand_string_value(rootmenu.prompt->text);
 menu_finalize(&rootmenu);
 for_all_symbols(i, sym) {
  if (sym_check_deps(sym))
   zconfnerrs++;
        }
 if (zconfnerrs)
  exit(1);
 sym_set_change_count(1);
}
static const char *zconf_tokenname(int token)
{
 switch (token) {
 case T_MENU: return "menu";
 case T_ENDMENU: return "endmenu";
 case T_CHOICE: return "choice";
 case T_ENDCHOICE: return "endchoice";
 case T_IF: return "if";
 case T_ENDIF: return "endif";
 case T_DEPENDS: return "depends";
 case T_VISIBLE: return "visible";
 }
 return "<token>";
}
static bool zconf_endtoken(const struct kconf_id *id, int starttoken, int endtoken)
{
 if (id->token != endtoken) {
  zconf_error("unexpected '%s' within %s block",
   kconf_id_strings + id->name, zconf_tokenname(starttoken));
  zconfnerrs++;
  return false;
 }
 if (current_menu->file != current_file) {
  zconf_error("'%s' in different file than '%s'",
   kconf_id_strings + id->name, zconf_tokenname(starttoken));
  fprintf(stderr, "%s:%d: location of the '%s'\n",
   current_menu->file->name, current_menu->lineno,
   zconf_tokenname(starttoken));
  zconfnerrs++;
  return false;
 }
 return true;
}
static void zconfprint(const char *err, ...)
{
 va_list ap;
 fprintf(stderr, "%s:%d: ", zconf_curname(), zconf_lineno());
 va_start(ap, err);
 vfprintf(stderr, err, ap);
 va_end(ap);
 fprintf(stderr, "\n");
}
static void zconf_error(const char *err, ...)
{
 va_list ap;
 zconfnerrs++;
 fprintf(stderr, "%s:%d: ", zconf_curname(), zconf_lineno());
 va_start(ap, err);
 vfprintf(stderr, err, ap);
 va_end(ap);
 fprintf(stderr, "\n");
}
static void zconferror(const char *err)
{
 fprintf(stderr, "%s:%d: %s\n", zconf_curname(), zconf_lineno() + 1, err);
}
static void print_quoted_string(FILE *out, const char *str)
{
 const char *p;
 int len;
 putc('"', out);
 while ((p = strchr(str, '"'))) {
  len = p - str;
  if (len)
   fprintf(out, "%.*s", len, str);
  fputs("\\\"", out);
  str = p + 1;
 }
 fputs(str, out);
 putc('"', out);
}
static void print_symbol(FILE *out, struct menu *menu)
{
 struct symbol *sym = menu->sym;
 struct property *prop;
 if (sym_is_choice(sym))
  fprintf(out, "\nchoice\n");
 else
  fprintf(out, "\nconfig %s\n", sym->name);
 switch (sym->type) {
 case S_BOOLEAN:
  fputs("  boolean\n", out);
  break;
 case S_TRISTATE:
  fputs("  tristate\n", out);
  break;
 case S_STRING:
  fputs("  string\n", out);
  break;
 case S_INT:
  fputs("  integer\n", out);
  break;
 case S_HEX:
  fputs("  hex\n", out);
  break;
 default:
  fputs("  ???\n", out);
  break;
 }
 for (prop = sym->prop; prop; prop = prop->next) {
  if (prop->menu != menu)
   continue;
  switch (prop->type) {
  case P_PROMPT:
   fputs("  prompt ", out);
   print_quoted_string(out, prop->text);
   if (!expr_is_yes(prop->visible.expr)) {
    fputs(" if ", out);
    expr_fprint(prop->visible.expr, out);
   }
   fputc('\n', out);
   break;
  case P_DEFAULT:
   fputs( "  default ", out);
   expr_fprint(prop->expr, out);
   if (!expr_is_yes(prop->visible.expr)) {
    fputs(" if ", out);
    expr_fprint(prop->visible.expr, out);
   }
   fputc('\n', out);
   break;
  case P_CHOICE:
   fputs("  #choice value\n", out);
   break;
  case P_SELECT:
   fputs( "  select ", out);
   expr_fprint(prop->expr, out);
   fputc('\n', out);
   break;
  case P_RANGE:
   fputs( "  range ", out);
   expr_fprint(prop->expr, out);
   fputc('\n', out);
   break;
  case P_MENU:
   fputs( "  menu ", out);
   print_quoted_string(out, prop->text);
   fputc('\n', out);
   break;
  default:
   fprintf(out, "  unknown prop %d!\n", prop->type);
   break;
  }
 }
 if (menu->help) {
  int len = strlen(menu->help);
  while (menu->help[--len] == '\n')
   menu->help[len] = 0;
  fprintf(out, "  help\n%s\n", menu->help);
 }
}
void zconfdump(FILE *out)
{
 struct property *prop;
 struct symbol *sym;
 struct menu *menu;
 menu = rootmenu.list;
 while (menu) {
  if ((sym = menu->sym))
   print_symbol(out, menu);
  else if ((prop = menu->prompt)) {
   switch (prop->type) {
   case P_COMMENT:
    fputs("\ncomment ", out);
    print_quoted_string(out, prop->text);
    fputs("\n", out);
    break;
   case P_MENU:
    fputs("\nmenu ", out);
    print_quoted_string(out, prop->text);
    fputs("\n", out);
    break;
   default:
    ;
   }
   if (!expr_is_yes(prop->visible.expr)) {
    fputs("  depends ", out);
    expr_fprint(prop->visible.expr, out);
    fputc('\n', out);
   }
  }
  if (menu->list)
   menu = menu->list;
  else if (menu->next)
   menu = menu->next;
  else while ((menu = menu->parent)) {
   if (menu->prompt && menu->prompt->type == P_MENU)
    fputs("\nendmenu\n", out);
   if (menu->next) {
    menu = menu->next;
    break;
   }
  }
 }
}
#line 3 "scripts/kconfig/zconf.lex.c_shipped"
#define YY_INT_ALIGNED short int
#define yy_create_buffer zconf_create_buffer
#define yy_delete_buffer zconf_delete_buffer
#define yy_flex_debug zconf_flex_debug
#define yy_init_buffer zconf_init_buffer
#define yy_flush_buffer zconf_flush_buffer
#define yy_load_buffer_state zconf_load_buffer_state
#define yy_switch_to_buffer zconf_switch_to_buffer
#define yyin zconfin
#define yyleng zconfleng
#define yylex zconflex
#define yylineno zconflineno
#define yyout zconfout
#define yyrestart zconfrestart
#define yytext zconftext
#define yywrap zconfwrap
#define yyalloc zconfalloc
#define yyrealloc zconfrealloc
#define yyfree zconffree
#define FLEX_SCANNER 
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 5
#define YY_FLEX_SUBMINOR_VERSION 35
#if YY_FLEX_SUBMINOR_VERSION > 0
#define FLEX_BETA 
#endif
#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
#else
typedef signed char flex_int8_t;
typedef short int flex_int16_t;
typedef int flex_int32_t;
typedef unsigned char flex_uint8_t;
typedef unsigned short int flex_uint16_t;
typedef unsigned int flex_uint32_t;
#endif
#ifndef INT8_MIN
#define INT8_MIN (-128)
#endif
#ifndef INT16_MIN
#define INT16_MIN (-32767-1)
#endif
#ifndef INT32_MIN
#define INT32_MIN (-2147483647-1)
#endif
#ifndef INT8_MAX
#define INT8_MAX (127)
#endif
#ifndef INT16_MAX
#define INT16_MAX (32767)
#endif
#ifndef INT32_MAX
#define INT32_MAX (2147483647)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX (255U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX (65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX (4294967295U)
#endif
#define YY_USE_CONST 
#ifdef YY_USE_CONST
#define yyconst const
#else
#define yyconst 
#endif
#define YY_NULL 0
#define YY_SC_TO_UI(c) ((unsigned int) (unsigned char) c)
#define BEGIN (yy_start) = 1 + 2 *
#define YY_START (((yy_start) - 1) / 2)
#define YYSTATE YY_START
#define YY_STATE_EOF(state) (YY_END_OF_BUFFER + state + 1)
#define YY_NEW_FILE zconfrestart(zconfin )
#define YY_END_OF_BUFFER_CHAR 0
#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE 16384
#endif
#define YY_STATE_BUF_SIZE ((YY_BUF_SIZE + 2) * sizeof(yy_state_type))
#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE 
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif
#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2
#define YY_LESS_LINENO(n) 
#define yyless(n) \
 do \
  { \
                                              \
        int yyless_macro_arg = (n); \
        YY_LESS_LINENO(yyless_macro_arg);\
  *yy_cp = (yy_hold_char); \
  YY_RESTORE_YY_MORE_OFFSET \
  (yy_c_buf_p) = yy_cp = yy_bp + yyless_macro_arg - YY_MORE_ADJ; \
  YY_DO_BEFORE_ACTION; \
  } \
 while ( 0 )
#define unput(c) yyunput( c, (yytext_ptr) )
#ifndef YY_READ_BUF_SIZE
#define YY_READ_BUF_SIZE 8192
#endif
#ifndef ECHO
#define ECHO do { if (fwrite( zconftext, zconfleng, 1, zconfout )) {} } while (0)
#endif
#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
    errno=0; \
    while ( (result = read( fileno(zconfin), (char *) buf, max_size )) < 0 ) { \
        if( errno != EINTR) { YY_FATAL_ERROR( "input in flex scanner failed" ); break; } \
        errno=0; clearerr(zconfin); \
    }
#endif
#ifndef yyterminate
#define yyterminate() return YY_NULL
#endif
#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif
#ifndef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg) yy_fatal_error( msg )
#endif
#ifndef YY_DECL
#define YY_DECL_IS_OURS 1
#define YY_DECL int zconflex (void)
#endif
#ifndef YY_USER_ACTION
#define YY_USER_ACTION 
#endif
#ifndef YY_BREAK
#define YY_BREAK break;
#endif
#define YY_RULE_SETUP YY_USER_ACTION
#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T 
typedef size_t yy_size_t;
#endif
#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE 
struct yy_buffer_state
 {
 FILE *yy_input_file;
 char *yy_ch_buf;
 char *yy_buf_pos;
 yy_size_t yy_buf_size;
 int yy_n_chars;
 int yy_is_our_buffer;
 int yy_is_interactive;
 int yy_at_bol;
    int yy_bs_lineno;
    int yy_bs_column;
 int yy_fill_buffer;
 int yy_buffer_status;
#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
# 5742 "dialog.all.c"
#define YY_BUFFER_EOF_PENDING 2
 };
#endif
static size_t yy_buffer_stack_top = 0;
static size_t yy_buffer_stack_max = 0;
static YY_BUFFER_STATE * yy_buffer_stack = 0;
#define YY_CURRENT_BUFFER ( (yy_buffer_stack) \
                          ? (yy_buffer_stack)[(yy_buffer_stack_top)] \
                          : NULL)
#define YY_CURRENT_BUFFER_LVALUE (yy_buffer_stack)[(yy_buffer_stack_top)]
static char yy_hold_char;
static int yy_n_chars;
static char *yy_c_buf_p = (char *) 0;
static int yy_init = 0;
static int yy_start = 0;
static int yy_did_buffer_switch_on_eof;
void zconfrestart (FILE *input_file );
void zconf_switch_to_buffer (YY_BUFFER_STATE new_buffer );
YY_BUFFER_STATE zconf_create_buffer (FILE *file,int size );
void zconf_delete_buffer (YY_BUFFER_STATE b );
void zconf_flush_buffer (YY_BUFFER_STATE b );
void zconfpush_buffer_state (YY_BUFFER_STATE new_buffer );
void zconfpop_buffer_state (void );
static void zconfensure_buffer_stack (void );
static void zconf_load_buffer_state (void );
static void zconf_init_buffer (YY_BUFFER_STATE b,FILE *file );
#define YY_FLUSH_BUFFER zconf_flush_buffer(YY_CURRENT_BUFFER )
YY_BUFFER_STATE zconf_scan_buffer (char *base,yy_size_t size );
YY_BUFFER_STATE zconf_scan_string (yyconst char *yy_str );
YY_BUFFER_STATE zconf_scan_bytes (yyconst char *bytes,int len );
void *zconfalloc (yy_size_t );
void *zconfrealloc (void *,yy_size_t );
void zconffree (void * );
#define yy_new_buffer zconf_create_buffer
#define yy_set_interactive(is_interactive) \
 { \
 if ( ! YY_CURRENT_BUFFER ){ \
        zconfensure_buffer_stack (); \
  YY_CURRENT_BUFFER_LVALUE = \
            zconf_create_buffer(zconfin,YY_BUF_SIZE ); \
 } \
 YY_CURRENT_BUFFER_LVALUE->yy_is_interactive = is_interactive; \
 }
#define yy_set_bol(at_bol) \
 { \
 if ( ! YY_CURRENT_BUFFER ){\
        zconfensure_buffer_stack (); \
  YY_CURRENT_BUFFER_LVALUE = \
            zconf_create_buffer(zconfin,YY_BUF_SIZE ); \
 } \
 YY_CURRENT_BUFFER_LVALUE->yy_at_bol = at_bol; \
 }
#define YY_AT_BOL() (YY_CURRENT_BUFFER_LVALUE->yy_at_bol)
#define zconfwrap(n) 1
#define YY_SKIP_YYWRAP 
typedef unsigned char YY_CHAR;
typedef int yy_state_type;
#define yytext_ptr zconftext
static yyconst flex_int16_t yy_nxt[][17] =
    {
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0
    },
    {
       11, 12, 13, 14, 12, 12, 15, 12, 12, 12,
       12, 12, 12, 12, 12, 12, 12
    },
    {
       11, 12, 13, 14, 12, 12, 15, 12, 12, 12,
       12, 12, 12, 12, 12, 12, 12
    },
    {
       11, 16, 16, 17, 16, 16, 16, 16, 16, 16,
       16, 16, 16, 18, 16, 16, 16
    },
    {
       11, 16, 16, 17, 16, 16, 16, 16, 16, 16,
       16, 16, 16, 18, 16, 16, 16
    },
    {
       11, 19, 20, 21, 19, 19, 19, 19, 19, 19,
       19, 19, 19, 19, 19, 19, 19
    },
    {
       11, 19, 20, 21, 19, 19, 19, 19, 19, 19,
       19, 19, 19, 19, 19, 19, 19
    },
    {
       11, 22, 22, 23, 22, 24, 22, 22, 24, 22,
       22, 22, 22, 22, 22, 25, 22
    },
    {
       11, 22, 22, 23, 22, 24, 22, 22, 24, 22,
       22, 22, 22, 22, 22, 25, 22
    },
    {
       11, 26, 26, 27, 28, 29, 30, 31, 29, 32,
       33, 34, 35, 35, 36, 37, 38
    },
    {
       11, 26, 26, 27, 28, 29, 30, 31, 29, 32,
       33, 34, 35, 35, 36, 37, 38
    },
    {
      -11, -11, -11, -11, -11, -11, -11, -11, -11, -11,
      -11, -11, -11, -11, -11, -11, -11
    },
    {
       11, -12, -12, -12, -12, -12, -12, -12, -12, -12,
      -12, -12, -12, -12, -12, -12, -12
    },
    {
       11, -13, 39, 40, -13, -13, 41, -13, -13, -13,
      -13, -13, -13, -13, -13, -13, -13
    },
    {
       11, -14, -14, -14, -14, -14, -14, -14, -14, -14,
      -14, -14, -14, -14, -14, -14, -14
    },
    {
       11, 42, 42, 43, 42, 42, 42, 42, 42, 42,
       42, 42, 42, 42, 42, 42, 42
    },
    {
       11, -16, -16, -16, -16, -16, -16, -16, -16, -16,
      -16, -16, -16, -16, -16, -16, -16
    },
    {
       11, -17, -17, -17, -17, -17, -17, -17, -17, -17,
      -17, -17, -17, -17, -17, -17, -17
    },
    {
       11, -18, -18, -18, -18, -18, -18, -18, -18, -18,
      -18, -18, -18, 44, -18, -18, -18
    },
    {
       11, 45, 45, -19, 45, 45, 45, 45, 45, 45,
       45, 45, 45, 45, 45, 45, 45
    },
    {
       11, -20, 46, 47, -20, -20, -20, -20, -20, -20,
      -20, -20, -20, -20, -20, -20, -20
    },
    {
       11, 48, -21, -21, 48, 48, 48, 48, 48, 48,
       48, 48, 48, 48, 48, 48, 48
    },
    {
       11, 49, 49, 50, 49, -22, 49, 49, -22, 49,
       49, 49, 49, 49, 49, -22, 49
    },
    {
       11, -23, -23, -23, -23, -23, -23, -23, -23, -23,
      -23, -23, -23, -23, -23, -23, -23
    },
    {
       11, -24, -24, -24, -24, -24, -24, -24, -24, -24,
      -24, -24, -24, -24, -24, -24, -24
    },
    {
       11, 51, 51, 52, 51, 51, 51, 51, 51, 51,
       51, 51, 51, 51, 51, 51, 51
    },
    {
       11, -26, -26, -26, -26, -26, -26, -26, -26, -26,
      -26, -26, -26, -26, -26, -26, -26
    },
    {
       11, -27, -27, -27, -27, -27, -27, -27, -27, -27,
      -27, -27, -27, -27, -27, -27, -27
    },
    {
       11, -28, -28, -28, -28, -28, -28, -28, -28, -28,
      -28, -28, -28, -28, 53, -28, -28
    },
    {
       11, -29, -29, -29, -29, -29, -29, -29, -29, -29,
      -29, -29, -29, -29, -29, -29, -29
    },
    {
       11, 54, 54, -30, 54, 54, 54, 54, 54, 54,
       54, 54, 54, 54, 54, 54, 54
    },
    {
       11, -31, -31, -31, -31, -31, -31, 55, -31, -31,
      -31, -31, -31, -31, -31, -31, -31
    },
    {
       11, -32, -32, -32, -32, -32, -32, -32, -32, -32,
      -32, -32, -32, -32, -32, -32, -32
    },
    {
       11, -33, -33, -33, -33, -33, -33, -33, -33, -33,
      -33, -33, -33, -33, -33, -33, -33
    },
    {
       11, -34, -34, -34, -34, -34, -34, -34, -34, -34,
      -34, 56, 57, 57, -34, -34, -34
    },
    {
       11, -35, -35, -35, -35, -35, -35, -35, -35, -35,
      -35, 57, 57, 57, -35, -35, -35
    },
    {
       11, -36, -36, -36, -36, -36, -36, -36, -36, -36,
      -36, -36, -36, -36, -36, -36, -36
    },
    {
       11, -37, -37, 58, -37, -37, -37, -37, -37, -37,
      -37, -37, -37, -37, -37, -37, -37
    },
    {
       11, -38, -38, -38, -38, -38, -38, -38, -38, -38,
      -38, -38, -38, -38, -38, -38, 59
    },
    {
       11, -39, 39, 40, -39, -39, 41, -39, -39, -39,
      -39, -39, -39, -39, -39, -39, -39
    },
    {
       11, -40, -40, -40, -40, -40, -40, -40, -40, -40,
      -40, -40, -40, -40, -40, -40, -40
    },
    {
       11, 42, 42, 43, 42, 42, 42, 42, 42, 42,
       42, 42, 42, 42, 42, 42, 42
    },
    {
       11, 42, 42, 43, 42, 42, 42, 42, 42, 42,
       42, 42, 42, 42, 42, 42, 42
    },
    {
       11, -43, -43, -43, -43, -43, -43, -43, -43, -43,
      -43, -43, -43, -43, -43, -43, -43
    },
    {
       11, -44, -44, -44, -44, -44, -44, -44, -44, -44,
      -44, -44, -44, 44, -44, -44, -44
    },
    {
       11, 45, 45, -45, 45, 45, 45, 45, 45, 45,
       45, 45, 45, 45, 45, 45, 45
    },
    {
       11, -46, 46, 47, -46, -46, -46, -46, -46, -46,
      -46, -46, -46, -46, -46, -46, -46
    },
    {
       11, 48, -47, -47, 48, 48, 48, 48, 48, 48,
       48, 48, 48, 48, 48, 48, 48
    },
    {
       11, -48, -48, -48, -48, -48, -48, -48, -48, -48,
      -48, -48, -48, -48, -48, -48, -48
    },
    {
       11, 49, 49, 50, 49, -49, 49, 49, -49, 49,
       49, 49, 49, 49, 49, -49, 49
    },
    {
       11, -50, -50, -50, -50, -50, -50, -50, -50, -50,
      -50, -50, -50, -50, -50, -50, -50
    },
    {
       11, -51, -51, 52, -51, -51, -51, -51, -51, -51,
      -51, -51, -51, -51, -51, -51, -51
    },
    {
       11, -52, -52, -52, -52, -52, -52, -52, -52, -52,
      -52, -52, -52, -52, -52, -52, -52
    },
    {
       11, -53, -53, -53, -53, -53, -53, -53, -53, -53,
      -53, -53, -53, -53, -53, -53, -53
    },
    {
       11, 54, 54, -54, 54, 54, 54, 54, 54, 54,
       54, 54, 54, 54, 54, 54, 54
    },
    {
       11, -55, -55, -55, -55, -55, -55, -55, -55, -55,
      -55, -55, -55, -55, -55, -55, -55
    },
    {
       11, -56, -56, -56, -56, -56, -56, -56, -56, -56,
      -56, 60, 57, 57, -56, -56, -56
    },
    {
       11, -57, -57, -57, -57, -57, -57, -57, -57, -57,
      -57, 57, 57, 57, -57, -57, -57
    },
    {
       11, -58, -58, -58, -58, -58, -58, -58, -58, -58,
      -58, -58, -58, -58, -58, -58, -58
    },
    {
       11, -59, -59, -59, -59, -59, -59, -59, -59, -59,
      -59, -59, -59, -59, -59, -59, -59
    },
    {
       11, -60, -60, -60, -60, -60, -60, -60, -60, -60,
      -60, 57, 57, 57, -60, -60, -60
    },
    } ;
static yy_state_type yy_get_previous_state (void );
static yy_state_type yy_try_NUL_trans (yy_state_type current_state );
static int yy_get_next_buffer (void );
static void yy_fatal_error (yyconst char msg[] );
#define YY_DO_BEFORE_ACTION \
 (yytext_ptr) = yy_bp; \
 zconfleng = (size_t) (yy_cp - yy_bp); \
 (yy_hold_char) = *yy_cp; \
 *yy_cp = '\0'; \
 (yy_c_buf_p) = yy_cp;
#define YY_NUM_RULES 33
#define YY_END_OF_BUFFER 34
struct yy_trans_info
 {
 flex_int32_t yy_verify;
 flex_int32_t yy_nxt;
 };
static yyconst flex_int16_t yy_accept[61] =
    { 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       34, 5, 4, 2, 3, 7, 8, 6, 32, 29,
       31, 24, 28, 27, 26, 22, 17, 13, 16, 20,
       22, 11, 12, 19, 19, 14, 22, 22, 4, 2,
        3, 3, 1, 6, 32, 29, 31, 30, 24, 23,
       26, 25, 15, 20, 9, 19, 19, 21, 10, 18
    } ;
static yyconst flex_int32_t yy_ec[256] =
    { 0,
        1, 1, 1, 1, 1, 1, 1, 1, 2, 3,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 2, 4, 5, 6, 1, 1, 7, 8, 9,
       10, 1, 1, 1, 11, 12, 12, 13, 13, 13,
       13, 13, 13, 13, 13, 13, 13, 1, 1, 1,
       14, 1, 1, 1, 13, 13, 13, 13, 13, 13,
       13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
       13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
        1, 15, 1, 1, 13, 1, 13, 13, 13, 13,
       13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
       13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
       13, 13, 1, 16, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1
    } ;
int zconf_flex_debug = 0;
#define REJECT reject_used_but_not_detected
#define yymore() yymore_used_but_not_detected
#define YY_MORE_ADJ 0
#define YY_RESTORE_YY_MORE_OFFSET 
char *zconftext;
#define YY_NO_INPUT 1
#define START_STRSIZE 16
static struct {
 struct file *file;
 int lineno;
} current_pos;
static char *text;
static int text_size, text_asize;
struct buffer {
        struct buffer *parent;
        YY_BUFFER_STATE state;
};
struct buffer *current_buf;
static int last_ts, first_ts;
static void zconf_endhelp(void);
static void zconf_endfile(void);
static void new_string(void)
{
 text = malloc(START_STRSIZE);
 text_asize = START_STRSIZE;
 text_size = 0;
 *text = 0;
}
static void append_string(const char *str, int size)
{
 int new_size = text_size + size + 1;
 if (new_size > text_asize) {
  new_size += START_STRSIZE - 1;
  new_size &= -START_STRSIZE;
  text = realloc(text, new_size);
  text_asize = new_size;
 }
 memcpy(text + text_size, str, size);
 text_size += size;
 text[text_size] = 0;
}
static void alloc_string(const char *str, int size)
{
 text = malloc(size + 1);
 memcpy(text, str, size);
 text[size] = 0;
}
#define INITIAL 0
#define COMMAND 1
#define HELP 2
#define STRING 3
#define PARAM 4
#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE void *
#endif
YY_EXTRA_TYPE zconfget_extra (void );
void zconfset_extra (YY_EXTRA_TYPE user_defined );
FILE *zconfget_in (void );
void zconfset_in (FILE * in_str );
FILE *zconfget_out (void );
void zconfset_out (FILE * out_str );
int zconfget_leng (void );
char *zconfget_text (void );
int zconfget_lineno (void );
void zconfset_lineno (int line_number );
#ifndef YY_SKIP_YYWRAP
 int zconfwrap (void );
#endif
static void yyunput (int c,char *buf_ptr );
#ifndef yytext_ptr
static void yy_flex_strncpy (char *,yyconst char *,int );
#endif
#ifdef YY_NEED_STRLEN
static int yy_flex_strlen (yyconst char * );
#endif
#ifndef YY_NO_INPUT
# ifdef __cplusplus
static int yyinput (void );
# else
static int input (void );
# endif
#endif
YY_DECL {
 register yy_state_type yy_current_state;
 register char *yy_cp, *yy_bp;
 register int yy_act;
 int str = 0;
 int ts, i;
 if ( !(yy_init) )
  {
  (yy_init) = 1;
#ifdef YY_USER_INIT
  YY_USER_INIT;
#endif
  if ( ! (yy_start) )
   (yy_start) = 1;
  if ( ! zconfin )
   zconfin = stdin;
  if ( ! zconfout )
   zconfout = stdout;
  if ( ! YY_CURRENT_BUFFER ) {
   zconfensure_buffer_stack ();
   YY_CURRENT_BUFFER_LVALUE =
    zconf_create_buffer(zconfin,YY_BUF_SIZE );
  }
  zconf_load_buffer_state( );
  }
 while ( 1 )
  {
  yy_cp = (yy_c_buf_p);
  *yy_cp = (yy_hold_char);
  yy_bp = yy_cp;
  yy_current_state = (yy_start);
yy_match:
  while ( (yy_current_state = yy_nxt[yy_current_state][ yy_ec[YY_SC_TO_UI(*yy_cp)] ]) > 0 )
   ++yy_cp;
  yy_current_state = -yy_current_state;
yy_find_action:
  yy_act = yy_accept[yy_current_state];
  YY_DO_BEFORE_ACTION;
do_action:
  switch ( yy_act )
 {
case 1:
case 2:
YY_RULE_SETUP
{
 current_file->lineno++;
 return T_EOL;
}
 YY_BREAK
case 3:
YY_RULE_SETUP
 YY_BREAK
case 4:
YY_RULE_SETUP
{
 BEGIN(COMMAND);
}
 YY_BREAK
case 5:
YY_RULE_SETUP
{
 unput(zconftext[0]);
 BEGIN(COMMAND);
}
 YY_BREAK
case 6:
YY_RULE_SETUP
{
  const struct kconf_id *id = kconf_id_lookup(zconftext, zconfleng);
  BEGIN(PARAM);
  current_pos.file = current_file;
  current_pos.lineno = current_file->lineno;
  if (id && id->flags & TF_COMMAND) {
   zconflval.id = id;
   return id->token;
  }
  alloc_string(zconftext, zconfleng);
  zconflval.string = text;
  return T_WORD;
 }
 YY_BREAK
case 7:
YY_RULE_SETUP
 YY_BREAK
case 8:
YY_RULE_SETUP
{
  BEGIN(INITIAL);
  current_file->lineno++;
  return T_EOL;
 }
 YY_BREAK
case 9:
YY_RULE_SETUP
return T_AND;
 YY_BREAK
case 10:
YY_RULE_SETUP
return T_OR;
 YY_BREAK
case 11:
YY_RULE_SETUP
return T_OPEN_PAREN;
 YY_BREAK
case 12:
YY_RULE_SETUP
return T_CLOSE_PAREN;
 YY_BREAK
case 13:
YY_RULE_SETUP
return T_NOT;
 YY_BREAK
case 14:
YY_RULE_SETUP
return T_EQUAL;
 YY_BREAK
case 15:
YY_RULE_SETUP
return T_UNEQUAL;
 YY_BREAK
case 16:
YY_RULE_SETUP
{
  str = zconftext[0];
  new_string();
  BEGIN(STRING);
 }
 YY_BREAK
case 17:
YY_RULE_SETUP
BEGIN(INITIAL); current_file->lineno++; return T_EOL;
 YY_BREAK
case 18:
YY_RULE_SETUP
 YY_BREAK
case 19:
YY_RULE_SETUP
{
  const struct kconf_id *id = kconf_id_lookup(zconftext, zconfleng);
  if (id && id->flags & TF_PARAM) {
   zconflval.id = id;
   return id->token;
  }
  alloc_string(zconftext, zconfleng);
  zconflval.string = text;
  return T_WORD;
 }
 YY_BREAK
case 20:
YY_RULE_SETUP
 YY_BREAK
case 21:
YY_RULE_SETUP
current_file->lineno++;
 YY_BREAK
case 22:
YY_RULE_SETUP
 YY_BREAK
case YY_STATE_EOF(PARAM):
{
  BEGIN(INITIAL);
 }
 YY_BREAK
case 23:
*yy_cp = (yy_hold_char);
(yy_c_buf_p) = yy_cp -= 1;
YY_DO_BEFORE_ACTION;
YY_RULE_SETUP
{
  append_string(zconftext, zconfleng);
  zconflval.string = text;
  return T_WORD_QUOTE;
 }
 YY_BREAK
case 24:
YY_RULE_SETUP
{
  append_string(zconftext, zconfleng);
 }
 YY_BREAK
case 25:
*yy_cp = (yy_hold_char);
(yy_c_buf_p) = yy_cp -= 1;
YY_DO_BEFORE_ACTION;
YY_RULE_SETUP
{
  append_string(zconftext + 1, zconfleng - 1);
  zconflval.string = text;
  return T_WORD_QUOTE;
 }
 YY_BREAK
case 26:
YY_RULE_SETUP
{
  append_string(zconftext + 1, zconfleng - 1);
 }
 YY_BREAK
case 27:
YY_RULE_SETUP
{
  if (str == zconftext[0]) {
   BEGIN(PARAM);
   zconflval.string = text;
   return T_WORD_QUOTE;
  } else
   append_string(zconftext, 1);
 }
 YY_BREAK
case 28:
YY_RULE_SETUP
{
  printf("%s:%d:warning: multi-line strings not supported\n", zconf_curname(), zconf_lineno());
  current_file->lineno++;
  BEGIN(INITIAL);
  return T_EOL;
 }
 YY_BREAK
case YY_STATE_EOF(STRING):
{
  BEGIN(INITIAL);
 }
 YY_BREAK
case 29:
YY_RULE_SETUP
{
  ts = 0;
  for (i = 0; i < zconfleng; i++) {
   if (zconftext[i] == '\t')
    ts = (ts & ~7) + 8;
   else
    ts++;
  }
  last_ts = ts;
  if (first_ts) {
   if (ts < first_ts) {
    zconf_endhelp();
    return T_HELPTEXT;
   }
   ts -= first_ts;
   while (ts > 8) {
    append_string("        ", 8);
    ts -= 8;
   }
   append_string("        ", ts);
  }
 }
 YY_BREAK
case 30:
*yy_cp = (yy_hold_char);
(yy_c_buf_p) = yy_cp -= 1;
YY_DO_BEFORE_ACTION;
YY_RULE_SETUP
{
  current_file->lineno++;
  zconf_endhelp();
  return T_HELPTEXT;
 }
 YY_BREAK
case 31:
YY_RULE_SETUP
{
  current_file->lineno++;
  append_string("\n", 1);
 }
 YY_BREAK
case 32:
YY_RULE_SETUP
{
  while (zconfleng) {
   if ((zconftext[zconfleng-1] != ' ') && (zconftext[zconfleng-1] != '\t'))
    break;
   zconfleng--;
  }
  append_string(zconftext, zconfleng);
  if (!first_ts)
   first_ts = last_ts;
 }
 YY_BREAK
case YY_STATE_EOF(HELP):
{
  zconf_endhelp();
  return T_HELPTEXT;
 }
 YY_BREAK
case YY_STATE_EOF(INITIAL):
case YY_STATE_EOF(COMMAND):
{
 if (current_file) {
  zconf_endfile();
  return T_EOL;
 }
 fclose(zconfin);
 yyterminate();
}
 YY_BREAK
case 33:
YY_RULE_SETUP
YY_FATAL_ERROR( "flex scanner jammed" );
 YY_BREAK
 case YY_END_OF_BUFFER:
  {
  int yy_amount_of_matched_text = (int) (yy_cp - (yytext_ptr)) - 1;
  *yy_cp = (yy_hold_char);
  YY_RESTORE_YY_MORE_OFFSET
  if ( YY_CURRENT_BUFFER_LVALUE->yy_buffer_status == YY_BUFFER_NEW )
   {
# 6690 "dialog.all.c"
   (yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;
   YY_CURRENT_BUFFER_LVALUE->yy_input_file = zconfin;
   YY_CURRENT_BUFFER_LVALUE->yy_buffer_status = YY_BUFFER_NORMAL;
   }
# 6702 "dialog.all.c"
  if ( (yy_c_buf_p) <= &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] )
   {
   yy_state_type yy_next_state;
   (yy_c_buf_p) = (yytext_ptr) + yy_amount_of_matched_text;
   yy_current_state = yy_get_previous_state( );
# 6719 "dialog.all.c"
   yy_next_state = yy_try_NUL_trans( yy_current_state );
   yy_bp = (yytext_ptr) + YY_MORE_ADJ;
   if ( yy_next_state )
    {
    yy_cp = ++(yy_c_buf_p);
    yy_current_state = yy_next_state;
    goto yy_match;
    }
   else
    {
    yy_cp = (yy_c_buf_p);
    goto yy_find_action;
    }
   }
  else switch ( yy_get_next_buffer( ) )
   {
   case EOB_ACT_END_OF_FILE:
    {
    (yy_did_buffer_switch_on_eof) = 0;
    if ( zconfwrap( ) )
     {
# 6755 "dialog.all.c"
     (yy_c_buf_p) = (yytext_ptr) + YY_MORE_ADJ;
     yy_act = YY_STATE_EOF(YY_START);
     goto do_action;
     }
    else
     {
     if ( ! (yy_did_buffer_switch_on_eof) )
      YY_NEW_FILE;
     }
    break;
    }
   case EOB_ACT_CONTINUE_SCAN:
    (yy_c_buf_p) =
     (yytext_ptr) + yy_amount_of_matched_text;
    yy_current_state = yy_get_previous_state( );
    yy_cp = (yy_c_buf_p);
    yy_bp = (yytext_ptr) + YY_MORE_ADJ;
    goto yy_match;
   case EOB_ACT_LAST_MATCH:
    (yy_c_buf_p) =
    &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)];
    yy_current_state = yy_get_previous_state( );
    yy_cp = (yy_c_buf_p);
    yy_bp = (yytext_ptr) + YY_MORE_ADJ;
    goto yy_find_action;
   }
  break;
  }
 default:
  YY_FATAL_ERROR(
   "fatal flex scanner internal error--no action found" );
 }
  }
}
# 6806 "dialog.all.c"
static int yy_get_next_buffer (void)
{
     register char *dest = YY_CURRENT_BUFFER_LVALUE->yy_ch_buf;
 register char *source = (yytext_ptr);
 register int number_to_move, i;
 int ret_val;
 if ( (yy_c_buf_p) > &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars) + 1] )
  YY_FATAL_ERROR(
  "fatal flex scanner internal error--end of buffer missed" );
 if ( YY_CURRENT_BUFFER_LVALUE->yy_fill_buffer == 0 )
  {
  if ( (yy_c_buf_p) - (yytext_ptr) - YY_MORE_ADJ == 1 )
   {
   return EOB_ACT_END_OF_FILE;
   }
  else
   {
   return EOB_ACT_LAST_MATCH;
   }
  }
 number_to_move = (int) ((yy_c_buf_p) - (yytext_ptr)) - 1;
 for ( i = 0; i < number_to_move; ++i )
  *(dest++) = *(source++);
 if ( YY_CURRENT_BUFFER_LVALUE->yy_buffer_status == YY_BUFFER_EOF_PENDING )
  YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars) = 0;
 else
  {
   int num_to_read =
   YY_CURRENT_BUFFER_LVALUE->yy_buf_size - number_to_move - 1;
  while ( num_to_read <= 0 )
   {
   YY_BUFFER_STATE b = YY_CURRENT_BUFFER;
   int yy_c_buf_p_offset =
    (int) ((yy_c_buf_p) - b->yy_ch_buf);
   if ( b->yy_is_our_buffer )
    {
    int new_size = b->yy_buf_size * 2;
    if ( new_size <= 0 )
     b->yy_buf_size += b->yy_buf_size / 8;
    else
     b->yy_buf_size *= 2;
    b->yy_ch_buf = (char *)
     zconfrealloc((void *) b->yy_ch_buf,b->yy_buf_size + 2 );
    }
   else
    b->yy_ch_buf = 0;
   if ( ! b->yy_ch_buf )
    YY_FATAL_ERROR(
    "fatal error - scanner input buffer overflow" );
   (yy_c_buf_p) = &b->yy_ch_buf[yy_c_buf_p_offset];
   num_to_read = YY_CURRENT_BUFFER_LVALUE->yy_buf_size -
      number_to_move - 1;
   }
  if ( num_to_read > YY_READ_BUF_SIZE )
   num_to_read = YY_READ_BUF_SIZE;
  YY_INPUT( (&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move]),
   (yy_n_chars), (size_t) num_to_read );
  YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
  }
 if ( (yy_n_chars) == 0 )
  {
  if ( number_to_move == YY_MORE_ADJ )
   {
   ret_val = EOB_ACT_END_OF_FILE;
   zconfrestart(zconfin );
   }
  else
   {
   ret_val = EOB_ACT_LAST_MATCH;
   YY_CURRENT_BUFFER_LVALUE->yy_buffer_status =
    YY_BUFFER_EOF_PENDING;
   }
  }
 else
  ret_val = EOB_ACT_CONTINUE_SCAN;
 if ((yy_size_t) ((yy_n_chars) + number_to_move) > YY_CURRENT_BUFFER_LVALUE->yy_buf_size) {
  yy_size_t new_size = (yy_n_chars) + number_to_move + ((yy_n_chars) >> 1);
  YY_CURRENT_BUFFER_LVALUE->yy_ch_buf = (char *) zconfrealloc((void *) YY_CURRENT_BUFFER_LVALUE->yy_ch_buf,new_size );
  if ( ! YY_CURRENT_BUFFER_LVALUE->yy_ch_buf )
   YY_FATAL_ERROR( "out of dynamic memory in yy_get_next_buffer()" );
 }
 (yy_n_chars) += number_to_move;
 YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] = YY_END_OF_BUFFER_CHAR;
 YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars) + 1] = YY_END_OF_BUFFER_CHAR;
 (yytext_ptr) = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[0];
 return ret_val;
}
    static yy_state_type yy_get_previous_state (void)
{
 register yy_state_type yy_current_state;
 register char *yy_cp;
 yy_current_state = (yy_start);
 for ( yy_cp = (yytext_ptr) + YY_MORE_ADJ; yy_cp < (yy_c_buf_p); ++yy_cp )
  {
  yy_current_state = yy_nxt[yy_current_state][(*yy_cp ? yy_ec[YY_SC_TO_UI(*yy_cp)] : 1)];
  }
 return yy_current_state;
}
    static yy_state_type yy_try_NUL_trans (yy_state_type yy_current_state )
{
 register int yy_is_jam;
 yy_current_state = yy_nxt[yy_current_state][1];
 yy_is_jam = (yy_current_state <= 0);
 return yy_is_jam ? 0 : yy_current_state;
}
    static void yyunput (int c, register char * yy_bp )
{
 register char *yy_cp;
    yy_cp = (yy_c_buf_p);
 *yy_cp = (yy_hold_char);
 if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 )
  {
  register int number_to_move = (yy_n_chars) + 2;
  register char *dest = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[
     YY_CURRENT_BUFFER_LVALUE->yy_buf_size + 2];
  register char *source =
    &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move];
  while ( source > YY_CURRENT_BUFFER_LVALUE->yy_ch_buf )
   *--dest = *--source;
  yy_cp += (int) (dest - source);
  yy_bp += (int) (dest - source);
  YY_CURRENT_BUFFER_LVALUE->yy_n_chars =
   (yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_buf_size;
  if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 )
   YY_FATAL_ERROR( "flex scanner push-back overflow" );
  }
 *--yy_cp = (char) c;
 (yytext_ptr) = yy_bp;
 (yy_hold_char) = *yy_cp;
 (yy_c_buf_p) = yy_cp;
}
#ifndef YY_NO_INPUT
#ifdef __cplusplus
    static int yyinput (void)
#else
    static int input (void)
#endif
{
 int c;
 *(yy_c_buf_p) = (yy_hold_char);
 if ( *(yy_c_buf_p) == YY_END_OF_BUFFER_CHAR )
  {
  if ( (yy_c_buf_p) < &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] )
   *(yy_c_buf_p) = '\0';
  else
   {
   int offset = (yy_c_buf_p) - (yytext_ptr);
   ++(yy_c_buf_p);
   switch ( yy_get_next_buffer( ) )
    {
    case EOB_ACT_LAST_MATCH:
# 7048 "dialog.all.c"
     zconfrestart(zconfin );
    case EOB_ACT_END_OF_FILE:
     {
     if ( zconfwrap( ) )
      return EOF;
     if ( ! (yy_did_buffer_switch_on_eof) )
      YY_NEW_FILE;
#ifdef __cplusplus
     return yyinput();
#else
     return input();
#endif
     }
    case EOB_ACT_CONTINUE_SCAN:
     (yy_c_buf_p) = (yytext_ptr) + offset;
     break;
    }
   }
  }
 c = *(unsigned char *) (yy_c_buf_p);
 *(yy_c_buf_p) = '\0';
 (yy_hold_char) = *++(yy_c_buf_p);
 return c;
}
#endif
    void zconfrestart (FILE * input_file )
{
 if ( ! YY_CURRENT_BUFFER ){
        zconfensure_buffer_stack ();
  YY_CURRENT_BUFFER_LVALUE =
            zconf_create_buffer(zconfin,YY_BUF_SIZE );
 }
 zconf_init_buffer(YY_CURRENT_BUFFER,input_file );
 zconf_load_buffer_state( );
}
    void zconf_switch_to_buffer (YY_BUFFER_STATE new_buffer )
{
 zconfensure_buffer_stack ();
 if ( YY_CURRENT_BUFFER == new_buffer )
  return;
 if ( YY_CURRENT_BUFFER )
  {
  *(yy_c_buf_p) = (yy_hold_char);
  YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
  YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
  }
 YY_CURRENT_BUFFER_LVALUE = new_buffer;
 zconf_load_buffer_state( );
 (yy_did_buffer_switch_on_eof) = 1;
}
static void zconf_load_buffer_state (void)
{
     (yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;
 (yytext_ptr) = (yy_c_buf_p) = YY_CURRENT_BUFFER_LVALUE->yy_buf_pos;
 zconfin = YY_CURRENT_BUFFER_LVALUE->yy_input_file;
 (yy_hold_char) = *(yy_c_buf_p);
}
    YY_BUFFER_STATE zconf_create_buffer (FILE * file, int size )
{
 YY_BUFFER_STATE b;
 b = (YY_BUFFER_STATE) zconfalloc(sizeof( struct yy_buffer_state ) );
 if ( ! b )
  YY_FATAL_ERROR( "out of dynamic memory in zconf_create_buffer()" );
 b->yy_buf_size = size;
 b->yy_ch_buf = (char *) zconfalloc(b->yy_buf_size + 2 );
 if ( ! b->yy_ch_buf )
  YY_FATAL_ERROR( "out of dynamic memory in zconf_create_buffer()" );
 b->yy_is_our_buffer = 1;
 zconf_init_buffer(b,file );
 return b;
}
    void zconf_delete_buffer (YY_BUFFER_STATE b )
{
 if ( ! b )
  return;
 if ( b == YY_CURRENT_BUFFER )
  YY_CURRENT_BUFFER_LVALUE = (YY_BUFFER_STATE) 0;
 if ( b->yy_is_our_buffer )
  zconffree((void *) b->yy_ch_buf );
 zconffree((void *) b );
}
    static void zconf_init_buffer (YY_BUFFER_STATE b, FILE * file )
{
 int oerrno = errno;
 zconf_flush_buffer(b );
 b->yy_input_file = file;
 b->yy_fill_buffer = 1;
    if (b != YY_CURRENT_BUFFER){
        b->yy_bs_lineno = 1;
        b->yy_bs_column = 0;
    }
        b->yy_is_interactive = 0;
 errno = oerrno;
}
    void zconf_flush_buffer (YY_BUFFER_STATE b )
{
     if ( ! b )
  return;
 b->yy_n_chars = 0;
 b->yy_ch_buf[0] = YY_END_OF_BUFFER_CHAR;
 b->yy_ch_buf[1] = YY_END_OF_BUFFER_CHAR;
 b->yy_buf_pos = &b->yy_ch_buf[0];
 b->yy_at_bol = 1;
 b->yy_buffer_status = YY_BUFFER_NEW;
 if ( b == YY_CURRENT_BUFFER )
  zconf_load_buffer_state( );
}
void zconfpush_buffer_state (YY_BUFFER_STATE new_buffer )
{
     if (new_buffer == NULL)
  return;
 zconfensure_buffer_stack();
 if ( YY_CURRENT_BUFFER )
  {
  *(yy_c_buf_p) = (yy_hold_char);
  YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
  YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
  }
 if (YY_CURRENT_BUFFER)
  (yy_buffer_stack_top)++;
 YY_CURRENT_BUFFER_LVALUE = new_buffer;
 zconf_load_buffer_state( );
 (yy_did_buffer_switch_on_eof) = 1;
}
void zconfpop_buffer_state (void)
{
     if (!YY_CURRENT_BUFFER)
  return;
 zconf_delete_buffer(YY_CURRENT_BUFFER );
 YY_CURRENT_BUFFER_LVALUE = NULL;
 if ((yy_buffer_stack_top) > 0)
  --(yy_buffer_stack_top);
 if (YY_CURRENT_BUFFER) {
  zconf_load_buffer_state( );
  (yy_did_buffer_switch_on_eof) = 1;
 }
}
static void zconfensure_buffer_stack (void)
{
 int num_to_alloc;
 if (!(yy_buffer_stack)) {
  num_to_alloc = 1;
  (yy_buffer_stack) = (struct yy_buffer_state**)zconfalloc
        (num_to_alloc * sizeof(struct yy_buffer_state*)
        );
  if ( ! (yy_buffer_stack) )
   YY_FATAL_ERROR( "out of dynamic memory in zconfensure_buffer_stack()" );
  memset((yy_buffer_stack), 0, num_to_alloc * sizeof(struct yy_buffer_state*));
  (yy_buffer_stack_max) = num_to_alloc;
  (yy_buffer_stack_top) = 0;
  return;
 }
 if ((yy_buffer_stack_top) >= ((yy_buffer_stack_max)) - 1){
  int grow_size = 8 ;
  num_to_alloc = (yy_buffer_stack_max) + grow_size;
  (yy_buffer_stack) = (struct yy_buffer_state**)zconfrealloc
        ((yy_buffer_stack),
        num_to_alloc * sizeof(struct yy_buffer_state*)
        );
  if ( ! (yy_buffer_stack) )
   YY_FATAL_ERROR( "out of dynamic memory in zconfensure_buffer_stack()" );
  memset((yy_buffer_stack) + (yy_buffer_stack_max), 0, grow_size * sizeof(struct yy_buffer_state*));
  (yy_buffer_stack_max) = num_to_alloc;
 }
}
YY_BUFFER_STATE zconf_scan_buffer (char * base, yy_size_t size )
{
 YY_BUFFER_STATE b;
 if ( size < 2 ||
      base[size-2] != YY_END_OF_BUFFER_CHAR ||
      base[size-1] != YY_END_OF_BUFFER_CHAR )
  return 0;
 b = (YY_BUFFER_STATE) zconfalloc(sizeof( struct yy_buffer_state ) );
 if ( ! b )
  YY_FATAL_ERROR( "out of dynamic memory in zconf_scan_buffer()" );
 b->yy_buf_size = size - 2;
 b->yy_buf_pos = b->yy_ch_buf = base;
 b->yy_is_our_buffer = 0;
 b->yy_input_file = 0;
 b->yy_n_chars = b->yy_buf_size;
 b->yy_is_interactive = 0;
 b->yy_at_bol = 1;
 b->yy_fill_buffer = 0;
 b->yy_buffer_status = YY_BUFFER_NEW;
 zconf_switch_to_buffer(b );
 return b;
}
# 7387 "dialog.all.c"
YY_BUFFER_STATE zconf_scan_string (yyconst char * yystr )
{
 return zconf_scan_bytes(yystr,strlen(yystr) );
}
# 7400 "dialog.all.c"
YY_BUFFER_STATE zconf_scan_bytes (yyconst char * yybytes, int _yybytes_len )
{
 YY_BUFFER_STATE b;
 char *buf;
 yy_size_t n;
 int i;
 n = _yybytes_len + 2;
 buf = (char *) zconfalloc(n );
 if ( ! buf )
  YY_FATAL_ERROR( "out of dynamic memory in zconf_scan_bytes()" );
 for ( i = 0; i < _yybytes_len; ++i )
  buf[i] = yybytes[i];
 buf[_yybytes_len] = buf[_yybytes_len+1] = YY_END_OF_BUFFER_CHAR;
 b = zconf_scan_buffer(buf,n );
 if ( ! b )
  YY_FATAL_ERROR( "bad buffer in zconf_scan_bytes()" );
 b->yy_is_our_buffer = 1;
 return b;
}
#ifndef YY_EXIT_FAILURE
#define YY_EXIT_FAILURE 2
#endif
static void yy_fatal_error (yyconst char* msg )
{
     (void) fprintf( stderr, "%s\n", msg );
 exit( YY_EXIT_FAILURE );
}
#undef yyless
#define yyless(n) \
 do \
  { \
                                              \
        int yyless_macro_arg = (n); \
        YY_LESS_LINENO(yyless_macro_arg);\
  zconftext[zconfleng] = (yy_hold_char); \
  (yy_c_buf_p) = zconftext + yyless_macro_arg; \
  (yy_hold_char) = *(yy_c_buf_p); \
  *(yy_c_buf_p) = '\0'; \
  zconfleng = yyless_macro_arg; \
  } \
 while ( 0 )
int zconfget_lineno (void)
{
    return zconflineno;
}
FILE *zconfget_in (void)
{
        return zconfin;
}
FILE *zconfget_out (void)
{
        return zconfout;
}
int zconfget_leng (void)
{
        return zconfleng;
}
char *zconfget_text (void)
{
        return zconftext;
}
void zconfset_lineno (int line_number )
{
    zconflineno = line_number;
}
void zconfset_in (FILE * in_str )
{
        zconfin = in_str ;
}
void zconfset_out (FILE * out_str )
{
        zconfout = out_str ;
}
# 7575 "dialog.all.c"
#ifndef yytext_ptr
static void yy_flex_strncpy (char* s1, yyconst char * s2, int n )
{
 register int i;
 for ( i = 0; i < n; ++i )
  s1[i] = s2[i];
}
#endif
#ifdef YY_NEED_STRLEN
static int yy_flex_strlen (yyconst char * s )
{
 register int n;
 for ( n = 0; s[n]; ++n )
  ;
 return n;
}
#endif
void *zconfalloc (yy_size_t size )
{
 return (void *) malloc( size );
}
void *zconfrealloc (void * ptr, yy_size_t size )
{
 return (void *) realloc( (char *) ptr, size );
}
void zconffree (void * ptr )
{
 free( (char *) ptr );
}
#define YYTABLES_NAME "yytables"
void zconf_starthelp(void)
{
 new_string();
 last_ts = first_ts = 0;
 BEGIN(HELP);
}
static void zconf_endhelp(void)
{
 zconflval.string = text;
 BEGIN(INITIAL);
}
# 7640 "dialog.all.c"
FILE *zconf_fopen(const char *name)
{
 char *env, fullname[PATH_MAX+1];
 FILE *f;
 f = fopen(name, "r");
 if (!f && name != NULL && name[0] != '/') {
  env = getenv(SRCTREE);
  if (env) {
   sprintf(fullname, "%s/%s", env, name);
   f = fopen(fullname, "r");
  }
 }
 return f;
}
void zconf_initscan(const char *name)
{
 zconfin = zconf_fopen(name);
 if (!zconfin) {
  printf("can't find file %s\n", name);
  exit(1);
 }
 current_buf = malloc(sizeof(*current_buf));
 memset(current_buf, 0, sizeof(*current_buf));
 current_file = file_lookup(name);
 current_file->lineno = 1;
}
void zconf_nextfile(const char *name)
{
 struct file *iter;
 struct file *file = file_lookup(name);
 struct buffer *buf = malloc(sizeof(*buf));
 memset(buf, 0, sizeof(*buf));
 current_buf->state = YY_CURRENT_BUFFER;
 zconfin = zconf_fopen(file->name);
 if (!zconfin) {
  printf("%s:%d: can't open file \"%s\"\n",
      zconf_curname(), zconf_lineno(), file->name);
  exit(1);
 }
 zconf_switch_to_buffer(zconf_create_buffer(zconfin,YY_BUF_SIZE));
 buf->parent = current_buf;
 current_buf = buf;
 for (iter = current_file->parent; iter; iter = iter->parent ) {
  if (!strcmp(current_file->name,iter->name) ) {
   printf("%s:%d: recursive inclusion detected. "
          "Inclusion path:\n  current file : '%s'\n",
          zconf_curname(), zconf_lineno(),
          zconf_curname());
   iter = current_file->parent;
   while (iter && \
          strcmp(iter->name,current_file->name)) {
    printf("  included from: '%s:%d'\n",
           iter->name, iter->lineno-1);
    iter = iter->parent;
   }
   if (iter)
    printf("  included from: '%s:%d'\n",
           iter->name, iter->lineno+1);
   exit(1);
  }
 }
 file->lineno = 1;
 file->parent = current_file;
 current_file = file;
}
static void zconf_endfile(void)
{
 struct buffer *parent;
 current_file = current_file->parent;
 parent = current_buf->parent;
 if (parent) {
  fclose(zconfin);
  zconf_delete_buffer(YY_CURRENT_BUFFER);
  zconf_switch_to_buffer(parent->state);
 }
 free(current_buf);
 current_buf = parent;
}
int zconf_lineno(void)
{
 return current_pos.lineno;
}
const char *zconf_curname(void)
{
 return current_pos.file ? current_pos.file->name : "<none>";
}
struct file *file_lookup(const char *name)
{
 struct file *file;
 const char *file_name = sym_expand_string_value(name);
 for (file = file_list; file; file = file->next) {
  if (!strcmp(name, file->name)) {
   free((void *)file_name);
   return file;
  }
 }
 file = malloc(sizeof(*file));
 memset(file, 0, sizeof(*file));
 file->name = file_name;
 file->next = file_list;
 file_list = file;
 return file;
}
int file_write_dep(const char *name)
{
 struct symbol *sym, *env_sym;
 struct expr *e;
 struct file *file;
 FILE *out;
 if (!name)
  name = ".kconfig.d";
 out = fopen("..config.tmp", "w");
 if (!out)
  return 1;
 fprintf(out, "deps_config := \\\n");
 for (file = file_list; file; file = file->next) {
  if (file->next)
   fprintf(out, "\t%s \\\n", file->name);
  else
   fprintf(out, "\t%s\n", file->name);
 }
 fprintf(out, "\n%s: \\\n"
       "\t$(deps_config)\n\n", conf_get_autoconfig_name());
 expr_list_for_each_sym(sym_env_list, e, sym) {
  struct property *prop;
  const char *value;
  prop = sym_get_env_prop(sym);
  env_sym = prop_get_symbol(prop);
  if (!env_sym)
   continue;
  value = getenv(env_sym->name);
  if (!value)
   value = "";
  fprintf(out, "ifneq \"$(%s)\" \"%s\"\n", env_sym->name, value);
  fprintf(out, "%s: FORCE\n", conf_get_autoconfig_name());
  fprintf(out, "endif\n");
 }
 fprintf(out, "\n$(deps_config): ;\n");
 fclose(out);
 rename("..config.tmp", name);
 return 0;
}
struct gstr str_new(void)
{
 struct gstr gs;
 gs.s = malloc(sizeof(char) * 64);
 gs.len = 64;
 gs.max_width = 0;
 strcpy(gs.s, "\0");
 return gs;
}
struct gstr str_assign(const char *s)
{
 struct gstr gs;
 gs.s = strdup(s);
 gs.len = strlen(s) + 1;
 gs.max_width = 0;
 return gs;
}
void str_free(struct gstr *gs)
{
 if (gs->s)
  free(gs->s);
 gs->s = NULL;
 gs->len = 0;
}
void str_append(struct gstr *gs, const char *s)
{
 size_t l;
 if (s) {
  l = strlen(gs->s) + strlen(s) + 1;
  if (l > gs->len) {
   gs->s = realloc(gs->s, l);
   gs->len = l;
  }
  strcat(gs->s, s);
 }
}
void str_printf(struct gstr *gs, const char *fmt, ...)
{
 va_list ap;
 char s[10000];
 va_start(ap, fmt);
 vsnprintf(s, sizeof(s), fmt, ap);
 str_append(gs, s);
 va_end(ap);
}
const char *str_get(struct gstr *gs)
{
 return gs->s;
}
static void conf_warning(const char *fmt, ...)
 __attribute__ ((format (printf, 1, 2)));
static void conf_message(const char *fmt, ...)
 __attribute__ ((format (printf, 1, 2)));
static const char *conf_filename;
static int conf_lineno, conf_warnings, conf_unsaved;
const char conf_defname[] = "./tools/watch-config.dlg";
static char* autoconfig_name= "./autoconf.h";
static void conf_warning(const char *fmt, ...)
{
 va_list ap;
 va_start(ap, fmt);
 fprintf(stderr, "%s:%d:warning: ", conf_filename, conf_lineno);
 vfprintf(stderr, fmt, ap);
 fprintf(stderr, "\n");
 va_end(ap);
 conf_warnings++;
}
static void conf_default_message_callback(const char *fmt, va_list ap)
{
 printf("#\n# ");
 vprintf(fmt, ap);
 printf("\n#\n");
}
static void (*conf_message_callback) (const char *fmt, va_list ap) =
 conf_default_message_callback;
void conf_set_message_callback(void (*fn) (const char *fmt, va_list ap))
{
 conf_message_callback = fn;
}
static void conf_message(const char *fmt, ...)
{
 va_list ap;
 va_start(ap, fmt);
 if (conf_message_callback)
  conf_message_callback(fmt, ap);
}
const char *conf_get_configname(void) {
    char *name = getenv("BUILD_DIALOG_CONFIG");
    return name ? name : "./tools/watch-config.dlg";
}
const char *conf_get_autoconfig_name(void) { return autoconfig_name; }
static char *conf_expand_value(const char *in) {
 struct symbol *sym;
 const char *src;
 static char res_value[SYMBOL_MAXLENGTH];
 char *dst, name[SYMBOL_MAXLENGTH];
 res_value[0] = 0;
 dst = name;
 while ((src = strchr(in, '$'))) {
  strncat(res_value, in, src - in);
  src++;
  dst = name;
  while (isalnum(*src) || *src == '_')
   *dst++ = *src++;
  *dst = 0;
  sym = sym_lookup(name, 0);
  sym_calc_value(sym);
  strcat(res_value, sym_get_string_value(sym));
  in = src;
 }
 strcat(res_value, in);
 return res_value;
}
char *conf_get_default_confname(void)
{
 struct stat buf;
 static char fullname[PATH_MAX+1];
 char *env, *name;
 name = conf_expand_value(conf_defname);
 env = getenv(SRCTREE);
 if (env) {
  sprintf(fullname, "%s/%s", env, name);
  if (!stat(fullname, &buf))
   return fullname;
 }
 return name;
}
static int conf_set_sym_val(struct symbol *sym, int def, int def_flags, char *p)
{
 char *p2;
 switch (sym->type) {
 case S_TRISTATE:
  if (p[0] == 'm') {
   sym->def[def].tri = mod;
   sym->flags |= def_flags;
   break;
  }
 case S_BOOLEAN:
  if (p[0] == '1') {
   sym->def[def].tri = yes;
   sym->flags |= def_flags;
   break;
  }
  if (p[0] == '0') {
   sym->def[def].tri = no;
   sym->flags |= def_flags;
   break;
  }
  return 1;
 case S_OTHER:
  if (*p != '"') {
   for (p2 = p; *p2 && !isspace(*p2); p2++)
    ;
   sym->type = S_STRING;
   goto done;
  }
 case S_STRING:
  if (*p++ != '"')
   break;
  for (p2 = p; (p2 = strpbrk(p2, "\"\\")); p2++) {
   if (*p2 == '"') {
    *p2 = 0;
    break;
   }
   memmove(p2, p2 + 1, strlen(p2));
  }
  if (!p2) {
   conf_warning("invalid string found");
   return 1;
  }
 case S_INT:
 case S_HEX:
 done:
  if (sym_string_valid(sym, p)) {
   sym->def[def].val = strdup(p);
   sym->flags |= def_flags;
  } else {
   conf_warning("symbol value '%s' invalid for %s", p, sym->name);
   return 1;
  }
  break;
 default:
  ;
 }
 return 0;
}
#define LINE_GROWTH 16
static int add_byte(int c, char **lineptr, size_t slen, size_t *n)
{
 char *nline;
 size_t new_size = slen + 1;
 if (new_size > *n) {
  new_size += LINE_GROWTH - 1;
  new_size *= 2;
  nline = realloc(*lineptr, new_size);
  if (!nline)
   return -1;
  *lineptr = nline;
  *n = new_size;
 }
 (*lineptr)[slen] = c;
 return 0;
}
static ssize_t compat_getline(char **lineptr, size_t *n, FILE *stream)
{
 char *line = *lineptr;
 size_t slen = 0;
 for (;;) {
  int c = getc(stream);
  switch (c) {
  case '\n':
   if (add_byte(c, &line, slen, n) < 0)
    goto e_out;
   slen++;
  case EOF:
   if (add_byte('\0', &line, slen, n) < 0)
    goto e_out;
   *lineptr = line;
   if (slen == 0)
    return -1;
   return slen;
  default:
   if (add_byte(c, &line, slen, n) < 0)
    goto e_out;
   slen++;
  }
 }
e_out:
 line[slen-1] = '\0';
 *lineptr = line;
 return -1;
}
int conf_read_simple(const char *name, int def)
{
 FILE *in = NULL;
 char *line = NULL;
 size_t line_asize = 0;
 char *p, *p2;
 struct symbol *sym;
 int i, def_flags;
 if (name) {
  in = zconf_fopen(name);
 } else {
  struct property *prop;
  name = conf_get_configname();
  in = zconf_fopen(name);
  if (in)
   goto load;
  sym_add_change_count(1);
  if (!sym_defconfig_list) {
   if (modules_sym)
    sym_calc_value(modules_sym);
   return 1;
  }
  for_all_defaults(sym_defconfig_list, prop) {
   if (expr_calc_value(prop->visible.expr) == no ||
       prop->expr->type != E_SYMBOL)
    continue;
   name = conf_expand_value(prop->expr->left.sym->name);
   in = zconf_fopen(name);
   if (in) {
    conf_message(_("using defaults found in %s"),
      name);
    goto load;
   }
  }
 }
 if (!in)
  return 1;
load:
 conf_filename = name;
 conf_lineno = 0;
 conf_warnings = 0;
 conf_unsaved = 0;
 def_flags = SYMBOL_DEF << def;
 for_all_symbols(i, sym) {
  sym->flags |= SYMBOL_CHANGED;
  sym->flags &= ~(def_flags|SYMBOL_VALID);
  if (sym_is_choice(sym))
   sym->flags |= def_flags;
  switch (sym->type) {
  case S_INT:
  case S_HEX:
  case S_STRING:
   if (sym->def[def].val)
    free(sym->def[def].val);
  default:
   sym->def[def].val = NULL;
   sym->def[def].tri = no;
  }
 }
 while (compat_getline(&line, &line_asize, in) != -1) {
  conf_lineno++;
  sym = NULL;
  if (line[0] == '#') {
   if (memcmp(line + 2, CONFIG_, strlen(CONFIG_)))
    continue;
   p = strchr(line + 2 + strlen(CONFIG_), ' ');
   if (!p)
    continue;
   *p++ = 0;
   if (strstr(p, "is not set"))
    continue;
   if (def == S_DEF_USER) {
    sym = sym_find(line + 2 + strlen(CONFIG_));
    if (!sym) {
     sym_add_change_count(1);
     goto setsym;
    }
   } else {
    sym = sym_lookup(line + 2 + strlen(CONFIG_), 0);
    if (sym->type == S_UNKNOWN)
     sym->type = S_BOOLEAN;
   }
   if (sym->flags & def_flags) {
    conf_warning("override: reassigning to symbol %s", sym->name);
   }
   switch (sym->type) {
   case S_BOOLEAN:
   case S_TRISTATE:
    sym->def[def].tri = no;
    sym->flags |= def_flags;
    break;
   default:
    ;
   }
  } else if (memcmp(line, CONFIG_, strlen(CONFIG_)) == 0) {
   p = strchr(line + strlen(CONFIG_), '=');
   if (!p)
    continue;
   *p++ = 0;
   p2 = strchr(p, '\n');
   if (p2) {
    *p2-- = 0;
    if (*p2 == '\r')
     *p2 = 0;
   }
   if (def == S_DEF_USER) {
    sym = sym_find(line + strlen(CONFIG_));
    if (!sym) {
     sym_add_change_count(1);
     goto setsym;
    }
   } else {
    sym = sym_lookup(line + strlen(CONFIG_), 0);
    if (sym->type == S_UNKNOWN)
     sym->type = S_OTHER;
   }
   if (sym->flags & def_flags) {
    conf_warning("override: reassigning to symbol %s", sym->name);
   }
   if (conf_set_sym_val(sym, def, def_flags, p))
    continue;
  } else {
   if (line[0] != '\r' && line[0] != '\n')
    conf_warning("unexpected data");
   continue;
  }
setsym:
  if (sym && sym_is_choice_value(sym)) {
   struct symbol *cs = prop_get_symbol(sym_get_choice_prop(sym));
   switch (sym->def[def].tri) {
   case no:
    break;
   case mod:
    if (cs->def[def].tri == yes) {
     conf_warning("%s creates inconsistent choice state", sym->name);
     cs->flags &= ~def_flags;
    }
    break;
   case yes:
    if (cs->def[def].tri != no)
     conf_warning("override: %s changes choice state", sym->name);
    cs->def[def].val = sym;
    break;
   }
   cs->def[def].tri = EXPR_OR(cs->def[def].tri, sym->def[def].tri);
  }
 }
 free(line);
 fclose(in);
 if (modules_sym)
  sym_calc_value(modules_sym);
 return 0;
}
int conf_read(const char *name)
{
 struct symbol *sym;
 int i;
 sym_set_change_count(0);
 if (conf_read_simple(name, S_DEF_USER))
  return 1;
 for_all_symbols(i, sym) {
  sym_calc_value(sym);
  if (sym_is_choice(sym) || (sym->flags & SYMBOL_AUTO))
   continue;
  if (sym_has_value(sym) && (sym->flags & SYMBOL_WRITE)) {
   switch (sym->type) {
   case S_BOOLEAN:
   case S_TRISTATE:
    if (sym->def[S_DEF_USER].tri != sym_get_tristate_value(sym))
     break;
    if (!sym_is_choice(sym))
     continue;
   default:
    if (!strcmp(sym->curr.val, sym->def[S_DEF_USER].val))
     continue;
    break;
   }
  } else if (!sym_has_value(sym) && !(sym->flags & SYMBOL_WRITE))
   continue;
  conf_unsaved++;
 }
 for_all_symbols(i, sym) {
  if (sym_has_value(sym) && !sym_is_choice_value(sym)) {
   if (sym->visible == no && !conf_unsaved)
    sym->flags &= ~SYMBOL_DEF_USER;
   switch (sym->type) {
   case S_STRING:
   case S_INT:
   case S_HEX:
    if (sym_string_within_range(sym, sym->def[S_DEF_USER].val))
     break;
    sym->flags &= ~(SYMBOL_VALID|SYMBOL_DEF_USER);
    conf_unsaved++;
    break;
   default:
    break;
   }
  }
 }
 sym_add_change_count(conf_warnings || conf_unsaved);
 return 0;
}
# 8309 "dialog.all.c"
static void
kconfig_print_symbol(FILE *fp, struct symbol *sym, const char *value, void *arg)
{
 switch (sym->type) {
 case S_BOOLEAN:
 case S_TRISTATE:
  if (*value == '0') {
   bool skip_unset = (arg != NULL);
   if (!skip_unset)
    fprintf(fp, "%s%s=; # is not set\n", CONFIG_, sym->name);
   return;
  }
  break;
 default:
  break;
 }
 char *setv = ((strlen(value) == 1) ?
                (((value[0] == 'y') || (value[0] == '1')) ? (char*)&"1" :
                    (((value[0] == 'm') || (value[0] == '2')) ? (char*)&"1" : (char*)&"0")) : (char*)value);
 fprintf(fp, "%s%s=%s\n", CONFIG_, sym->name, setv);
}
static void
kconfig_print_comment(FILE *fp, const char *value, void *arg)
{
 const char *p = value;
 size_t l;
 for (;;) {
  l = strcspn(p, "\n");
  fprintf(fp, "#");
  if (l) {
   fprintf(fp, " ");
   xfwrite(p, l, 1, fp);
   p += l;
  }
  fprintf(fp, "\n");
  if (*p++ == '\0')
   break;
 }
}
static struct conf_printer config_printer_cb =
{
 .print_symbol = kconfig_print_symbol,
 .print_comment = kconfig_print_comment,
};
static void
header_print_symbol(FILE *fp, struct symbol *sym, const char *value, void *arg)
{
 switch (sym->type) {
 case S_BOOLEAN:
 case S_TRISTATE: {
  const char *suffix = "";
  switch (*value) {
  case 'n':
  case '0':
   break;
  case 'm':
  case '2':
   suffix = "_MODULE";
  default:
   fprintf(fp, "#define %s%s%s 1\n", CONFIG_, sym->name, suffix);
  }
  break;
 }
 case S_HEX: {
  const char *prefix = "";
  if (value[0] != '0' || (value[1] != 'x' && value[1] != 'X'))
   prefix = "0x";
  fprintf(fp, "#define %s%s %s%s\n",
      CONFIG_, sym->name, prefix, value);
  break;
 }
 case S_STRING:
 case S_INT:
  fprintf(fp, "#define %s%s %s\n",
      CONFIG_, sym->name, value);
  break;
 default:
  break;
 }
}
static void header_print_comment(FILE *fp, const char *value, void *arg)
{
 const char *p = value;
 size_t l;
 fprintf(fp, "/*\n");
 for (;;) {
  l = strcspn(p, "\n");
  fprintf(fp, " *");
  if (l) {
   fprintf(fp, " ");
   xfwrite(p, l, 1, fp);
   p += l;
  }
  fprintf(fp, "\n");
  if (*p++ == '\0')
   break;
 }
 fprintf(fp, " */\n");
}
static struct conf_printer header_printer_cb =
{
 .print_symbol = header_print_symbol,
 .print_comment = header_print_comment,
};
static void
tristate_print_symbol(FILE *fp, struct symbol *sym, const char *value, void *arg)
{
 if (sym->type == S_TRISTATE && *value != '0')
  fprintf(fp, "%s%s=%c\n", CONFIG_, sym->name, (char)toupper(*value));
}
static struct conf_printer tristate_printer_cb =
{
 .print_symbol = tristate_print_symbol,
 .print_comment = kconfig_print_comment,
};
static void conf_write_symbol(FILE *fp, struct symbol *sym,
         struct conf_printer *printer, void *printer_arg)
{
 const char *str;
 switch (sym->type) {
 case S_OTHER:
 case S_UNKNOWN:
  break;
 case S_STRING:
  str = sym_get_string_value(sym);
  str = sym_escape_string_value(str);
  printer->print_symbol(fp, sym, str, printer_arg);
  free((void *)str);
  break;
 default:
  str = sym_get_string_value(sym);
  printer->print_symbol(fp, sym, str, printer_arg);
 }
}
static void
conf_write_heading(FILE *fp, struct conf_printer *printer, void *printer_arg)
{
 char buf[256];
 snprintf(buf, sizeof(buf),
     "\n"
     "Automatically generated file; DO NOT EDIT.\n"
     "%s\n",
     rootmenu.prompt->text);
 printer->print_comment(fp, buf, printer_arg);
}
int conf_write_defconfig(const char *filename)
{
 struct symbol *sym;
 struct menu *menu;
 FILE *out;
 out = fopen(filename, "w");
 if (!out)
  return 1;
 sym_clear_all_valid();
 menu = rootmenu.list;
 while (menu != NULL)
 {
  sym = menu->sym;
  if (sym == NULL) {
   if (!menu_is_visible(menu))
    goto next_menu;
  } else if (!sym_is_choice(sym)) {
   sym_calc_value(sym);
   if (!(sym->flags & SYMBOL_WRITE))
    goto next_menu;
   sym->flags &= ~SYMBOL_WRITE;
   if (!sym_is_changable(sym))
    goto next_menu;
   if (strcmp(sym_get_string_value(sym), sym_get_string_default(sym)) == 0)
    goto next_menu;
# 8528 "dialog.all.c"
   if (sym_is_choice_value(sym)) {
    struct symbol *cs;
    struct symbol *ds;
    cs = prop_get_symbol(sym_get_choice_prop(sym));
    ds = sym_choice_default(cs);
    if (!sym_is_optional(cs) && sym == ds) {
     if ((sym->type == S_BOOLEAN) &&
         sym_get_tristate_value(sym) == yes)
      goto next_menu;
    }
   }
   conf_write_symbol(out, sym, &config_printer_cb, NULL);
  }
next_menu:
  if (menu->list != NULL) {
   menu = menu->list;
  }
  else if (menu->next != NULL) {
   menu = menu->next;
  } else {
   while ((menu = menu->parent)) {
    if (menu->next != NULL) {
     menu = menu->next;
     break;
    }
   }
  }
 }
 fclose(out);
 return 0;
}
int conf_write(const char *name)
{
 FILE *out;
 struct symbol *sym;
 struct menu *menu;
 const char *basename;
 const char *str;
 char dirname[PATH_MAX+1], tmpname[PATH_MAX+1], newname[PATH_MAX+1];
 char *env;
 dirname[0] = 0;
 if (name && name[0]) {
  struct stat st;
  char *slash;
  if (!stat(name, &st) && S_ISDIR(st.st_mode)) {
   strcpy(dirname, name);
   strcat(dirname, "/");
   basename = conf_get_configname();
  } else if ((slash = strrchr(name, '/'))) {
   int size = slash - name + 1;
   memcpy(dirname, name, size);
   dirname[size] = 0;
   if (slash[1])
    basename = slash + 1;
   else
    basename = conf_get_configname();
  } else
   basename = name;
 } else
  basename = conf_get_configname();
 sprintf(newname, "%s%s", dirname, basename);
 env = getenv("IPSNG_CONFIG_OVERWRITE");
 if (!env || !*env) {
  sprintf(tmpname, "%s.tmpconfig.%d", dirname, (int)getpid());
  out = fopen(tmpname, "w");
 } else {
  *tmpname = 0;
  out = fopen(newname, "w");
 }
 if (!out)
  return 1;
 conf_write_heading(out, &config_printer_cb, NULL);
 if (!conf_get_changed())
  sym_clear_all_valid();
 menu = rootmenu.list;
 while (menu) {
  sym = menu->sym;
  if (!sym) {
   if (!menu_is_visible(menu))
    goto next;
   str = menu_get_prompt(menu);
   fprintf(out, "\n"
         "#\n"
         "# %s\n"
         "#\n", str);
  } else if (!(sym->flags & SYMBOL_CHOICE)) {
   sym_calc_value(sym);
   if (!(sym->flags & SYMBOL_WRITE))
    goto next;
   sym->flags &= ~SYMBOL_WRITE;
   conf_write_symbol(out, sym, &config_printer_cb, NULL);
  }
next:
  if (menu->list) {
   menu = menu->list;
   continue;
  }
  if (menu->next)
   menu = menu->next;
  else while ((menu = menu->parent)) {
   if (menu->next) {
    menu = menu->next;
    break;
   }
  }
 }
 fclose(out);
 if (*tmpname) {
  strcat(dirname, basename);
  strcat(dirname, ".old");
  rename(newname, dirname);
  if (rename(tmpname, newname))
   return 1;
 }
 conf_message(_("configuration written to %s"), newname);
 sym_set_change_count(0);
 return 0;
}
static int conf_split_config(void)
{
 const char *name;
 char path[PATH_MAX+1];
 char *s, *d, c;
 struct symbol *sym;
 struct stat sb;
 int res, i, fd;
 name = conf_get_autoconfig_name();
 conf_read_simple(name, S_DEF_AUTO);
 if (chdir("tmp")) {
  if (mkdir("tmp", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
   return 1;
  else
   if (chdir("tmp"))
    return 1;
 }
 res = 0;
 for_all_symbols(i, sym) {
  sym_calc_value(sym);
  if ((sym->flags & SYMBOL_AUTO) || !sym->name)
   continue;
  if (sym->flags & SYMBOL_WRITE) {
   if (sym->flags & SYMBOL_DEF_AUTO) {
    switch (sym->type) {
    case S_BOOLEAN:
    case S_TRISTATE:
     if (sym_get_tristate_value(sym) ==
         sym->def[S_DEF_AUTO].tri)
      continue;
     break;
    case S_STRING:
    case S_HEX:
    case S_INT:
     if (!strcmp(sym_get_string_value(sym),
          sym->def[S_DEF_AUTO].val))
      continue;
     break;
    default:
     break;
    }
   } else {
    switch (sym->type) {
    case S_BOOLEAN:
    case S_TRISTATE:
     if (sym_get_tristate_value(sym) == no)
      continue;
     break;
    default:
     break;
    }
   }
  } else if (!(sym->flags & SYMBOL_DEF_AUTO))
   continue;
  s = sym->name;
  d = path;
  while ((c = *s++)) {
   c = tolower(c);
   *d++ = (c == '_') ? '/' : c;
  }
  strcpy(d, ".h");
  fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
   if (errno != ENOENT) {
    res = 1;
    break;
   }
   d = path;
   while ((d = strchr(d, '/'))) {
    *d = 0;
    if (stat(path, &sb) && mkdir(path, 0755)) {
     res = 1;
     goto out;
    }
    *d++ = '/';
   }
   fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
   if (fd == -1) {
    res = 1;
    break;
   }
  }
  close(fd);
 }
out:
 if (chdir("..")) {
  return 1;
 }
 return res;
}
int conf_write_autoconf(const char* filename)
{
 struct symbol *sym;
 autoconfig_name=(char*)filename;
 const char *name=conf_get_autoconfig_name();
 FILE *out, *tristate, *out_h;
 int i;
 file_write_dep("./dialog-auto.cmd");
 if (conf_split_config()) {
  return 1;
 }
 out = fopen("./dialog-config-tmp", "w");
 if (!out) {
  return 1;
 }
 tristate = fopen("./dialog-config-tmp-tristate", "w");
 if (!tristate) {
  fclose(out);
  return 1;
 }
 out_h = fopen("./.tmpconfig.h", "w");
 if (!out_h) {
  fclose(out);
  fclose(tristate);
  return 1;
 }
 conf_write_heading(out, &config_printer_cb, NULL);
 conf_write_heading(tristate, &tristate_printer_cb, NULL);
 conf_write_heading(out_h, &header_printer_cb, NULL);
 for_all_symbols(i, sym) {
  sym_calc_value(sym);
  if (!(sym->flags & SYMBOL_WRITE) || !sym->name)
   continue;
  conf_write_symbol(out, sym, &config_printer_cb, (void *)1);
  conf_write_symbol(tristate, sym, &tristate_printer_cb, (void *)1);
  conf_write_symbol(out_h, sym, &header_printer_cb, NULL);
 }
 fclose(out);
 fclose(tristate);
 fclose(out_h);
 if (rename("./.tmpconfig.h", name)) {
  return 1;
 }
 return 0;
}
static int sym_change_count;
static void (*conf_changed_callback)(void);
void sym_set_change_count(int count)
{
 int _sym_change_count = sym_change_count;
 sym_change_count = count;
 if (conf_changed_callback &&
     (bool)_sym_change_count != (bool)count)
  conf_changed_callback();
}
void sym_add_change_count(int count)
{
 sym_set_change_count(count + sym_change_count);
}
bool conf_get_changed(void)
{
 return sym_change_count;
}
void conf_set_changed_callback(void (*fn)(void))
{
 conf_changed_callback = fn;
}
static void randomize_choice_values(struct symbol *csym)
{
 struct property *prop;
 struct symbol *sym;
 struct expr *e;
 int cnt, def;
 if (csym->curr.tri != yes)
  return;
 prop = sym_get_choice_prop(csym);
 cnt = 0;
 expr_list_for_each_sym(prop->expr, e, sym)
  cnt++;
 def = (rand() % cnt);
 cnt = 0;
 expr_list_for_each_sym(prop->expr, e, sym) {
  if (def == cnt++) {
   sym->def[S_DEF_USER].tri = yes;
   csym->def[S_DEF_USER].val = sym;
  }
  else {
   sym->def[S_DEF_USER].tri = no;
  }
 }
 csym->flags |= SYMBOL_DEF_USER;
 csym->flags &= ~(SYMBOL_VALID);
}
static void set_all_choice_values(struct symbol *csym)
{
 struct property *prop;
 struct symbol *sym;
 struct expr *e;
 prop = sym_get_choice_prop(csym);
 expr_list_for_each_sym(prop->expr, e, sym) {
  if (!sym_has_value(sym))
   sym->def[S_DEF_USER].tri = no;
 }
 csym->flags |= SYMBOL_DEF_USER;
 csym->flags &= ~(SYMBOL_VALID);
}
void conf_set_all_new_symbols(enum conf_def_mode mode)
{
 struct symbol *sym, *csym;
 int i, cnt;
 for_all_symbols(i, sym) {
  if (sym_has_value(sym))
   continue;
  switch (sym_get_type(sym)) {
  case S_BOOLEAN:
  case S_TRISTATE:
   switch (mode) {
   case def_yes:
    sym->def[S_DEF_USER].tri = yes;
    break;
   case def_mod:
    sym->def[S_DEF_USER].tri = mod;
    break;
   case def_no:
    sym->def[S_DEF_USER].tri = no;
    break;
   case def_random:
    cnt = sym_get_type(sym) == S_TRISTATE ? 3 : 2;
    sym->def[S_DEF_USER].tri = (tristate)(rand() % cnt);
    break;
   default:
    continue;
   }
   if (!(sym_is_choice(sym) && mode == def_random))
    sym->flags |= SYMBOL_DEF_USER;
   break;
  default:
   break;
  }
 }
 sym_clear_all_valid();
# 8979 "dialog.all.c"
 for_all_symbols(i, csym) {
  if (sym_has_value(csym) || !sym_is_choice(csym))
   continue;
  sym_calc_value(csym);
  if (mode == def_random)
   randomize_choice_values(csym);
  else
   set_all_choice_values(csym);
 }
}
#define DEBUG_EXPR 0
struct expr *expr_alloc_symbol(struct symbol *sym)
{
 struct expr *e = calloc(1, sizeof(*e));
 e->type = E_SYMBOL;
 e->left.sym = sym;
 return e;
}
struct expr *expr_alloc_one(enum expr_type type, struct expr *ce)
{
 struct expr *e = calloc(1, sizeof(*e));
 e->type = type;
 e->left.expr = ce;
 return e;
}
struct expr *expr_alloc_two(enum expr_type type, struct expr *e1, struct expr *e2)
{
 struct expr *e = calloc(1, sizeof(*e));
 e->type = type;
 e->left.expr = e1;
 e->right.expr = e2;
 return e;
}
struct expr *expr_alloc_comp(enum expr_type type, struct symbol *s1, struct symbol *s2)
{
 struct expr *e = calloc(1, sizeof(*e));
 e->type = type;
 e->left.sym = s1;
 e->right.sym = s2;
 return e;
}
struct expr *expr_alloc_and(struct expr *e1, struct expr *e2)
{
 if (!e1)
  return e2;
 return e2 ? expr_alloc_two(E_AND, e1, e2) : e1;
}
struct expr *expr_alloc_or(struct expr *e1, struct expr *e2)
{
 if (!e1)
  return e2;
 return e2 ? expr_alloc_two(E_OR, e1, e2) : e1;
}
struct expr *expr_copy(const struct expr *org)
{
 struct expr *e;
 if (!org)
  return NULL;
 e = malloc(sizeof(*org));
 memcpy(e, org, sizeof(*org));
 switch (org->type) {
 case E_SYMBOL:
  e->left = org->left;
  break;
 case E_NOT:
  e->left.expr = expr_copy(org->left.expr);
  break;
 case E_EQUAL:
 case E_UNEQUAL:
  e->left.sym = org->left.sym;
  e->right.sym = org->right.sym;
  break;
 case E_AND:
 case E_OR:
 case E_LIST:
  e->left.expr = expr_copy(org->left.expr);
  e->right.expr = expr_copy(org->right.expr);
  break;
 default:
  printf("can't copy type %d\n", e->type);
  free(e);
  e = NULL;
  break;
 }
 return e;
}
void expr_free(struct expr *e)
{
 if (!e)
  return;
 switch (e->type) {
 case E_SYMBOL:
  break;
 case E_NOT:
  expr_free(e->left.expr);
  return;
 case E_EQUAL:
 case E_UNEQUAL:
  break;
 case E_OR:
 case E_AND:
  expr_free(e->left.expr);
  expr_free(e->right.expr);
  break;
 default:
  printf("how to free type %d?\n", e->type);
  break;
 }
 free(e);
}
static int trans_count;
#define e1 (*ep1)
#define e2 (*ep2)
static void __expr_eliminate_eq(enum expr_type type, struct expr **ep1, struct expr **ep2)
{
 if (e1->type == type) {
  __expr_eliminate_eq(type, &e1->left.expr, &e2);
  __expr_eliminate_eq(type, &e1->right.expr, &e2);
  return;
 }
 if (e2->type == type) {
  __expr_eliminate_eq(type, &e1, &e2->left.expr);
  __expr_eliminate_eq(type, &e1, &e2->right.expr);
  return;
 }
 if (e1->type == E_SYMBOL && e2->type == E_SYMBOL &&
     e1->left.sym == e2->left.sym &&
     (e1->left.sym == &symbol_yes || e1->left.sym == &symbol_no))
  return;
 if (!expr_eq(e1, e2))
  return;
 trans_count++;
 expr_free(e1); expr_free(e2);
 switch (type) {
 case E_OR:
  e1 = expr_alloc_symbol(&symbol_no);
  e2 = expr_alloc_symbol(&symbol_no);
  break;
 case E_AND:
  e1 = expr_alloc_symbol(&symbol_yes);
  e2 = expr_alloc_symbol(&symbol_yes);
  break;
 default:
  ;
 }
}
void expr_eliminate_eq(struct expr **ep1, struct expr **ep2)
{
 if (!e1 || !e2)
  return;
 switch (e1->type) {
 case E_OR:
 case E_AND:
  __expr_eliminate_eq(e1->type, ep1, ep2);
 default:
  ;
 }
 if (e1->type != e2->type) switch (e2->type) {
 case E_OR:
 case E_AND:
  __expr_eliminate_eq(e2->type, ep1, ep2);
 default:
  ;
 }
 e1 = expr_eliminate_yn(e1);
 e2 = expr_eliminate_yn(e2);
}
#undef e1
#undef e2
int expr_eq(struct expr *e1, struct expr *e2)
{
 int res, old_count;
 if (e1->type != e2->type)
  return 0;
 switch (e1->type) {
 case E_EQUAL:
 case E_UNEQUAL:
  return e1->left.sym == e2->left.sym && e1->right.sym == e2->right.sym;
 case E_SYMBOL:
  return e1->left.sym == e2->left.sym;
 case E_NOT:
  return expr_eq(e1->left.expr, e2->left.expr);
 case E_AND:
 case E_OR:
  e1 = expr_copy(e1);
  e2 = expr_copy(e2);
  old_count = trans_count;
  expr_eliminate_eq(&e1, &e2);
  res = (e1->type == E_SYMBOL && e2->type == E_SYMBOL &&
         e1->left.sym == e2->left.sym);
  expr_free(e1);
  expr_free(e2);
  trans_count = old_count;
  return res;
 case E_LIST:
 case E_RANGE:
 case E_NONE:
             ;
 }
 if (DEBUG_EXPR) {
  expr_fprint(e1, stdout);
  printf(" = ");
  expr_fprint(e2, stdout);
  printf(" ?\n");
 }
 return 0;
}
struct expr *expr_eliminate_yn(struct expr *e)
{
 struct expr *tmp;
 if (e) switch (e->type) {
 case E_AND:
  e->left.expr = expr_eliminate_yn(e->left.expr);
  e->right.expr = expr_eliminate_yn(e->right.expr);
  if (e->left.expr->type == E_SYMBOL) {
   if (e->left.expr->left.sym == &symbol_no) {
    expr_free(e->left.expr);
    expr_free(e->right.expr);
    e->type = E_SYMBOL;
    e->left.sym = &symbol_no;
    e->right.expr = NULL;
    return e;
   } else if (e->left.expr->left.sym == &symbol_yes) {
    free(e->left.expr);
    tmp = e->right.expr;
    *e = *(e->right.expr);
    free(tmp);
    return e;
   }
  }
  if (e->right.expr->type == E_SYMBOL) {
   if (e->right.expr->left.sym == &symbol_no) {
    expr_free(e->left.expr);
    expr_free(e->right.expr);
    e->type = E_SYMBOL;
    e->left.sym = &symbol_no;
    e->right.expr = NULL;
    return e;
   } else if (e->right.expr->left.sym == &symbol_yes) {
    free(e->right.expr);
    tmp = e->left.expr;
    *e = *(e->left.expr);
    free(tmp);
    return e;
   }
  }
  break;
 case E_OR:
  e->left.expr = expr_eliminate_yn(e->left.expr);
  e->right.expr = expr_eliminate_yn(e->right.expr);
  if (e->left.expr->type == E_SYMBOL) {
   if (e->left.expr->left.sym == &symbol_no) {
    free(e->left.expr);
    tmp = e->right.expr;
    *e = *(e->right.expr);
    free(tmp);
    return e;
   } else if (e->left.expr->left.sym == &symbol_yes) {
    expr_free(e->left.expr);
    expr_free(e->right.expr);
    e->type = E_SYMBOL;
    e->left.sym = &symbol_yes;
    e->right.expr = NULL;
    return e;
   }
  }
  if (e->right.expr->type == E_SYMBOL) {
   if (e->right.expr->left.sym == &symbol_no) {
    free(e->right.expr);
    tmp = e->left.expr;
    *e = *(e->left.expr);
    free(tmp);
    return e;
   } else if (e->right.expr->left.sym == &symbol_yes) {
    expr_free(e->left.expr);
    expr_free(e->right.expr);
    e->type = E_SYMBOL;
    e->left.sym = &symbol_yes;
    e->right.expr = NULL;
    return e;
   }
  }
  break;
 default:
  ;
 }
 return e;
}
struct expr *expr_trans_bool(struct expr *e)
{
 if (!e)
  return NULL;
 switch (e->type) {
 case E_AND:
 case E_OR:
 case E_NOT:
  e->left.expr = expr_trans_bool(e->left.expr);
  e->right.expr = expr_trans_bool(e->right.expr);
  break;
 case E_UNEQUAL:
  if (e->left.sym->type == S_TRISTATE) {
   if (e->right.sym == &symbol_no) {
    e->type = E_SYMBOL;
    e->right.sym = NULL;
   }
  }
  break;
 default:
  ;
 }
 return e;
}
static struct expr *expr_join_or(struct expr *e1, struct expr *e2)
{
 struct expr *tmp;
 struct symbol *sym1, *sym2;
 if (expr_eq(e1, e2))
  return expr_copy(e1);
 if (e1->type != E_EQUAL && e1->type != E_UNEQUAL && e1->type != E_SYMBOL && e1->type != E_NOT)
  return NULL;
 if (e2->type != E_EQUAL && e2->type != E_UNEQUAL && e2->type != E_SYMBOL && e2->type != E_NOT)
  return NULL;
 if (e1->type == E_NOT) {
  tmp = e1->left.expr;
  if (tmp->type != E_EQUAL && tmp->type != E_UNEQUAL && tmp->type != E_SYMBOL)
   return NULL;
  sym1 = tmp->left.sym;
 } else
  sym1 = e1->left.sym;
 if (e2->type == E_NOT) {
  if (e2->left.expr->type != E_SYMBOL)
   return NULL;
  sym2 = e2->left.expr->left.sym;
 } else
  sym2 = e2->left.sym;
 if (sym1 != sym2)
  return NULL;
 if (sym1->type != S_BOOLEAN && sym1->type != S_TRISTATE)
  return NULL;
 if (sym1->type == S_TRISTATE) {
  if (e1->type == E_EQUAL && e2->type == E_EQUAL &&
      ((e1->right.sym == &symbol_yes && e2->right.sym == &symbol_mod) ||
       (e1->right.sym == &symbol_mod && e2->right.sym == &symbol_yes))) {
   return expr_alloc_comp(E_UNEQUAL, sym1, &symbol_no);
  }
  if (e1->type == E_EQUAL && e2->type == E_EQUAL &&
      ((e1->right.sym == &symbol_yes && e2->right.sym == &symbol_no) ||
       (e1->right.sym == &symbol_no && e2->right.sym == &symbol_yes))) {
   return expr_alloc_comp(E_UNEQUAL, sym1, &symbol_mod);
  }
  if (e1->type == E_EQUAL && e2->type == E_EQUAL &&
      ((e1->right.sym == &symbol_mod && e2->right.sym == &symbol_no) ||
       (e1->right.sym == &symbol_no && e2->right.sym == &symbol_mod))) {
   return expr_alloc_comp(E_UNEQUAL, sym1, &symbol_yes);
  }
 }
 if (sym1->type == S_BOOLEAN && sym1 == sym2) {
  if ((e1->type == E_NOT && e1->left.expr->type == E_SYMBOL && e2->type == E_SYMBOL) ||
      (e2->type == E_NOT && e2->left.expr->type == E_SYMBOL && e1->type == E_SYMBOL))
   return expr_alloc_symbol(&symbol_yes);
 }
 if (DEBUG_EXPR) {
  printf("optimize (");
  expr_fprint(e1, stdout);
  printf(") || (");
  expr_fprint(e2, stdout);
  printf(")?\n");
 }
 return NULL;
}
static struct expr *expr_join_and(struct expr *e1, struct expr *e2)
{
 struct expr *tmp;
 struct symbol *sym1, *sym2;
 if (expr_eq(e1, e2))
  return expr_copy(e1);
 if (e1->type != E_EQUAL && e1->type != E_UNEQUAL && e1->type != E_SYMBOL && e1->type != E_NOT)
  return NULL;
 if (e2->type != E_EQUAL && e2->type != E_UNEQUAL && e2->type != E_SYMBOL && e2->type != E_NOT)
  return NULL;
 if (e1->type == E_NOT) {
  tmp = e1->left.expr;
  if (tmp->type != E_EQUAL && tmp->type != E_UNEQUAL && tmp->type != E_SYMBOL)
   return NULL;
  sym1 = tmp->left.sym;
 } else
  sym1 = e1->left.sym;
 if (e2->type == E_NOT) {
  if (e2->left.expr->type != E_SYMBOL)
   return NULL;
  sym2 = e2->left.expr->left.sym;
 } else
  sym2 = e2->left.sym;
 if (sym1 != sym2)
  return NULL;
 if (sym1->type != S_BOOLEAN && sym1->type != S_TRISTATE)
  return NULL;
 if ((e1->type == E_SYMBOL && e2->type == E_EQUAL && e2->right.sym == &symbol_yes) ||
     (e2->type == E_SYMBOL && e1->type == E_EQUAL && e1->right.sym == &symbol_yes))
  return expr_alloc_comp(E_EQUAL, sym1, &symbol_yes);
 if ((e1->type == E_SYMBOL && e2->type == E_UNEQUAL && e2->right.sym == &symbol_no) ||
     (e2->type == E_SYMBOL && e1->type == E_UNEQUAL && e1->right.sym == &symbol_no))
  return expr_alloc_symbol(sym1);
 if ((e1->type == E_SYMBOL && e2->type == E_UNEQUAL && e2->right.sym == &symbol_mod) ||
     (e2->type == E_SYMBOL && e1->type == E_UNEQUAL && e1->right.sym == &symbol_mod))
  return expr_alloc_comp(E_EQUAL, sym1, &symbol_yes);
 if (sym1->type == S_TRISTATE) {
  if (e1->type == E_EQUAL && e2->type == E_UNEQUAL) {
   sym2 = e1->right.sym;
   if ((e2->right.sym->flags & SYMBOL_CONST) && (sym2->flags & SYMBOL_CONST))
    return sym2 != e2->right.sym ? expr_alloc_comp(E_EQUAL, sym1, sym2)
            : expr_alloc_symbol(&symbol_no);
  }
  if (e1->type == E_UNEQUAL && e2->type == E_EQUAL) {
   sym2 = e2->right.sym;
   if ((e1->right.sym->flags & SYMBOL_CONST) && (sym2->flags & SYMBOL_CONST))
    return sym2 != e1->right.sym ? expr_alloc_comp(E_EQUAL, sym1, sym2)
            : expr_alloc_symbol(&symbol_no);
  }
  if (e1->type == E_UNEQUAL && e2->type == E_UNEQUAL &&
      ((e1->right.sym == &symbol_yes && e2->right.sym == &symbol_no) ||
       (e1->right.sym == &symbol_no && e2->right.sym == &symbol_yes)))
   return expr_alloc_comp(E_EQUAL, sym1, &symbol_mod);
  if (e1->type == E_UNEQUAL && e2->type == E_UNEQUAL &&
      ((e1->right.sym == &symbol_yes && e2->right.sym == &symbol_mod) ||
       (e1->right.sym == &symbol_mod && e2->right.sym == &symbol_yes)))
   return expr_alloc_comp(E_EQUAL, sym1, &symbol_no);
  if (e1->type == E_UNEQUAL && e2->type == E_UNEQUAL &&
      ((e1->right.sym == &symbol_mod && e2->right.sym == &symbol_no) ||
       (e1->right.sym == &symbol_no && e2->right.sym == &symbol_mod)))
   return expr_alloc_comp(E_EQUAL, sym1, &symbol_yes);
  if ((e1->type == E_SYMBOL && e2->type == E_EQUAL && e2->right.sym == &symbol_mod) ||
      (e2->type == E_SYMBOL && e1->type == E_EQUAL && e1->right.sym == &symbol_mod) ||
      (e1->type == E_SYMBOL && e2->type == E_UNEQUAL && e2->right.sym == &symbol_yes) ||
      (e2->type == E_SYMBOL && e1->type == E_UNEQUAL && e1->right.sym == &symbol_yes))
   return NULL;
 }
 if (DEBUG_EXPR) {
  printf("optimize (");
  expr_fprint(e1, stdout);
  printf(") && (");
  expr_fprint(e2, stdout);
  printf(")?\n");
 }
 return NULL;
}
static void expr_eliminate_dups1(enum expr_type type, struct expr **ep1, struct expr **ep2)
{
#define e1 (*ep1)
#define e2 (*ep2)
 struct expr *tmp;
 if (e1->type == type) {
  expr_eliminate_dups1(type, &e1->left.expr, &e2);
  expr_eliminate_dups1(type, &e1->right.expr, &e2);
  return;
 }
 if (e2->type == type) {
  expr_eliminate_dups1(type, &e1, &e2->left.expr);
  expr_eliminate_dups1(type, &e1, &e2->right.expr);
  return;
 }
 if (e1 == e2)
  return;
 switch (e1->type) {
 case E_OR: case E_AND:
  expr_eliminate_dups1(e1->type, &e1, &e1);
 default:
  ;
 }
 switch (type) {
 case E_OR:
  tmp = expr_join_or(e1, e2);
  if (tmp) {
   expr_free(e1); expr_free(e2);
   e1 = expr_alloc_symbol(&symbol_no);
   e2 = tmp;
   trans_count++;
  }
  break;
 case E_AND:
  tmp = expr_join_and(e1, e2);
  if (tmp) {
   expr_free(e1); expr_free(e2);
   e1 = expr_alloc_symbol(&symbol_yes);
   e2 = tmp;
   trans_count++;
  }
  break;
 default:
  ;
 }
#undef e1
#undef e2
}
static void expr_eliminate_dups2(enum expr_type type, struct expr **ep1, struct expr **ep2)
{
#define e1 (*ep1)
#define e2 (*ep2)
 struct expr *tmp, *tmp1, *tmp2;
 if (e1->type == type) {
  expr_eliminate_dups2(type, &e1->left.expr, &e2);
  expr_eliminate_dups2(type, &e1->right.expr, &e2);
  return;
 }
 if (e2->type == type) {
  expr_eliminate_dups2(type, &e1, &e2->left.expr);
  expr_eliminate_dups2(type, &e1, &e2->right.expr);
 }
 if (e1 == e2)
  return;
 switch (e1->type) {
 case E_OR:
  expr_eliminate_dups2(e1->type, &e1, &e1);
  tmp1 = expr_transform(expr_alloc_one(E_NOT, expr_copy(e1)));
  tmp2 = expr_copy(e2);
  tmp = expr_extract_eq_and(&tmp1, &tmp2);
  if (expr_is_yes(tmp1)) {
   expr_free(e1);
   e1 = expr_alloc_symbol(&symbol_no);
   trans_count++;
  }
  expr_free(tmp2);
  expr_free(tmp1);
  expr_free(tmp);
  break;
 case E_AND:
  expr_eliminate_dups2(e1->type, &e1, &e1);
  tmp1 = expr_transform(expr_alloc_one(E_NOT, expr_copy(e1)));
  tmp2 = expr_copy(e2);
  tmp = expr_extract_eq_or(&tmp1, &tmp2);
  if (expr_is_no(tmp1)) {
   expr_free(e1);
   e1 = expr_alloc_symbol(&symbol_yes);
   trans_count++;
  }
  expr_free(tmp2);
  expr_free(tmp1);
  expr_free(tmp);
  break;
 default:
  ;
 }
#undef e1
#undef e2
}
struct expr *expr_eliminate_dups(struct expr *e)
{
 int oldcount;
 if (!e)
  return e;
 oldcount = trans_count;
 while (1) {
  trans_count = 0;
  switch (e->type) {
  case E_OR: case E_AND:
   expr_eliminate_dups1(e->type, &e, &e);
   expr_eliminate_dups2(e->type, &e, &e);
  default:
   ;
  }
  if (!trans_count)
   break;
  e = expr_eliminate_yn(e);
 }
 trans_count = oldcount;
 return e;
}
struct expr *expr_transform(struct expr *e)
{
 struct expr *tmp;
 if (!e)
  return NULL;
 switch (e->type) {
 case E_EQUAL:
 case E_UNEQUAL:
 case E_SYMBOL:
 case E_LIST:
  break;
 default:
  e->left.expr = expr_transform(e->left.expr);
  e->right.expr = expr_transform(e->right.expr);
 }
 switch (e->type) {
 case E_EQUAL:
  if (e->left.sym->type != S_BOOLEAN)
   break;
  if (e->right.sym == &symbol_no) {
   e->type = E_NOT;
   e->left.expr = expr_alloc_symbol(e->left.sym);
   e->right.sym = NULL;
   break;
  }
  if (e->right.sym == &symbol_mod) {
   printf("boolean symbol %s tested for 'm/2'? test forced to 'n/0'\n", e->left.sym->name);
   e->type = E_SYMBOL;
   e->left.sym = &symbol_no;
   e->right.sym = NULL;
   break;
  }
  if (e->right.sym == &symbol_yes) {
   e->type = E_SYMBOL;
   e->right.sym = NULL;
   break;
  }
  break;
 case E_UNEQUAL:
  if (e->left.sym->type != S_BOOLEAN)
   break;
  if (e->right.sym == &symbol_no) {
   e->type = E_SYMBOL;
   e->right.sym = NULL;
   break;
  }
  if (e->right.sym == &symbol_mod) {
   printf("boolean symbol %s tested for 'm'? test forced to 'y'\n", e->left.sym->name);
   e->type = E_SYMBOL;
   e->left.sym = &symbol_yes;
   e->right.sym = NULL;
   break;
  }
  if (e->right.sym == &symbol_yes) {
   e->type = E_NOT;
   e->left.expr = expr_alloc_symbol(e->left.sym);
   e->right.sym = NULL;
   break;
  }
  break;
 case E_NOT:
  switch (e->left.expr->type) {
  case E_NOT:
   tmp = e->left.expr->left.expr;
   free(e->left.expr);
   free(e);
   e = tmp;
   e = expr_transform(e);
   break;
  case E_EQUAL:
  case E_UNEQUAL:
   tmp = e->left.expr;
   free(e);
   e = tmp;
   e->type = e->type == E_EQUAL ? E_UNEQUAL : E_EQUAL;
   break;
  case E_OR:
   tmp = e->left.expr;
   e->type = E_AND;
   e->right.expr = expr_alloc_one(E_NOT, tmp->right.expr);
   tmp->type = E_NOT;
   tmp->right.expr = NULL;
   e = expr_transform(e);
   break;
  case E_AND:
   tmp = e->left.expr;
   e->type = E_OR;
   e->right.expr = expr_alloc_one(E_NOT, tmp->right.expr);
   tmp->type = E_NOT;
   tmp->right.expr = NULL;
   e = expr_transform(e);
   break;
  case E_SYMBOL:
   if (e->left.expr->left.sym == &symbol_yes) {
    tmp = e->left.expr;
    free(e);
    e = tmp;
    e->type = E_SYMBOL;
    e->left.sym = &symbol_no;
    break;
   }
   if (e->left.expr->left.sym == &symbol_mod) {
    tmp = e->left.expr;
    free(e);
    e = tmp;
    e->type = E_SYMBOL;
    e->left.sym = &symbol_mod;
    break;
   }
   if (e->left.expr->left.sym == &symbol_no) {
    tmp = e->left.expr;
    free(e);
    e = tmp;
    e->type = E_SYMBOL;
    e->left.sym = &symbol_yes;
    break;
   }
   break;
  default:
   ;
  }
  break;
 default:
  ;
 }
 return e;
}
int expr_contains_symbol(struct expr *dep, struct symbol *sym)
{
 if (!dep)
  return 0;
 switch (dep->type) {
 case E_AND:
 case E_OR:
  return expr_contains_symbol(dep->left.expr, sym) ||
         expr_contains_symbol(dep->right.expr, sym);
 case E_SYMBOL:
  return dep->left.sym == sym;
 case E_EQUAL:
 case E_UNEQUAL:
  return dep->left.sym == sym ||
         dep->right.sym == sym;
 case E_NOT:
  return expr_contains_symbol(dep->left.expr, sym);
 default:
  ;
 }
 return 0;
}
bool expr_depends_symbol(struct expr *dep, struct symbol *sym)
{
 if (!dep)
  return false;
 switch (dep->type) {
 case E_AND:
  return expr_depends_symbol(dep->left.expr, sym) ||
         expr_depends_symbol(dep->right.expr, sym);
 case E_SYMBOL:
  return dep->left.sym == sym;
 case E_EQUAL:
  if (dep->left.sym == sym) {
   if (dep->right.sym == &symbol_yes || dep->right.sym == &symbol_mod)
    return true;
  }
  break;
 case E_UNEQUAL:
  if (dep->left.sym == sym) {
   if (dep->right.sym == &symbol_no)
    return true;
  }
  break;
 default:
  ;
 }
  return false;
}
struct expr *expr_extract_eq_and(struct expr **ep1, struct expr **ep2)
{
 struct expr *tmp = NULL;
 expr_extract_eq(E_AND, &tmp, ep1, ep2);
 if (tmp) {
  *ep1 = expr_eliminate_yn(*ep1);
  *ep2 = expr_eliminate_yn(*ep2);
 }
 return tmp;
}
struct expr *expr_extract_eq_or(struct expr **ep1, struct expr **ep2)
{
 struct expr *tmp = NULL;
 expr_extract_eq(E_OR, &tmp, ep1, ep2);
 if (tmp) {
  *ep1 = expr_eliminate_yn(*ep1);
  *ep2 = expr_eliminate_yn(*ep2);
 }
 return tmp;
}
void expr_extract_eq(enum expr_type type, struct expr **ep, struct expr **ep1, struct expr **ep2)
{
#define e1 (*ep1)
#define e2 (*ep2)
 if (e1->type == type) {
  expr_extract_eq(type, ep, &e1->left.expr, &e2);
  expr_extract_eq(type, ep, &e1->right.expr, &e2);
  return;
 }
 if (e2->type == type) {
  expr_extract_eq(type, ep, ep1, &e2->left.expr);
  expr_extract_eq(type, ep, ep1, &e2->right.expr);
  return;
 }
 if (expr_eq(e1, e2)) {
  *ep = *ep ? expr_alloc_two(type, *ep, e1) : e1;
  expr_free(e2);
  if (type == E_AND) {
   e1 = expr_alloc_symbol(&symbol_yes);
   e2 = expr_alloc_symbol(&symbol_yes);
  } else if (type == E_OR) {
   e1 = expr_alloc_symbol(&symbol_no);
   e2 = expr_alloc_symbol(&symbol_no);
  }
 }
#undef e1
#undef e2
}
struct expr *expr_trans_compare(struct expr *e, enum expr_type type, struct symbol *sym)
{
 struct expr *e1, *e2;
 if (!e) {
  e = expr_alloc_symbol(sym);
  if (type == E_UNEQUAL)
   e = expr_alloc_one(E_NOT, e);
  return e;
 }
 switch (e->type) {
 case E_AND:
  e1 = expr_trans_compare(e->left.expr, E_EQUAL, sym);
  e2 = expr_trans_compare(e->right.expr, E_EQUAL, sym);
  if (sym == &symbol_yes)
   e = expr_alloc_two(E_AND, e1, e2);
  if (sym == &symbol_no)
   e = expr_alloc_two(E_OR, e1, e2);
  if (type == E_UNEQUAL)
   e = expr_alloc_one(E_NOT, e);
  return e;
 case E_OR:
  e1 = expr_trans_compare(e->left.expr, E_EQUAL, sym);
  e2 = expr_trans_compare(e->right.expr, E_EQUAL, sym);
  if (sym == &symbol_yes)
   e = expr_alloc_two(E_OR, e1, e2);
  if (sym == &symbol_no)
   e = expr_alloc_two(E_AND, e1, e2);
  if (type == E_UNEQUAL)
   e = expr_alloc_one(E_NOT, e);
  return e;
 case E_NOT:
  return expr_trans_compare(e->left.expr, type == E_EQUAL ? E_UNEQUAL : E_EQUAL, sym);
 case E_UNEQUAL:
 case E_EQUAL:
  if (type == E_EQUAL) {
   if (sym == &symbol_yes)
    return expr_copy(e);
   if (sym == &symbol_mod)
    return expr_alloc_symbol(&symbol_no);
   if (sym == &symbol_no)
    return expr_alloc_one(E_NOT, expr_copy(e));
  } else {
   if (sym == &symbol_yes)
    return expr_alloc_one(E_NOT, expr_copy(e));
   if (sym == &symbol_mod)
    return expr_alloc_symbol(&symbol_yes);
   if (sym == &symbol_no)
    return expr_copy(e);
  }
  break;
 case E_SYMBOL:
  return expr_alloc_comp(type, e->left.sym, sym);
 case E_LIST:
 case E_RANGE:
 case E_NONE:
             ;
 }
 return NULL;
}
tristate expr_calc_value(struct expr *e)
{
 tristate val1, val2;
 const char *str1, *str2;
 if (!e)
  return yes;
 switch (e->type) {
 case E_SYMBOL:
  sym_calc_value(e->left.sym);
  return e->left.sym->curr.tri;
 case E_AND:
  val1 = expr_calc_value(e->left.expr);
  val2 = expr_calc_value(e->right.expr);
  return EXPR_AND(val1, val2);
 case E_OR:
  val1 = expr_calc_value(e->left.expr);
  val2 = expr_calc_value(e->right.expr);
  return EXPR_OR(val1, val2);
 case E_NOT:
  val1 = expr_calc_value(e->left.expr);
  return EXPR_NOT(val1);
 case E_EQUAL:
  sym_calc_value(e->left.sym);
  sym_calc_value(e->right.sym);
  str1 = sym_get_string_value(e->left.sym);
  str2 = sym_get_string_value(e->right.sym);
  return !strcmp(str1, str2) ? yes : no;
 case E_UNEQUAL:
  sym_calc_value(e->left.sym);
  sym_calc_value(e->right.sym);
  str1 = sym_get_string_value(e->left.sym);
  str2 = sym_get_string_value(e->right.sym);
  return !strcmp(str1, str2) ? no : yes;
 default:
  printf("expr_calc_value: %d?\n", e->type);
  return no;
 }
}
int expr_compare_type(enum expr_type t1, enum expr_type t2)
{
#if 0
 return 1;
#else
 if (t1 == t2)
  return 0;
 switch (t1) {
 case E_EQUAL:
 case E_UNEQUAL:
  if (t2 == E_NOT)
   return 1;
 case E_NOT:
  if (t2 == E_AND)
   return 1;
 case E_AND:
  if (t2 == E_OR)
   return 1;
 case E_OR:
  if (t2 == E_LIST)
   return 1;
 case E_LIST:
  if (t2 == 0)
   return 1;
 default:
  return -1;
 }
 printf("[%dgt%d?]", t1, t2);
 return 0;
#endif
}
static inline struct expr *
expr_get_leftmost_symbol(const struct expr *e)
{
 if (e == NULL)
  return NULL;
 while (e->type != E_SYMBOL)
  e = e->left.expr;
 return expr_copy(e);
}
struct expr *expr_simplify_unmet_dep(struct expr *e1, struct expr *e2)
{
 struct expr *ret;
 switch (e1->type) {
 case E_OR:
  return expr_alloc_and(
      expr_simplify_unmet_dep(e1->left.expr, e2),
      expr_simplify_unmet_dep(e1->right.expr, e2));
 case E_AND: {
  struct expr *e;
  e = expr_alloc_and(expr_copy(e1), expr_copy(e2));
  e = expr_eliminate_dups(e);
  ret = (!expr_eq(e, e1)) ? e1 : NULL;
  expr_free(e);
  break;
  }
 default:
  ret = e1;
  break;
 }
 return expr_get_leftmost_symbol(ret);
}
void expr_print(struct expr *e, void (*fn)(void *, struct symbol *, const char *), void *data, int prevtoken)
{
 if (!e) {
  fn(data, NULL, "y");
  return;
 }
 if (expr_compare_type(prevtoken, e->type) > 0)
  fn(data, NULL, "(");
 switch (e->type) {
 case E_SYMBOL:
  if (e->left.sym->name)
   fn(data, e->left.sym, e->left.sym->name);
  else
   fn(data, NULL, "<choice>");
  break;
 case E_NOT:
  fn(data, NULL, "!");
  expr_print(e->left.expr, fn, data, E_NOT);
  break;
 case E_EQUAL:
  if (e->left.sym->name)
   fn(data, e->left.sym, e->left.sym->name);
  else
   fn(data, NULL, "<choice>");
  fn(data, NULL, "=");
  fn(data, e->right.sym, e->right.sym->name);
  break;
 case E_UNEQUAL:
  if (e->left.sym->name)
   fn(data, e->left.sym, e->left.sym->name);
  else
   fn(data, NULL, "<choice>");
  fn(data, NULL, "!=");
  fn(data, e->right.sym, e->right.sym->name);
  break;
 case E_OR:
  expr_print(e->left.expr, fn, data, E_OR);
  fn(data, NULL, " || ");
  expr_print(e->right.expr, fn, data, E_OR);
  break;
 case E_AND:
  expr_print(e->left.expr, fn, data, E_AND);
  fn(data, NULL, " && ");
  expr_print(e->right.expr, fn, data, E_AND);
  break;
 case E_LIST:
  fn(data, e->right.sym, e->right.sym->name);
  if (e->left.expr) {
   fn(data, NULL, " ^ ");
   expr_print(e->left.expr, fn, data, E_LIST);
  }
  break;
 case E_RANGE:
  fn(data, NULL, "[");
  fn(data, e->left.sym, e->left.sym->name);
  fn(data, NULL, " ");
  fn(data, e->right.sym, e->right.sym->name);
  fn(data, NULL, "]");
  break;
 default:
   {
  char buf[32];
  sprintf(buf, "<unknown type %d>", e->type);
  fn(data, NULL, buf);
  break;
   }
 }
 if (expr_compare_type(prevtoken, e->type) > 0)
  fn(data, NULL, ")");
}
static void expr_print_file_helper(void *data, struct symbol *sym, const char *str)
{
 xfwrite(str, strlen(str), 1, data);
}
void expr_fprint(struct expr *e, FILE *out)
{
 expr_print(e, expr_print_file_helper, out, E_NONE);
}
static void expr_print_gstr_helper(void *data, struct symbol *sym, const char *str)
{
 struct gstr *gs = (struct gstr*)data;
 const char *sym_str = NULL;
 if (sym)
  sym_str = sym_get_string_value(sym);
 if (gs->max_width) {
  unsigned extra_length = strlen(str);
  const char *last_cr = strrchr(gs->s, '\n');
  unsigned last_line_length;
  if (sym_str)
   extra_length += 4 + strlen(sym_str);
  if (!last_cr)
   last_cr = gs->s;
  last_line_length = strlen(gs->s) - (last_cr - gs->s);
  if ((last_line_length + extra_length) > gs->max_width)
   str_append(gs, "\\\n");
 }
 str_append(gs, str);
 if (sym && sym->type != S_UNKNOWN)
  str_printf(gs, " [=%s]", sym_str);
}
void expr_gstr_print(struct expr *e, struct gstr *gs)
{
 expr_print(e, expr_print_gstr_helper, gs, E_NONE);
}
struct symbol symbol_yes = {
 .name = "1",
 .curr = { "1", yes },
 .flags = SYMBOL_CONST|SYMBOL_VALID,
}, symbol_mod = {
 .name = "2",
 .curr = { "2", mod },
 .flags = SYMBOL_CONST|SYMBOL_VALID,
}, symbol_no = {
 .name = "0",
 .curr = { "0", no },
 .flags = SYMBOL_CONST|SYMBOL_VALID,
}, symbol_empty = {
 .name = "",
 .curr = { "", no },
 .flags = SYMBOL_VALID,
};
struct symbol *sym_defconfig_list;
struct symbol *modules_sym;
tristate modules_val;
static void sym_add_default(struct symbol *sym, const char *def)
{
 struct property *prop = prop_alloc(P_DEFAULT, sym);
 prop->expr = expr_alloc_symbol(sym_lookup(def, SYMBOL_CONST));
}
void sym_init(void)
{
 struct symbol *sym;
 struct utsname uts;
 static bool inited = false;
 if (inited)
  return;
 inited = true;
 uname(&uts);
 sym = sym_lookup("UNAME_RELEASE", 0);
 sym->type = S_STRING;
 sym->flags |= SYMBOL_AUTO;
 sym_add_default(sym, uts.release);
}
enum symbol_type sym_get_type(struct symbol *sym)
{
 enum symbol_type type = sym->type;
 if (type == S_TRISTATE) {
  if (sym_is_choice_value(sym) && sym->visible == yes)
   type = S_BOOLEAN;
  else if (modules_val == no)
   type = S_BOOLEAN;
 }
 return type;
}
const char *sym_type_name(enum symbol_type type)
{
 switch (type) {
 case S_BOOLEAN:
  return "boolean";
 case S_TRISTATE:
  return "tristate";
 case S_INT:
  return "integer";
 case S_HEX:
  return "hex";
 case S_STRING:
  return "string";
 case S_UNKNOWN:
  return "unknown";
 case S_OTHER:
  break;
 }
 return "???";
}
struct property *sym_get_choice_prop(struct symbol *sym)
{
 struct property *prop;
 for_all_choices(sym, prop)
  return prop;
 return NULL;
}
struct property *sym_get_env_prop(struct symbol *sym)
{
 struct property *prop;
 for_all_properties(sym, prop, P_ENV)
  return prop;
 return NULL;
}
struct property *sym_get_default_prop(struct symbol *sym)
{
 struct property *prop;
 for_all_defaults(sym, prop) {
  prop->visible.tri = expr_calc_value(prop->visible.expr);
  if (prop->visible.tri != no)
   return prop;
 }
 return NULL;
}
static struct property *sym_get_range_prop(struct symbol *sym)
{
 struct property *prop;
 for_all_properties(sym, prop, P_RANGE) {
  prop->visible.tri = expr_calc_value(prop->visible.expr);
  if (prop->visible.tri != no)
   return prop;
 }
 return NULL;
}
static int sym_get_range_val(struct symbol *sym, int base)
{
 sym_calc_value(sym);
 switch (sym->type) {
 case S_INT:
  base = 10;
  break;
 case S_HEX:
  base = 16;
  break;
 default:
  break;
 }
 return strtol(sym->curr.val, NULL, base);
}
static void sym_validate_range(struct symbol *sym)
{
 struct property *prop;
 int base, val, val2;
 char str[64];
 switch (sym->type) {
 case S_INT:
  base = 10;
  break;
 case S_HEX:
  base = 16;
  break;
 default:
  return;
 }
 prop = sym_get_range_prop(sym);
 if (!prop)
  return;
 val = strtol(sym->curr.val, NULL, base);
 val2 = sym_get_range_val(prop->expr->left.sym, base);
 if (val >= val2) {
  val2 = sym_get_range_val(prop->expr->right.sym, base);
  if (val <= val2)
   return;
 }
 if (sym->type == S_INT)
  sprintf(str, "%d", val2);
 else
  sprintf(str, "0x%x", val2);
 sym->curr.val = strdup(str);
}
static void sym_calc_visibility(struct symbol *sym)
{
 struct property *prop;
 tristate tri;
 tri = no;
 for_all_prompts(sym, prop) {
  prop->visible.tri = expr_calc_value(prop->visible.expr);
  tri = EXPR_OR(tri, prop->visible.tri);
 }
 if (tri == mod && (sym->type != S_TRISTATE || modules_val == no))
  tri = yes;
 if (sym->visible != tri) {
  sym->visible = tri;
  sym_set_changed(sym);
 }
 if (sym_is_choice_value(sym))
  return;
 tri = yes;
 if (sym->dir_dep.expr)
  tri = expr_calc_value(sym->dir_dep.expr);
 if (tri == mod)
  tri = yes;
 if (sym->dir_dep.tri != tri) {
  sym->dir_dep.tri = tri;
  sym_set_changed(sym);
 }
 tri = no;
 if (sym->rev_dep.expr)
  tri = expr_calc_value(sym->rev_dep.expr);
 if (tri == mod && sym_get_type(sym) == S_BOOLEAN)
  tri = yes;
 if (sym->rev_dep.tri != tri) {
  sym->rev_dep.tri = tri;
  sym_set_changed(sym);
 }
}
struct symbol *sym_choice_default(struct symbol *sym)
{
 struct symbol *def_sym;
 struct property *prop;
 struct expr *e;
 for_all_defaults(sym, prop) {
  prop->visible.tri = expr_calc_value(prop->visible.expr);
  if (prop->visible.tri == no)
   continue;
  def_sym = prop_get_symbol(prop);
  if (def_sym->visible != no)
   return def_sym;
 }
 prop = sym_get_choice_prop(sym);
 expr_list_for_each_sym(prop->expr, e, def_sym)
  if (def_sym->visible != no)
   return def_sym;
 return NULL;
}
static struct symbol *sym_calc_choice(struct symbol *sym)
{
 struct symbol *def_sym;
 struct property *prop;
 struct expr *e;
 int flags;
 flags = sym->flags;
 prop = sym_get_choice_prop(sym);
 expr_list_for_each_sym(prop->expr, e, def_sym) {
  sym_calc_visibility(def_sym);
  if (def_sym->visible != no)
   flags &= def_sym->flags;
 }
 sym->flags &= flags | ~SYMBOL_DEF_USER;
 def_sym = sym->def[S_DEF_USER].val;
 if (def_sym && def_sym->visible != no)
  return def_sym;
 def_sym = sym_choice_default(sym);
 if (def_sym == NULL)
  sym->curr.tri = no;
 return def_sym;
}
void sym_calc_value(struct symbol *sym)
{
 struct symbol_value newval, oldval;
 struct property *prop;
 struct expr *e;
 if (!sym)
  return;
 if (sym->flags & SYMBOL_VALID)
  return;
 sym->flags |= SYMBOL_VALID;
 oldval = sym->curr;
 switch (sym->type) {
 case S_INT:
 case S_HEX:
 case S_STRING:
  newval = symbol_empty.curr;
  break;
 case S_BOOLEAN:
 case S_TRISTATE:
  newval = symbol_no.curr;
  break;
 default:
  sym->curr.val = sym->name;
  sym->curr.tri = no;
  return;
 }
 if (!sym_is_choice_value(sym))
  sym->flags &= ~SYMBOL_WRITE;
 sym_calc_visibility(sym);
 sym->curr = newval;
 switch (sym_get_type(sym)) {
 case S_BOOLEAN:
 case S_TRISTATE:
  if (sym_is_choice_value(sym) && sym->visible == yes) {
   prop = sym_get_choice_prop(sym);
   newval.tri = (prop_get_symbol(prop)->curr.val == sym) ? yes : no;
  } else {
   if (sym->visible != no) {
    sym->flags |= SYMBOL_WRITE;
    if (sym_has_value(sym)) {
     newval.tri = EXPR_AND(sym->def[S_DEF_USER].tri,
             sym->visible);
     goto calc_newval;
    }
   }
   if (sym->rev_dep.tri != no)
    sym->flags |= SYMBOL_WRITE;
   if (!sym_is_choice(sym)) {
    prop = sym_get_default_prop(sym);
    if (prop) {
     sym->flags |= SYMBOL_WRITE;
     newval.tri = EXPR_AND(expr_calc_value(prop->expr),
             prop->visible.tri);
    }
   }
  calc_newval:
   if (sym->dir_dep.tri == no && sym->rev_dep.tri != no) {
    struct expr *e;
    e = expr_simplify_unmet_dep(sym->rev_dep.expr,
        sym->dir_dep.expr);
    fprintf(stderr, "warning: (");
    expr_fprint(e, stderr);
    fprintf(stderr, ") selects %s which has unmet direct dependencies (",
     sym->name);
    expr_fprint(sym->dir_dep.expr, stderr);
    fprintf(stderr, ")\n");
    expr_free(e);
   }
   newval.tri = EXPR_OR(newval.tri, sym->rev_dep.tri);
  }
  if (newval.tri == mod && sym_get_type(sym) == S_BOOLEAN)
   newval.tri = yes;
  break;
 case S_STRING:
 case S_HEX:
 case S_INT:
  if (sym->visible != no) {
   sym->flags |= SYMBOL_WRITE;
   if (sym_has_value(sym)) {
    newval.val = sym->def[S_DEF_USER].val;
    break;
   }
  }
  prop = sym_get_default_prop(sym);
  if (prop) {
   struct symbol *ds = prop_get_symbol(prop);
   if (ds) {
    sym->flags |= SYMBOL_WRITE;
    sym_calc_value(ds);
    newval.val = ds->curr.val;
   }
  }
  break;
 default:
  ;
 }
 sym->curr = newval;
 if (sym_is_choice(sym) && newval.tri == yes)
  sym->curr.val = sym_calc_choice(sym);
 sym_validate_range(sym);
 if (memcmp(&oldval, &sym->curr, sizeof(oldval))) {
  sym_set_changed(sym);
  if (modules_sym == sym) {
   sym_set_all_changed();
   modules_val = modules_sym->curr.tri;
  }
 }
 if (sym_is_choice(sym)) {
  struct symbol *choice_sym;
  prop = sym_get_choice_prop(sym);
  expr_list_for_each_sym(prop->expr, e, choice_sym) {
   if ((sym->flags & SYMBOL_WRITE) &&
       choice_sym->visible != no)
    choice_sym->flags |= SYMBOL_WRITE;
   if (sym->flags & SYMBOL_CHANGED)
    sym_set_changed(choice_sym);
  }
 }
 if (sym->flags & SYMBOL_AUTO)
  sym->flags &= ~SYMBOL_WRITE;
}
void sym_clear_all_valid(void)
{
 struct symbol *sym;
 int i;
 for_all_symbols(i, sym)
  sym->flags &= ~SYMBOL_VALID;
 sym_add_change_count(1);
 if (modules_sym)
  sym_calc_value(modules_sym);
}
void sym_set_changed(struct symbol *sym)
{
 struct property *prop;
 sym->flags |= SYMBOL_CHANGED;
 for (prop = sym->prop; prop; prop = prop->next) {
  if (prop->menu)
   prop->menu->flags |= MENU_CHANGED;
 }
}
void sym_set_all_changed(void)
{
 struct symbol *sym;
 int i;
 for_all_symbols(i, sym)
  sym_set_changed(sym);
}
bool sym_tristate_within_range(struct symbol *sym, tristate val)
{
 int type = sym_get_type(sym);
 if (sym->visible == no)
  return false;
 if (type != S_BOOLEAN && type != S_TRISTATE)
  return false;
 if (type == S_BOOLEAN && val == mod)
  return false;
 if (sym->visible <= sym->rev_dep.tri)
  return false;
 if (sym_is_choice_value(sym) && sym->visible == yes)
  return val == yes;
 return val >= sym->rev_dep.tri && val <= sym->visible;
}
bool sym_set_tristate_value(struct symbol *sym, tristate val)
{
 tristate oldval = sym_get_tristate_value(sym);
 if (oldval != val && !sym_tristate_within_range(sym, val))
  return false;
 if (!(sym->flags & SYMBOL_DEF_USER)) {
  sym->flags |= SYMBOL_DEF_USER;
  sym_set_changed(sym);
 }
 if (sym_is_choice_value(sym) && val == yes) {
  struct symbol *cs = prop_get_symbol(sym_get_choice_prop(sym));
  struct property *prop;
  struct expr *e;
  cs->def[S_DEF_USER].val = sym;
  cs->flags |= SYMBOL_DEF_USER;
  prop = sym_get_choice_prop(cs);
  for (e = prop->expr; e; e = e->left.expr) {
   if (e->right.sym->visible != no)
    e->right.sym->flags |= SYMBOL_DEF_USER;
  }
 }
 sym->def[S_DEF_USER].tri = val;
 if (oldval != val)
  sym_clear_all_valid();
 return true;
}
tristate sym_toggle_tristate_value(struct symbol *sym)
{
 tristate oldval, newval;
 oldval = newval = sym_get_tristate_value(sym);
 do {
  switch (newval) {
  case no:
   newval = mod;
   break;
  case mod:
   newval = yes;
   break;
  case yes:
   newval = no;
   break;
  }
  if (sym_set_tristate_value(sym, newval))
   break;
 } while (oldval != newval);
 return newval;
}
bool sym_string_valid(struct symbol *sym, const char *str)
{
 signed char ch;
 switch (sym->type) {
 case S_STRING:
  return true;
 case S_INT:
  ch = *str++;
  if (ch == '-')
   ch = *str++;
  if (!isdigit(ch))
   return false;
  if (ch == '0' && *str != 0)
   return false;
  while ((ch = *str++)) {
   if (!isdigit(ch))
    return false;
  }
  return true;
 case S_HEX:
  if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
   str += 2;
  ch = *str++;
  do {
   if (!isxdigit(ch))
    return false;
  } while ((ch = *str++));
  return true;
 case S_BOOLEAN:
 case S_TRISTATE:
  switch (str[0]) {
  case 'y': case 'Y': case '1':
  case 'm': case 'M': case '2':
  case 'n': case 'N': case '0':
   return true;
  }
  return false;
 default:
  return false;
 }
}
bool sym_string_within_range(struct symbol *sym, const char *str)
{
 struct property *prop;
 int val;
 switch (sym->type) {
 case S_STRING:
  return sym_string_valid(sym, str);
 case S_INT:
  if (!sym_string_valid(sym, str))
   return false;
  prop = sym_get_range_prop(sym);
  if (!prop)
   return true;
  val = strtol(str, NULL, 10);
  return val >= sym_get_range_val(prop->expr->left.sym, 10) &&
         val <= sym_get_range_val(prop->expr->right.sym, 10);
 case S_HEX:
  if (!sym_string_valid(sym, str))
   return false;
  prop = sym_get_range_prop(sym);
  if (!prop)
   return true;
  val = strtol(str, NULL, 16);
  return val >= sym_get_range_val(prop->expr->left.sym, 16) &&
         val <= sym_get_range_val(prop->expr->right.sym, 16);
 case S_BOOLEAN:
 case S_TRISTATE:
  switch (str[0]) {
  case 'y': case 'Y': case '1':
   return sym_tristate_within_range(sym, yes);
  case 'm': case 'M': case '2':
   return sym_tristate_within_range(sym, mod);
  case 'n': case 'N': case '0':
   return sym_tristate_within_range(sym, no);
  }
  return false;
 default:
  return false;
 }
}
bool sym_set_string_value(struct symbol *sym, const char *newval)
{
 const char *oldval;
 char *val;
 int size;
 switch (sym->type) {
 case S_BOOLEAN:
 case S_TRISTATE:
  switch (newval[0]) {
  case 'y': case 'Y': case '1':
   return sym_set_tristate_value(sym, yes);
  case 'm': case 'M': case '2':
   return sym_set_tristate_value(sym, mod);
  case 'n': case 'N': case '0':
   return sym_set_tristate_value(sym, no);
  }
  return false;
 default:
  ;
 }
 if (!sym_string_within_range(sym, newval))
  return false;
 if (!(sym->flags & SYMBOL_DEF_USER)) {
  sym->flags |= SYMBOL_DEF_USER;
  sym_set_changed(sym);
 }
 oldval = sym->def[S_DEF_USER].val;
 size = strlen(newval) + 1;
 if (sym->type == S_HEX && (newval[0] != '0' || (newval[1] != 'x' && newval[1] != 'X'))) {
  size += 2;
  sym->def[S_DEF_USER].val = val = malloc(size);
  *val++ = '0';
  *val++ = 'x';
 } else if (!oldval || strcmp(oldval, newval))
  sym->def[S_DEF_USER].val = val = malloc(size);
 else
  return true;
 strcpy(val, newval);
 free((void *)oldval);
 sym_clear_all_valid();
 return true;
}
# 10813 "dialog.all.c"
const char *sym_get_string_default(struct symbol *sym)
{
 struct property *prop;
 struct symbol *ds;
 const char *str;
 tristate val;
 sym_calc_visibility(sym);
 sym_calc_value(modules_sym);
 val = symbol_no.curr.tri;
 str = symbol_empty.curr.val;
 prop = sym_get_default_prop(sym);
 if (prop != NULL) {
  switch (sym->type) {
  case S_BOOLEAN:
  case S_TRISTATE:
   val = EXPR_AND(expr_calc_value(prop->expr), prop->visible.tri);
   break;
  default:
   ds = prop_get_symbol(prop);
   if (ds != NULL) {
    sym_calc_value(ds);
    str = (const char *)ds->curr.val;
   }
  }
 }
 val = EXPR_OR(val, sym->rev_dep.tri);
 if (val == mod)
  if (!sym_is_choice_value(sym) && modules_sym->curr.tri == no)
   val = yes;
 if (sym->type == S_BOOLEAN && val == mod)
  val = yes;
 switch (sym->type) {
 case S_BOOLEAN:
 case S_TRISTATE:
  switch (val) {
  case no: return "0";
  case mod: return "2";
  case yes: return "1";
  }
 case S_INT:
 case S_HEX:
  return str;
 case S_STRING:
  return str;
 case S_OTHER:
 case S_UNKNOWN:
  break;
 }
 return "";
}
const char *sym_get_string_value(struct symbol *sym) {
 tristate val;
 switch (sym->type) {
 case S_BOOLEAN:
 case S_TRISTATE:
  val = sym_get_tristate_value(sym);
  switch (val) {
  case no:
   return "0";
  case mod:
   sym_calc_value(modules_sym);
   return ((modules_sym->curr.tri == no) ? "0" : "2");
  case yes:
   return "1";
  }
  break;
 default:
  ;
 }
 return (const char *)sym->curr.val;
}
bool sym_is_changable(struct symbol *sym)
{
 return sym->visible > sym->rev_dep.tri;
}
static unsigned strhash(const char *s)
{
 unsigned hash = 2166136261U;
 for (; *s; s++)
  hash = (hash ^ *s) * 0x01000193;
 return hash;
}
struct symbol *sym_lookup(const char *name, int flags)
{
 struct symbol *symbol;
 char *new_name;
 int hash;
 if (name) {
  if (name[0] && !name[1]) {
   switch (name[0]) {
   case 'y': case '1': { return &symbol_yes; }
   case 'm': case '2': { return &symbol_mod; }
   case 'n': case '0': { return &symbol_no;  }
   }
  }
  hash = strhash(name) % SYMBOL_HASHSIZE;
  for (symbol = symbol_hash[hash]; symbol; symbol = symbol->next) {
   if (symbol->name &&
       !strcmp(symbol->name, name) &&
       (flags ? symbol->flags & flags
       : !(symbol->flags & (SYMBOL_CONST|SYMBOL_CHOICE))))
    return symbol;
  }
  new_name = strdup(name);
 } else {
  new_name = NULL;
  hash = 0;
 }
 symbol = malloc(sizeof(*symbol));
 memset(symbol, 0, sizeof(*symbol));
 symbol->name = new_name;
 symbol->type = S_UNKNOWN;
 symbol->flags |= flags;
 symbol->next = symbol_hash[hash];
 symbol_hash[hash] = symbol;
 return symbol;
}
struct symbol *sym_find(const char *name)
{
 struct symbol *symbol = NULL;
 int hash = 0;
 if (!name)
  return NULL;
 if (name[0] && !name[1]) {
  switch (name[0]) {
   case 'y': case '1': { return &symbol_yes; }
   case 'm': case '2': { return &symbol_mod; }
   case 'n': case '0': { return &symbol_no;  }
  }
 }
 hash = strhash(name) % SYMBOL_HASHSIZE;
 for (symbol = symbol_hash[hash]; symbol; symbol = symbol->next) {
  if (symbol->name &&
      !strcmp(symbol->name, name) &&
      !(symbol->flags & SYMBOL_CONST))
    break;
 }
 return symbol;
}
const char *sym_expand_string_value(const char *in)
{
 const char *src;
 char *res;
 size_t reslen;
 reslen = strlen(in) + 1;
 res = malloc(reslen);
 res[0] = '\0';
 while ((src = strchr(in, '$'))) {
  char *p, name[SYMBOL_MAXLENGTH];
  const char *symval = "";
  struct symbol *sym;
  size_t newlen;
  strncat(res, in, src - in);
  src++;
  p = name;
  while (isalnum(*src) || *src == '_')
   *p++ = *src++;
  *p = '\0';
  sym = sym_find(name);
  if (sym != NULL) {
   sym_calc_value(sym);
   symval = sym_get_string_value(sym);
  }
  newlen = strlen(res) + strlen(symval) + strlen(src) + 1;
  if (newlen > reslen) {
   reslen = newlen;
   res = realloc(res, reslen);
  }
  strcat(res, symval);
  in = src;
 }
 strcat(res, in);
 return res;
}
const char *sym_escape_string_value(const char *in)
{
 const char *p;
 size_t reslen;
 char *res;
 size_t l;
 reslen = strlen(in) + strlen("\"\"") + 1;
 p = in;
 for (;;) {
  l = strcspn(p, "\"\\");
  p += l;
  if (p[0] == '\0')
   break;
  reslen++;
  p++;
 }
 res = malloc(reslen);
 res[0] = '\0';
 strcat(res, "\"");
 p = in;
 for (;;) {
  l = strcspn(p, "\"\\");
  strncat(res, p, l);
  p += l;
  if (p[0] == '\0')
   break;
  strcat(res, "\\");
  strncat(res, p++, 1);
 }
 strcat(res, "\"");
 return res;
}
struct symbol **sym_re_search(const char *pattern)
{
 struct symbol *sym, **sym_arr = NULL;
 int i, cnt, size;
 regex_t re;
 cnt = size = 0;
 if (strlen(pattern) == 0)
  return NULL;
 if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB|REG_ICASE))
  return NULL;
 for_all_symbols(i, sym) {
  if (sym->flags & SYMBOL_CONST || !sym->name)
   continue;
  if (regexec(&re, sym->name, 0, NULL, 0))
   continue;
  if (cnt + 1 >= size) {
   void *tmp = sym_arr;
   size += 16;
   sym_arr = realloc(sym_arr, size * sizeof(struct symbol *));
   if (!sym_arr) {
    free(tmp);
    return NULL;
   }
  }
  sym_calc_value(sym);
  sym_arr[cnt++] = sym;
 }
 if (sym_arr)
  sym_arr[cnt] = NULL;
 regfree(&re);
 return sym_arr;
}
static struct dep_stack {
 struct dep_stack *prev, *next;
 struct symbol *sym;
 struct property *prop;
 struct expr *expr;
} *check_top;
static void dep_stack_insert(struct dep_stack *stack, struct symbol *sym)
{
 memset(stack, 0, sizeof(*stack));
 if (check_top)
  check_top->next = stack;
 stack->prev = check_top;
 stack->sym = sym;
 check_top = stack;
}
static void dep_stack_remove(void)
{
 check_top = check_top->prev;
 if (check_top)
  check_top->next = NULL;
}
static void sym_check_print_recursive(struct symbol *last_sym)
{
 struct dep_stack *stack;
 struct symbol *sym, *next_sym;
 struct menu *menu = NULL;
 struct property *prop;
 struct dep_stack cv_stack;
 if (sym_is_choice_value(last_sym)) {
  dep_stack_insert(&cv_stack, last_sym);
  last_sym = prop_get_symbol(sym_get_choice_prop(last_sym));
 }
 for (stack = check_top; stack != NULL; stack = stack->prev)
  if (stack->sym == last_sym)
   break;
 if (!stack) {
  fprintf(stderr, "unexpected recursive dependency error\n");
  return;
 }
 for (; stack; stack = stack->next) {
  sym = stack->sym;
  next_sym = stack->next ? stack->next->sym : last_sym;
  prop = stack->prop;
  if (prop == NULL)
   prop = stack->sym->prop;
  if (sym_is_choice(sym) || sym_is_choice_value(sym)) {
   for (prop = sym->prop; prop; prop = prop->next) {
    menu = prop->menu;
    if (prop->menu)
     break;
   }
  }
  if (stack->sym == last_sym)
   fprintf(stderr, "%s:%d:error: recursive dependency detected!\n",
    prop->file->name, prop->lineno);
  if (stack->expr) {
   fprintf(stderr, "%s:%d:\tsymbol %s %s value contains %s\n",
    prop->file->name, prop->lineno,
    sym->name ? sym->name : "<choice>",
    prop_get_type_name(prop->type),
    next_sym->name ? next_sym->name : "<choice>");
  } else if (stack->prop) {
   fprintf(stderr, "%s:%d:\tsymbol %s depends on %s\n",
    prop->file->name, prop->lineno,
    sym->name ? sym->name : "<choice>",
    next_sym->name ? next_sym->name : "<choice>");
  } else if (sym_is_choice(sym)) {
   fprintf(stderr, "%s:%d:\tchoice %s contains symbol %s\n",
    menu->file->name, menu->lineno,
    sym->name ? sym->name : "<choice>",
    next_sym->name ? next_sym->name : "<choice>");
  } else if (sym_is_choice_value(sym)) {
   fprintf(stderr, "%s:%d:\tsymbol %s is part of choice %s\n",
    menu->file->name, menu->lineno,
    sym->name ? sym->name : "<choice>",
    next_sym->name ? next_sym->name : "<choice>");
  } else {
   fprintf(stderr, "%s:%d:\tsymbol %s is selected by %s\n",
    prop->file->name, prop->lineno,
    sym->name ? sym->name : "<choice>",
    next_sym->name ? next_sym->name : "<choice>");
  }
 }
 if (check_top == &cv_stack)
  dep_stack_remove();
}
static struct symbol *sym_check_expr_deps(struct expr *e)
{
 struct symbol *sym;
 if (!e)
  return NULL;
 switch (e->type) {
 case E_OR:
 case E_AND:
  sym = sym_check_expr_deps(e->left.expr);
  if (sym)
   return sym;
  return sym_check_expr_deps(e->right.expr);
 case E_NOT:
  return sym_check_expr_deps(e->left.expr);
 case E_EQUAL:
 case E_UNEQUAL:
  sym = sym_check_deps(e->left.sym);
  if (sym)
   return sym;
  return sym_check_deps(e->right.sym);
 case E_SYMBOL:
  return sym_check_deps(e->left.sym);
 default:
  break;
 }
 printf("Oops! How to check %d?\n", e->type);
 return NULL;
}
static struct symbol *sym_check_sym_deps(struct symbol *sym)
{
 struct symbol *sym2;
 struct property *prop;
 struct dep_stack stack;
 dep_stack_insert(&stack, sym);
 sym2 = sym_check_expr_deps(sym->rev_dep.expr);
 if (sym2)
  goto out;
 for (prop = sym->prop; prop; prop = prop->next) {
  if (prop->type == P_CHOICE || prop->type == P_SELECT)
   continue;
  stack.prop = prop;
  sym2 = sym_check_expr_deps(prop->visible.expr);
  if (sym2)
   break;
  if (prop->type != P_DEFAULT || sym_is_choice(sym))
   continue;
  stack.expr = prop->expr;
  sym2 = sym_check_expr_deps(prop->expr);
  if (sym2)
   break;
  stack.expr = NULL;
 }
out:
 dep_stack_remove();
 return sym2;
}
static struct symbol *sym_check_choice_deps(struct symbol *choice)
{
 struct symbol *sym, *sym2;
 struct property *prop;
 struct expr *e;
 struct dep_stack stack;
 dep_stack_insert(&stack, choice);
 prop = sym_get_choice_prop(choice);
 expr_list_for_each_sym(prop->expr, e, sym)
  sym->flags |= (SYMBOL_CHECK | SYMBOL_CHECKED);
 choice->flags |= (SYMBOL_CHECK | SYMBOL_CHECKED);
 sym2 = sym_check_sym_deps(choice);
 choice->flags &= ~SYMBOL_CHECK;
 if (sym2)
  goto out;
 expr_list_for_each_sym(prop->expr, e, sym) {
  sym2 = sym_check_sym_deps(sym);
  if (sym2)
   break;
 }
out:
 expr_list_for_each_sym(prop->expr, e, sym)
  sym->flags &= ~SYMBOL_CHECK;
 if (sym2 && sym_is_choice_value(sym2) &&
     prop_get_symbol(sym_get_choice_prop(sym2)) == choice)
  sym2 = choice;
 dep_stack_remove();
 return sym2;
}
struct symbol *sym_check_deps(struct symbol *sym)
{
 struct symbol *sym2;
 struct property *prop;
 if (sym->flags & SYMBOL_CHECK) {
  sym_check_print_recursive(sym);
  return sym;
 }
 if (sym->flags & SYMBOL_CHECKED)
  return NULL;
 if (sym_is_choice_value(sym)) {
  struct dep_stack stack;
  dep_stack_insert(&stack, sym);
  prop = sym_get_choice_prop(sym);
  sym2 = sym_check_deps(prop_get_symbol(prop));
  dep_stack_remove();
 } else if (sym_is_choice(sym)) {
  sym2 = sym_check_choice_deps(sym);
 } else {
  sym->flags |= (SYMBOL_CHECK | SYMBOL_CHECKED);
  sym2 = sym_check_sym_deps(sym);
  sym->flags &= ~SYMBOL_CHECK;
 }
 if (sym2 && sym2 == sym)
  sym2 = NULL;
 return sym2;
}
struct property *prop_alloc(enum prop_type type, struct symbol *sym)
{
 struct property *prop;
 struct property **propp;
 prop = malloc(sizeof(*prop));
 memset(prop, 0, sizeof(*prop));
 prop->type = type;
 prop->sym = sym;
 prop->file = current_file;
 prop->lineno = zconf_lineno();
 if (sym) {
  for (propp = &sym->prop; *propp; propp = &(*propp)->next)
   ;
  *propp = prop;
 }
 return prop;
}
struct symbol *prop_get_symbol(struct property *prop)
{
 if (prop->expr && (prop->expr->type == E_SYMBOL ||
      prop->expr->type == E_LIST))
  return prop->expr->left.sym;
 return NULL;
}
const char *prop_get_type_name(enum prop_type type)
{
 switch (type) {
 case P_PROMPT:
  return "prompt";
 case P_ENV:
  return "env";
 case P_COMMENT:
  return "comment";
 case P_MENU:
  return "menu";
 case P_DEFAULT:
  return "default";
 case P_CHOICE:
  return "choice";
 case P_SELECT:
  return "select";
 case P_RANGE:
  return "range";
 case P_SYMBOL:
  return "symbol";
 case P_UNKNOWN:
  break;
 }
 return "unknown";
}
static void prop_add_env(const char *env)
{
 struct symbol *sym, *sym2;
 struct property *prop;
 char *p;
 sym = current_entry->sym;
 sym->flags |= SYMBOL_AUTO;
 for_all_properties(sym, prop, P_ENV) {
  sym2 = prop_get_symbol(prop);
  if (strcmp(sym2->name, env))
   menu_warn(current_entry, "redefining environment symbol from %s",
      sym2->name);
  return;
 }
 prop = prop_alloc(P_ENV, sym);
 prop->expr = expr_alloc_symbol(sym_lookup(env, SYMBOL_CONST));
 sym_env_list = expr_alloc_one(E_LIST, sym_env_list);
 sym_env_list->right.sym = sym;
 p = getenv(env);
 if (p)
  sym_add_default(sym, p);
 else
  menu_warn(current_entry, "environment variable %s undefined", env);
}
static const char nohelp_text[] = "There is no help available for this option.\n" \
    "See documentation in: http://ipset-ng.pick-nik.ru/en/\n" \
    "or source: https://code.pick-nik.ru/git/ipsetng_v2.git/\n" \
    "allow Bugs tracker for questions: https://code.pick-nik.ru/bt/\n";
struct menu rootmenu;
static struct menu **last_entry_ptr;
struct file *file_list;
struct file *current_file;
void menu_warn(struct menu *menu, const char *fmt, ...)
{
 va_list ap;
 va_start(ap, fmt);
 fprintf(stderr, "%s:%d:warning: ", menu->file->name, menu->lineno);
 vfprintf(stderr, fmt, ap);
 fprintf(stderr, "\n");
 va_end(ap);
}
static void prop_warn(struct property *prop, const char *fmt, ...)
{
 va_list ap;
 va_start(ap, fmt);
 fprintf(stderr, "%s:%d:warning: ", prop->file->name, prop->lineno);
 vfprintf(stderr, fmt, ap);
 fprintf(stderr, "\n");
 va_end(ap);
}
void _menu_init(void)
{
 current_entry = current_menu = &rootmenu;
 last_entry_ptr = &rootmenu.list;
}
void menu_add_entry(struct symbol *sym)
{
 struct menu *menu;
 menu = malloc(sizeof(*menu));
 memset(menu, 0, sizeof(*menu));
 menu->sym = sym;
 menu->parent = current_menu;
 menu->file = current_file;
 menu->lineno = zconf_lineno();
 *last_entry_ptr = menu;
 last_entry_ptr = &menu->next;
 current_entry = menu;
 if (sym)
  menu_add_symbol(P_SYMBOL, sym, NULL);
}
void menu_end_entry(void)
{
}
struct menu *menu_add_menu(void)
{
 menu_end_entry();
 last_entry_ptr = &current_entry->list;
 return current_menu = current_entry;
}
void menu_end_menu(void)
{
 last_entry_ptr = &current_menu->next;
 current_menu = current_menu->parent;
}
static struct expr *menu_check_dep(struct expr *e)
{
 if (!e)
  return e;
 switch (e->type) {
 case E_NOT:
  e->left.expr = menu_check_dep(e->left.expr);
  break;
 case E_OR:
 case E_AND:
  e->left.expr = menu_check_dep(e->left.expr);
  e->right.expr = menu_check_dep(e->right.expr);
  break;
 case E_SYMBOL:
  if (e->left.sym == &symbol_mod)
   return expr_alloc_and(e, expr_alloc_symbol(modules_sym));
  break;
 default:
  break;
 }
 return e;
}
void menu_add_dep(struct expr *dep)
{
 current_entry->dep = expr_alloc_and(current_entry->dep, menu_check_dep(dep));
}
void menu_set_type(int type)
{
 struct symbol *sym = current_entry->sym;
 if (sym->type == type)
  return;
 if (sym->type == S_UNKNOWN) {
  sym->type = type;
  return;
 }
 menu_warn(current_entry, "type of '%s' redefined from '%s' to '%s'",
     sym->name ? sym->name : "<choice>",
     sym_type_name(sym->type), sym_type_name(type));
}
struct property *menu_add_prop(enum prop_type type, char *prompt, struct expr *expr, struct expr *dep)
{
 struct property *prop = prop_alloc(type, current_entry->sym);
 prop->menu = current_entry;
 prop->expr = expr;
 prop->visible.expr = menu_check_dep(dep);
 if (prompt) {
  if (isspace(*prompt)) {
   prop_warn(prop, "leading whitespace ignored");
   while (isspace(*prompt))
    prompt++;
  }
  if (current_entry->prompt && current_entry != &rootmenu)
   prop_warn(prop, "prompt redefined");
  if(type == P_PROMPT) {
   struct menu *menu = current_entry;
   while ((menu = menu->parent) != NULL) {
    if (!menu->visibility)
     continue;
    prop->visible.expr
     = expr_alloc_and(prop->visible.expr,
        menu->visibility);
   }
  }
  current_entry->prompt = prop;
 }
 prop->text = prompt;
 return prop;
}
struct property *menu_add_prompt(enum prop_type type, char *prompt, struct expr *dep)
{
 return menu_add_prop(type, prompt, NULL, dep);
}
void menu_add_visibility(struct expr *expr)
{
 current_entry->visibility = expr_alloc_and(current_entry->visibility,
     expr);
}
void menu_add_expr(enum prop_type type, struct expr *expr, struct expr *dep)
{
 menu_add_prop(type, NULL, expr, dep);
}
void menu_add_symbol(enum prop_type type, struct symbol *sym, struct expr *dep)
{
 menu_add_prop(type, NULL, expr_alloc_symbol(sym), dep);
}
void menu_add_option(int token, char *arg)
{
 struct property *prop;
 switch (token) {
 case T_OPT_MODULES:
  prop = prop_alloc(P_DEFAULT, modules_sym);
  prop->expr = expr_alloc_symbol(current_entry->sym);
  break;
 case T_OPT_DEFCONFIG_LIST:
  if (!sym_defconfig_list)
   sym_defconfig_list = current_entry->sym;
  else if (sym_defconfig_list != current_entry->sym)
   zconf_error("trying to redefine defconfig symbol");
  break;
 case T_OPT_ENV:
  prop_add_env(arg);
  break;
 }
}
static int menu_validate_number(struct symbol *sym, struct symbol *sym2)
{
 return sym2->type == S_INT || sym2->type == S_HEX ||
        (sym2->type == S_UNKNOWN && sym_string_valid(sym, sym2->name));
}
static void sym_check_prop(struct symbol *sym)
{
 struct property *prop;
 struct symbol *sym2;
 for (prop = sym->prop; prop; prop = prop->next) {
  switch (prop->type) {
  case P_DEFAULT:
   if ((sym->type == S_STRING || sym->type == S_INT || sym->type == S_HEX) &&
       prop->expr->type != E_SYMBOL)
    prop_warn(prop,
        "default for config symbol '%s'"
        " must be a single symbol", sym->name);
   if (prop->expr->type != E_SYMBOL)
    break;
   sym2 = prop_get_symbol(prop);
   if (sym->type == S_HEX || sym->type == S_INT) {
    if (!menu_validate_number(sym, sym2))
     prop_warn(prop,
         "'%s': number is invalid",
         sym->name);
   }
   break;
  case P_SELECT:
   sym2 = prop_get_symbol(prop);
   if (sym->type != S_BOOLEAN && sym->type != S_TRISTATE)
    prop_warn(prop,
        "config symbol '%s' uses select, but is "
        "not boolean or tristate", sym->name);
   else if (sym2->type != S_UNKNOWN &&
            sym2->type != S_BOOLEAN &&
            sym2->type != S_TRISTATE)
    prop_warn(prop,
        "'%s' has wrong type. 'select' only "
        "accept arguments of boolean and "
        "tristate type", sym2->name);
   break;
  case P_RANGE:
   if (sym->type != S_INT && sym->type != S_HEX)
    prop_warn(prop, "range is only allowed "
                    "for int or hex symbols");
   if (!menu_validate_number(sym, prop->expr->left.sym) ||
       !menu_validate_number(sym, prop->expr->right.sym))
    prop_warn(prop, "range is invalid");
   break;
  default:
   ;
  }
 }
}
void menu_finalize(struct menu *parent)
{
 struct menu *menu, *last_menu;
 struct symbol *sym;
 struct property *prop;
 struct expr *parentdep, *basedep, *dep, *dep2, **ep;
 sym = parent->sym;
 if (parent->list) {
  if (sym && sym_is_choice(sym)) {
   if (sym->type == S_UNKNOWN) {
    current_entry = parent;
    for (menu = parent->list; menu; menu = menu->next) {
     if (menu->sym && menu->sym->type != S_UNKNOWN) {
      menu_set_type(menu->sym->type);
      break;
     }
    }
   }
   for (menu = parent->list; menu; menu = menu->next) {
    current_entry = menu;
    if (menu->sym && menu->sym->type == S_UNKNOWN)
     menu_set_type(sym->type);
   }
   parentdep = expr_alloc_symbol(sym);
  } else if (parent->prompt)
   parentdep = parent->prompt->visible.expr;
  else
   parentdep = parent->dep;
  for (menu = parent->list; menu; menu = menu->next) {
   basedep = expr_transform(menu->dep);
   basedep = expr_alloc_and(expr_copy(parentdep), basedep);
   basedep = expr_eliminate_dups(basedep);
   menu->dep = basedep;
   if (menu->sym)
    prop = menu->sym->prop;
   else
    prop = menu->prompt;
   for (; prop; prop = prop->next) {
    if (prop->menu != menu)
     continue;
    dep = expr_transform(prop->visible.expr);
    dep = expr_alloc_and(expr_copy(basedep), dep);
    dep = expr_eliminate_dups(dep);
    if (menu->sym && menu->sym->type != S_TRISTATE)
     dep = expr_trans_bool(dep);
    prop->visible.expr = dep;
    if (prop->type == P_SELECT) {
     struct symbol *es = prop_get_symbol(prop);
     es->rev_dep.expr = expr_alloc_or(es->rev_dep.expr,
       expr_alloc_and(expr_alloc_symbol(menu->sym), expr_copy(dep)));
    }
   }
  }
  for (menu = parent->list; menu; menu = menu->next)
   menu_finalize(menu);
 } else if (sym) {
  basedep = parent->prompt ? parent->prompt->visible.expr : NULL;
  basedep = expr_trans_compare(basedep, E_UNEQUAL, &symbol_no);
  basedep = expr_eliminate_dups(expr_transform(basedep));
  last_menu = NULL;
  for (menu = parent->next; menu; menu = menu->next) {
   dep = menu->prompt ? menu->prompt->visible.expr : menu->dep;
   if (!expr_contains_symbol(dep, sym))
    break;
   if (expr_depends_symbol(dep, sym))
    goto next;
   dep = expr_trans_compare(dep, E_UNEQUAL, &symbol_no);
   dep = expr_eliminate_dups(expr_transform(dep));
   dep2 = expr_copy(basedep);
   expr_eliminate_eq(&dep, &dep2);
   expr_free(dep);
   if (!expr_is_yes(dep2)) {
    expr_free(dep2);
    break;
   }
   expr_free(dep2);
  next:
   menu_finalize(menu);
   menu->parent = parent;
   last_menu = menu;
  }
  if (last_menu) {
   parent->list = parent->next;
   parent->next = last_menu->next;
   last_menu->next = NULL;
  }
  sym->dir_dep.expr = expr_alloc_or(sym->dir_dep.expr, parent->dep);
 }
 for (menu = parent->list; menu; menu = menu->next) {
  if (sym && sym_is_choice(sym) &&
      menu->sym && !sym_is_choice_value(menu->sym)) {
   current_entry = menu;
   menu->sym->flags |= SYMBOL_CHOICEVAL;
   if (!menu->prompt)
    menu_warn(menu, "choice value must have a prompt");
   for (prop = menu->sym->prop; prop; prop = prop->next) {
    if (prop->type == P_DEFAULT)
     prop_warn(prop, "defaults for choice "
        "values not supported");
    if (prop->menu == menu)
     continue;
    if (prop->type == P_PROMPT &&
        prop->menu->parent->sym != sym)
     prop_warn(prop, "choice value used outside its choice group");
   }
   if (sym->type == S_TRISTATE && menu->sym->type != S_TRISTATE) {
    basedep = expr_alloc_comp(E_EQUAL, sym, &symbol_yes);
    menu->dep = expr_alloc_and(basedep, menu->dep);
    for (prop = menu->sym->prop; prop; prop = prop->next) {
     if (prop->menu != menu)
      continue;
     prop->visible.expr = expr_alloc_and(expr_copy(basedep),
             prop->visible.expr);
    }
   }
   menu_add_symbol(P_CHOICE, sym, NULL);
   prop = sym_get_choice_prop(sym);
   for (ep = &prop->expr; *ep; ep = &(*ep)->left.expr)
    ;
   *ep = expr_alloc_one(E_LIST, NULL);
   (*ep)->right.sym = menu->sym;
  }
  if (menu->list && (!menu->prompt || !menu->prompt->text)) {
   for (last_menu = menu->list; ; last_menu = last_menu->next) {
    last_menu->parent = parent;
    if (!last_menu->next)
     break;
   }
   last_menu->next = menu->next;
   menu->next = menu->list;
   menu->list = NULL;
  }
 }
 if (sym && !(sym->flags & SYMBOL_WARNED)) {
  if (sym->type == S_UNKNOWN)
   menu_warn(parent, "config symbol defined without type");
  if (sym_is_choice(sym) && !parent->prompt)
   menu_warn(parent, "choice must have a prompt");
  sym_check_prop(sym);
  sym->flags |= SYMBOL_WARNED;
 }
 if (sym && !sym_is_optional(sym) && parent->prompt) {
  sym->rev_dep.expr = expr_alloc_or(sym->rev_dep.expr,
    expr_alloc_and(parent->prompt->visible.expr,
     expr_alloc_symbol(&symbol_mod)));
 }
}
bool menu_has_prompt(struct menu *menu)
{
 if (!menu->prompt)
  return false;
 return true;
}
bool menu_is_visible(struct menu *menu)
{
 struct menu *child;
 struct symbol *sym;
 tristate visible;
 if (!menu->prompt)
  return false;
 if (menu->visibility) {
  if (expr_calc_value(menu->visibility) == no)
   return no;
 }
 sym = menu->sym;
 if (sym) {
  sym_calc_value(sym);
  visible = menu->prompt->visible.tri;
 } else
  visible = menu->prompt->visible.tri = expr_calc_value(menu->prompt->visible.expr);
 if (visible != no)
  return true;
 if (!sym || sym_get_tristate_value(menu->sym) == no)
  return false;
 for (child = menu->list; child; child = child->next) {
  if (menu_is_visible(child)) {
   if (sym)
    sym->flags |= SYMBOL_DEF_USER;
   return true;
  }
 }
 return false;
}
const char *menu_get_prompt(struct menu *menu)
{
 if (menu->prompt)
  return menu->prompt->text;
 else if (menu->sym)
  return menu->sym->name;
 return NULL;
}
struct menu *menu_get_root_menu(struct menu *menu)
{
 return &rootmenu;
}
struct menu *menu_get_parent_menu(struct menu *menu)
{
 enum prop_type type;
 for (; menu != &rootmenu; menu = menu->parent) {
  type = menu->prompt ? menu->prompt->type : 0;
  if (type == P_MENU)
   break;
 }
 return menu;
}
bool menu_has_help(struct menu *menu)
{
 return menu->help != NULL;
}
const char *menu_get_help(struct menu *menu)
{
 if (menu->help)
  return menu->help;
 else
  return "";
}
static void get_prompt_str(struct gstr *r, struct property *prop,
      struct list_head *head)
{
 int i, j;
 struct menu *submenu[8], *menu, *location = NULL;
 struct jump_key *jump;
 str_printf(r, _("Prompt: %s\n"), _(prop->text));
 str_printf(r, _("  Defined at %s:%d\n"), prop->menu->file->name,
  prop->menu->lineno);
 if (!expr_is_yes(prop->visible.expr)) {
  str_append(r, _("  Depends on: "));
  expr_gstr_print(prop->visible.expr, r);
  str_append(r, "\n");
 }
 menu = prop->menu->parent;
 for (i = 0; menu != &rootmenu && i < 8; menu = menu->parent) {
  bool accessible = menu_is_visible(menu);
  submenu[i++] = menu;
  if (location == NULL && accessible)
   location = menu;
 }
 if (head && location) {
  jump = malloc(sizeof(struct jump_key));
  if (menu_is_visible(prop->menu)) {
   jump->target = prop->menu;
  } else
   jump->target = location;
  if (list_empty(head))
   jump->index = 0;
  else
   jump->index = list_entry(head->prev, struct jump_key,
       entries)->index + 1;
  list_add_tail(&jump->entries, head);
 }
 if (i > 0) {
  str_printf(r, _("  Location:\n"));
  for (j = 4; --i >= 0; j += 2) {
   menu = submenu[i];
   if (head && location && menu == location)
    jump->offset = r->len - 1;
   str_printf(r, "%*c-> %s", j, ' ',
       _(menu_get_prompt(menu)));
   if (menu->sym) {
    str_printf(r, " (%s [=%s])", menu->sym->name ?
     menu->sym->name : _("<choice>"),
     sym_get_string_value(menu->sym));
   }
   str_append(r, "\n");
  }
 }
}
void get_symbol_str(struct gstr *r, struct symbol *sym,
      struct list_head *head)
{
 bool hit;
 struct property *prop;
 if (sym && sym->name) {
  str_printf(r, "Symbol: %s [=%s]\n", sym->name,
      sym_get_string_value(sym));
  str_printf(r, "Type  : %s\n", sym_type_name(sym->type));
  if (sym->type == S_INT || sym->type == S_HEX) {
   prop = sym_get_range_prop(sym);
   if (prop) {
    str_printf(r, "Range : ");
    expr_gstr_print(prop->expr, r);
    str_append(r, "\n");
   }
  }
 }
 for_all_prompts(sym, prop)
  get_prompt_str(r, prop, head);
 hit = false;
 for_all_properties(sym, prop, P_SELECT) {
  if (!hit) {
   str_append(r, "  Selects: ");
   hit = true;
  } else
   str_printf(r, " && ");
  expr_gstr_print(prop->expr, r);
 }
 if (hit)
  str_append(r, "\n");
 if (sym->rev_dep.expr) {
  str_append(r, _("  Selected by: "));
  expr_gstr_print(sym->rev_dep.expr, r);
  str_append(r, "\n");
 }
 str_append(r, "\n\n");
}
struct gstr get_relations_str(struct symbol **sym_arr, struct list_head *head)
{
 struct symbol *sym;
 struct gstr res = str_new();
 int i;
 for (i = 0; sym_arr && (sym = sym_arr[i]); i++)
  get_symbol_str(&res, sym, head);
 if (!i)
  str_append(&res, _("No matches found.\n"));
 return res;
}
void menu_get_ext_help(struct menu *menu, struct gstr *help)
{
 struct symbol *sym = menu->sym;
 const char *help_text = nohelp_text;
 if (menu_has_help(menu)) {
  if (sym->name)
   str_printf(help, "%s%s:\n\n", CONFIG_, sym->name);
  help_text = menu_get_help(menu);
 }
 str_printf(help, "%s\n", _(help_text));
 if (sym)
  get_symbol_str(help, sym, NULL);
}
#line 1 "dialog.c"
static const char mconf_readme[] = N_(
"Overview\n"
"--------\n"
"This interface let you select features and parameters for the build.\n"
"Features can either be built-in, modularized, or ignored. Parameters\n"
"must be entered in as decimal or hexadecimal numbers or text.\n"
"\n"
"Menu items beginning with following braces represent features that\n"
"  [ ] can be built in or removed\n"
"  < > can be built in, modularized or removed\n"
"  { } can be built in or modularized (selected by other feature)\n"
"  - - are selected by other feature,\n"
"while *, M or whitespace inside braces means to build in, build as\n"
"a module or to exclude the feature respectively.\n"
"\n"
"To change any of these features, highlight it with the cursor\n"
"keys and press <Y> to build it in, <M> to make it a module or\n"
"<N> to removed it.  You may also press the <Space Bar> to cycle\n"
"through the available options (ie. Y->N->M->Y).\n"
"\n"
"Some additional keyboard hints:\n"
"\n"
"Menus\n"
"----------\n"
"o  Use the Up/Down arrow keys (cursor keys) to highlight the item\n"
"   you wish to change or submenu wish to select and press <Enter>.\n"
"   Submenus are designated by \"--->\".\n"
"\n"
"   Shortcut: Press the option's highlighted letter (hotkey).\n"
"             Pressing a hotkey more than once will sequence\n"
"             through all visible items which use that hotkey.\n"
"\n"
"   You may also use the <PAGE UP> and <PAGE DOWN> keys to scroll\n"
"   unseen options into view.\n"
"\n"
"o  To exit a menu use the cursor keys to highlight the <Exit> button\n"
"   and press <ENTER>.\n"
"\n"
"   Shortcut: Press <ESC><ESC> or <E> or <X> if there is no hotkey\n"
"             using those letters.  You may press a single <ESC>, but\n"
"             there is a delayed response which you may find annoying.\n"
"\n"
"   Also, the <TAB> and cursor keys will cycle between <Select>,\n"
"   <Exit> and <Help>.\n"
"\n"
"o  To get help with an item, use the cursor keys to highlight <Help>\n"
"   and press <ENTER>.\n"
"\n"
"   Shortcut: Press <H> or <?>.\n"
"\n"
"o  To toggle the display of hidden options, press <Z>.\n"
"\n"
"\n"
"Radiolists  (Choice lists)\n"
"-----------\n"
"o  Use the cursor keys to select the option you wish to set and press\n"
"   <S> or the <SPACE BAR>.\n"
"\n"
"   Shortcut: Press the first letter of the option you wish to set then\n"
"             press <S> or <SPACE BAR>.\n"
"\n"
"o  To see available help for the item, use the cursor keys to highlight\n"
"   <Help> and Press <ENTER>.\n"
"\n"
"   Shortcut: Press <H> or <?>.\n"
"\n"
"   Also, the <TAB> and cursor keys will cycle between <Select> and\n"
"   <Help>\n"
"\n"
"\n"
"Data Entry\n"
"-----------\n"
"o  Enter the requested information and press <ENTER>\n"
"   If you are entering hexadecimal values, it is not necessary to\n"
"   add the '0x' prefix to the entry.\n"
"\n"
"o  For help, use the <TAB> or cursor keys to highlight the help option\n"
"   and press <ENTER>.  You can try <TAB><H> as well.\n"
"\n"
"\n"
"Text Box    (Help Window)\n"
"--------\n"
"o  Use the cursor keys to scroll up/down/left/right.  The VI editor\n"
"   keys h,j,k,l function here as do <u>, <d>, <SPACE BAR> and <B> for \n"
"   those who are familiar with less and lynx.\n"
"\n"
"o  Press <E>, <X>, <q>, <Enter> or <Esc><Esc> to exit.\n"
"\n"
"\n"
"Alternate Configuration Files\n"
"-----------------------------\n"
"Menuconfig supports the use of alternate configuration files for\n"
"those who, for various reasons, find it necessary to switch\n"
"between different configurations.\n"
"\n"
"At the end of the main menu you will find two options.  One is\n"
"for saving the current configuration to a file of your choosing.\n"
"The other option is for loading a previously saved alternate\n"
"configuration.\n"
"\n"
"Even if you don't use alternate configuration files, but you\n"
"find during a Menuconfig session that you have completely messed\n"
"up your settings, you may use the \"Load Alternate...\" option to\n"
"restore your previously saved settings from \".config\" without\n"
"restarting Menuconfig.\n"
"\n"
"Other information\n"
"-----------------\n"
"If you use Menuconfig in an XTERM window make sure you have your\n"
"$TERM variable set to point to a xterm definition which supports color.\n"
"Otherwise, Menuconfig will look rather bad.  Menuconfig will not\n"
"display correctly in a RXVT window because rxvt displays only one\n"
"intensity of color, bright.\n"
"\n"
"Menuconfig will display larger menus on screens or xterms which are\n"
"set to display more than the standard 25 row by 80 column geometry.\n"
"In order for this to work, the \"stty size\" command must be able to\n"
"display the screen's current row and column geometry.  I STRONGLY\n"
"RECOMMEND that you make sure you do NOT have the shell variables\n"
"LINES and COLUMNS exported into your environment.  Some distributions\n"
"export those variables via /etc/profile.  Some ncurses programs can\n"
"become confused when those variables (LINES & COLUMNS) don't reflect\n"
"the true screen size.\n"
"\n"
"Optional personality available\n"
"------------------------------\n"
"If you prefer to have all of the options listed in a single menu, rather\n"
"than the default multimenu hierarchy, run the menuconfig with\n"
"MENUCONFIG_MODE environment variable set to single_menu. Example:\n"
"\n"
"make MENUCONFIG_MODE=single_menu menuconfig\n"
"\n"
"<Enter> will then unroll the appropriate category, or enfold it if it\n"
"is already unrolled.\n"
"\n"
"Note that this mode can eventually be a little more CPU expensive\n"
"(especially with a larger number of unrolled categories) than the\n"
"default mode.\n"
"\n"
"Different color themes available\n"
"--------------------------------\n"
"It is possible to select different color themes using the variable\n"
"MENUCONFIG_COLOR. To select a theme use:\n"
"\n"
"make MENUCONFIG_COLOR=<theme> menuconfig\n"
"\n"
"Available themes are\n"
" mono       => selects colors suitable for monochrome displays\n"
" blackbg    => selects a color scheme with black background\n"
" classic    => theme with blue background. The classic look\n"
" bluetitle  => a LCD friendly version of classic. (default)\n"
"\n"),
menu_instructions[] = N_(
 "Arrow keys navigate the menu.  "
 "<Enter> selects submenus --->.  "
 "Highlighted letters are hotkeys.  "
 "Pressing <Y> includes, <N> excludes, <M> modularizes features.  "
 "Press <Esc><Esc> to exit, <?> for Help, </> for Search.  "
 "Legend: [*] built-in  [ ] excluded  <M> module  < > module capable"),
radiolist_instructions[] = N_(
 "Use the arrow keys to navigate this window or "
 "press the hotkey of the item you wish to select "
 "followed by the <SPACE BAR>. "
 "Press <?> for additional information about this option."),
inputbox_instructions_int[] = N_(
 "Please enter a decimal value. "
 "Fractions will not be accepted.  "
 "Use the <TAB> key to move from the input field to the buttons below it."),
inputbox_instructions_hex[] = N_(
 "Please enter a hexadecimal value. "
 "Use the <TAB> key to move from the input field to the buttons below it."),
inputbox_instructions_string[] = N_(
 "Please enter a string value. "
 "Use the <TAB> key to move from the input field to the buttons below it."),
setmod_text[] = N_(
 "This feature depends on another which has been configured as a module.\n"
 "As a result, this feature will be built as a module."),
load_config_text[] = N_(
 "Enter the name of the configuration file you wish to load.  "
 "Accept the name shown to restore the configuration you "
 "last retrieved.  Leave blank to abort."),
load_config_help[] = N_(
 "\n"
 "For various reasons, one may wish to keep several different\n"
 "configurations available on a single machine.\n"
 "\n"
 "If you have saved a previous configuration in a file other than the\n"
 "default one, entering its name here will allow you to modify that\n"
 "configuration.\n"
 "\n"
 "If you are uncertain, then you have probably never used alternate\n"
 "configuration files. You should therefore leave this blank to abort.\n"),
save_config_text[] = N_(
 "Enter a filename to which this configuration should be saved "
 "as an alternate.  Leave blank to abort."),
save_config_help[] = N_(
 "\n"
 "For various reasons, one may wish to keep different configurations\n"
 "available on a single machine.\n"
 "\n"
 "Entering a file name here will allow you to later retrieve, modify\n"
 "and use the current configuration as an alternate to whatever\n"
 "configuration options you have selected at that time.\n"
 "\n"
 "If you are uncertain what all this means then you should probably\n"
 "leave this blank.\n"),
search_help[] = N_(
 "\n"
 "Search for symbols and display their relations.\n"
 "Regular expressions are allowed.\n"
 "Example: search for \"^FOO\"\n"
 "Result:\n"
 "-----------------------------------------------------------------\n"
 "Symbol: FOO [=m]\n"
 "Type  : tristate\n"
 "Prompt: Foo bus is used to drive the bar HW\n"
 "  Defined at drivers/pci/Kconfig:47\n"
 "  Depends on: X86_LOCAL_APIC && X86_IO_APIC || IA64\n"
 "  Location:\n"
 "    -> Bus options (PCI, PCMCIA, EISA, ISA)\n"
 "      -> PCI support (PCI [=y])\n"
 "(1)     -> PCI access mode (<choice> [=y])\n"
 "  Selects: LIBCRC32\n"
 "  Selected by: BAR\n"
 "-----------------------------------------------------------------\n"
 "o The line 'Type:' shows the type of the configuration option for\n"
 "  this symbol (boolean, tristate, string, ...)\n"
 "o The line 'Prompt:' shows the text used in the menu structure for\n"
 "  this symbol\n"
 "o The 'Defined at' line tell at what file / line number the symbol\n"
 "  is defined\n"
 "o The 'Depends on:' line tell what symbols needs to be defined for\n"
 "  this symbol to be visible in the menu (selectable)\n"
 "o The 'Location:' lines tell where in the menu structure this symbol\n"
 "  is located\n"
 "    A location followed by a [=y] indicates that this is a\n"
 "    selectable menu item - and the current value is displayed inside\n"
 "    brackets.\n"
 "    Press the key in the (#) prefix to jump directly to that\n"
 "    location. You will be returned to the current search results\n"
 "    after exiting this new menu.\n"
 "o The 'Selects:' line tell what symbol will be automatically\n"
 "  selected if this symbol is selected (y or m)\n"
 "o The 'Selected by' line tell what symbol has selected this symbol\n"
 "\n"
 "Only relevant lines are shown.\n"
 "\n\n"
 "Search examples:\n"
 "Examples: USB	=> find all symbols containing USB\n"
 "          ^USB => find all symbols starting with USB\n"
 "          USB$ => find all symbols ending with USB\n"
 "\n");
static int indent;
static struct menu *current_menu;
static int child_count;
static int single_menu_mode;
static int show_all_options;
static void conf(struct menu *menu, struct menu *active_menu);
static void conf_choice(struct menu *menu);
static void conf_string(struct menu *menu);
static void conf_load(void);
static void conf_save(void);
static int show_textbox_ext(const char *title, char *text, int r, int c,
       int *keys, int *vscroll, int *hscroll,
       update_text_fn update_text, void *data);
static void show_textbox(const char *title, const char *text, int r, int c);
static void show_helptext(const char *title, const char *text);
static void show_help(struct menu *menu);
static char filename[PATH_MAX+1];
static void set_config_filename(const char *config_filename)
{
 static char menu_backtitle[PATH_MAX+128];
 int size;
 size = snprintf(menu_backtitle, sizeof(menu_backtitle),
                 "%s - %s", config_filename, rootmenu.prompt->text);
 if (size >= sizeof(menu_backtitle))
  menu_backtitle[sizeof(menu_backtitle)-1] = '\0';
 set_dialog_backtitle(menu_backtitle);
 size = snprintf(filename, sizeof(filename), "%s", config_filename);
 if (size >= sizeof(filename))
  filename[sizeof(filename)-1] = '\0';
}
struct search_data {
 struct list_head *head;
 struct menu **targets;
 int *keys;
};
static void update_text(char *buf, size_t start, size_t end, void *_data)
{
 struct search_data *data = _data;
 struct jump_key *pos;
 int k = 0;
 list_for_each_entry(pos, data->head, entries) {
  if (pos->offset >= start && pos->offset < end) {
   char header[4];
   if (k < JUMP_NB) {
    int key = '0' + (pos->index % JUMP_NB) + 1;
    sprintf(header, "(%c)", key);
    data->keys[k] = key;
    data->targets[k] = pos->target;
    k++;
   } else {
    sprintf(header, "   ");
   }
   memcpy(buf + pos->offset, header, sizeof(header) - 1);
  }
 }
 data->keys[k] = 0;
}
static void search_conf(void)
{
 struct symbol **sym_arr;
 struct gstr res;
 char *dialog_input;
 int dres, vscroll = 0, hscroll = 0;
 bool again;
again:
 dialog_clear();
 dres = dialog_inputbox(_("Search Configuration Parameter"),
         _("Enter " CONFIG_ " (sub)string to search for "
    "(with or without \"" CONFIG_ "\")"),
         10, 75, "");
 switch (dres) {
 case 0:
  break;
 case 1:
  show_helptext(_("Search Configuration"), search_help);
  goto again;
 default:
  return;
 }
 dialog_input = dialog_input_result;
 if (strncasecmp(dialog_input_result, CONFIG_, strlen(CONFIG_)) == 0)
  dialog_input += strlen(CONFIG_);
 sym_arr = sym_re_search(dialog_input);
 do {
  LIST_HEAD(head);
  struct menu *targets[JUMP_NB];
  int keys[JUMP_NB + 1], i;
  struct search_data data = {
   .head = &head,
   .targets = targets,
   .keys = keys,
  };
  res = get_relations_str(sym_arr, &head);
  dres = show_textbox_ext(_("Search Results"), (char *)
     str_get(&res), 0, 0, keys, &vscroll,
     &hscroll, &update_text, (void *)
     &data);
  again = false;
  for (i = 0; i < JUMP_NB && keys[i]; i++)
   if (dres == keys[i]) {
    conf(targets[i]->parent, targets[i]);
    again = true;
   }
  str_free(&res);
 } while (again);
 free(sym_arr);
}
static void build_conf(struct menu *menu)
{
 struct symbol *sym;
 struct property *prop;
 struct menu *child;
 int type, tmp, doint = 2;
 tristate val;
 char ch;
 bool visible;
 visible = menu_is_visible(menu);
 if (show_all_options && !menu_has_prompt(menu))
  return;
 else if (!show_all_options && !visible)
  return;
 sym = menu->sym;
 prop = menu->prompt;
 if (!sym) {
  if (prop && menu != current_menu) {
   const char *prompt = menu_get_prompt(menu);
   switch (prop->type) {
   case P_MENU:
    child_count++;
    prompt = _(prompt);
    if (single_menu_mode) {
     item_make("%s%*c%s",
        menu->data ? "-->" : "++>",
        indent + 1, ' ', prompt);
    } else
     item_make("   %*c%s  --->", indent + 1, ' ', prompt);
    item_set_tag('m');
    item_set_data(menu);
    if (single_menu_mode && menu->data)
     goto conf_childs;
    return;
   case P_COMMENT:
    if (prompt) {
     child_count++;
     item_make("   %*c*** %s ***", indent + 1, ' ', _(prompt));
     item_set_tag(':');
     item_set_data(menu);
    }
    break;
   default:
    if (prompt) {
     child_count++;
     item_make("---%*c%s", indent + 1, ' ', _(prompt));
     item_set_tag(':');
     item_set_data(menu);
    }
   }
  } else
   doint = 0;
  goto conf_childs;
 }
 type = sym_get_type(sym);
 if (sym_is_choice(sym)) {
  struct symbol *def_sym = sym_get_choice_value(sym);
  struct menu *def_menu = NULL;
  child_count++;
  for (child = menu->list; child; child = child->next) {
   if (menu_is_visible(child) && child->sym == def_sym)
    def_menu = child;
  }
  val = sym_get_tristate_value(sym);
  if (sym_is_changable(sym)) {
   switch (type) {
   case S_BOOLEAN:
    item_make("[%c]", val == no ? ' ' : '*');
    break;
   case S_TRISTATE:
    switch (val) {
    case yes: ch = '*'; break;
    case mod: ch = 'M'; break;
    default: ch = ' '; break;
    }
    item_make("<%c>", ch);
    break;
   }
   item_set_tag('t');
   item_set_data(menu);
  } else {
   item_make("   ");
   item_set_tag(def_menu ? 't' : ':');
   item_set_data(menu);
  }
  item_add_str("%*c%s", indent + 1, ' ', _(menu_get_prompt(menu)));
  if (val == yes) {
   if (def_menu) {
    item_add_str(" (%s)", _(menu_get_prompt(def_menu)));
    item_add_str("  --->");
    if (def_menu->list) {
     indent += 2;
     build_conf(def_menu);
     indent -= 2;
    }
   }
   return;
  }
 } else {
  if (menu == current_menu) {
   item_make("---%*c%s", indent + 1, ' ', _(menu_get_prompt(menu)));
   item_set_tag(':');
   item_set_data(menu);
   goto conf_childs;
  }
  child_count++;
  val = sym_get_tristate_value(sym);
  if (sym_is_choice_value(sym) && val == yes) {
   item_make("   ");
   item_set_tag(':');
   item_set_data(menu);
  } else {
   switch (type) {
   case S_BOOLEAN:
    if (sym_is_changable(sym))
     item_make("[%c]", val == no ? ' ' : '*');
    else
     item_make("-%c-", val == no ? ' ' : '*');
    item_set_tag('t');
    item_set_data(menu);
    break;
   case S_TRISTATE:
    switch (val) {
    case yes: ch = '*'; break;
    case mod: ch = 'M'; break;
    default: ch = ' '; break;
    }
    if (sym_is_changable(sym)) {
     if (sym->rev_dep.tri == mod)
      item_make("{%c}", ch);
     else
      item_make("<%c>", ch);
    } else
     item_make("-%c-", ch);
    item_set_tag('t');
    item_set_data(menu);
    break;
   default:
    tmp = 2 + strlen(sym_get_string_value(sym));
    item_make("(%s)", sym_get_string_value(sym));
    tmp = indent - tmp + 4;
    if (tmp < 0)
     tmp = 0;
    item_add_str("%*c%s%s", tmp, ' ', _(menu_get_prompt(menu)),
          (sym_has_value(sym) || !sym_is_changable(sym)) ?
          "" : _(" (NEW)"));
    item_set_tag('s');
    item_set_data(menu);
    goto conf_childs;
   }
  }
  item_add_str("%*c%s%s", indent + 1, ' ', _(menu_get_prompt(menu)),
     (sym_has_value(sym) || !sym_is_changable(sym)) ?
     "" : _(" (NEW)"));
  if (menu->prompt->type == P_MENU) {
   item_add_str("  --->");
   return;
  }
 }
conf_childs:
 indent += doint;
 for (child = menu->list; child; child = child->next)
  build_conf(child);
 indent -= doint;
}
static void conf(struct menu *menu, struct menu *active_menu)
{
 struct menu *submenu;
 const char *prompt = menu_get_prompt(menu);
 struct symbol *sym;
 int res;
 int s_scroll = 0;
 while (1) {
  item_reset();
  current_menu = menu;
  build_conf(menu);
  if (!child_count)
   break;
  if (menu == &rootmenu) {
   item_make("--- ");
   item_set_tag(':');
   item_make(_("    Load an Alternate Configuration File"));
   item_set_tag('L');
   item_make(_("    Save an Alternate Configuration File"));
   item_set_tag('S');
  }
  dialog_clear();
  res = dialog_menu(prompt ? _(prompt) : _("IPSET-NG main Menu"),
      _(menu_instructions),
      active_menu, &s_scroll);
  if (res == 1 || res == KEY_ESC || res == -ERRDISPLAYTOOSMALL)
   break;
  if (!item_activate_selected())
   continue;
  if (!item_tag())
   continue;
  submenu = item_data();
  active_menu = item_data();
  if (submenu)
   sym = submenu->sym;
  else
   sym = NULL;
  switch (res) {
  case 0:
   switch (item_tag()) {
   case 'm':
    if (single_menu_mode)
     submenu->data = (void *) (long) !submenu->data;
    else
     conf(submenu, NULL);
    break;
   case 't':
    if (sym_is_choice(sym) && sym_get_tristate_value(sym) == yes)
     conf_choice(submenu);
    else if (submenu->prompt->type == P_MENU)
     conf(submenu, NULL);
    break;
   case 's':
    conf_string(submenu);
    break;
   case 'L':
    conf_load();
    break;
   case 'S':
    conf_save();
    break;
   }
   break;
  case 2:
   if (sym)
    show_help(submenu);
   else
    show_helptext(_("README"), _(mconf_readme));
   break;
  case 3:
   if (item_is_tag('t')) {
    if (sym_set_tristate_value(sym, yes))
     break;
    if (sym_set_tristate_value(sym, mod))
     show_textbox(NULL, setmod_text, 6, 74);
   }
   break;
  case 4:
   if (item_is_tag('t'))
    sym_set_tristate_value(sym, no);
   break;
  case 5:
   if (item_is_tag('t'))
    sym_set_tristate_value(sym, mod);
   break;
  case 6:
   if (item_is_tag('t'))
    sym_toggle_tristate_value(sym);
   else if (item_is_tag('m'))
    conf(submenu, NULL);
   break;
  case 7:
   search_conf();
   break;
  case 8:
   show_all_options = !show_all_options;
   break;
  }
 }
}
static int show_textbox_ext(const char *title, char *text, int r, int c, int
       *keys, int *vscroll, int *hscroll, update_text_fn
       update_text, void *data)
{
 dialog_clear();
 return dialog_textbox(title, text, r, c, keys, vscroll, hscroll,
         update_text, data);
}
static void show_textbox(const char *title, const char *text, int r, int c)
{
 show_textbox_ext(title, (char *) text, r, c, (int []) {0}, NULL, NULL,
    NULL, NULL);
}
static void show_helptext(const char *title, const char *text)
{
 show_textbox(title, text, 0, 0);
}
static void show_help(struct menu *menu)
{
 struct gstr help = str_new();
 help.max_width = getmaxx(stdscr) - 10;
 menu_get_ext_help(menu, &help);
 show_helptext(_(menu_get_prompt(menu)), str_get(&help));
 str_free(&help);
}
static void conf_choice(struct menu *menu)
{
 const char *prompt = _(menu_get_prompt(menu));
 struct menu *child;
 struct symbol *active;
 active = sym_get_choice_value(menu->sym);
 while (1) {
  int res;
  int selected;
  item_reset();
  current_menu = menu;
  for (child = menu->list; child; child = child->next) {
   if (!menu_is_visible(child))
    continue;
   if (child->sym)
    item_make("%s", _(menu_get_prompt(child)));
   else {
    item_make("*** %s ***", _(menu_get_prompt(child)));
    item_set_tag(':');
   }
   item_set_data(child);
   if (child->sym == active)
    item_set_selected(1);
   if (child->sym == sym_get_choice_value(menu->sym))
    item_set_tag('X');
  }
  dialog_clear();
  res = dialog_checklist(prompt ? _(prompt) : _("IPSET-NG main Menu"),
     _(radiolist_instructions),
      15, 70, 6);
  selected = item_activate_selected();
  switch (res) {
  case 0:
   if (selected) {
    child = item_data();
    if (!child->sym)
     break;
    sym_set_tristate_value(child->sym, yes);
   }
   return;
  case 1:
   if (selected) {
    child = item_data();
    show_help(child);
    active = child->sym;
   } else
    show_help(menu);
   break;
  case KEY_ESC:
   return;
  case -ERRDISPLAYTOOSMALL:
   return;
  }
 }
}
static void conf_string(struct menu *menu)
{
 const char *prompt = menu_get_prompt(menu);
 while (1) {
  int res;
  const char *heading;
  switch (sym_get_type(menu->sym)) {
  case S_INT:
   heading = _(inputbox_instructions_int);
   break;
  case S_HEX:
   heading = _(inputbox_instructions_hex);
   break;
  case S_STRING:
   heading = _(inputbox_instructions_string);
   break;
  default:
   heading = _("Internal mconf error!");
  }
  dialog_clear();
  res = dialog_inputbox(prompt ? _(prompt) : _("IPSET-NG main Menu"),
          heading, 10, 75,
          sym_get_string_value(menu->sym));
  switch (res) {
  case 0:
   if (sym_set_string_value(menu->sym, dialog_input_result))
    return;
   show_textbox(NULL, _("You have made an invalid entry."), 5, 43);
   break;
  case 1:
   show_help(menu);
   break;
  case KEY_ESC:
   return;
  }
 }
}
static void conf_load(void)
{
 while (1) {
  int res;
  dialog_clear();
  res = dialog_inputbox(NULL, load_config_text,
          11, 55, filename);
  switch(res) {
  case 0:
   if (!dialog_input_result[0])
    return;
   if (!conf_read(dialog_input_result)) {
    set_config_filename(dialog_input_result);
    sym_set_change_count(1);
    return;
   }
   show_textbox(NULL, _("File does not exist!"), 5, 38);
   break;
  case 1:
   show_helptext(_("Load Alternate Configuration"), load_config_help);
   break;
  case KEY_ESC:
   return;
  }
 }
}
static void conf_save(void)
{
 while (1) {
  int res;
  dialog_clear();
  res = dialog_inputbox(NULL, save_config_text,
          11, 55, filename);
  switch(res) {
  case 0:
   if (!dialog_input_result[0])
    return;
   if (!conf_write(dialog_input_result)) {
    set_config_filename(dialog_input_result);
    return;
   }
   show_textbox(NULL, _("Can't create file!  Probably a nonexistent directory."), 5, 60);
   break;
  case 1:
   show_helptext(_("Save Alternate Configuration"), save_config_help);
   break;
  case KEY_ESC:
   return;
  }
 }
}
static int handle_exit(void)
{
 int res;
 dialog_clear();
 if (conf_get_changed())
  res = dialog_yesno(NULL,
       _("Do you wish to save your new configuration ?\n"
         "<ESC><ESC> to continue."),
       6, 60);
 else
  res = -1;
 end_dialog(saved_x, saved_y);
 switch (res) {
 case 0:
  if (conf_write(filename)) {
   fprintf(stderr, _("\n\n"
       "Error while writing of the configuration.\n"
       "Your configuration changes were NOT saved."
       "\n\n"));
   return 1;
  }
 case -1:
  printf(_("\n\n"
    "*** End of the configuration.\n"
    "*** Execute 'make' to start the build or try 'make help'."
    "\n\n"));
  res = 0;
  break;
 default:
  fprintf(stderr, _("\n\n"
      "Your configuration changes were NOT saved."
      "\n\n"));
  if (res != KEY_ESC)
   res = 0;
 }
 return res;
}
static void sig_handler(int signo)
{
 exit(handle_exit());
}
int main(int ac, char **av)
{
 char *mode;
 int res;
 setlocale(LC_ALL, "");
 bindtextdomain(PACKAGE, LOCALEDIR);
 textdomain(PACKAGE);
 signal(SIGINT, sig_handler);
 conf_parse(av[1]);
 conf_read(NULL);
 mode = getenv("MENUCONFIG_MODE");
 if (mode) {
  if (!strcasecmp(mode, "single_menu"))
   single_menu_mode = 1;
 }
 if (init_dialog(NULL)) {
  fprintf(stderr, N_("Your display is too small to run Menuconfig!\n"));
  fprintf(stderr, N_("It must be at least 19 lines by 80 columns.\n"));
  return 1;
 }
 set_config_filename(conf_get_configname());
 do {
  conf(&rootmenu, NULL);
  res = handle_exit();
 } while (res == KEY_ESC);
 return res;
}
