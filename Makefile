example: examples/number.atm
	@echo "=== Testing the automaton that recognize numbers ==="
	@./cmp automaton examples/number.atm 123.545
	@./cmp automaton examples/number.atm +12
	@./cmp automaton examples/number.atm 1.14.5
	@./cmp automaton examples/number.atm -41.6
	@./cmp automaton examples/number.atm -
	@./cmp automaton examples/number.atm coucou

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