/* src/swap.c */
#include "config.h"
#include "engine.h"

void swap(Vector2I **a, Vector2I **b) {
  Vector2I *temp = *a;
  *a = *b;
  *b = temp;
}