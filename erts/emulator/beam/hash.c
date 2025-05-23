/*
 * %CopyrightBegin%
 *
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Copyright Ericsson AB 1996-2025. All Rights Reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * %CopyrightEnd%
 */

/*
** General hash functions
**
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "sys.h"
#include "hash.h"

/*
** Get info about hash
**
*/

#define MAX_SHIFT (ERTS_SIZEOF_TERM * 8)

static int hash_get_slots(Hash *h) {
    return UWORD_CONSTANT(1) << (MAX_SHIFT - h->shift);
}

void hash_get_info(HashInfo *hi, Hash *h)
{
    int size = hash_get_slots(h);
    int i;
    int max_depth = 0;
    int objects = 0;
    int used = 0;

    for (i = 0; i < size; i++) {
	int depth = 0;
	HashBucket* b = h->bucket[i];

	while (b != (HashBucket*) 0) {
	    objects++;
	    depth++;
	    b = b->next;
	}
        if (depth) {
            used++;
            if (depth > max_depth)
                max_depth = depth;
        }
    }
    ASSERT(objects == h->nobjs);
    (void)objects;

    hi->name  = h->name;
    hi->size  = hash_get_slots(h);
    hi->used  = used;
    hi->objs  = h->nobjs;
    hi->depth = max_depth;
}

/*
** Display info about hash
**
*/

void hash_info(fmtfn_t to, void *arg, Hash* h)
{
    HashInfo hi;

    hash_get_info(&hi, h);

    h->fun.meta_print(to, arg, "=hash_table:%s\n", hi.name);
    h->fun.meta_print(to, arg, "size: %d\n",       hi.size);
    h->fun.meta_print(to, arg, "used: %d\n",       hi.used);
    h->fun.meta_print(to, arg, "objs: %d\n",       hi.objs);
    h->fun.meta_print(to, arg, "depth: %d\n",      hi.depth);
}


/*
 * Returns size of table in bytes. Stored objects not included.
 */
int
hash_table_sz(Hash *h)
{
  const int name_len = strlen(h->name) + 1;
  return sizeof(Hash) + hash_get_slots(h)*sizeof(HashBucket*) + name_len;
}


static ERTS_INLINE void set_thresholds(Hash* h)
{
    h->grow_threshold = (8*hash_get_slots(h))/5;   /* grow at 160% load */
    if (h->shift < h->max_shift)
        h->shrink_threshold = hash_get_slots(h) / 5;  /* shrink at 20% load */
    else
        h->shrink_threshold = -1;  /* never shrink below initial size */
}

/*
** init a pre allocated or static hash structure
** and allocate buckets.
*/
Hash* hash_init(int type, Hash* h, char* name, int size, HashFunctions fun)
{
    int sz;
    int shift = 1;

    h->meta_alloc_type = type;

    while ((UWORD_CONSTANT(1) << shift) < size)
        shift++;

    h->is_allocated = 0;
    h->name = name;
    h->fun = fun;
    h->shift = MAX_SHIFT - shift;
    h->max_shift = h->shift;
    h->nobjs = 0;
    set_thresholds(h);

    sz = hash_get_slots(h) * sizeof(HashBucket*);
    h->bucket = (HashBucket**) fun.meta_alloc(h->meta_alloc_type, sz);
    memzero(h->bucket, sz);

    ASSERT(h->shift > 0 && h->shift < 64);

    return h;
}

/*
** Create a new hash table
*/
Hash* hash_new(int type, char* name, int size, HashFunctions fun)
{
    Hash* h;

    h = fun.meta_alloc(type, sizeof(Hash));

    h = hash_init(type, h, name, size, fun);
    h->is_allocated =  1;
    return h;
}

/*
** Delete hash table and all objects
*/
void hash_delete(Hash* h)
{
    int old_size = hash_get_slots(h);
    int i;

    for (i = 0; i < old_size; i++) {
	HashBucket* b = h->bucket[i];
	while (b != (HashBucket*) 0) {
	    HashBucket* b_next = b->next;

	    h->fun.free((void*) b);
	    b = b_next;
	}
    }
    h->fun.meta_free(h->meta_alloc_type, h->bucket);
    if (h->is_allocated)
	h->fun.meta_free(h->meta_alloc_type, (void*) h);
}

/*
** Rehash all objects
*/
static void rehash(Hash* h, int grow)
{
    int sz;
    int old_size = hash_get_slots(h);
    HashBucket** new_bucket;
    int i;

    if (grow) {
	h->shift--;
    }
    else {
	if (h->shift == h->max_shift)
	    return;
	h->shift++;
    }

    sz = hash_get_slots(h)*sizeof(HashBucket*);

    new_bucket = (HashBucket **) h->fun.meta_alloc(h->meta_alloc_type, sz);
    memzero(new_bucket, sz);

    for (i = 0; i < old_size; i++) {
	HashBucket* b = h->bucket[i];
	while (b != (HashBucket*) 0) {
	    HashBucket* b_next = b->next;
	    Uint ix = hash_get_slot(h, b->hvalue);
	    b->next = new_bucket[ix];
	    new_bucket[ix] = b;
	    b = b_next;
	}
    }
    h->fun.meta_free(h->meta_alloc_type, (void *) h->bucket);
    h->bucket = new_bucket;
    set_thresholds(h);
}

/*
** Find an object in the hash table
**
*/
void* hash_get(Hash* h, void* tmpl)
{
    return hash_fetch(h, tmpl, h->fun.hash, h->fun.cmp);
}

/*
** Find or insert an object in the hash table
*/
void* hash_put(Hash* h, void* tmpl)
{
    HashValue hval = h->fun.hash(tmpl);
    Uint ix = hash_get_slot(h, hval);
    HashBucket* b = h->bucket[ix];

    while(b != (HashBucket*) 0) {
	if ((b->hvalue == hval) && (h->fun.cmp(tmpl, (void*)b) == 0))
	    return (void*) b;
	b = b->next;
    }
    b = (HashBucket*) h->fun.alloc(tmpl);

    b->hvalue = hval;
    b->next = h->bucket[ix];
    h->bucket[ix] = b;

    if (++h->nobjs > h->grow_threshold)
	rehash(h, 1);
    return (void*) b;
}

/*
** Erase hash entry return template if erased
** return 0 if not erased
*/
void* hash_erase(Hash* h, void* tmpl)
{
    HashValue hval = h->fun.hash(tmpl);
    Uint ix = hash_get_slot(h, hval);
    HashBucket* b = h->bucket[ix];
    HashBucket* prev = 0;

    while(b != 0) {
	if ((b->hvalue == hval) && (h->fun.cmp(tmpl, (void*)b) == 0)) {
	    if (prev != 0)
		prev->next = b->next;
	    else
		h->bucket[ix] = b->next;
	    h->fun.free((void*)b);
	    if (--h->nobjs < h->shrink_threshold)
		rehash(h, 0);
	    return tmpl;
	}
	prev = b;
	b = b->next;
    }
    return (void*)0;
}

/*
** Remove hash entry from table return entry if removed
** return NULL if not removed
** NOTE: hash_remove() differs from hash_erase() in that
**       it returns entry (not the template) and does
**       *not* call the free() callback.
*/
void *
hash_remove(Hash *h, void *tmpl)
{
    HashValue hval = h->fun.hash(tmpl);
    Uint ix = hash_get_slot(h, hval);
    HashBucket *b = h->bucket[ix];
    HashBucket *prev = NULL;

    while (b) {
	if ((b->hvalue == hval) && (h->fun.cmp(tmpl, (void*)b) == 0)) {
	    if (prev)
		prev->next = b->next;
	    else
		h->bucket[ix] = b->next;
	    if (--h->nobjs < h->shrink_threshold)
		rehash(h, 0);
	    return (void *) b;
	}
	prev = b;
	b = b->next;
    }
    return NULL;
}

void hash_foreach(Hash* h, HFOREACH_FUN func, void *func_arg2)
{
    int i;

    for (i = 0; i < hash_get_slots(h); i++) {
	HashBucket* b = h->bucket[i];
	while(b != (HashBucket*) 0) {
	    (*func)((void *) b, func_arg2);
	    b = b->next;
	}
    }
}
