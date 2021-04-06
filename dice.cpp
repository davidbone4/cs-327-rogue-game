#include <cstdlib>

#include "dice.h"
#include "dungeondefinitions.h"

int rand_range(int min, int max)
{
    static bool first = true;
    if (first)
    {
        srand(time(NULL)); //seeding for the first time only!
        first = false;
    }
    return min + rand() % ((max + 1) - min);
}

int32_t dice::roll(void) const
{
  int32_t total;
  uint32_t i;

  total = base;

  if (sides) {
    for (i = 0; i < number; i++) {
      total += rand_range(1, sides);
    }
  }

  return total;
}

std::ostream &dice::print(std::ostream &o)
{
  return o << base << '+' << number << 'd' << sides;
}

std::ostream &operator<<(std::ostream &o, dice &d)
{
  return d.print(o);
}
