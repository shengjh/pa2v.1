#include "cpu/exec.h"
#include "device/port-io.h"

void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
  TODO();

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
  TODO();

  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
  rtlreg_t target_eip;
  rtl_pop(&target_eip);
  rtl_add(&cpu.esp, &cpu.esp, &id_dest->val);
  rtl_jr(&target_eip);

  print_asm("iret");
}

make_EHelper(in) {
  t1 = pio_read(id_src->val, id_dest->width);
	operand_write(id_dest, &t1);
  print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(out) {
  pio_write(id_dest->val, id_src->val, id_dest->width);
	print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
