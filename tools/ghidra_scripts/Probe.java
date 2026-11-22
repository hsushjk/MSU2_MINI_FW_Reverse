import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;

public class Probe extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        long addr = Long.decode(a[0].startsWith("0x") ? a[0] : "0x" + a[0]);
        int count = a.length > 1 ? Integer.parseInt(a[1]) : 40;
        Address start = toAddr(addr);
        println("=== probe " + start + " ===");
        Instruction ins = getInstructionAt(start);
        if (ins == null) { disassemble(start); ins = getInstructionAt(start); }
        for (int i = 0; i < count && ins != null; i++) {
            println(String.format("%s  %-8s %s", ins.getAddress(), ins.getMnemonicString(), ins.toString()));
            ins = ins.getNext();
        }
        Function f = getFunctionContaining(start);
        if (f == null) {
            Function nf = getFunctionAt(start);
            if (nf == null) { nf = createFunction(start, String.format("F_%08x", addr)); }
            println("(created function " + (nf == null ? "FAILED" : nf.getName()) + ")");
            f = nf;
        }
        if (f == null) { println("(no containing function)"); }
        else {
            println("--- function " + f.getName() + " @ " + f.getEntryPoint() + " ---");
            DecompInterface di = new DecompInterface();
            di.openProgram(currentProgram);
            DecompileResults r = di.decompileFunction(f, 90, monitor);
            if (r != null && r.decompileCompleted()) println(r.getDecompiledFunction().getC());
            else println("decompile failed: " + (r == null ? "null" : r.getErrorMessage()));
        }
    }
}