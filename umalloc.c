#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void free(void *ap) {
  Header *bp, *p;

  bp = (Header *)ap - 1;
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr)) break;
  if (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header *morecore(uint nu) {
  char *p;
  Header *hp;

  if (nu < 4096) nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if (p == (char *)-1) return 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  return freep;
}

void *malloc(const uint requested_bytes) {
  Header *previous_block;

  const uint required_units =
      (requested_bytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  if ((previous_block = freep) == 0) {
    base.s.ptr = freep = previous_block = &base;
    base.s.size = 0;
  }
  for (Header *current_block = previous_block->s.ptr;;
       previous_block = current_block, current_block = current_block->s.ptr) {
    if (current_block->s.size >= required_units) {
      if (current_block->s.size == required_units)
        previous_block->s.ptr = current_block->s.ptr;
      else {
        current_block->s.size -= required_units;
        current_block += current_block->s.size;
        current_block->s.size = required_units;
      }
      freep = previous_block;
      return (void *)(current_block + 1);
    }
    if (current_block == freep)
      if ((current_block = morecore(required_units)) == 0) return 0;
  }
}
