void kernel_main() {
  // write ok
  // *((int*) 0xb8000) = 0x2f4b2f4f;
  *((int*) 0xb8000) = 0x2f412f41;
}
