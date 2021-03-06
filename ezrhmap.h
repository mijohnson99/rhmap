#ifndef EZRHMAP_H
#define EZRHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include "rhmap.h"

#ifndef EZRHMAP_MAX_LOAD
#define EZRHMAP_MAX_LOAD 0.9
#endif

#define EZRHMAP_DECLARE(map, hash, key, val, dtor)			\
									\
RHMAP_DECLARE(map, val)							\
									\
struct map *map##_create(void)						\
{									\
	struct map *m = malloc(sizeof(*m));				\
	struct map##_bucket *b = malloc(sizeof(*b));			\
	map##_init(m, b, sizeof(*b));					\
	return m;							\
}									\
									\
void map##_destroy(struct map *m)					\
{									\
	map##_clear(m, dtor);						\
	free(m->buckets);						\
	free(m);							\
}									\
									\
val *map##_set(struct map *m, key k, val v)				\
{									\
	size_t h = hash(k);						\
	if (m->population >= m->capacity * EZRHMAP_MAX_LOAD) {		\
		size_t new_cap = m->capacity * 2;			\
		struct map##_bucket *b = malloc(new_cap * sizeof(*b));	\
		free(map##_rehash(m, b, new_cap * sizeof(*b)));		\
	}								\
	val *p = map##_search(m, h);					\
	if (p == NULL) { 						\
		p = map##_insert(m, h, v);				\
	} else {							\
		if (dtor != NULL)					\
			dtor(*p);					\
		*p = v;							\
	}								\
	return p;							\
}									\
									\
val *map##_get(struct map *m, key k)					\
{									\
	return map##_search(m, hash(k));				\
}									\
									\
val *map##_del(struct map *m, key k)					\
{									\
	val *v = map##_remove(m, hash(k));				\
	if (m->population < m->capacity / 4) {				\
		size_t new_cap = m->capacity / 2;			\
		struct map##_bucket *b = malloc(new_cap * sizeof(*b));	\
		free(map##_rehash(m, b, new_cap * sizeof(*b)));		\
	}								\
	if (v != NULL)							\
		dtor(*v);						\
	return v;							\
}

#endif
