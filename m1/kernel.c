/*
 * Hello World program.
 * Author: Ishank Tandon, Fred Zhang, Jackie Zhang, Runzhi Yang
 */

int main() {

  /* Draws hello world */
  putInMemory(0xB000, 0x8000, 'H');
  putInMemory(0xB000, 0x8001, 0x7);
  putInMemory(0xB000, 0x8002, 'e');
  putInMemory(0xB000, 0x8003, 0x7);
  putInMemory(0xB000, 0x8004, 'l');
  putInMemory(0xB000, 0x8005, 0x7);
  putInMemory(0xB000, 0x8006, 'l');
  putInMemory(0xB000, 0x8007, 0x7);
  putInMemory(0xB000, 0x8008, 'o');
  putInMemory(0xB000, 0x8009, 0x7);
  putInMemory(0xB000, 0x800a, ' ');
  putInMemory(0xB000, 0x800c, 'W');
  putInMemory(0xB000, 0x800d, 0x7);
  putInMemory(0xB000, 0x800e, 'o');
  putInMemory(0xB000, 0x800f, 0x7);
  putInMemory(0xB000, 0x8010, 'r');
  putInMemory(0xB000, 0x8011, 0x7);
  putInMemory(0xB000, 0x8012, 'l');
  putInMemory(0xB000, 0x8013, 0x7);
  putInMemory(0xB000, 0x8014, 'd');
  putInMemory(0xB000, 0x8015, 0x7);

  while (1) {}
}