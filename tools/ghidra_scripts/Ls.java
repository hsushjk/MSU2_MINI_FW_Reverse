// Linear listing dump of an address range (instructions, or raw bytes where undecoded).
//@category MSU2
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;

public class Ls extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        long start = Long.decode(a[0]), end = Long.decode(a[1]);
        int max = a.length > 2 ? Integer.decode(a[2]) : 400;
        Address addr = toAddr(start);
        Address endA = toAddr(end);
        int n = 0;
        while (addr.compareTo(endA) <= 0 && n < max) {
            Instruction ins = getInstructionAt(addr);
            if (ins == null) {
                byte b = getByte(addr);
                println(String.format("  %s  db 0x%02x", addr, b & 0xff));
                addr = addr.add(1);
            } else {
                String fn = "";
                Function f = getFunctionContaining(addr);
                if (f != null && f.getEntryPoint().equals(addr)) fn = "  <-- " + f.getName();
                byte[] bs = ins.getBytes();
                StringBuilder hx = new StringBuilder();
                for (byte b : bs) hx.append(String.format("%02x", b & 0xff));
                println(String.format("  %-8s %-12s %-30s%s", addr, hx, ins.toString(), fn));
                addr = addr.add(ins.getLength());
            }
            n++;
        }
        if (n >= max) println("... (limit reached)");
    }
}