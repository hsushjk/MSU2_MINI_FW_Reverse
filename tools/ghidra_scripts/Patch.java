// Pre-analysis patch of the loaded program image (image.bin on disk is untouched).
//
// The firmware is built with -msave-restore: every non-trivial function starts with
//   jal  t0, __riscv_save_4      (0x100 / 0x108 / 0x116 share one tail)
//   __riscv_save_N:  addi16sp sp,-0x40 ; save s-regs ; sub sp,sp,t1 ; c.jr t0
// i.e. the "save" helper returns through t0 instead of ra.  Ghidra decompiles the
// trailing `c.jr t0` as an unrecovered indirect jump / call, which leaves every
// register undefined after the prologue ("extraout_a4", "extraout_a5", ...) and
// wrecks every function that has locals.
//
// Replacing that single 2-byte `c.jr t0` with `ret` lets the decompiler model the
// prologue as an ordinary call that adjusts its own frame, which is exactly what it
// does at run time.  Two sites: 0x12E (save stubs) and 0x13A (small save stub).
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;

public class Patch extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] sites = { 0x12EL, 0x13AL };
        for (long s : sites) {
            Address a = toAddr(s);
            byte[] cur = new byte[2];
            currentProgram.getMemory().getBytes(a, cur);
            println(String.format("patch %s: %02x %02x -> 82 80 (ret)",
                    a, cur[0] & 0xff, cur[1] & 0xff));
            clearListing(a, a.add(1));
            currentProgram.getMemory().setBytes(a, new byte[] { (byte) 0x82, (byte) 0x80 });
        }
        println("Patch done.");
    }
}