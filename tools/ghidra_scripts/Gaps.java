// Reports address ranges of the image not covered by any function body.
//@category MSU2
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;

public class Gaps extends GhidraScript {
    @Override
    public void run() throws Exception {
        long end = 0x5EA0L;
        long gapStart = -1;
        for (long x = 0; x < end; x++) {
            boolean inFunc = getFunctionContaining(toAddr(x)) != null;
            if (!inFunc && gapStart < 0) gapStart = x;
            if (inFunc && gapStart >= 0) {
                if (x - gapStart >= 4) println(String.format("GAP 0x%04x..0x%04x  %d bytes", gapStart, x - 1, x - gapStart));
                gapStart = -1;
            }
        }
        if (gapStart >= 0) println(String.format("GAP 0x%04x..0x%04x", gapStart, end - 1));
        println("functions: " + currentProgram.getFunctionManager().getFunctionCount());
    }
}