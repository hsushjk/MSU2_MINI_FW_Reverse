// Pre-analysis seed: crt0 enters the application via `mepc = 0x39B4; mret`,
// which Ghidra's flow analysis cannot follow. Create main() there explicitly so
// the whole application call tree becomes reachable.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;

public class Main extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] seeds = { 0x39B4L };
        String[] names = { "main" };
        for (int i = 0; i < seeds.length; i++) {
            Address a = toAddr(seeds[i]);
            if (getInstructionAt(a) == null) disassemble(a);
            Function f = getFunctionAt(a);
            if (f == null) f = createFunction(a, names[i]);
            println("seed: " + (f == null ? "FAILED " + a : f.getName() + " @ " + f.getEntryPoint()));
        }
        println("Main seed done.");
    }
}