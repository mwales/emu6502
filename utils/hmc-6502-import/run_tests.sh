#!/bin/bash

# My testing script to test emulator functionality and verify that I don't break it
# while implementing new features (or refactoring old ones)


HMCDIR=../../utils/hmc-6502-import

cd ../../src/DebugClient/

echo "Cleaning testOutput directory"
rm ${HMCDIR}/testOutput/*.txt

# First argument is the root name of the test rom
# Second argument is the trace options required for the test
execute_test_case () {

	TESTCASE=$1
	TESTARGS=$2

	echo "********** Starting ${TESTCASE} ******"
	
	./traceUtility.py ${HMCDIR}/testbin/${TESTCASE}.rom.bin ${TESTARGS}

	mv emulator_stdout.txt emulator_stderr.txt ${HMCDIR}/testOutput
	mv trace_output.txt ${HMCDIR}/testOutput/${TESTCASE}-trace.txt

	# Verify the results
	cmp ${HMCDIR}/testOutput/${TESTCASE}-trace.txt ${HMCDIR}/expectedResults/${TESTCASE}-trace.txt

	if [ $? -eq 0 ];
	then
		echo "${TESTCASE} results verified successfully with previous results"
	else
		echo "${TESTCASE} results don't match previous results"
		exit 1
	fi

}

execute_test_case test00-loadstore "f000 -d=0000 -d=0200"
execute_test_case test01-andorxor  "f000 -d=0000"
execute_test_case test02-incdec    "f000 -d=0000 -d=0100"
execute_test_case test03-bitshifts "f000 -d=0000 -d=0100"
execute_test_case test04-jumpsret  "f000 -d=0000"
execute_test_case test05-reginstrs "f000 -d=0000"
execute_test_case test06-addsub    "f000 -d=0000 -d=0100"

echo "All tests completed successfully"


