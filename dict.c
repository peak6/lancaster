#include "dict.h"
#include "error.h"
#include "table.h"
#include "xalloc.h"

struct dict_t
{
	table_handle sym2id;
	table_handle id2sym;
};

static int sym2id_hash_fn(table_key key)
{
	const char* p = key;
	int c, h = 5381;

	while ((c = *p++) != 0)
		h = ((h << 5) + h) ^ c;

	return h;
}

static boolean sym2id_eq_fn(table_key key1, table_key key2)
{
	return strcmp(key1, key2) == 0;
}

static void sym2id_dtor_fn(table_key key, table_value val)
{
	xfree(key);
}

status dict_create(dict_handle* pdict, size_t sz)
{
	status st;
	if (!pdict || sz == 0) {
		error_invalid_arg("dict_create");
		return FAIL;
	}

	*pdict = XMALLOC(struct dict_t);
	if (!*pdict)
		return NO_MEMORY;

	BZERO(*pdict);

	if (FAILED(st = table_create(&(*pdict)->sym2id, sz, sym2id_hash_fn, sym2id_eq_fn, sym2id_dtor_fn)) ||
		FAILED(st = table_create(&(*pdict)->id2sym, sz, NULL, NULL, NULL))) {
		dict_destroy(pdict);
		return st;
	}

	return OK;
}

void dict_destroy(dict_handle* pdict)
{
	if (!pdict || !*pdict)
		return;

	error_save_last();

	table_destroy(&(*pdict)->id2sym);
	table_destroy(&(*pdict)->sym2id);

	error_restore_last();

	xfree(*pdict);
	*pdict = NULL;
}

status dict_assoc(dict_handle dict, const char* symbol, int id)
{
	status st;
	const char* p;

	if (!symbol) {
		error_invalid_arg("dict_assoc");
		return FAIL;
	}

	p = xstrdup(symbol);
	if (!p)
		return NO_MEMORY;

	if (FAILED(st = table_insert(dict->sym2id, (table_key) p, (table_value) (long) id)))
		return st;

	return table_insert(dict->id2sym, (table_key) (long) id, (table_value) p);
}

status dict_get_id(dict_handle dict, const char* symbol, int* pid)
{
	table_value val;
	status st;

	if (!symbol || !pid) {
		error_invalid_arg("dict_get_id");
		return FAIL;
	}

	if (!FAILED(st = table_lookup(dict->sym2id, (table_key) symbol, &val)) && st)
		*pid = (long) val;

	return st;
}

status dict_get_symbol(dict_handle dict, int id, const char** psymbol)
{
	if (!psymbol) {
		error_invalid_arg("dict_get_symbol");
		return FAIL;
	}

	return table_lookup(dict->id2sym, (table_key) (long) id, (table_value*) (void**) psymbol);
}
