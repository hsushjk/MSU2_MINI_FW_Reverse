import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;

public class Seed extends GhidraScript {
    static final long END = 0x5EA0L;

    @Override
    public void run() throws Exception {
        // Reset vector word 0 is a `j` instruction into crt0; entry point.
        Address entry = toAddr(0x0L);
        disassemble(entry);
        try { createFunction(entry, "reset_entry"); } catch (Exception e) { println("entry: " + e); }

        // Interrupt vector table: 4-byte handler addresses, 0x04..0xFF+
        for (long a = 0x04L; a < 0x100L; a += 4) {
            long v = getInt(toAddr(a)) & 0xFFFFFFFFL;
            if (v == 0 || v >= END) continue;
            if ((v & 3) != 0) continue;
            Address t = toAddr(v);
            Instruction ins = getInstructionAt(t);
            if (ins == null) { disassemble(t); }
            String name = String.format("vec_%02X", a);
            try {
                Function f = getFunctionAt(t);
                if (f == null) createFunction(t, name);
            } catch (Exception e) { println("vec " + Long.toHexString(a) + ": " + e); }
        }
        println("Seed done.");
    }
}