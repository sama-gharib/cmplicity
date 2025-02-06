examples: examples/number.atm examples/test_lexing.rgx examples/test_lexing.src
	@echo "=== Testing the automaton that recognize numbers ==="
	@./cmp automaton examples/number.atm 123.545
	@./cmp automaton examples/number.atm +12
	@./cmp automaton examples/number.atm 1.14.5
	@./cmp automaton examples/number.atm -41.6
	@./cmp automaton examples/number.atm -
	@./cmp automaton examples/number.atm coucou
	@echo ""
	@echo ""
	@echo "=== Testing the regex that recognizes time ==="
	@./cmp regex "((1|0)[0-9]|2[0-3])(:|h)[0-5][0-9](('|m)[0-5][0-9](''|s))?" "23:43'12''"
	@./cmp regex "((1|0)[0-9]|2[0-3])(:|h)[0-5][0-9](('|m)[0-5][0-9](''|s))?" "14:05'81''"
	@./cmp regex "((1|0)[0-9]|2[0-3])(:|h)[0-5][0-9](('|m)[0-5][0-9](''|s))?" "12:01"
	@./cmp regex "((1|0)[0-9]|2[0-3])(:|h)[0-5][0-9](('|m)[0-5][0-9](''|s))?" "56:30"
	@./cmp regex "((1|0)[0-9]|2[0-3])(:|h)[0-5][0-9](('|m)[0-5][0-9](''|s))?" "03h16m20s"
	@echo ""
	@echo ""
	@echo "=== Testing lexing ==="
	@./cmp lexing examples/test_lexing.rgx examples/test_lexing.src

examples/test_lexing.rgx examples/test_lexing.src: examples
	@echo "Number (\+|\-)?[0-9]+(\.[0-9]+)?" >> examples/test_lexing.rgx
	@echo "Identifier ([a-z]|_)+" >> examples/test_lexing.rgx
	@echo "Macro [A-Z]+" >> examples/test_lexing.rgx
	@echo "Separator ;" >> examples/test_lexing.rgx
	@echo "Declaration let" >> examples/test_lexing.rgx
	@echo "Operator \+|\-|\*|/|<|>|<=|>=" >> examples/test_lexing.rgx
	@echo "Assignation <\-|=" >> examples/test_lexing.rgx
	@echo "BlockStart {" >> examples/test_lexing.rgx
	@echo "BlockEnd }" >> examples/test_lexing.rgx
	@echo "Space  " >> examples/test_lexing.rgx
	@echo "BreakLine \n" >> examples/test_lexing.rgx
	@echo "Tabulation \t" >> examples/test_lexing.rgx

	@echo "let my_variable = -123.641;" >> examples/test_lexing.src
	@echo "let my_other_variable = ASK;" >> examples/test_lexing.src
	@echo "let my_last_variable = my_variable + my_other_variable;" >> examples/test_lexing.src
	@echo "if my_last_variable > 0 {" >> examples/test_lexing.src
	@echo "	PRINT 1;" >> examples/test_lexing.src
	@echo "}" >> examples/test_lexing.src


examples/number.atm: cmp
	@mkdir -p examples
	@echo "States: Sign, FirstDigitAfterSign, IntegerPart, FloatPart;" > examples/number.atm
	@echo "Finals: IntegerPart, FloatPart;" >> examples/number.atm
	@echo "Initial: Sign;" >> examples/number.atm
	@echo "Transitions:" >> examples/number.atm
	@echo "    (Sign, +, FirstDigitAfterSign)," >> examples/number.atm
	@echo "    (Sign, -, FirstDigitAfterSign)," >> examples/number.atm
	@echo "    (FirstDigitAfterSign, [0-9], IntegerPart)," >> examples/number.atm
	@echo "    (Sign, [0-9], IntegerPart)," >> examples/number.atm
	@echo "    (IntegerPart, [0-9], IntegerPart)," >> examples/number.atm
	@echo "    (IntegerPart, ., FloatPart)," >> examples/number.atm
	@echo "    (FloatPart, [0-9], FloatPart);" >> examples/number.atm

cmp: src/*.c
	@cc src/*.c -o cmp