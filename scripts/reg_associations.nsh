${If} $MAKE_LMC_ASSOC == 1
${RegisterExtension} "$INSTDIR\\bin\\LMVM.exe" ".lmc" "Compiled Little Man Computer Bytecode"
${EndIf}

${If} $MAKE_LMASM_ASSOC == 1
${RegisterExtension} "$INSTDIR\\bin\\LMASM.exe" ".lmasm" "Little Man Computer Assembly"
${EndIf}