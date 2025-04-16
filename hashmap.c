#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    if (map == NULL || key == NULL) return;
    long pos = hash(key, map->capacity);
    for (int i = 0; i < map->capacity; i++) {
        long idx = (pos + i) % map->capacity;
        if (map->buckets[idx] == NULL) {
            map->buckets[idx] = createPair(key, value);
            map->size++;
            map->current = idx;
            float loadFactor = (float)map->size / map->capacity;
            if (loadFactor > 0.7f) {
                enlarge(map);
            }
            return;
        }
        if (map->buckets[idx]->key != NULL && is_equal(map->buckets[idx]->key, key)) {
            return;
        }
    }
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    long old_capacity = map->capacity;
    Pair **old_buckets = map->buckets;
    map->capacity *= 2;
    map->buckets = (Pair **) calloc(map->capacity, sizeof(Pair *));
    map->size = 0;
    for (long i = 0; i < old_capacity; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
        }
    }
    free(old_buckets);
}


HashMap * createMap(long capacity) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        printf("Error al asignar memoria para el HashMap.\n");
        return NULL;
    }
    map->capacity = capacity;  
    map->size = 0;              
    map->current = -1;          
    map->buckets = (Pair **)calloc(capacity, sizeof(Pair *));
    if (map->buckets == NULL) {
        free(map);
        printf("Error al asignar memoria para los buckets.\n");
        return NULL;
    }
    return map;

}

void eraseMap(HashMap * map,  char * key) {    
    long pos = hash(key, map->capacity);
    for (int i = 0; i < map->capacity; i++) {
        long idx = (pos + i) % map->capacity;
        Pair *pair = map->buckets[idx];
        if (pair == NULL) return;
        if (pair->key != NULL && is_equal(pair->key, key)) {
            pair->key = NULL;
            map->size--;
            return;
        }
    }
}

Pair * searchMap(HashMap * map,  char * key) {   
    long pos = hash(key, map->capacity);
    for (int i = 0; i < map->capacity; i++) {
        long idx = (pos + i) % map->capacity;
        Pair *pair = map->buckets[idx];
        if (pair == NULL) return NULL; 
        if (pair->key != NULL && is_equal(pair->key, key)) {
            map->current = idx;
            return pair;
        }
    }
    return NULL;
}

Pair * firstMap(HashMap * map) {
    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    for (long i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}
