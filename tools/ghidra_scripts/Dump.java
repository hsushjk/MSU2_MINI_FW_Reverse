import java.io.*;
import java.util.*;

import ghidra.app.decompiler.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.data.*;

public class Dump extends GhidraScript {
    PrintWriter idx, graph;
    String outDir;

    @Override
    public void run() throws Exception {
        outDir = System.getenv("DUMP_DIR");
        if (outDir == null) outDir = "C:/Users/hsush/Desktop/MSU2_MINI/work/dump";
        new File(outDir + "/funcs").mkdirs();

        DecompInterface di = new DecompInterface();
        DecompileOptions opts = new DecompileOptions();
        di.setOptions(opts);
        di.toggleCCode(true);
        di.toggleSyntaxTree(true);
        di.openProgram(currentProgram);

        idx = new PrintWriter(new FileWriter(outDir + "/index.tsv"));
        graph = new PrintWriter(new FileWriter(outDir + "/calls.tsv"));
        idx.println("addr\tname\tparams\tinsns\tcalls\tjumpTargets\tcallsNames\tcallers");

        FunctionManager fm = currentProgram.getFunctionManager();
        int n = 0, failed = 0;
        for (Function f : fm.getFunctions(true)) {
            if (monitor.isCancelled()) break;
            n++;
            Address ea = f.getEntryPoint();
            int insnCount = 0;
            for (Instruction i : currentProgram.getListing().getInstructions(f.getBody(), true)) insnCount++;

            Set<Address> callees = new TreeSet<>();
            Set<String> calledNames = new TreeSet<>();
            for (Instruction ins : currentProgram.getListing().getInstructions(f.getBody(), true)) {
                for (Reference r : ins.getReferencesFrom()) {
                    if (r.getReferenceType().isCall()) {
                        Function t = fm.getFunctionAt(r.getToAddress());
                        if (t != null) { callees.add(t.getEntryPoint()); calledNames.add(t.getName()); }
                    }
                }
            }
            // callers
            Set<String> callers = new TreeSet<>();
            for (Function c : f.getCallingFunctions(monitor)) callers.add(c.getName());

            String params = "";
            for (Parameter p : f.getParameters()) params += p.getDataType().getName() + " " + p.getName() + ", ";

            idx.printf("%08x\t%s\t%s\t%d\t%d\t%s\t%s\t%s%n", ea.getOffset(), f.getName(),
                params.isEmpty() ? "" : params, insnCount, callees.size(),
                joinAddrs(callees), String.join("|", calledNames), String.join("|", callers));
            for (Address c : callees) graph.printf("%08x\t%08x%n", ea.getOffset(), c.getOffset());

            // decompile
            try {
                DecompileResults res = di.decompileFunction(f, 60, monitor);
                String body;
                if (res != null && res.decompileCompleted()) {
                    body = res.getDecompiledFunction().getC();
                } else {
                    body = "// DECOMPILE FAILED: " + (res == null ? "null" : res.getErrorMessage()) + "\n";
                    failed++;
                }
                String fn = String.format("%08x_%s.c", ea.getOffset(),
                        f.getName().replaceAll("[^A-Za-z0-9_.-]", "_"));
                PrintWriter pw = new PrintWriter(new FileWriter(outDir + "/funcs/" + fn));
                pw.println("// Function: " + f.getName() + " @ " + ea);
                pw.println("// Signature: " + f.getPrototypeString(false, false));
                pw.println("// Callers: " + String.join(", ", callers));
                pw.println("// Callees: " + String.join(", ", calledNames));
                pw.println(body);
                pw.close();
            } catch (Exception e) {
                failed++;
                println("decompile fail " + ea + ": " + e);
            }
        }
        idx.close();
        graph.close();
        println("functions: " + n + ", decompile failures: " + failed);

        // strings + xrefs
        PrintWriter st = new PrintWriter(new FileWriter(outDir + "/strings.tsv"));
        st.println("addr\trefs\ttext");
        DataIterator dit = currentProgram.getListing().getDefinedData(true);
        for (Data d : dit) {
            if (d == null) break;
            if (d.getValue() instanceof String) {
                StringBuilder refs = new StringBuilder();
                for (Reference r : getReferencesTo(d.getAddress())) {
                    Function f = getFunctionContaining(r.getFromAddress());
                    refs.append(f == null ? "?" : f.getName()).append('@').append(r.getFromAddress()).append(' ');
                }
                st.printf("%08x\t%s\t%s%n", d.getAddress().getOffset(), refs.toString().replace("\n", " "),
                        d.getValue().toString().replace("\n", "\\n"));
            }
        }
        st.close();

        // memory blocks
        PrintWriter mm = new PrintWriter(new FileWriter(outDir + "/memory.tsv"));
        for (MemoryBlock b : currentProgram.getMemory().getBlocks())
            mm.printf("%s\t%08x-%08x\t%s%n", b.getName(), b.getStart().getOffset(), b.getEnd().getOffset(),
                b.isExecute() ? "X" : "-");
        mm.close();
        println("dump complete -> " + outDir);
    }

    String joinAddrs(Set<Address> s) {
        StringBuilder sb = new StringBuilder();
        for (Address a : s) { if (sb.length() > 0) sb.append('|'); sb.append(String.format("%08x", a.getOffset())); }
        return sb.toString();
    }
}